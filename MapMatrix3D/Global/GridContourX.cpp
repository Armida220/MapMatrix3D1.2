// GeoBuilderContourX.cpp: implementation of the GridContour class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GridContourX.h"
//#include "ImmediateGeom.h"


#define  NULL_DEM -9999.9

GridContour::GridContour()
{
   m_iIndexInterval = 5;
   m_bLasMode=false;
   scnt_x = NULL;
   scnt_y = NULL;
}

GridContour::~GridContour()
{
	for (int i = 0; i < m_iLineNum; i++)
	{
		CGeoCurve *pCurve = contours[i];
		if (pCurve)
		{
			delete pCurve;
			contours[i] = NULL;
		}
	}
	if( NULL!=scnt_x ){delete [] scnt_x;scnt_x=NULL;}
	if( NULL!=scnt_y ){delete [] scnt_y;scnt_y=NULL;}
}

int GridContour::Init(double* x, double *y, double *z, int iNum, float intervalx, float intervaly, float cntinterval)
{
	if (iNum < 20) return 0;

	double lfBaseX=x[0];
	double lfBaseY=y[0];
	double lfMaxX = x[0];
	double lfMaxY = y[0];
	double lfMinZ = z[0];
	double lfMaxZ = z[0];
	m_lfInterval=cntinterval;
    for(int i=1;i<iNum;i++)
	{
	 if(lfMinZ<(NULL_DEM+1)&&z[i]>(NULL_DEM+1))
	 {
		 lfMinZ = z[i];
	 }
	 if (x[i]>lfMaxX)
	 {
		 lfMaxX=x[i];
	 } 
	 if (y[i]>lfMaxY)
	 {
		 lfMaxY = y[i];
	 } 
	 if (x[i]<lfBaseX)
	 {
		 lfBaseX = x[i];
	 } 
	 if (y[i]<lfBaseY)
	 {
		 lfBaseY = y[i];
	 } 
	 if (z[i]<(NULL_DEM + 1))
		 continue;
	 if (z[i]<lfMinZ)
	 {
		 lfMinZ = z[i];
	 } 
	 if (z[i]>lfMaxZ)
	 {
		 lfMaxZ = z[i];
	 } 
	}
	m_lfBaseX=lfBaseX;
	m_lfBaseY=lfBaseY;

	/////////////////////
    int iColumn=(lfMaxX-lfBaseX)/intervalx+1;
	int iRow=(lfMaxY-lfBaseY)/intervaly+1;
	if(iColumn<=0||iRow<=0)
	{
		return false;
	}
    float * lfZ=NULL;
	long iSize= iColumn*iRow;
	if(iSize<0) return false;
	try
	{
 	 lfZ=new float[iSize];
	}
	catch(...)
	{
		return false;
	}
	for(int i=0;i<iRow;i++)
	{
     for(int j=0;j<iColumn;j++)
	 {
	  lfZ[i*iColumn+j]=NULL_DEM;
	 }
	}
	////
	lfMinZ=floor(lfMinZ/cntinterval)*cntinterval;
	{
		for(int i=0;i<iNum;i++)
		{
			int iTmpCol = (x[i] - lfBaseX) / intervalx;
			int iTmpRow = (y[i] - lfBaseY) / intervaly;
		  if(iTmpCol>=0&&iTmpCol<iColumn&&iTmpRow>=0&&iTmpRow<iRow)
		  {
			  if (z[i]>NULL_DEM + 1)
		   {
			   lfZ[iTmpRow*iColumn + iTmpCol] = z[i];
		   }
		  }
		}
	}
    /////
	float lfCurrentZ=lfMinZ;
    unsigned char * pFlagH=new unsigned char[iColumn*iRow+100];
	unsigned char * pFlagV=new unsigned char[iColumn*iRow+100];
	double   pdem[6];
	pdem[0]=lfBaseX;
	pdem[1]=lfBaseY;
	pdem[2]=0;
	pdem[3]=intervalx;
	pdem[4]=intervaly; 
 	do
	{
	  for(int i=0;i<iRow*iColumn;i++)
		 {
		  if(lfZ[i]==lfCurrentZ)
		  {
			lfZ[i]+=0.01;
		  }
		 }
		 SetFlags (lfZ,pFlagH,pFlagV,iRow,iColumn,lfCurrentZ); 
		 PrimaryTrack(lfCurrentZ, lfZ,  pFlagH, pFlagV,iRow,iColumn, pdem );
		 lfCurrentZ+=cntinterval;
	}
	while(lfCurrentZ<lfMaxZ);
 
 
	delete lfZ;
	delete pFlagH;
	delete pFlagV;
 
	return m_iLineNum;
}

