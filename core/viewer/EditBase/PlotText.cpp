// PlotText.cpp: implementation of the CPlotText class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PlotText.h"
#include "GrBuffer.h"
#include "GeoText.h"
#include "PlotChar.h"
#include "PlotWChar.h"
#include "SmartViewFunctions.h"
#include "Resource.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



MyNameSpaceBegin


/*
�ƶ������������ֵ
*/
void MovefromPts(GrVertex *pts, int ptsnum, double dx, double dy, double dz)
{
	for( int i=0; i<ptsnum; i++)
	{
		pts[i].x += dx;
		pts[i].y += dy;
		pts[i].z += dz;
	}	
}


/*
�������ܣ��Ե㴮�������ȴ��к����ŵı任
������
pts, �㴮����
ptsnum, �����Ŀ
x0,x1, ���ʺ�����ʱ���б任��x����ԭ��
xscale, x��������ű���
yscale, y��������ű���
dir, ���б任�ķ���0�����ʣ�1�����ʣ�2�����ʣ�3������
angle, ���нǶ�
*/


void TransformPts(GrVertex *pts, int ptsnum, double x0, double x1, double xscale, double yscale, int dir, double angle)
{
	//�������ž���
	double mscale[4]={xscale,0.0,0.0,yscale};

	//�Ƕ�����ֵ
	double b = tan(angle);
	if( b>1e3 )b = 1e3;
	else if( b<-1e3 )b = -1e3;
	else if( b>-1e-3 && b<1e-3 )b = b<0?(-1e-3):(1e-3);

	//�������ž���
	double mangle[4]={1.0,0.0,0.0,1.0};
	double xo = 0, yo = 0;

	switch( dir )
	{
	case 0: mangle[0]=1.0; mangle[1]=-b; mangle[2]=0.0; mangle[3]=1.0; break;
	case 1: mangle[0]=1.0; mangle[1]=b; mangle[2]=0.0; mangle[3]=1.0; break;
	case 2: mangle[0]=1.0; mangle[1]=0.0; mangle[2]=b; mangle[3]=1.0; xo = x0; break;
	case 3: mangle[0]=1.0; mangle[1]=0.0; mangle[2]=-b; mangle[3]=1.0; xo = x1; break;
	}

	double tx,ty;
	for( int i=0; i<ptsnum; i++)
	{
		//���б任
		tx = (pts[i].x-xo)*mangle[0] + (pts[i].y-yo)*mangle[1] + xo;
		ty = (pts[i].x-xo)*mangle[2] + (pts[i].y-yo)*mangle[3] + yo;

		pts[i].x = tx;  pts[i].y = ty;

		//���ű任
		tx = (pts[i].x)*mscale[0] + (pts[i].y)*mscale[1];
		ty = (pts[i].x)*mscale[2] + (pts[i].y)*mscale[3];

		pts[i].x = tx;  pts[i].y = ty;
	}
}

/*
�������ܣ�������֪�������е㴮����
������
linespt, ��֪���ߵĵ�
lineptnum, ��֪���ߵĵ�����
curpt, �����������������е���ʼ��ţ�����ֵ��������㴮���ݺ󣬻��ߵ��µ���������
offlen, ���ߴ���curpt�ŵĵ�������ƫ�Ƶĳ���, ���������֮��ſ�ʼ����������
        ����ֵ��������㴮���ݺ󣬻����µ��ߴ������ϵ�ŵ��µ�ƫ�Ƴ���
pts, ��Ҫ���еĵ�
ptsnum, ���еĵ���Ŀ
uselen, ���еĵ㼯ռ�õĳ���
*/

int LayoutPtsByLines( PT_3D *linespt, int lineptnum, 
							 int& curpt, double& offlen,
							 GrVertex *pts, int ptsnum, double charlen, double charinv)
{
	double uselen = charlen+charinv;
	double len0 = offlen, len1 = offlen+uselen*0.5, len2 = offlen+uselen, lent=0, lenlast=0;

	double xoff=0;
	int nums=-1, nume=-1, ret=0;
	for( int i=curpt; i<lineptnum-1; i++ )
	{
		//�����ۼƻ��ߵĳ���
		lent += sqrt((linespt[i+1].x-linespt[i].x)*(linespt[i+1].x-linespt[i].x)
			+ (linespt[i+1].y-linespt[i].y)*(linespt[i+1].y-linespt[i].y));

		//���ۼƳ��ȸոճ������ϵ㴮������λ��ʱ����ǰ�ĵ�ž���Ϊ�㴮���еĻ��㣬
		//�����ڸû����ˮƽƫ�ƾ�����ƫ�Ƴ��ȼ�ȥ�ϴε��ۼƳ���
		if( nums<0 && lent>len1 )
		{
			nums = i;
			xoff = len0-lenlast;
		}

		//���ۼƳ��ȸոճ������ϵ㴮��β��λ��ʱ����ǰ�ĵ�ž���Ϊ�㴮���е��յ㣬
		//����Ը��յ��ˮƽƫ��Ҳ��������ƫ�ƣ�
		//������һ�����е�ƫ�Ƴ���
		if( nume<0 && lent>=len2 )
		{
			nume = i;
			if( nume==curpt )offlen += uselen;
			else offlen = offlen+uselen-lenlast;
		}

		if( nums>=0 && nume>=0 )break;
		lenlast = lent;
	}

	if( nume<0 )ret = 1; //�����ڻ����ڣ�β���ڻ�����
	if( nums<0 )ret = 2; //���ļ�β�˶��ڻ�����

	//û���ҵ����ʵ����ϵ㣬�����ַ��Ѿ������˻��������ǵĳ��ȣ�
	//�Ǿʹӵ�ǰ���������ϵ㿪ʼ���㣻
	if(ret==1)
	{
		nume = curpt = lineptnum-1;
		offlen = len2-lent;
	}
	else if (ret==2)
	{
		nums = nume = curpt = lineptnum-1;
		xoff = len0-lent;
		offlen = len2-lent;		
	}
	else
		curpt = nume;

	double angle = 0;

	if( lineptnum>1)
	{
		if( nums>=lineptnum-1 )
			angle = GraphAPI::GGetAngle(linespt[lineptnum-2].x,linespt[lineptnum-2].y,
				linespt[lineptnum-1].x,linespt[lineptnum-1].y);
		else
			angle = GraphAPI::GGetAngle(linespt[nums].x,linespt[nums].y,linespt[nums+1].x,linespt[nums+1].y);
	}

	double cosa = cos(angle), sina = sin(angle);
	double m[4] = {cosa,-sina,sina,cosa};
	
	double tx,ty,tz;
	for( i=0; i<ptsnum; i++)
	{
		//ƽ��xoff
		pts[i].x += xoff;

		//�������z
		if( nums>=lineptnum-1 )tz = linespt[nums].z;
		else
		{
			lent = sqrt((linespt[nums+1].x-linespt[nums].x)*(linespt[nums+1].x-linespt[nums].x)
				+ (linespt[nums+1].y-linespt[nums].y)*(linespt[nums+1].y-linespt[nums].y));
			if( fabs(lent)<1e-3 )tz = linespt[nums].z;
			else tz = linespt[nums].z + (linespt[nums+1].z-linespt[nums].z)*xoff/lent;
		}

		//��ת
		tx = pts[i].x*m[0] + pts[i].y*m[1];
		ty = pts[i].x*m[2] + pts[i].y*m[3];

		pts[i].x = tx+linespt[nums].x;  pts[i].y = ty+linespt[nums].y;
		pts[i].z = tz;
	}	
	return ret;
}


