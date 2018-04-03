// StreamCompress.cpp: implementation of the CStreamCompress class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StreamCompress.h"
#include "math.h"
#include "SmartViewBaseType.h"
#include "SmartViewFunctions.h"
#include "Linearizer.h"

MyNameSpaceBegin

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStreamCompress2_PT_3D::CStreamCompress2_PT_3D()
{
	m_lfLimit= -1;
	m_lfMaxLen = 0;
	m_arrOldPts.SetSize(0,128);
}

CStreamCompress2_PT_3D::~CStreamCompress2_PT_3D()
{
	
}

void CStreamCompress2_PT_3D::BeginCompress(double limit)
{
	if( limit>0 )m_lfLimit = limit;
	m_nCurPt = 0;
	
	m_arrOldPts.RemoveAll();
	m_arrNewPts.RemoveAll();
}


void CStreamCompress2_PT_3D::EndCompress()
{
	Compress();
}

BOOL CStreamCompress2_PT_3D::GetPts(PT_3D *&pts, int &npt)
{
	if( m_arrNewPts.GetSize()>0 )
	{
		pts = m_arrNewPts.GetData();
		npt = m_arrNewPts.GetSize();
		return TRUE;
	}
	return FALSE;
}

BOOL CStreamCompress2_PT_3D::GetCurCompPt(PT_3D &pt)
{
	if( m_arrOldPts.GetSize()>0 )
	{
		pt = m_arrOldPts.GetAt(m_arrOldPts.GetSize()-1);
		return TRUE;
	}
	return FALSE;
}



int CompressByHeight_PT_3D(double r, double maxlen, PT_3D *in, int nPtIn, PT_3D *out)
{
	if( nPtIn<=2 )
	{
		memcpy(out,in,sizeof(PT_3D)*nPtIn);
		return nPtIn;
	}
	
	//准备计算参数
	double dis, m = 0;
	int k = -1;
	
	double dx = in[nPtIn-1].x-in[0].x, dy = in[nPtIn-1].y-in[0].y;
	double d0 = sqrt(dx*dx + dy*dy);
	double d2 = d0*d0;
	
	double c = in[0].y * in[nPtIn-1].x - in[nPtIn-1].y * in[0].x;
	
	if( d0<1e-10 )
	{
		for( int i=1; i<nPtIn-1; i++)
		{
			//找到距离最远的点
			dis = (in[i].x-in[0].x)*(in[i].x-in[0].x)+(in[i].y-in[0].y)*(in[i].y-in[0].y);
			if( k<0 || dis>m )
			{
				k = i;
				m = dis;
			}
		}

		if( m>0 )m = sqrt(m);
	}
	else
	{
		for( int i=1; i<nPtIn-1; i++)
		{
			//找到距离最远的点
			dis = fabs(in[i].x*dy-in[i].y*dx+c)/d0;
			if( k<0 || dis>m )
			{
				k = i;
				m = dis;
			}
		}
	}
	
	//如果平均弦高值大于管道半径，就进一步压缩处理
	if( /*(d0>r||d0<1e-6) &&*/ m>r || (maxlen>0 && (d0+m)>maxlen) )
	{
		int nOut = CompressByHeight_PT_3D(r,maxlen,in,k+1,out);
		nOut += CompressByHeight_PT_3D(r,maxlen,in+k,nPtIn-k,out+nOut-1);
		return nOut-1;
	}
	else
	{
		out[0] = in[0];
		out[1] = in[nPtIn-1];
		return 2;
	}
}

void CStreamCompress2_PT_3D::Compress()
{
	if( m_arrOldPts.GetSize()<=0 )
		return;

	CArray<PT_3D, PT_3D> arrPts;
	arrPts.SetSize(m_arrOldPts.GetSize());
	int nOut = CompressByHeight_PT_3D(m_lfLimit,m_lfMaxLen,m_arrOldPts.GetData(),m_arrOldPts.GetSize(),arrPts.GetData());
	arrPts.SetSize(nOut);
	if( m_arrNewPts.GetSize()>0 )
		m_arrNewPts.RemoveAt(m_arrNewPts.GetSize()-1);
	m_arrNewPts.Append(arrPts);
	
	m_arrOldPts.RemoveAll();
	m_arrOldPts.SetSize(0,128);
	m_arrOldPts.Add(arrPts.GetAt(arrPts.GetSize()-1));
}