CGeoCurve* GridContour::GetLine(int iNum)
{
      if(iNum>m_iLineNum-1)
		  return nullptr;
	  return contours[iNum];
}
int GridContour::GetContourNumber()
{
		return m_iLineNum;
}
 int GridContour::LineTrace(int pas, int * in, float * lfZ, float * x, float * y, int * np,
	                           unsigned char * pHW, unsigned char * pVW, int * row, int * col,
							   float z1, float z2, float z3, float z4, double lfDx, double lfDy,
							   int nx, int ny, float * x0, float * y0)
{
	
	     int        pasw=0; 
		if(pas==1)
		{
				*(x+ *np)= *x0+z1*(float)lfDx/(z1-z2);
				*(y+ *np)= *y0;  
				*y0-= (lfDy);
				*pHW=0; 
				*in=3;
				pasw= -nx;  
				(*row)--;
		} 
		if(pas==2)
		{
				*x0+= (lfDx); 
				*(x+ *np)= *x0;
				*(y+ *np)= *y0+z2*(float)lfDy/(z2-z3);
				*(pVW+1)=0; *in=4; pasw=1; (*col)++;
		} 
			
		if(pas==3) 
		{
				*(x+ *np)= *x0+z4*(float)lfDx/(z4-z3);
				*y0+= (lfDy); 
				*(y+ *np)= *y0;
				*(pHW+nx)=0; *in=1; pasw=nx; (*row)++;
		} 
			
		if(pas==4)
		{
				*(x+ *np)= *x0;
				*x0-= (lfDx);
				*(y+ *np)= *y0+z1*(float)lfDy/(z1-z4);
				*pVW=0; *in=2; pasw= -1;  (*col)--;
		}  
	    return (pasw);
	
}
 int  GridContour::MarkPass(float iz, float * lfZ, int nx, int ny, double lfDx, double lfDy, int * in, unsigned char * pHW, unsigned char * pVW, int * row, int * col, float * x0, float * y0, float * x, float * y, int * np, int * bEnd)
{	///给每条边加上标记-- <<数字摄影测量学>>
	int          pas,pw;
	float          zc,z1,z2,z3,z4;
	
	if (*lfZ    <float(NULL_DEM)+1 || *(lfZ+1)   <float(NULL_DEM)+1  || 
		*(lfZ+nx)<float(NULL_DEM)+1 || *(lfZ+nx+1)<float(NULL_DEM)+1) {
		*bEnd += 1;	
		return 0;
	}
	
	(*np)++;
	
	z1= float(*lfZ-iz); 
	z2= float(*(lfZ+1)-iz);
	z4= float(*(lfZ+nx)-iz);
	z3= float(*(lfZ+nx+1)-iz);
	
	zc= float((z1+z2+z3+z4)/4.);

	if(zc == float(0.0)) //避免为0
		zc += 0.01f;
	
	switch (*in)
	{ 
		case 1: 
			if (*pVW==1 && *(pVW+1)==1) 
			{
				if (zc*z2<0.0) 
					pas=2;   
				else
					if(zc*z1<0.0) 	pas=4;
				else 		
					return(0);
			}
			else
			{
				
				if(*(pVW+1) ==1) 	pas=2;
				else if(*(pHW+nx)>=1) 	pas=3;
				else if(*pVW==1) 	pas=4;
				else 		
					return(0);
				
			}
			
			break;
			
		
	case 2:
		
		if (*pHW>=1 && *(pHW+nx)>=1) 
		{
			if (zc*z3<0.0) 		pas=3;
			else if(zc*z2<0.0) 	pas=1;
			else 			return(0);
		}
		else 
		{
			
			if (*(pHW+nx)>=1)      	pas=3;
			else if (*pVW==1) 	pas=4;
			else if (*pHW>=1)      	pas=1;
			else 			return(0);
			
		}
		
		break;
		
		
	case 3:
		
		if (*pVW==1 && *(pVW+1)==1) {
			if (zc*z4<0.0) 		pas=4;
			else if(zc*z3<0.0) 	pas=2;
			else 			return(0);
		}
		else {
			
			if (*pVW==1) 		pas=4;
			else if (*pHW>=1)      	pas=1;
			else if (*(pVW+1)==1) 	pas=2;
			else 			return(0);
			
		}
		
		break;
		
		
	case 4:
		
		if (*pHW>=1 && *(pHW+nx)>=1) {
			if (zc*z1<0.0) 		pas=1;
			else if(zc*z4<0.0) 	pas=3;
			else 			return(0);
		}
		else {
			
			if (*pHW>=1)          	pas=1;
			else if (*(pVW+1)==1) 	pas=2;
			else if (*(pHW+nx)>=1) 	pas=3;
			else 			return(0);
			
		}
		
		break;
		
	default:
		pas=0;
		return(0);
		break;
	}
	
	
	pw=LineTrace (pas,in,lfZ,x,y,np,pHW,pVW,row,col,z1,z2,z3,z4,lfDx,lfDy,nx,ny,x0,y0);
	
	return (pw);
	
} 
void GridContour::Flip(int np)
{
	for (int k=0; k<(np+1)/2; k++)
		{
			float x1=scnt_x[k]; scnt_x[k]=scnt_x[np-k]; scnt_x[np-k]=x1;
			float y1=scnt_y[k]; scnt_y[k]=scnt_y[np-k]; scnt_y[np-k]=y1;
		}
}
 void GridContour::LocalTrack(int row,int col,int r,int c,float x0,float y0,float * lfZ, double lfDx, double lfDy,
	                                 int in,unsigned char *pHW,unsigned char * pVW,float iz,int iFlipX)
 {
			int np=0;
			int bEnd=0;
			while (0<c && c<col && 0<r && r<row && bEnd==0) 
			{
				int w=MarkPass(iz,lfZ,  col,row,lfDx,lfDy,&in,pHW,pVW,&r,&c,
					&x0,&y0,scnt_x,scnt_y,&np,&bEnd);
				
				if (w==0 && bEnd==0) 
				{
					np--; 
					break;
				}
				lfZ+=w; pHW+=w; pVW+=w;
			}
			
			if (iFlipX==1)
			{
				Flip(np);
			}
				
		 	np++;
				
	     	if (np>3) 
				write_cnt( np,scnt_x,scnt_y,float(iz) );
 }