int LayoutPtsByLines0( PT_3D *linespt, int lineptnum, 
					 int& curpt, double& offlen,
					 PT_3D &textPos, double &angle, double charlen, double charinv)
{
 	double uselen = charlen+charinv;
	double len0 = offlen, len1 = offlen+uselen*0.5, len2 = offlen+uselen, lent=0, lenlast=0;
	
	double xoff=0;
	int nums=-1, nume=-1, ret=0;
	for( int i=curpt; i<lineptnum-1; i++ )
	{
		//�����ۼƻ��ߵĳ���
		lent += sqrt((linespt[i+1].x-linespt[i].x)*(linespt[i+1].x-linespt[i].x)
			+ (linespt[i+1].y-linespt[i].y)*(linespt[i+1].y-linespt[i].y));
		
		//���ۼƳ��ȸոճ������ϵ㴮������λ��ʱ����ǰ�ĵ�ž���Ϊ�㴮���еĻ��㣬
		//�����ڸû����ˮƽƫ�ƾ�����ƫ�Ƴ��ȼ�ȥ�ϴε��ۼƳ���
		if( nums<0 && lent>len1 )
		{
			nums = i;
			xoff = len0-lenlast;
		}
		
		//���ۼƳ��ȸոճ������ϵ㴮��β��λ��ʱ����ǰ�ĵ�ž���Ϊ�㴮���е��յ㣬
		//����Ը��յ��ˮƽƫ��Ҳ��������ƫ�ƣ�
		//������һ�����е�ƫ�Ƴ���
		if( nume<0 && lent>=len2 )
		{
			nume = i;
			if( nume==curpt )offlen += uselen;
			else offlen = offlen+uselen-lenlast;
		}
		
		if( nums>=0 && nume>=0 )break;
		lenlast = lent;
	}
	
	if( nume<0 )ret = 1; //�����ڻ����ڣ�β���ڻ�����
	if( nums<0 )ret = 2; //���ļ�β�˶��ڻ�����
	
	//û���ҵ����ʵ����ϵ㣬�����ַ��Ѿ������˻��������ǵĳ��ȣ�
	//�Ǿʹӵ�ǰ���������ϵ㿪ʼ���㣻
	if(ret==1)
	{
		nume = curpt = lineptnum-1;
		offlen = len2-lent;
	}
	else if (ret==2)
	{
		nums = nume = curpt = lineptnum-1;
		offlen = len2-lent;
		xoff = len0-lent;		
	}
	else
		curpt = nume;
	
	angle = 0;
	if( lineptnum>1 )
	{
		if( nums>=lineptnum-1 )
			angle = GraphAPI::GGetAngle(linespt[lineptnum-2].x,linespt[lineptnum-2].y,
			linespt[lineptnum-1].x,linespt[lineptnum-1].y);
		else
			angle = GraphAPI::GGetAngle(linespt[nums].x,linespt[nums].y,linespt[nums+1].x,linespt[nums+1].y);
	}
	
	double cosa = cos(angle), sina = sin(angle);
	double tz;
//	double m[4] = {cosa,-sina,sina,cosa};
	textPos.x = linespt[nums].x+xoff*cosa;
	textPos.y = linespt[nums].y+xoff*sina;
	if( nums>=lineptnum-1 )tz = linespt[nums].z;	
	else
	{
		lent = sqrt((linespt[nums+1].x-linespt[nums].x)*(linespt[nums+1].x-linespt[nums].x)
			+ (linespt[nums+1].y-linespt[nums].y)*(linespt[nums+1].y-linespt[nums].y));
		if( fabs(lent)<1e-3 )tz = linespt[nums].z;
		else tz = linespt[nums].z + (linespt[nums+1].z-linespt[nums].z)*xoff/lent;			
	}
	textPos.z = tz;
	
	return ret;
}


CPlotTextEx::CPlotTextEx()
{
	m_arrpPts.RemoveAll();
	m_npPts = 0;
	
	m_pShape = NULL;
	m_settings = TEXT_SETTINGS0();
	m_charList.Format(_T("%s"),_T(""));
}

CPlotTextEx::~CPlotTextEx()
{
	if (m_arrpPts.GetSize()>0)
	{
		for (int i=0;i<m_arrpPts.GetSize();i++)
		{
			if (m_arrpPts[i])
			{
				delete ((CArray<GrVertex,GrVertex> *)(m_arrpPts[i]));
				m_arrpPts[i] = NULL;
			}
		}
	}
	m_arrpPts.RemoveAll();
}


void CPlotTextEx::SetText(LPCTSTR text)
{
	m_charList.Format(_T("%s"),text);	
}


void CPlotTextEx::SetSettings(TEXT_SETTINGS0 *settings)
{
	memcpy(&m_settings,settings,sizeof(TEXT_SETTINGS0));
}

void CPlotTextEx::SetShape(const CShapeLine *pSL)
{
	m_pShape = pSL;	
}