int  CStreamCompress2_PT_3D::AddStreamPt(PT_3D pt)
{
	m_arrOldPts.Add(pt);
	if( m_arrOldPts.GetSize()>=100 )
	{
		Compress();
	}

	return 1;
}


int CompressByAngle(double anglimit, PT_3D *in, int nPtIn, PT_3D *out)
{
	if( nPtIn<=2 )
	{
		memcpy(out,in,sizeof(PT_3D)*nPtIn);
		return nPtIn;
	}

	if( anglimit<0 )
	{
		out[0] = in[0];
		out[1] = in[nPtIn-1];
		return 2;
	}

	//计算偏移最大的点
	double dis, m = 0;
	int k = -1;
	for( int i=1; i<nPtIn-1; i++)
	{
		dis = GraphAPI::GGetNearestDisOfPtToLine(in[0].x,in[0].y,in[nPtIn-1].x,in[nPtIn-1].y,in[i].x,in[i].y);
		if( k<0 || dis>m )
		{
			m = dis;
			k = i;
		}
	}

	double d = sqrt((in[0].x-in[nPtIn-1].x)*(in[0].x-in[nPtIn-1].x)+(in[0].y-in[nPtIn-1].y)*(in[0].y-in[nPtIn-1].y));
	if( d>1e-6 && (m/d)>=(anglimit) )
	{
		int nOut = CompressByAngle(anglimit,in,k+1,out);
		nOut += CompressByAngle(anglimit,in+k,nPtIn-k,out+nOut-1);
		return nOut-1;
	}
	else
	{
		out[0] = in[0];
		out[1] = in[nPtIn-1];
		return 2;
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStreamCompress3_PT_3DEX::CStreamCompress3_PT_3DEX()
{
	m_nCurPt = -1;
	m_lfLimit= -1;
	m_lfMinX = m_lfMaxX = m_lfMinY = m_lfMaxY = 0;

	m_lfAngLimit = -1.0;

	m_arrOldPts.SetSize(0,128);
}

CStreamCompress3_PT_3DEX::~CStreamCompress3_PT_3DEX()
{
	
}

void CStreamCompress3_PT_3DEX::BeginCompress(double limit)
{
	if( limit>0 )m_lfLimit = limit;
	m_nCurPt = 0;
	m_lfMinX = m_lfMaxX = m_lfMinY = m_lfMaxY = 0;

	m_arrOldPts.RemoveAll();
	m_arrNewPts.RemoveAll();
}


void CStreamCompress3_PT_3DEX::EndCompress()
{
	m_nCurPt = -1;

	m_arrOldPts.RemoveAll();
	m_arrNewPts.RemoveAll();
}

BOOL CStreamCompress3_PT_3DEX::GetLastCompPt(PT_3D *&pts, int &npt)
{
	if( m_nCurPt>=2 )
	{
		pts = m_arrNewPts.GetData()+1;
		npt = m_arrNewPts.GetSize()-1;
		return TRUE;
	}
	return FALSE;
}

BOOL CStreamCompress3_PT_3DEX::GetCurCompPt(PT_3D &pt)
{
	if( m_nCurPt>=1 )
	{
		pt = m_ptCurComp;
		return TRUE;
	}
	return FALSE;
}

int  CStreamCompress3_PT_3DEX::AddStreamPt(PT_3D pt)
{
	if( m_nCurPt<0 )
		return 0;

	m_arrOldPts.Add(pt);
	
	if( m_nCurPt==0 )
	{
		m_ptCurComp = pt;
		m_nCurPt++;
		return 1;
	}
	
	if( m_nCurPt==1 )
	{
		m_ptLastComp = m_ptCurComp;
		m_ptCurComp = pt;
		m_lfMaxX = sqrt((m_ptLastComp.x-m_ptCurComp.x)*(m_ptLastComp.x-m_ptCurComp.x)+
			(m_ptLastComp.y-m_ptCurComp.y)*(m_ptLastComp.y-m_ptCurComp.y)+
			(m_ptLastComp.z-m_ptCurComp.z)*(m_ptLastComp.z-m_ptCurComp.z));
		m_nCurPt++;

		m_arrNewPts.RemoveAll();
		m_arrNewPts.Add(m_ptLastComp);

		return 2;
	}

	m_nCurPt++;
 
	//求 m_ptCurComp 在 m_ptLastComp-pt 直线上的垂足
	double x,y,z,t;
	{
		double dx,dy,dz;
		
		dx = m_ptCurComp.x-m_ptLastComp.x; dy = m_ptCurComp.y-m_ptLastComp.y; dz = m_ptCurComp.z-m_ptLastComp.z;
		if( _FABS(dx)<1e-10 && _FABS(dy)<1e-10 && _FABS(dz)<1e-10 )
		{
			x = m_ptLastComp.x; y = m_ptLastComp.y; z = m_ptLastComp.z;
			t = 0;
		}
		else
		{
			
			t = ((pt.x-m_ptLastComp.x)*dx+(pt.y-m_ptLastComp.y)*dy+(pt.z-m_ptLastComp.z)*dz)/
				(dx*dx+dy*dy+dz*dz);
			
			x = m_ptLastComp.x + t * dx; y = m_ptLastComp.y + t * dy; z = m_ptLastComp.z + t * dz;
		}
		
	}

	//判断 m_ptCurComp 在m_ptLastComp-pt 直线上的方位(是向左还是向右)
	PT_3D pts[3];
	pts[0] = m_ptLastComp;  pts[1] = m_ptCurComp; pts[2] = pt;  
	bool bclockwise = GraphAPI::GIsClockwise(pts,3)==1;

	//计算各个方向的偏距
	double disx = sqrt((x-m_ptCurComp.x)*(x-m_ptCurComp.x)+(y-m_ptCurComp.y)*(y-m_ptCurComp.y)+(z-m_ptCurComp.z)*(z-m_ptCurComp.z));
	double disy = sqrt((x-pt.x)*(x-pt.x)+(y-pt.y)*(y-pt.y)+(z-pt.z)*(z-pt.z));

	if( t>=1.0 || t<0.0  )
	{
		m_lfMaxX += disx;
		if( m_lfMinX>=disx )m_lfMinX -= disx;
		else m_lfMinX = 0;
	}
	else
	{
		m_lfMinX += disx;
		if( m_lfMaxX>=disx )m_lfMaxX -= disx;
		else m_lfMaxX = 0;
	}

	if( bclockwise )
	{
		m_lfMaxY += disy;
		if( m_lfMinY>=disy )m_lfMinY -= disy;
		else m_lfMinY = 0;
	}
	else
	{
		m_lfMinY += disy;	
		if( m_lfMaxY>=disy )m_lfMaxY -= disy;
		else m_lfMaxY = 0;
	}

	double minX = m_lfMaxX;
	if( minX>=m_lfLimit || m_lfMinY>=m_lfLimit || m_lfMaxY>=m_lfLimit )
	{
		m_ptLastComp = m_ptCurComp;
		m_ptCurComp = pt;
		m_lfMaxX = sqrt((m_ptLastComp.x-m_ptCurComp.x)*(m_ptLastComp.x-m_ptCurComp.x)+
			(m_ptLastComp.y-m_ptCurComp.y)*(m_ptLastComp.y-m_ptCurComp.y)+
			(m_ptLastComp.z-m_ptCurComp.z)*(m_ptLastComp.z-m_ptCurComp.z));
		m_lfMinX = m_lfMinY = m_lfMaxY = 0;

		m_arrNewPts.RemoveAll();
		m_arrNewPts.SetSize(m_arrOldPts.GetSize(),128);
		int nOut = CompressByAngle(m_lfAngLimit,m_arrOldPts.GetData(),m_arrOldPts.GetSize()-1,m_arrNewPts.GetData());
		m_arrNewPts.SetSize(nOut);

		m_arrOldPts.RemoveAll();
		m_arrOldPts.SetSize(0,128);
		m_arrOldPts.Add(m_ptLastComp);
		m_arrOldPts.Add(m_ptCurComp);

		return 2;
	}

	m_ptCurComp = pt;
	return 1;
}




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStreamCompress4_PT_3DEX::CStreamCompress4_PT_3DEX()
{
	m_lfLimit= -1;
	
	m_arrPts.SetSize(0,128);
}

CStreamCompress4_PT_3DEX::~CStreamCompress4_PT_3DEX()
{
	
}

void CStreamCompress4_PT_3DEX::BeginCompress(double limit)
{
	if( limit>0 )m_lfLimit = limit;
	m_arrPts.RemoveAll();
	m_compress.BeginCompress(m_arrPts.GetData(),0,m_lfLimit,true);
}


void CStreamCompress4_PT_3DEX::EndCompress()
{
	m_compress.EndCompress();
}

BOOL CStreamCompress4_PT_3DEX::GetPts(PT_3DEX *&pts, int &npt)
{
	pts = m_compress.m_pPts;
	npt = m_compress.m_nNum;

	return TRUE;
}

BOOL CStreamCompress4_PT_3DEX::GetCurCompPt(PT_3D &pt)
{
	if( m_compress.m_nNum>=1 )
	{
		COPY_3DPT(pt,m_compress.m_pPts[m_compress.m_nNum-1]);
		return TRUE;
	}
	return FALSE;
}

int  CStreamCompress4_PT_3DEX::AddStreamPt(PT_3D pt)
{
	PT_3DEX expt;
	COPY_3DPT(expt,pt);
	expt.pencode = penStream;

	if( m_arrPts.GetSize()<=m_compress.m_nNum )
	{
		m_arrPts.Add(expt);
		m_compress.m_nNum++;
	}
	else
	{
		m_arrPts.SetAt(m_compress.m_nNum,expt);
		m_compress.m_nNum++;
	}

	m_compress.m_lfLimit = m_lfLimit;
	m_compress.m_pPts = m_arrPts.GetData();
	m_compress.StepCompress();
	
	return 1;
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStreamCompress5_PT_3DEX::CStreamCompress5_PT_3DEX()
{
	m_lfLimit= -1;

	m_lfScale = 1.0;
	m_lfMaxLen = 0;
	m_arrOldPts.SetSize(0,128);
}

CStreamCompress5_PT_3DEX::~CStreamCompress5_PT_3DEX()
{
	
}

void CStreamCompress5_PT_3DEX::BeginCompress(double limit)
{
	if( limit>0 )m_lfLimit = limit;
	m_nCurPt = 0;
	
	m_arrOldPts.RemoveAll();
	m_arrNewPts.RemoveAll();
}


void CStreamCompress5_PT_3DEX::EndCompress()
{
	Compress();
}

BOOL CStreamCompress5_PT_3DEX::GetPts(PT_3DEX *&pts, int &npt)
{
	if( m_arrNewPts.GetSize()>0 )
	{
		pts = m_arrNewPts.GetData();
		npt = m_arrNewPts.GetSize();
		return TRUE;
	}
	return FALSE;
}

BOOL CStreamCompress5_PT_3DEX::GetCurCompPt(PT_3D &pt)
{
	if( m_arrOldPts.GetSize()>0 )
	{
		pt = m_arrOldPts.GetAt(m_arrOldPts.GetSize()-1);
		return TRUE;
	}
	return FALSE;
}



int CompressByAverageHei_PT_3DEX(double r, double maxlen, PT_3DEX *in, int nPtIn, PT_3DEX *out)
{
	if( nPtIn<=2 )
	{
		memcpy(out,in,sizeof(PT_3DEX)*nPtIn);
		return nPtIn;
	}
	
	//准备计算参数
	double all = 0, m = 0, dis, all1 = 0;
	int k = (nPtIn>=10?(nPtIn/2):(-1));
	
	double dx = in[nPtIn-1].x-in[0].x, dy = in[nPtIn-1].y-in[0].y;
	double d0 = sqrt(dx*dx + dy*dy), t;
	double d2 = d0*d0;
	
	double c = in[0].y * in[nPtIn-1].x - in[nPtIn-1].y * in[0].x;
	
	if( d0<1e-10 )
	{
		out[0] = in[0];

		for( int i=1; i<nPtIn-1; i++)
		{
			dis = (in[i].x-in[0].x)*(in[i].x-in[0].x)+(in[i].y-in[0].y)*(in[i].y-in[0].y);

			//弦高累加
			all += sqrt(dis);
		}

		k = nPtIn/2;
		all1 = all;

		return 1;
	}
	else
	{
		for( int i=1; i<nPtIn-1; i++)
		{
			//通常点数比较多时，取中间序号的点就差不多
			if( nPtIn<10 )
			{
				//找到距离中点最近的点
				t = (dx*(in[i].x-in[0].x )+dy*(in[i].y-in[0].y))/d2;
				t = fabs(t-0.5);
				if( k<0 || t<m )
				{
					m = t;
					k = i;
				}
			}

			//弦高累加
			all += fabs(in[i].x*dy-in[i].y*dx+c)/d0;
			all1 += (in[i].x*dy-in[i].y*dx+c)/d0;
		}

		all1 = fabs(all1);
	}
	
	//如果平均弦高值大于管道半径，就进一步压缩处理
	if( (d0>r||d0<1e-10) && all/(nPtIn-2)>r )
	{
		int nOut = CompressByAverageHei_PT_3DEX(r,maxlen,in,k+1,out);
		nOut += CompressByAverageHei_PT_3DEX(r,maxlen,in+k,nPtIn-k,out+nOut-1);
		return nOut-1;
	}
	else if( maxlen>0 && (all/(nPtIn-2)+d0)>maxlen )
	{
		int nOut = CompressByAverageHei_PT_3DEX(r,maxlen,in,k+1,out);
		nOut += CompressByAverageHei_PT_3DEX(r,maxlen,in+k,nPtIn-k,out+nOut-1);
		return nOut-1;
	}
	//对局部的小转弯地方检查一下弦高比
	else if( d0>r && nPtIn>=5 && all1/(d0*(nPtIn-2))>0.08 )
	{
		int nOut = CompressByAverageHei_PT_3DEX(r*0.15,maxlen,in,k+1,out);
		nOut += CompressByAverageHei_PT_3DEX(r*0.15,maxlen,in+k,nPtIn-k,out+nOut-1);
		return nOut-1;
	}
	else
	{
		out[0] = in[0];
		out[1] = in[nPtIn-1];
		return 2;
	}
}

void CStreamCompress5_PT_3DEX::Compress()
{
	if( m_arrOldPts.GetSize()<=0 )
		return;

	CArray<PT_3DEX, PT_3DEX> arrPts;
	arrPts.SetSize(m_arrOldPts.GetSize());
	int nOut = CompressByAverageHei_PT_3DEX(m_lfLimit*m_lfScale*0.5,m_lfMaxLen,m_arrOldPts.GetData(),m_arrOldPts.GetSize(),arrPts.GetData());
	arrPts.SetSize(nOut);
	if( m_arrNewPts.GetSize()>0 )
		m_arrNewPts.RemoveAt(m_arrNewPts.GetSize()-1);
	m_arrNewPts.Append(arrPts);
	
	m_arrOldPts.RemoveAll();
	m_arrOldPts.SetSize(0,128);
	m_arrOldPts.Add(arrPts.GetAt(arrPts.GetSize()-1));
}

int  CStreamCompress5_PT_3DEX::AddStreamPt(PT_3D pt)
{
	PT_3DEX expt;
	COPY_3DPT(expt,pt);
	expt.pencode = penStream;

	m_arrOldPts.Add(expt);
	if( m_arrOldPts.GetSize()>=100 )
	{
		Compress();
	}

	return 1;
}


MyNameSpaceEnd