void GridContour::PrimaryTrack(float iz, float * lfZ, unsigned char * pFlagH, unsigned char * pFlagV, int row, int col,  double * pdem )
{	
	int            i,j,k,bEnd;
	unsigned char  *pHW,*pVW,*pFlagH0,*pFlagV0;
	int             in,r,c,np,w,iFlipX,reverse;  
	float            *h0; 
	float          x0,y0,x1,y1;
	double	       dem_x0,dem_y0;
	double lfDx,lfDy; 
	try
	{
		if( NULL==scnt_x )scnt_x = new float [2018000];
		if( NULL==scnt_y )scnt_y = new float [2018000];
	}
	catch (...) 
	{
		if( NULL==scnt_x||NULL==scnt_y )
		{
			if( NULL!=scnt_x )
			{
				delete [] scnt_x;
				scnt_x=NULL;
			}
			if( NULL!=scnt_y ){delete [] scnt_y;scnt_y=NULL;}
			return;
		}
	}
	
	dem_x0 = *pdem;
	dem_y0 = *(pdem+1);
	lfDx = *(pdem+3);
	lfDy = *(pdem+4);
 
	
	pFlagH0=pFlagH; pFlagV0=pFlagV; h0=lfZ; 

	for (i=0; i<col-1; i++) 
	{
		if (*pFlagH==1) 
		{
			if (*(h0+i+1) > *(h0+i))
				iFlipX = 1;
			else                  
				iFlipX = 0;
			
			in=1; 
			pHW=pFlagH; 
			pVW=pFlagV;
			*pFlagH=0;
			r=1;
			c=i+1;
			x0=float(i*lfDx);
			y0=0;
			lfZ=h0+i;
			scnt_x[0]=float(x0+lfDx*(iz- *lfZ)/(*(lfZ+1)- *lfZ));
			scnt_y[0]=y0;

			LocalTrack(row,col,r,c,x0,y0,lfZ, lfDx,lfDy,in,pHW,pVW,iz,iFlipX);

		}
		pFlagH++; pFlagV++;
	}
	pFlagH=pFlagH0; pFlagV=pFlagV0; lfZ=h0;
	 
	pFlagH=pFlagH0+(row-1)*col;
	pFlagV=pFlagV0+(row-1)*col;
	
	for (i=0; i<col-1; i++) 
	{
		if (*pFlagH==1) 
		{
			if (*(h0+(row-1)*col+i+1) < *(h0+(row-1)*col+i))
				iFlipX = 1;
			else 
				iFlipX = 0;
			
			in=3; pHW=pFlagH-col; pVW=pFlagV-col; *pFlagH=0;
			r=row-1; c=i+1;
			x0=float(i*lfDx);
			y0=float((row-2)*lfDy);
			lfZ=h0+(row-1)*col+i;
			scnt_x[0]=float(x0+lfDx*(iz- *lfZ)/(*(lfZ+1)- *lfZ));
			scnt_y[0]=float(y0+lfDy);
			lfZ-=col;
			LocalTrack(row,col,r,c,x0,y0,lfZ, lfDx,lfDy,in,pHW,pVW,iz,iFlipX);
			pFlagH++; pFlagV++;
		}
	}
	pFlagH=pFlagH0; pFlagV=pFlagV0; lfZ=h0;
	 
	for (i=0; i<row-1; i++) 
	{
		if (*pFlagV==1) 
		{
			if (*(h0+(i+1)*col) < *(h0+i*col)) iFlipX = 1;
			else 				   iFlipX = 0;
			
			in=4; pHW=pFlagH; pVW=pFlagV; *pFlagV=0;
			r=i+1; c=1;
			x0=0; 
			y0=float(i*lfDy);
			lfZ=h0+i*col;
			scnt_x[0]=x0;
			scnt_y[0]=float(y0+lfDy*(iz- *lfZ)/(*(lfZ+col)- *lfZ));
			LocalTrack(row,col,r,c,x0,y0,lfZ, lfDx,lfDy,in,pHW,pVW,iz,iFlipX);
			pFlagH+=col; pFlagV+=col;
		}
	}
	pFlagH=pFlagH0; 
	pFlagV=pFlagV0;
	lfZ=h0; 
	pFlagH+=col-1;
	pFlagV+=col-1;
	for (i=0; i<row-1; i++) 
	{
		if (*pFlagV==1) 
		{
			if (*(h0+(i+2)*col-1) > *(h0+(i+1)*col-1))
				iFlipX = 1;
			else 
				iFlipX = 0;
			
			in=2; pHW=pFlagH-1; pVW=pFlagV-1; *pFlagV=0;
			r=i+1; c=col-1;
			x0=float((col-2)*lfDx);
			y0=float(i*lfDy);
			lfZ=h0+(i+1)*col-1;
			scnt_x[0]=float(x0+lfDx);
			scnt_y[0]=float(y0+lfDy*(iz- *lfZ)/(*(lfZ+col)- *lfZ));
	      	lfZ--;
	        LocalTrack(row,col,r,c,x0,y0,lfZ, lfDx,lfDy,in,pHW,pVW,iz,iFlipX);
		}
		pFlagH+=col; pFlagV+=col;
	}
	pFlagH=pFlagH0; pFlagV=pFlagV0; lfZ=h0;
	
	 
	pFlagH+=col; 
	pFlagV+=col;
	for (i=1; i<row-1; i++) 
	{
		for (j=0; j<col-1; j++) {
			if (*pFlagH==1) {
				if (*(h0+i*col+j+1) > *(h0+i*col+j)) iFlipX = 1;
				else 				     iFlipX = 0;
				
				in=1; pHW=pFlagH; pVW=pFlagV;
				r=i+1; c=j+1;
				x0=float(j*lfDx); 
				y0=float(i*lfDy);
				lfZ=h0+i*col+j;
				scnt_x[0]=float(x0+lfDx*(iz- *lfZ)/(*(lfZ+1)- *lfZ));
				scnt_y[0]=y0;
				
				np=0;
				bEnd= -1;
				reverse=0;
				
				do {
					if (0<c && c<col && 0<r && r<row)
						
						w=MarkPass(iz,lfZ,  col,row,lfDx,lfDy,&in,pHW,pVW,&r,&c,
						&x0,&y0,scnt_x,scnt_y,&np,&bEnd);
					else bEnd+=1;		
					
					if (bEnd==0 && reverse==0) 
					{
						for (k=0; k<(np+1)/2; k++)
						{
							x1=scnt_x[k]; scnt_x[k]=scnt_x[np-k]; scnt_x[np-k]=x1;
							y1=scnt_y[k]; scnt_y[k]=scnt_y[np-k]; scnt_y[np-k]=y1;
						}
						
						in=3; pHW=pFlagH; pVW=pFlagV;
						r=i; c=j+1;
						lfZ=h0+i*col+j;
						x0=float(j*lfDx);
						y0=float((i-1)*lfDy);
						w= -col;
						reverse=1;
					}
					
					if (w==0 && (bEnd==-1 || bEnd==0)) 
					{
						np--; 
						break;
					}
					lfZ+=w; pHW+=w; pVW+=w;
					
					if(scnt_x[np]==scnt_x[0] && scnt_y[np]==scnt_y[0]) bEnd=1;
				}       while (bEnd<1);
				
				    if (abs(reverse-iFlipX)==1)
			           	Flip(np);
				 
						np++;
						
						if (np>7) 
						{ 
							 write_cnt( np,scnt_x,scnt_y,(float)iz );
						}
			}
			pFlagH++; pFlagV++;
		}
		pFlagH++; pFlagV++;
	}
	pFlagH=pFlagH0; pFlagV=pFlagV0; lfZ=h0;
 
}
void GridContour::SearchOneLine(float iz, float * lfZ, unsigned char * pFlagH, unsigned char * pFlagV, int row, int col)
{
	register int  i,j; 
	float      *h0; 
	float    d1,d2; 
	int      buffer; 
	
	buffer=row*col;
	h0=lfZ; 
	
	for (i=0; i<buffer; i++) { if (*lfZ==iz) (*lfZ)+=0.1; lfZ++; } 
	lfZ=h0; 
	
	for (i=0; i<row-1; i++) 
	{ 
		for (j=0; j<col-1; j++) 
		{
			if ( *lfZ > float(NULL_DEM)+1 && *(lfZ+1) > float(NULL_DEM)+1) 
			{ 
				*pFlagH=(*lfZ-iz)*(*(lfZ+1)-iz)<0?1:0;
			}
			else 
				*pFlagH=0;
			
			if ( *lfZ > float(NULL_DEM)+1 && *(lfZ+col) > float(NULL_DEM)+1) 
			{ 
				*pFlagV=(*lfZ-iz)*(*(lfZ+col)-iz)<0?1:0;
			}
			else *pFlagV = 0; 
			
			lfZ++; pFlagH++; pFlagV++; 
		} 
		
		if ( *lfZ > float(NULL_DEM)+1 && *(lfZ+col) > float(NULL_DEM)+1) 
		{
		 
			*pFlagV=(*lfZ-iz)*(*(lfZ+col)-iz)<0?1:0;
			*pFlagH =0; 
		}
		else
			*pFlagV = *pFlagH = 0; 
		lfZ++; pFlagH++; pFlagV++; 
	} 
	
	for (j=0; j<col-1; j++) {
		if ( *lfZ > float(NULL_DEM)+1 && *(lfZ+1) > float(NULL_DEM)+1) 
		{  
			*pFlagH=(*lfZ-iz)*(*(lfZ+1)-iz)<0?1:0; 
			*pFlagV=0; 
		}
		else *pFlagV = *pFlagH = 0; 
		lfZ++; pFlagH++; pFlagV++; 
	} 
	
}
void GridContour::write_cnt(int iNum,float * x,float *y,float z)
{
	CGeoCurve *pCurve = new CGeoCurve();
	if (!pCurve) return;

	CArray<PT_3DEX, PT_3DEX> arrPts;
    //等高线的光滑处理  
	for(int i= 0; i<iNum ; i++)
	{
		PT_3DEX pt(m_lfBaseX + x[i], m_lfBaseY + y[i], z, penLine);
		arrPts.Add(pt);
	} 
	if(!pCurve->CreateShape(arrPts.GetData(), arrPts.GetSize()))
	{
		delete pCurve;
		return;
	}
	contours.push_back(pCurve);
	m_iLineNum++;
}