void CPlotTextEx::GetAddConstGrBuffer(GrBuffer *pBuf)
{
	int pos = 0, pencode, i, num, curpt=0;	
	const GrBuffer *pGrBuf=NULL;
	
	PT_3D *bpts = NULL;
	Envelope evlp;

	CArray<PT_3DEX,PT_3DEX> KeyPts;	
	m_pShape->GetKeyPts(KeyPts);
	int size = KeyPts.GetSize();
	
	if( size<=0 )return;
	
	TextSettings set;
	set.CopyFrom(&m_settings);
	TEXTMETRICW tm = g_Fontlib.SetSettings(&set)->textMetric;

	double charIntv = (m_settings.fCharIntervalScale*tm.tmAveCharWidth);
	double offlen = charIntv/2.0, cdis = 0;

	//���һ����
	int varPt = size-1;	

	if( m_settings.nPlaceType==singPt || m_settings.nPlaceType==byLineH || m_settings.nPlaceType==byLineV )
	{
		if( size<=1 )goto ERR_EXIT;

		//��ֱ����ʱ��Ϊ�����κÿ���ȱʡ�������ּ��
		if( m_settings.nPlaceType==byLineV )
		{
			charIntv = (m_settings.fCharIntervalScale+0.5)*tm.tmAveCharWidth;
		}
		
		//�õ��ڵ���ͼ�ε��еĶ�Ӧ���
		CArray<int,int> KeyPos;
		if(!m_pShape->GetKeyPosOfBaseLines(KeyPos))goto ERR_EXIT;
		
		//�õ������λ��
		pos = 0;
		pencode = KeyPts[size-1].pencode;
		if( pencode==penArc )
		{
			if( varPt<3 )goto ERR_EXIT;
			
			pos = varPt-1;
		}
		else if( pencode==penSpline )
		{
			if( varPt<=3 )goto ERR_EXIT;
			
			pos = varPt-3;
		}
		else
		{
			pos = varPt-1;
		}
		
		//ȡ���ǳ����

		CArray<PT_3DEX,PT_3DEX> pts;
		
		if(!m_pShape->GetPts(pts))goto ERR_EXIT;
		num = pts.GetSize();
		bpts = new PT_3D[num];
		if( !bpts )goto ERR_EXIT;		
		
		for( i=KeyPos[pos]; i<num; i++)
		{
			COPY_3DPT(bpts[i-KeyPos[pos]],pts[i]);
		}

		//���㳣���
		for( i=0; i<KeyPos[pos]; i++)
		{
			cdis = cdis + sqrt( (pts[i].x-pts[i+1].x)*(pts[i].x-pts[i+1].x) +
				(pts[i].y-pts[i+1].y)*(pts[i].y-pts[i+1].y) );
		}
		offlen = cdis - charIntv/2;
		
		DrawText(NULL);

		//���������е��ַ��ߴ�
		for( i=0; i<m_npPts; i++ )
		{
			//���������е��ַ��ߴ�
			if( offlen>m_arrWHs[i].nextPos/2 )
			{
				offlen = offlen-m_arrWHs[i].nextPos;
				continue;
			}
			else
			{
				offlen = -offlen;
				break;
			}
		}

		for( ; i<m_npPts; i++ )
		{
			CArray<GrVertex,GrVertex> *pArrPts = (CArray<GrVertex,GrVertex>*)m_arrpPts[i];

			if( pArrPts->GetSize()<=0 )continue;

			double charwid = m_arrWHs[i].nextPos;
			
			if( m_settings.nPlaceType==byLineV )
				charwid = m_arrWHs[i].hei;

			if( LayoutPtsByLines(bpts,num-KeyPos[pos],curpt,offlen,pArrPts->GetData(),pArrPts->GetSize(),charwid,charIntv)<=1 )
			{
				pBuf->BeginLineString(0,0);
				pBuf->Lines(pArrPts->GetData(),pArrPts->GetSize());	
				pBuf->End();
			}
			else
				break;
		
		}

		pBuf->RefreshEnvelope();
	}
	else if( m_settings.nPlaceType==mutiPt )
	{		
		//�õ������λ��
		pos = varPt;
		
		//ȡ���ǳ����
		PT_3DEX expt = KeyPts[pos];
		
		
		DrawText(NULL);

		if( pos<m_npPts )
		{
			if( ((CArray<GrVertex,GrVertex> *)m_arrpPts[pos])->GetSize()<=0 )goto ERR_EXIT;		

			//��Ԫ�ص����괦��: ���е㴮
			MovefromPts(((CArray<GrVertex,GrVertex> *)m_arrpPts[pos])->GetData(),((CArray<GrVertex,GrVertex> *)m_arrpPts[pos])->GetSize(),expt.x,expt.y,expt.z);

			pBuf->BeginLineString(0,0);
			pBuf->Lines(((CArray<GrVertex,GrVertex> *)m_arrpPts[pos])->GetData(),((CArray<GrVertex,GrVertex> *)m_arrpPts[pos])->GetSize());	
			pBuf->End();
			
		}

		pBuf->RefreshEnvelope();
	}
	if (m_arrpPts.GetSize()>0)
	{		
		for (int i=0;i<m_arrpPts.GetSize();i++)
		{
			if (m_arrpPts[i])
			{
				delete ((CArray<GrVertex,GrVertex> *)(m_arrpPts[i]));
				m_arrpPts[i] = NULL;
			}
		}
	}
	m_arrpPts.RemoveAll();

	if( bpts )delete[] bpts;
	return;

ERR_EXIT:
	if( bpts )delete[] bpts;
}


void CPlotTextEx::GetConstGrBuffer(GrBuffer *pBuf)
{
	int pos = 0, pencode, i, num, curpt=0;	
	const GrBuffer *pGrBuf=NULL;

	PT_3D *bpts = NULL;
	Envelope evlp;

	CArray<PT_3DEX,PT_3DEX> KeyPts;	
	m_pShape->GetKeyPts(KeyPts);
	int size = KeyPts.GetSize();
	if( size<=0 )return;

	TextSettings set;
	set.CopyFrom(&m_settings);
	TEXTMETRICW tm = g_Fontlib.SetSettings(&set)->textMetric;

	double charIntv = (m_settings.fCharIntervalScale*tm.tmAveCharWidth);
	double offlen = charIntv/2.0, cdis = 0;

	//���һ����

	int	varPt = size-1;

	if( m_settings.nPlaceType==singPt || m_settings.nPlaceType==byLineH || m_settings.nPlaceType==byLineV )
	{
		if( size<=1 )goto ERR_EXIT;

		//��ֱ����ʱ��Ϊ�����κÿ���ȱʡ�������ּ��
		if( m_settings.nPlaceType==byLineV )
		{
			charIntv = (m_settings.fCharIntervalScale+0.5)*tm.tmAveCharWidth;
		}

		//�õ��ڵ���ͼ�ε��еĶ�Ӧ���
		CArray<int,int> KeyPos;
		if(!m_pShape->GetKeyPosOfBaseLines(KeyPos))goto ERR_EXIT;

		//�õ������λ��
		pos = 0;
		pencode = KeyPts[size-1].pencode;
		if( pencode==penArc )
		{
			if( varPt<3 )goto ERR_EXIT;

			pos = varPt-1;
		}
		else if( pencode==penSpline )
		{
			if( varPt<=3 )goto ERR_EXIT;
			pos = varPt-3;
		}
		else
		{
			pos = varPt-1;
		}
		
		//ȡ�������
		CArray<PT_3DEX,PT_3DEX> pts;
		
		if(!m_pShape->GetPts(pts))goto ERR_EXIT;
		num = pts.GetSize();
		bpts = new PT_3D[num];
		if( !bpts )goto ERR_EXIT;		
		
		for( i=0; i<num; i++)
		{
			COPY_3DPT(bpts[i],pts[i]);
		}		

		//���㳣���
		for( i=0; i<KeyPos[pos]; i++)
		{
			cdis = cdis + sqrt( (pts[i].x-pts[i+1].x)*(pts[i].x-pts[i+1].x) +
				(pts[i].y-pts[i+1].y)*(pts[i].y-pts[i+1].y) );
		}
		cdis -= offlen;
		DrawText(NULL);
		for( i=0; i<m_npPts; i++ )
		{
			CArray<GrVertex,GrVertex> *pArrPts = (CArray<GrVertex,GrVertex>*)m_arrpPts[i];

			if( pArrPts->GetSize()<=0 )continue;			
		
			//��������㴮�ĳ��ȣ��Ϳ��Խ�����
			if( cdis>evlp.Width()/2 )
			{
				cdis = cdis-(m_arrWHs[i].nextPos+charIntv);
			}
			else
				break;
			
			if( curpt>=num-1 )break;
			
			double charwid = m_arrWHs[i].nextPos;
			
			if( m_settings.nPlaceType==byLineV )
				charwid = m_arrWHs[i].hei;
			
			//��Ԫ�ص����괦��: ���е㴮
			LayoutPtsByLines(bpts,num,curpt,offlen,pArrPts->GetData(),pArrPts->GetSize(),charwid,charIntv);

			pBuf->BeginLineString(0,0);
			pBuf->Lines(pArrPts->GetData(),pArrPts->GetSize());	
			pBuf->End();
	
		}

		pBuf->RefreshEnvelope();
	}
	else if( m_settings.nPlaceType==mutiPt )
	{
		if( size<=0 )goto ERR_EXIT;

		//�õ������λ��
		pos = varPt+1;
		bpts = new PT_3D[pos];
		if (!bpts)goto ERR_EXIT;
		
		for (i=0;i<pos;i++)
		{
			COPY_3DPT(bpts[i],KeyPts[i]);
		}
		//ȡ���ǳ����
		PT_3DEX expt;
		
		DrawText(NULL);
		if( pos>m_npPts )pos = m_npPts;
		for( i=0; i<pos; i++ )
		{
			CArray<GrVertex,GrVertex> *pArrPts = (CArray<GrVertex,GrVertex>*)m_arrpPts[i];

			if( pArrPts->GetSize()<=0 )continue;		
							
			for( int j=0; j<pArrPts->GetSize(); j++)
			{
				(*pArrPts)[j].x += bpts[i].x;
				(*pArrPts)[j].y += bpts[i].y;
				(*pArrPts)[j].z += bpts[i].z;
			}
			pBuf->BeginLineString(0,0);
			pBuf->Lines(pArrPts->GetData(),pArrPts->GetSize());	
			pBuf->End();			
		}

		pBuf->RefreshEnvelope();
	}

	if (m_arrpPts.GetSize()>0)
	{
		for (int i=0;i<m_arrpPts.GetSize();i++)
		{
			if (m_arrpPts[i])
			{
				delete ((CArray<GrVertex,GrVertex>*)(m_arrpPts[i]));
				m_arrpPts[i] = NULL;
			}
		}
	}
	m_arrpPts.RemoveAll();
	if( bpts )delete[] bpts;
	return;

ERR_EXIT:
	if( bpts )delete[] bpts;
}


void CPlotTextEx::GetVariantGrBuffer(GrBuffer *pBuf, int varPt)
{
	int pos = 0, pencode, i, num, curpt=0;
	const GrBuffer *pGrBuf=NULL;
	PT_3D *bpts = NULL;
	Envelope evlp;

	CArray<PT_3DEX,PT_3DEX> KeyPts;	
	m_pShape->GetKeyPts(KeyPts);
	int size = KeyPts.GetSize();
	
	//���һ����
	if( varPt==-1 )
	{
		varPt = size-1;
	}
	else if( varPt<0 || varPt>size )
	{	
		return;
	}
	
	TextSettings set;
	set.CopyFrom(&m_settings);
	TEXTMETRICW tm = g_Fontlib.SetSettings(&set)->textMetric;

	double charIntv = (m_settings.fCharIntervalScale*tm.tmAveCharWidth);
	double offlen = charIntv/2.0, cdis = 0;
	
	if( m_settings.nPlaceType==singPt || m_settings.nPlaceType==byLineH || m_settings.nPlaceType==byLineV )
	{
		if( size<=1 )goto ERR_EXIT;

		//��ֱ����ʱ��Ϊ�����κÿ���ȱʡ�������ּ��
		if( m_settings.nPlaceType==byLineV )
		{
			charIntv = (m_settings.fCharIntervalScale+0.5)*tm.tmAveCharWidth;
		}

		//�õ��ڵ���ͼ�ε��еĶ�Ӧ���
		CArray<int,int> KeyPos;
		if(!m_pShape->GetKeyPosOfBaseLines(KeyPos))goto ERR_EXIT;
	
		//�õ������λ��
		pos = 0;
		pencode = KeyPts[size-1].pencode;
		if( pencode==penArc )
		{
			if( varPt<3 )goto ERR_EXIT;
			
			pos = varPt-1;
		}
		else if( pencode==penSpline )
		{
			if( varPt<=3 )goto ERR_EXIT;
			
			pos = varPt-3;
		}
		else
		{
			pos = varPt-1;
		}
		
		//ȡ���ǳ����
		CArray<PT_3DEX,PT_3DEX> pts;
		if(!m_pShape->GetPts(pts)) goto ERR_EXIT;		
		num = pts.GetSize();
		bpts = new PT_3D[num];
		if( !bpts )goto ERR_EXIT;		
		
		int num2 = num;
		if( varPt<size-1 )num2 = KeyPos[varPt+1]+1;
		if( pos<0 )pos = 0;

		for( i=KeyPos[pos]; i<num2; i++)
		{
			COPY_3DPT(bpts[i-KeyPos[pos]],pts[i]);
		}

		//���㳣���
		for( i=0; i<KeyPos[pos]; i++)
		{
			cdis = cdis + sqrt( (pts[i].x-pts[i+1].x)*(pts[i].x-pts[i+1].x) +
				(pts[i].y-pts[i+1].y)*(pts[i].y-pts[i+1].y) );
		}
		offlen = cdis - charIntv/2;
		
		DrawText(NULL);

		//���������е��ַ��ߴ�
		for( i=0; i<m_npPts; i++ )
		{
			CArray<GrVertex,GrVertex> *pArrPts = (CArray<GrVertex,GrVertex>*)m_arrpPts[i];

			if( pArrPts->GetSize()<=0 )continue;
			
			//���������е��ַ��ߴ�
			if( offlen>m_arrWHs[i].nextPos/2 )
			{
				offlen = offlen-m_arrWHs[i].nextPos;
				continue;
			}
			else
			{
				offlen = -offlen;
				break;
			}
		}

		for( ; i<m_npPts; i++ )
		{
			CArray<GrVertex,GrVertex> *pArrPts = (CArray<GrVertex,GrVertex>*)m_arrpPts[i];

			if( pArrPts->GetSize()<=0 )continue;

			double charwid = m_arrWHs[i].nextPos;

			if( m_settings.nPlaceType==byLineV )
				charwid = m_arrWHs[i].hei;
	
			//��Ԫ�ص����괦��: ���е㴮
			if(LayoutPtsByLines(bpts,num2-KeyPos[pos],curpt,offlen,pArrPts->GetData(),pArrPts->GetSize(),charwid,charIntv)>1)
				break;

			pBuf->BeginLineString(0,0);
			pBuf->Lines(pArrPts->GetData(),pArrPts->GetSize());			
			pBuf->End();
		}

		pBuf->RefreshEnvelope();
	}
	else if( m_settings.nPlaceType==mutiPt )
	{
		if( size<=0 )goto ERR_EXIT;

		//�õ������λ��
		pos = varPt;
		
		PT_3DEX expt = KeyPts[pos];
		offlen = 0;
		DrawText(NULL);

		if( pos<m_npPts )
		{
			int ptsnum = ((CArray<GrVertex,GrVertex> *)m_arrpPts[pos])->GetSize();
			if( ptsnum>0 )
			{
				//��Ԫ�ص����괦��: ���е㴮
				MovefromPts(((CArray<GrVertex,GrVertex> *)m_arrpPts[pos])->GetData(),((CArray<GrVertex,GrVertex> *)m_arrpPts[pos])->GetSize(),expt.x,expt.y,expt.z);
					
				pBuf->BeginLineString(0,0);
				pBuf->Lines(((CArray<GrVertex,GrVertex> *)m_arrpPts[pos])->GetData(),((CArray<GrVertex,GrVertex> *)m_arrpPts[pos])->GetSize());	
				pBuf->End();
			}
		}
		
		pBuf->RefreshEnvelope();
	}	
	if(  m_settings.nPlaceType==byLineGridH || m_settings.nPlaceType==byLineGridV )
	{
		if( size<=1 )goto ERR_EXIT;
		double allLineLen,charInv;
		CArray<PT_3DEX,PT_3DEX> pts;
		if(!m_pShape->GetPts(pts)) goto ERR_EXIT;		
		num = pts.GetSize();
		bpts = new PT_3D[num];
		if( !bpts )goto ERR_EXIT;
		offlen = 0;
		for( i=0; i<num; i++)
		{
			COPY_3DPT(bpts[i],pts[i]);
		}
		DrawText(NULL);	
		allLineLen = GraphAPI::GGetAllLen2D(bpts,num);
		charInv = (allLineLen - m_lfAllCharLen)/(m_npPts-1);
		if (1/*charInv>=0*/)//������ʱ�ı��ɷ��ص���ʾ
		{			
			for(i=0 ; i<m_npPts; i++ )
			{
				CArray<GrVertex,GrVertex> *pArrPts = (CArray<GrVertex,GrVertex>*)m_arrpPts[i];

				if( pArrPts->GetSize()<=0 )continue;

				double charwid = m_arrWHs[i].nextPos;
				
				if( m_settings.nPlaceType==byLineGridV )
					charwid = m_arrWHs[i].hei;

				LayoutPtsByLines(bpts,num,curpt,offlen,pArrPts->GetData(),pArrPts->GetSize(),charwid,charInv);

				if( curpt>=num )
				{				
					break;
				}
				pBuf->BeginLineString(0,0);
				pBuf->Lines(pArrPts->GetData(),pArrPts->GetSize());			
				pBuf->End();
			}
			pBuf->RefreshEnvelope();
		}
	}

	
	if (m_arrpPts.GetSize()>0)
	{	
		for (int i=0;i<m_arrpPts.GetSize();i++)
		{
			if (m_arrpPts[i])
			{
				delete ((CArray<GrVertex,GrVertex> *)(m_arrpPts[i]));
				m_arrpPts[i] = NULL;
			}
		}
	}
	m_arrpPts.RemoveAll();
	if( bpts )delete[] bpts;
	return;

ERR_EXIT:
	if( bpts )delete[] bpts;
}