void GridContour::SetFlags(float * Grid,unsigned char *HFlags,unsigned char *VFlags,short Row,short Col,double Height)
{	
	
  unsigned char  *HFlags10,*VFlags10,*HFlags0,*VFlags0;
  register int   i,j,k;
  float            *Grid0,*Grid10;

  HFlags0=HFlags;  VFlags0=VFlags;  Grid0=Grid;

  for (i=0; i<Row-1; i++) {
      for (j=0; j<Col-1; j++) {
	  if ((*Grid>Height && *(Grid+1)<Height) || 
              (*(Grid+1)>Height && *Grid<Height))
	  {
		    if( (*Grid)<(NULL_DEM+1)&&( (*(Grid+1))<(NULL_DEM+1)))
		  	  *HFlags=0;
		    else
		  	  *HFlags=1;
	  }
 	  else *HFlags=0;

	  if ((*Grid>Height && *(Grid+Col)<Height) ||
              (*(Grid+Col)>Height && *Grid<Height)) 
	  {
               if( (*Grid)<(NULL_DEM+1)&&( (*(Grid+Col))<(NULL_DEM+1)))
		 	   *VFlags=0;
		    else
                *VFlags=1;

	  }
	  else *VFlags=0;

	  Grid++; HFlags++; VFlags++;
      }
      if ((*Grid>Height && *(Grid+Col)<Height) || 
          (*(Grid+Col)>Height && *Grid<Height)) 
	  {
            if( (*Grid)<(NULL_DEM+1)&&( (*(Grid+Col))<(NULL_DEM+1)))
		    *VFlags=0;
		     else
		  	*VFlags=1;   

	  }
      else *VFlags=0;
	
      Grid++; HFlags++; VFlags++;

  }

  for (j=0; j<Col-1; j++)  {
      if ((*Grid>Height && *(Grid+1)<Height) || 
          (*(Grid+1)>Height && *Grid<Height))  
	  {
	      if( (*Grid)<(NULL_DEM+1)&&( (*(Grid+1))<(NULL_DEM+1)))
		    *HFlags=0;
		   else
            *HFlags=1;

	  }
      else *HFlags=0;
	 
      Grid++; HFlags++;
  }
     
  Grid=Grid0;  HFlags=HFlags0;   VFlags=VFlags0;   
}