extern float GetSymbolDrawScale();

void CPlotTextEx::DrawText(GrBuffer *pBuf)
{
	if (m_arrpPts.GetSize()>0)
	{		
		for (int i=0;i<m_arrpPts.GetSize();i++)
		{
			if (m_arrpPts[i])
			{
				delete (CArray<GrVertex,GrVertex> *)m_arrpPts[i];
				m_arrpPts[i] = NULL;
			}
		}
	}
	m_arrpPts.RemoveAll();	
	m_npPts = 0;

	m_arrWHs.RemoveAll();

	int num = m_charList.GetLength();
	if( num<=0 )return;
#ifdef _UNICODE
	wstring wstr = LPCTSTR(m_charList);
	
#else
	wstring wstr;
	ConvertCharToWstring(LPCTSTR(m_charList),wstr,CP_ACP);
#endif
	num = wstr.size();

	for (int i=0;i<num;i++)
	{
		m_arrpPts.Add(new CArray<GrVertex,GrVertex>);
		m_arrWHs.Add(CharWH());
	}

	TEXT_SETTINGS0 *setting = &m_settings;
	if (_tcslen(setting->strFontName) <= 0)_tcscpy(setting->strFontName, StrFromResID(IDS_DEFFONT));

	TextSettings set;

	set.CopyFrom(setting);	
	set.fTextAngle = 0;

	//������ֱ����ʱ�������Ǵ�ֱ������
	if (setting->nPlaceType==byLineV|| setting->nPlaceType==byLineGridV)
	{
		set.nAlignment = (set.nAlignment&TAV_CLEAR)|TAV_TOP;
		set.fTextAngle += 90;
	}
	//����ˮƽ����ʱ��������ˮƽ������
	else if (setting->nPlaceType==byLineH|| setting->nPlaceType==byLineGridH)
	{
		set.nAlignment = (set.nAlignment&TAH_CLEAR)|TAH_LEFT;	
	}

	set.fHeight = PC_SIZE;

	g_Fontlib.SetSettings(&set);

	//�������ž���
	double scale = setting->fHeight;
	double mscale[4]={scale/PC_SIZE,0.0,0.0,scale/PC_SIZE};
	double tx,ty;
	CharWH info;

	int j = 0,k=0;
	if (m_settings.nPlaceType==byLineGridV||m_settings.nPlaceType==byLineGridH)
	{
		m_lfAllCharLen = 0.0;
	}
	for (i=0;i<num;i++)
	{
		if (wstr[i]==L'\n')
		{
			continue;
		}

		CArray<GrVertex,GrVertex> *pArrPts = (CArray<GrVertex,GrVertex>*)m_arrpPts[j];

		g_Fontlib.Plot(wstr[i],pArrPts,&info.nextPos,&info.wid,&info.hei);
		info.nextPos *= setting->fHeight/PC_SIZE;
		info.wid *= setting->fHeight/PC_SIZE;
		info.hei *= setting->fHeight/PC_SIZE;

		m_arrWHs.SetAt(i,info);

		GrVertex *pVertex = pArrPts->GetData();
		for (k=0; k<pArrPts->GetSize(); k++)
		{
			//���ű任
			tx = (pVertex[k].x)*mscale[0] + (pVertex[k].y)*mscale[1];
			ty = (pVertex[k].x)*mscale[2] + (pVertex[k].y)*mscale[3];
			
			pVertex[k].x = tx;  pVertex[k].y = ty;
		}
		if (m_settings.nPlaceType==byLineGridH||m_settings.nPlaceType==byLineGridV)
		{
			Envelope e;
			e.CreateFromPts(pVertex,pArrPts->GetSize(),sizeof(GrVertex));
			m_lfAllCharLen += e.Width();
		}	
		j++;
	}
	m_npPts = j;
}


void CPlotTextEx::CreateCharWH()
{
	m_arrWHs.RemoveAll();

	int num = m_charList.GetLength();
	if( num<=0 )return;
#ifdef _UNICODE
	wstring wstr = LPCTSTR(m_charList);
	
#else
	wstring wstr;
	ConvertCharToWstring(LPCTSTR(m_charList),wstr,CP_ACP);
#endif
	num = wstr.size();

	for (int i=0;i<num;i++)
	{
		m_arrWHs.Add(CharWH());
	}

	TEXT_SETTINGS0 *setting = &m_settings;
	if (_tcslen(setting->strFontName) <= 0)_tcscpy(setting->strFontName, StrFromResID(IDS_DEFFONT));

	TextSettings set;

	set.CopyFrom(setting);	
	set.fTextAngle = 0;

	//������ֱ����ʱ�������Ǵ�ֱ������
	if (setting->nPlaceType==byLineV|| setting->nPlaceType==byLineGridV)
	{
		set.nAlignment = (set.nAlignment&TAV_CLEAR)|TAV_TOP;
		set.fTextAngle += 90;
	}
	//����ˮƽ����ʱ��������ˮƽ������
	else if (setting->nPlaceType==byLineH|| setting->nPlaceType==byLineGridH)
	{
		set.nAlignment = (set.nAlignment&TAH_CLEAR)|TAH_LEFT;	
	}

	set.fHeight = PC_SIZE;

	g_Fontlib.SetSettings(&set);

	//�������ž���
	CharWH info;

	for (i=0;i<num;i++)
	{
		if (wstr[i]==L'\n')
		{
			continue;
		}

		g_Fontlib.Plot(wstr[i],NULL,&info.nextPos,&info.wid,&info.hei);
		info.nextPos *= setting->fHeight/PC_SIZE;
		info.wid *= setting->fHeight/PC_SIZE;
		info.hei *= setting->fHeight/PC_SIZE;

		m_arrWHs.SetAt(i,info);
	}
}

void CPlotTextEx::GetOutLineBuf(GrBuffer* pBuf)
{
	int pos = 0, i, num, curpt=0;	
	const GrBuffer *pGrBuf=NULL;

	PT_3D *bpts = NULL;
	Envelope evlp;

	CArray<PT_3DEX,PT_3DEX> KeyPts;	
	m_pShape->GetKeyPts(KeyPts);
	int size = KeyPts.GetSize();
	if( size<=0 )return;

	TextSettings set;
	set.CopyFrom(&m_settings);
	TEXTMETRICW tm = g_Fontlib.SetSettings(&set)->textMetric;

	double charIntv = (m_settings.fCharIntervalScale*tm.tmAveCharWidth);
	double offlen = charIntv/2.0, cdis = 0;
	//���һ����

	//�õ�����ȷ�� OutLine
	const int PcSize = 400;

	int	varPt = size-1;
	if ( m_settings.nPlaceType==singPt )
	{
		TextSettings setting;
		setting.CopyFrom(&m_settings);
		setting.fHeight = PcSize;

		CArray<GrVertex,GrVertex> pts;

		g_Fontlib.SetSettings(&setting);
		g_Fontlib.Plot(m_charList,&pts);

		GrVertex *pVertex = pts.GetData();
		double scale = m_settings.fHeight,tx,ty;
		double mscale[4]={scale/PcSize,0.0,0.0,scale/PcSize};
		for (int k=0;k<pts.GetSize();k++)
		{
			//���ű任
			tx = (pVertex[k].x)*mscale[0] + (pVertex[k].y)*mscale[1];
			ty = (pVertex[k].x)*mscale[2] + (pVertex[k].y)*mscale[3];
			
			pVertex[k].x = tx;  pVertex[k].y = ty;
		}
		MovefromPts(pts.GetData(),pts.GetSize(),KeyPts[0].x,KeyPts[0].y,KeyPts[0].z);
		pBuf->BeginLineString(0,0);
		pBuf->Lines(pts.GetData(),pts.GetSize());	
		pBuf->End();		

	}
	else if(m_settings.nPlaceType==byLineH || m_settings.nPlaceType==byLineV )
	{
		if( size<=1 )goto ERR_EXIT;

		//��ֱ����ʱ��Ϊ�����κÿ���ȱʡ�������ּ��
		if( m_settings.nPlaceType==byLineV )
		{
			charIntv = (m_settings.fCharIntervalScale+0.5)*tm.tmAveCharWidth;
		}
		
		//ȡ�������
		CArray<PT_3DEX,PT_3DEX> pts;
		
		if(!m_pShape->GetPts(pts))goto ERR_EXIT;
		num = pts.GetSize();
		bpts = new PT_3D[num];
		if( !bpts )goto ERR_EXIT;		
		
		for( i=0; i<num; i++)
		{
			COPY_3DPT(bpts[i],pts[i]);
		}

		DrawText(NULL);
		for( i=0; i<m_npPts; i++ )
		{
			CArray<GrVertex,GrVertex> *pArrPts = (CArray<GrVertex,GrVertex>*)m_arrpPts[i];
			if( pArrPts->GetSize()<=0 )continue;
			
			if( curpt>=num-1 )break;	
			
			double charwid = m_arrWHs[i].nextPos;
			
			if( m_settings.nPlaceType==byLineV )
				charwid = m_arrWHs[i].hei;
			
			//��Ԫ�ص����괦��: ���е㴮
			LayoutPtsByLines(bpts,num,curpt,offlen,pArrPts->GetData(),pArrPts->GetSize(),charwid,charIntv);
			
			evlp.CreateFromPts(pArrPts->GetData(),pArrPts->GetSize(),sizeof(GrVertex));
			
			pBuf->BeginLineString(0,0);
			pBuf->Lines(pArrPts->GetData(),pArrPts->GetSize());	
			pBuf->End();
	
		}

		pBuf->RefreshEnvelope();
	}
	if(  m_settings.nPlaceType==byLineGridH || m_settings.nPlaceType==byLineGridV )
	{
		if( size<=1 )goto ERR_EXIT;
		double allLineLen,charInv;
		CArray<PT_3DEX,PT_3DEX> pts;
		if(!m_pShape->GetPts(pts)) goto ERR_EXIT;		
		num = pts.GetSize();
		bpts = new PT_3D[num];
		if( !bpts )goto ERR_EXIT;
		offlen = 0;
		for( i=0; i<num; i++)
		{
			COPY_3DPT(bpts[i],pts[i]);
		}
		DrawText(NULL);	
		allLineLen = GraphAPI::GGetAllLen2D(bpts,num);
		charInv = (allLineLen - m_lfAllCharLen)/(m_npPts-1);
		if (charInv>=0)
		{
			for(i=0 ; i<m_npPts; i++ )
			{
				CArray<GrVertex,GrVertex> *pArrPts = (CArray<GrVertex,GrVertex>*)m_arrpPts[i];

				if( pArrPts->GetSize()<=0 )continue;

				LayoutPtsByLines(bpts,num,curpt,offlen,pArrPts->GetData(),pArrPts->GetSize(),m_arrWHs[i].wid,charInv);
				if( curpt>=num )
				{				
					break;
				}
				pBuf->BeginLineString(0,0);
				pBuf->Lines(pArrPts->GetData(),pArrPts->GetSize());			
				pBuf->End();
			}
			pBuf->RefreshEnvelope();
		}
	}
	else if( m_settings.nPlaceType==mutiPt )
	{
		if( size<=0 )goto ERR_EXIT;

		//�õ������λ��
		pos = varPt+1;
		bpts = new PT_3D[pos];
		if (!bpts)goto ERR_EXIT;
		
		for (i=0;i<pos;i++)
		{
			COPY_3DPT(bpts[i],KeyPts[i]);
		}
		//ȡ���ǳ����
		PT_3DEX expt;
		
		DrawText(NULL);
		if( pos>m_npPts )pos = m_npPts;
		for( i=0; i<pos; i++ )
		{
			CArray<GrVertex,GrVertex> *pArrPts = (CArray<GrVertex,GrVertex>*)m_arrpPts[i];

			if( pArrPts->GetSize()<=0 )continue;

			for( int j=0; j<pArrPts->GetSize(); j++)
			{
				(*pArrPts)[j].x += bpts[i].x;
				(*pArrPts)[j].y += bpts[i].y;
				(*pArrPts)[j].z += bpts[i].z;
			}
			pBuf->BeginLineString(0,0);
			pBuf->Lines(pArrPts->GetData(),pArrPts->GetSize());	
			pBuf->End();			
		}

		pBuf->RefreshEnvelope();
	}
	if( bpts )delete[] bpts;

	if (m_arrpPts.GetSize()>0)
	{
		for (int i=0;i<m_arrpPts.GetSize();i++)
		{
			if (m_arrpPts[i])
			{
				delete ((CArray<GrVertex,GrVertex>*)(m_arrpPts[i]));
				m_arrpPts[i] = NULL;
			}
		}
	}
	m_arrpPts.RemoveAll();
	
	return;

ERR_EXIT:
	if( bpts )delete[] bpts;
}