void GridContour::TraceContours (float * Grid,unsigned char *HFlags,unsigned char *VFlags,int Row,int Col,double lfZ,int Grid_DX,int Grid_DY ,int * i0,int * j0,int * end,int * np,float * x,float *y)
{   unsigned char  *HFlags10,*VFlags10,*HFlags0,*VFlags0;
  register int   i,j,k;
  float		     *Grid0,*Grid10;
  int          ilb,jlb,ilb0,jlb0,nct;
  int          w,x0,y0,x00,y00,in0,in,pas,first,d1,d2,z1,z2,z3,z4;
  int          prt,z4z;

  k      =0;
  *end   =0;
  Grid0  =Grid;
  HFlags0=HFlags;
  VFlags0=VFlags;

  *np=0;
  for (i= *i0; i<Row-1; i++) {
      Grid10  = Grid0  + i*Col;       
      HFlags10= HFlags0+ i*Col;
      VFlags10= VFlags0+ i*Col;

      for (j= *j0; j<Col-1; j++) {
          Grid  =Grid10  +j;
          HFlags=HFlags10+j;
          VFlags=VFlags10+j;
          first=1;

          if (*HFlags==1) {
             in0=in=1;  

	     /* the start point cooordinates */
             *x=x00=x0=j*Grid_DX+(*Grid - lfZ)*Grid_DX/( *Grid - *(Grid+1));
             *y=y00=y0=i*Grid_DY; 
            
             ilb0=ilb=i; jlb0=jlb=j;

             do {    /* search next point */
                *np+=1; pas = 0;
			 
				    
                z4z = (*(Grid+1) + *(Grid+Col) + *Grid + *(Grid+Col+1))/4;

                switch (in) {
                       case 1:     
                              if (*(VFlags+1)==1) {
       				 pas=2;  jlb+=1;
                                 *(x+ *np)= jlb*Grid_DX;
                                 *(y+ *np)= ilb*Grid_DY+(lfZ- *(Grid+1))*Grid_DY/( *(Grid+1+Col) - *(Grid+1));
                              }
                              else if(*(HFlags+Col)==1) {
                                 pas=3; ilb+=1;
                                 *(y+ *np)= ilb*Grid_DY;
                                 *(x+ *np)= jlb*Grid_DX+(lfZ- *(Grid+Col))*Grid_DX/( *(Grid+1+Col)- *(Grid+Col));
                              }
                              else  if (*VFlags==1) {         
                                 *(x+ *np)= jlb*Grid_DX;
                                 *(y+ *np)= ilb*Grid_DY + (lfZ- *Grid)*Grid_DY/(*(Grid+Col)-  *Grid);
                                 pas=4; jlb-=1;
                              }    /* exit line == 4 */

                              break;
                       case 2:    /* entrance No. is 2 */
                             if (*(HFlags+Col)==1) {
                                pas=3; ilb+=1;
                                *(x+ *np)=jlb*Grid_DX+(lfZ- *(Grid+Col))*Grid_DX/(*(Grid+1+Col)- *(Grid+Col));
                                *(y+ *np)=ilb*Grid_DY;
                             }
                             else if(*VFlags==1) {
                                pas=4;
                                *(x+ *np)= jlb*Grid_DX; jlb-=1;
                                *(y+ *np)= ilb*Grid_DY+ (lfZ- *Grid)*Grid_DY/(*(Grid+Col)- *Grid);
                             }
                             else if (*HFlags==1) {
                                pas=1;
                                *(x+ *np)= jlb*Grid_DX+(lfZ- *Grid)*Grid_DX/(*(Grid+1)- *Grid);
                                *(y+ *np)= ilb*Grid_DY;  ilb-=1;
                             }

                             break;
                      case 3:     
                             if (*VFlags==1) {
                                pas=4;
                                *(x+ *np)= jlb*Grid_DX; jlb-=1;
                                *(y+ *np)= ilb*Grid_DY+(lfZ- *Grid)*Grid_DY/(*(Grid+Col)- *Grid);
                             }
                             else if(*HFlags==1) {
                                pas=1;
                                *(x+ *np)= jlb*Grid_DX+(lfZ- *Grid)*Grid_DX/(*(Grid+1)- *Grid);
                                *(y+ *np)= ilb*Grid_DY;  ilb-=1;
                             }
                             else  if (*(VFlags+1)==1) {
                                pas=2;  jlb+=1;
                                *(x+ *np)= jlb*Grid_DX;
                                *(y+ *np)= ilb*Grid_DY+(lfZ- *(Grid+1))*Grid_DY/( *(Grid+1+Col) - *(Grid+1));
                             }

                             break;
                      case 4:    
                             if (*HFlags==1) {
                                pas=1;
                                *(x+ *np)= jlb*Grid_DX+(lfZ- *Grid)*Grid_DX/(*(Grid+1)- *Grid);
                                *(y+ *np)= ilb*Grid_DY;  ilb-=1;
                             }
                             else if(*(VFlags+1)==1) {
                                pas=2;  jlb+=1;
                                *(x+ *np)= jlb*Grid_DX;
                                *(y+ *np)= ilb*Grid_DY+(lfZ- *(Grid+1))*Grid_DY/( *(Grid+1+Col) - *(Grid+1));
                             }
                             else if (*(HFlags+Col)==1) {
                                pas=3; ilb+=1;
                                *(x+ *np)=jlb*Grid_DX+(lfZ- *(Grid+Col))*Grid_DX/(*(Grid+1+Col)- *(Grid+Col));
                                *(y+ *np)=ilb*Grid_DY;
                             }

                             break;
                      default:
                             printf(" in= %d\n",in);
                             pas=0; 

                             break;
				}   

               switch (pas) {
                      case 1:    
                             *HFlags=0;  in=3;  w= -Col;
                             break;

                      case 2:    
                             *(VFlags+1)=0;    in=4; w=1;
                             break;

                      case 3:   
                             *(HFlags+Col)=0;  in=1; w=Col;
                             break;

                      case 4:    
                             *VFlags=0;    in=2; w= -1;
                             break;

                      default :
                             w = 0; 
                             break; 
               }    
               if( w== 0 )  
				   break;   
               Grid+=w; HFlags+=w; VFlags+=w;  

            } while (ilb0!= ilb || jlb0!=jlb);

            *i0=i; *j0=j+1;  

			if(w!=0)
			{
			 	*np+=1; 
				*np=0;
               *(x+ *np)=*x;
			   *(y+ *np)=*y;
			}  

            if (*j0==Col-1) {*j0=0; *i0+=1;}
            return;
         }      
     }       
     *j0=0;
  }         
  *end=1;  
}