BOOL CPlotTextEx::Plot(LPCTSTR strText , PT_3D *bpts, int ptsnum, TEXT_SETTINGS0 *setting,BOOL bGrdSize, GrBuffer* pBuf, BOOL bSepMode)
{
	if( !strText || !bpts || ptsnum<=0 || !setting || !pBuf )
		return FALSE;
	LPCWSTR text;
#ifdef _UNICODE
	text = strText;
#else
	wstring wstr;
	ConvertCharToWstring(strText,wstr,CP_ACP);
	text = wstr.c_str();
#endif
	if( wcslen(text)<=0 )return FALSE;
	HDC Memdc = g_Memdc.GetHdc();
	
	if( _tcslen(setting->strFontName)<=0 )return FALSE;

	SetSettings(setting);
	SetText(strText);

	if( setting->nPlaceType==byLineH || setting->nPlaceType==byLineV || setting->nPlaceType==byLineGridH || setting->nPlaceType==byLineGridV )
	{
		CreateCharWH();
	}

	CArray<GrVertex,GrVertex> pts;

	int num = wcslen(text);
	TextSettings set;
	
	set.CopyFrom(setting);
	set.fHeight = PC_SIZE;

	TextSettings set0 = set;
	TEXTMETRICW tm = g_Fontlib.SetSettings(&set0)->textMetric;
	
	double charIntv = setting->fCharIntervalScale*tm.tmAveCharWidth*setting->fHeight/PC_SIZE;

	set.fHeight = setting->fHeight;

	if (setting->nPlaceType==singPt)
	{
		if (bSepMode)
		{
			CArray<PT_3D,PT_3D> arrPts;
			g_Fontlib.SetSettings(&set);

			g_Fontlib.PlotOrigins(strText,&arrPts);

			if( set.nAlignment==TAH_DOT )
			{
				set.nAlignment = (TAH_RIGHT|TAV_BOTTOM);
			}

			PT_3D *pts = arrPts.GetData();
			PT_3D *pt_max = pts+arrPts.GetSize();

			wchar_t wch[4]={0};
			for( int i=0; i<num; i++)
			{
				if( text[i]==L'\n' )
					continue;

				wcsncpy(wch,text+i,1);

				pts->x += bpts[0].x;
				pts->y += bpts[0].y;
				pts->z = bpts[0].z;

#ifdef _UNICODE
				pBuf->Text(RGB(255,255,255), pts,wch, &set, bGrdSize);
#else
				string str;
				ConvertWcharToString(wch,str,CP_ACP,NULL,0);	
				pBuf->Text(RGB(255,255,255), pts,str.c_str(), &set, bGrdSize);
#endif

				pts++;
				if( pts>=pt_max )
					break;
			}
		}
		else
		{
			pBuf->Text(RGB(255,255,255), bpts,strText, &set, bGrdSize);
		}
	}
	if( setting->nPlaceType==mutiPt )
	{			
		string str;		
		wchar_t wch[4]={0};

		for (int i=0,j=0;i<num&&j<ptsnum-1;i++,j++)
		{
			wcsncpy(wch,text+i,1);
			if(wch[0]==L'\n')
			{
				j--;
				continue;
			}
#ifdef _UNICODE				
			pBuf->Text(RGB(255,255,255), &bpts[j], wch, &set, bGrdSize);
#else
			
			ConvertWcharToString(wch,str,CP_ACP,NULL,0);	
			pBuf->Text(RGB(255,255,255), &bpts[j],str.c_str(), &set, bGrdSize);
#endif
		}
		if (j>=ptsnum-1&&i<num)
		{
			if( bSepMode )
			{
				

				CArray<PT_3D,PT_3D> arrPts;
				g_Fontlib.SetSettings(&set);
#ifdef _UNICODE				
				g_Fontlib.PlotOrigins(text + i, &arrPts);
#else
				ConvertWcharToString(text+i,str,CP_ACP,NULL,0);	
				g_Fontlib.PlotOrigins(str.c_str(), &arrPts);
#endif
				
				PT_3D *pts = arrPts.GetData();
				PT_3D *pt_max = pts+arrPts.GetSize();
				
				wchar_t wch[4]={0};
				for( int j=i; j<num; j++)
				{
					if( text[j]==L'\n' )
						continue;
					
					wcsncpy(wch,text+j,1);
					
					pts->x += bpts[i].x;
					pts->y += bpts[i].y;
					pts->z = bpts[i].z;

#ifdef _UNICODE
					pBuf->Text(RGB(255, 255, 255), pts, wch, &set, bGrdSize);
#else
					string str;
					ConvertWcharToString(wch,str,CP_ACP,NULL,0);	
					pBuf->Text(RGB(255,255,255), pts,str.c_str(), &set, bGrdSize);
#endif
					
					pts++;
					if( pts>=pt_max )
						break;
				}
				return TRUE;
			}
			else
			{
#ifdef _UNICODE				
				pBuf->Text(RGB(255,255,255), &bpts[i],text+i, &set, bGrdSize);
#else			
				ConvertWcharToString(text+i,str,CP_ACP,NULL,0);	
				pBuf->Text(RGB(255,255,255), &bpts[i], str.c_str(), &set, bGrdSize);
#endif

			}

		}
	}
	else if( setting->nPlaceType==byLineH)
	{	
		set.nAlignment = (set.nAlignment&TAH_CLEAR)|TAH_LEFT;

		double nextPos;		
		int lineptnum = ptsnum, curpt = 0, nMove = 0;
		double offlen = charIntv/2.0;
		PT_3D *linespt = bpts;
		float xoff=0;
		float yoff=0;
		PT_3D textPos;
		double angle;
		wchar_t wch[4]={0};
		for( int i=0; i<num; i++)
		{			
			if (text[i]==L'\n')
			{
				continue;
			}
			wcsncpy(wch,text+i,1);
			
			nextPos = m_arrWHs[i].nextPos;
			LayoutPtsByLines0(linespt,lineptnum,curpt,offlen,textPos,angle,nextPos,charIntv);
			
			set.fTextAngle = angle*180/PI;
#ifdef _UNICODE				
			pBuf->Text(RGB(255,255,255), &textPos, wch, &set, bGrdSize);
#else
			string str;
			ConvertWcharToString(wch,str,CP_ACP,NULL,0);	
			pBuf->Text(RGB(255,255,255), &textPos,str.c_str(), &set, bGrdSize);
#endif			
		}
	}
	else if( setting->nPlaceType==byLineV)
	{
		set.nAlignment = (set.nAlignment&TAV_CLEAR)|TAV_TOP;

		//��ֱ����ʱ��Ϊ�����κÿ���ȱʡ�������ּ��
		charIntv = (setting->fCharIntervalScale+0.5)*tm.tmAveCharWidth*setting->fHeight/PC_SIZE;

		string str;
		double nextPos;		
		int lineptnum = ptsnum, curpt = 0, nMove = 0;	
		double offlen = charIntv/2;
		PT_3D *linespt = bpts;
		float xoff=0;
		float yoff=0;
		PT_3D textPos;
		double angle;
		wchar_t wch[4]={0};		
		for( int i=0; i<num; i++)
		{			
			if (text[i]==L'\n')
			{
				continue;
			}
			wcsncpy(wch,text+i,1);

			nextPos = m_arrWHs[i].hei;

			LayoutPtsByLines0(linespt,lineptnum,curpt,offlen,textPos,angle,nextPos,charIntv);

			set.fTextAngle = angle*180/PI + 90;
#ifdef _UNICODE				
			pBuf->Text(RGB(255,255,255), &textPos, wch, &set, bGrdSize);
#else
			string str;
			ConvertWcharToString(wch,str,CP_ACP,NULL,0);	
			pBuf->Text(RGB(255,255,255), &textPos,str.c_str(), &set, bGrdSize);
#endif			
		}

	}	
	else if( setting->nPlaceType==byLineGridH)
	{
		set.nAlignment = (set.nAlignment&TAH_CLEAR)|TAH_LEFT;

		string str;
		double nextPos;		
		int lineptnum = ptsnum, curpt = 0;	
		double offlen = 0.0;
		PT_3D *linespt = bpts;
		PT_3D textPos;
		double angle;
		wchar_t wch[4]={0};
		CArray<double,double> disArr;
		double AllCharLen = 0.0;
		for( int i=0; i<num; i++)
		{			
			if (text[i]==L'\n')
			{
				continue;
			}
			wcsncpy(wch,text+i,1);
	
			nextPos = m_arrWHs[i].wid;
			AllCharLen += nextPos;
			disArr.Add(nextPos);		
		}
		int size = disArr.GetSize();
		double allLineLen = GraphAPI::GGetAllLen2D(linespt,lineptnum);
		double charInv = (allLineLen-AllCharLen)/(size-1);
		{
			double* pData = disArr.GetData();
			for( i=0; i<num; i++,pData++)
			{		
				if (text[i]==L'\n')
				{
					continue;
				}
				wcsncpy(wch,text+i,1);
				int ret = LayoutPtsByLines0(linespt,lineptnum,curpt,offlen,textPos,angle,*pData,charInv);
				
				set.fTextAngle = angle*180/PI;
#ifdef _UNICODE				
				pBuf->Text(RGB(255,255,255), &textPos,wch, &set, bGrdSize);
#else
				string str;
				ConvertWcharToString(wch,str,CP_ACP,NULL,0);	
				pBuf->Text(RGB(255,255,255), &textPos,str.c_str(), &set, bGrdSize);
#endif			
			}
		}
	}
	else if( setting->nPlaceType==byLineGridV)
	{
		set.nAlignment = (set.nAlignment&TAV_CLEAR)|TAV_TOP;

		string str;
		double nextPos;		
		int lineptnum = ptsnum, curpt = 0;	
		double offlen = 0.0;
		PT_3D *linespt = bpts;
		PT_3D textPos;
		double angle;
		wchar_t wch[4]={0};
		
		CArray<double,double> disArr;
		double AllCharLen = 0.0;
		for( int i=0; i<num; i++)
		{			
			if (text[i]==L'\n')
			{
				continue;
			}
			wcsncpy(wch,text+i,1);
			
			nextPos = m_arrWHs[i].hei;
			AllCharLen += nextPos;
			disArr.Add(nextPos);		
		}
		int size = disArr.GetSize();
		double allLineLen = GraphAPI::GGetAllLen2D(linespt,lineptnum);
		double charInv = (allLineLen-AllCharLen)/(size-1);

		{
			double* pData = disArr.GetData();
			for( i=0; i<num; i++,pData++)
			{		
				if (text[i]==L'\n')
				{
					continue;
				}
				wcsncpy(wch,text+i,1);
				int ret = LayoutPtsByLines0(linespt,lineptnum,curpt,offlen,textPos,angle,*pData,charInv);

				set.fTextAngle = angle*180/PI + 90;				
#ifdef _UNICODE				
				pBuf->Text(RGB(255,255,255), &textPos, wch, &set, bGrdSize);
#else
				string str;
				ConvertWcharToString(wch,str,CP_ACP,NULL,0);	
				pBuf->Text(RGB(255,255,255), &textPos,str.c_str(), &set, bGrdSize);
#endif			
			}
		}
	}

	return TRUE;
}

MyNameSpaceEnd


