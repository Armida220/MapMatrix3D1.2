
#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include "SmartViewFunctions.h"

const double THIS_PI=3.141593;
#define   INTPAPI_SUCESS  0
#define   INTPAPI_INVALIDE_PARAM  1
#define   INTPAPI_ERROR  2
int GetAngle(double x1,double y1,double x2, double y2,int retFlag, double* k,double* angle)
{
	if( (retFlag & 0x01) && k!=NULL )
	{
		if( fabs(x1-x2)<=1e-6 && fabs(y1-y2)<=1e-6 )return 0;			
		else if( fabs(x1-x2)>1e-6 )*k=(y1-y2)/(x1-x2);
		else *k=0xffffff;			
	}
	if( (retFlag & 0x02) && angle!=NULL )
	{
		double tmpK=0;
		if( (retFlag&0x01)==0 )
		{
			if( fabs(x1-x2)<=1e-6 && fabs(y1-y2)<=1e-6 )return 0;		
			else if( fabs(x1-x2)>1e-6 )tmpK=(y1-y2)/(x1-x2);
			else tmpK=0xffffff;	
		}
		else tmpK = *k;
		*angle=atan(tmpK);
		if( x2>=x1 && y2>=y1 );//0-90
		else if( x2>=x1 && y2< y1 )*angle=*angle+2*THIS_PI;//270-360
		else if( x2< x1 && y2>=y1 )*angle=*angle+THIS_PI;//90-180
		else if( x2< x1 && y2< y1 )*angle=*angle+THIS_PI;//180-270
	}
	return 1;
}
int Intersect(double x0,double y0,double x1,double y1,double x2,double y2,double x3,double y3,double *px,double *py)
{
	double delta_k,t1,t2;
	if(fabs(x0-x1) < 1e-6 && fabs(y0-y1) < 1e-6)return 0;
	delta_k = (y3-y2)*(x1-x0)-(y1-y0)*(x3-x2);
	if( fabs(delta_k)< 1e-6)return 0;
	t2 = ( (y1-y0)*(x2-x0)-(y2-y0)*(x1-x0) )/delta_k;
	if( t2>=1 || t2<=0 )return 0;
	if( fabs(x0-x1) < 1e-6 )
		t1 = ( y2-y0 + (y3-y2)*t2 )/(y1-y0);
	else
		t1 = ( x2-x0 + (x3-x2)*t2 )/(x1-x0);
	if( t1>=1 || t1<=0 )return 0;
	*px = x0 + (x1-x0)*t1;
	*py = y0 + (y1-y0)*t1;
	return 1;
}
int  IsIntersectant(PT_3D* pt1, PT_3D* pt2, PT_3D* ptArray,int sum)
{
	double x,y;
	PT_3D* tmpPt=ptArray;
	if(tmpPt==NULL || sum<2 )return 0;
	for(;tmpPt<ptArray+sum-1;tmpPt+=1)
	{
		if( Intersect(pt1->x,pt1->y,pt2->x,pt2->y,
			tmpPt->x,tmpPt->y,(tmpPt+1)->x,(tmpPt+1)->y,&x,&y)== 1)
			break;
	}
	if( tmpPt>=ptArray+sum-1 )return 0;
	else return 1;
}
int  FixIntersectPoints( PT_3D* ptArray1,int idx,PT_3D* ptArray2,
								int* pt2No,PT_3D* ptArray,int sum1,int sum2, double scale )
{
	int count=0;
	int i=0,order=0, i1=0, i2=0;
	int* tmpPtNo=NULL;
	PT_3D* pt=NULL;
	if( idx!=0 && idx!=sum1-1 )return count;
	for( i=0, tmpPtNo=pt2No,order=0; i<sum1-1 && tmpPtNo<pt2No+sum1-1; i++)
	{
		int* tmpNo = NULL;
		if( *tmpPtNo==-1 )continue;
		tmpNo = tmpPtNo+1;
		while( *tmpNo==-1 && tmpNo<=pt2No+sum1-1 )tmpNo++;
		if( tmpNo>pt2No+sum1-1 )break;
		if( *tmpPtNo<*tmpNo )order++;
		else if( *tmpPtNo>*tmpNo )order--;
		tmpPtNo=tmpNo; 
	}
	if( ptArray1[0].x==ptArray1[sum1-1].x && 
		ptArray1[0].y==ptArray1[sum1-1].y )return count;
	if( ptArray2[0].x==ptArray2[sum2-1].x && 
		ptArray2[0].y==ptArray2[sum2-1].y )return count;
	if( (fabs(ptArray1[0].x-ptArray1[sum1-1].x)+fabs(ptArray1[0].y-ptArray1[sum1-1].y))/
		(fabs(ptArray1[0].x)+fabs(ptArray1[sum1-1].x)+fabs(ptArray1[0].y)+fabs(ptArray1[sum1-1].y))
		<1e-6 )
		return count;
	if( (fabs(ptArray2[0].x-ptArray2[sum2-1].x)+fabs(ptArray2[0].y-ptArray2[sum2-1].y))/
		(fabs(ptArray2[0].x)+fabs(ptArray2[sum2-1].x)+fabs(ptArray2[0].y)+fabs(ptArray2[sum2-1].y))
		<1e-6 )
		return count;
	tmpPtNo=pt2No;
	pt=ptArray;
	if( order>0 )
	{
		tmpPtNo=pt2No;
		while( tmpPtNo-pt2No<sum1-1 ) 
		{
			if( tmpPtNo[0]!=-1 && tmpPtNo[1]!=-1 && tmpPtNo[1]>tmpPtNo[0] )
			{
				i1 = tmpPtNo[0];
				break;
			}
			tmpPtNo++;
		}
		tmpPtNo=pt2No+sum1-2;
		while( tmpPtNo-pt2No>=0 ) 
		{
			if( tmpPtNo[0]!=-1 && tmpPtNo[1]!=-1 && tmpPtNo[1]>tmpPtNo[0] )
			{
				i2 = tmpPtNo[0];
				break;
			}
			tmpPtNo--;
		}
	}
	else
	{
		tmpPtNo=pt2No;
		while( tmpPtNo-pt2No<sum1-1 ) 
		{
			if( tmpPtNo[0]!=-1 && tmpPtNo[1]!=-1 && tmpPtNo[1]<tmpPtNo[0] )
			{
				i1 = tmpPtNo[0];
				break;
			}
			tmpPtNo++;
		}
		tmpPtNo=pt2No+sum1-2;
		while( tmpPtNo-pt2No>=0 ) 
		{
			if( tmpPtNo[0]!=-1 && tmpPtNo[1]!=-1 && tmpPtNo[1]<tmpPtNo[0] )
			{
				i2 = tmpPtNo[0];
				break;
			}
			tmpPtNo--;
		}
	}
	tmpPtNo=pt2No;
	if( idx==0 )
	{
		if( order>0 )
		{
			int min = -1;
			while( tmpPtNo-pt2No<sum1-1 ) 
			{
				if( tmpPtNo[0]!=-1 && tmpPtNo[1]!=-1 && tmpPtNo[1]>tmpPtNo[0] )
				{
					if( min<0 || min>tmpPtNo[0] )min = tmpPtNo[0];
				}
				tmpPtNo++;
			}
			if( min<i1 || min>i2 )return count;
			for(i=0; i<min; i++,ptArray2++)
			{				
				pt->x = ptArray1->x*scale + ptArray2->x*(1-scale);
				pt->y = ptArray1->y*scale + ptArray2->y*(1-scale);
				pt->z = ptArray1->z*scale + ptArray2->z*(1-scale);
				count++,pt++;
			}
		}
		else if( order<0 )
		{
			int max = -1;
			while( tmpPtNo-pt2No<sum1-1 ) 
			{
				if( tmpPtNo[0]!=-1 && tmpPtNo[1]!=-1 && tmpPtNo[1]<tmpPtNo[0] )
				{
					if( max<0 || max<tmpPtNo[0] )max = tmpPtNo[0];
				}
				tmpPtNo++;
			}
			if( max<0 )return count;
			if( max<i1 || max>i2 )return count;
			ptArray2 += (sum2-1);
			for(i=sum2-1; i>max; i--,ptArray2--)
			{				
				pt->x = ptArray1->x*scale + ptArray2->x*(1-scale);
				pt->y = ptArray1->y*scale + ptArray2->y*(1-scale);
				pt->z = ptArray1->z*scale + ptArray2->z*(1-scale);
				count++,pt++;
			}
		}
	}
	else if( idx==sum1-1 )
	{
		if( order>0 )
		{	
			int max = -1;
			while( tmpPtNo-pt2No<sum1-1 ) 
			{
				if( tmpPtNo[0]!=-1 && tmpPtNo[1]!=-1 && tmpPtNo[1]>tmpPtNo[0] )
				{
					if( max<0 || max<tmpPtNo[1] )max = tmpPtNo[1];
				}
				tmpPtNo++;
			}
			if( max<0 )return count;
			if( max<i1 || max>i2 )return count;
			ptArray1 += (sum1-1);
			ptArray2 += max;
			for(i=max+1; i<sum2; i++,ptArray2++)
			{				
				pt->x = ptArray1->x*scale + ptArray2->x*(1-scale);
				pt->y = ptArray1->y*scale + ptArray2->y*(1-scale);
				pt->z = ptArray1->z*scale + ptArray2->z*(1-scale);
				count++,pt++;
			}
		}
		else if( order<0 )
		{		
			int min = -1;
			while( tmpPtNo-pt2No<sum1-1 ) 
			{
				if( tmpPtNo[0]!=-1 && tmpPtNo[1]!=-1 && tmpPtNo[1]<tmpPtNo[0] )
				{
					if( min<0 || min>tmpPtNo[1] )min = tmpPtNo[0];
				}
				tmpPtNo++;
			}
			if( min<i1 || min>i2 )return count;
			ptArray1 += (sum1-1);
			ptArray2 += min;	
			for(i=min-1; i>=0; i--,ptArray2--)
			{				
				pt->x = ptArray1->x*scale + ptArray2->x*(1-scale);
				pt->y = ptArray1->y*scale + ptArray2->y*(1-scale);
				pt->z = ptArray1->z*scale + ptArray2->z*(1-scale);
				count++,pt++;
			}
		}
	}
	return count;
}

static void GetInsertPoints(PT_3D* ptArray1,PT_3D* ptArray2,int sum,
						PT_3D* ptArray ,int* pSum, float scale)
{
	if(ptArray1==NULL || ptArray2==NULL || ptArray==NULL || sum<=0 || scale<=0||pSum==NULL)
		return ;
	*pSum=sum; 
	//double z=(ptArray2[0].z-ptArray1[0].z)*scale+ptArray1[0].z;
	for(int i=0;i<sum;i++)
	{
		ptArray[i].x=(ptArray2[i].x-ptArray1[i].x)*scale+ptArray1[i].x;
		ptArray[i].y=(ptArray2[i].y-ptArray1[i].y)*scale+ptArray1[i].y;
		ptArray[i].z=(ptArray2[i].z-ptArray1[i].z)*scale+ptArray1[i].z;
		//ptArray[i].z=z;
	}

}

static void GetEqualPoint(PT_3D * ptArray1,int sum1, PT_3D * ptArray2,int sum2)
{
	ASSERT(ptArray2);
	ptArray2[0]=ptArray1[0];
	ptArray2[sum2-1]=ptArray1[sum1-1];
	//double z = ptArray1[0].z;
	double alllen = GraphAPI::GGetAllLen3D(ptArray1,sum1);
	double len = alllen/(sum2-1);
	int n=0;
	
	double temp=0;
	int j = 2;
	for (int i=1;i<sum2-1;i++)
	{
		n++;		
		for(;(temp=GraphAPI::GGetAllLen3D(ptArray1,j))<len*n;j++);
		
		double tem1=temp-len*n;
		tem1=tem1/GraphAPI::GGet3DDisOf2P(ptArray1[j-1],ptArray1[j-2]);
		double x = (ptArray1[j-2].x-ptArray1[j-1].x)*tem1+ptArray1[j-1].x;
		double y = (ptArray1[j-2].y-ptArray1[j-1].y)*tem1+ptArray1[j-1].y;
		double z = (ptArray1[j-2].z-ptArray1[j-1].z)*tem1+ptArray1[j-1].z;
		ptArray2[i].x = x;
		ptArray2[i].y = y;
		ptArray2[i].z = z;

	}
	
}

int InterpolateContour(PT_3D * ptArray1,int sum1, PT_3D * ptArray2,int sum2,
						 PT_3D * ptArray ,int* pSum, int nIntNum, int nPart)
{
	int            i=0;
	int            newNum;
	float          scale;
	//PT_3D        *array1=NULL,*array2=NULL;
    PT_3D        *newElPt=NULL,*tmpPt=NULL;
	
	if(ptArray1==NULL || ptArray2==NULL ||ptArray==NULL|| sum1<=0 || sum2<=0 || nIntNum<=0)
		return 1;
	
	int num=(sum1+sum2)/2;
	PT_3D *pts1 = new PT_3D[num];
	PT_3D *pts2 = new PT_3D[num];
	if( !pts1||!pts2) return -1;
	
	GetEqualPoint(ptArray1,sum1,pts1,num);
	GetEqualPoint(ptArray2,sum2,pts2,num);

	// 整体内插时候需要 调整两段线的首尾点 2点线段无法处理
	if (0 == nPart)
	{
		double dis1 = GraphAPI::GGet2DDisOf2P(pts1[0], pts2[0]);
		double dis2 = GraphAPI::GGet2DDisOf2P(pts1[0], pts2[num-1]);
		if(dis1>dis2)
		{
			for (i=0; i<num/2; i++)
			{
				PT_3D t = pts2[i];
				pts2[i] = pts2[num-i-1];
				pts2[num-i-1] = t;
			}
		}
	}

	//array1	= ptArray1;
	//array2	= ptArray2;
	newElPt = ptArray;
	scale   = ((float)1)/(nIntNum+1);
	for ( i=1; i<nIntNum+1; i++)
	{		
		//scale = ((float)1)/(nIntNum+1);
		
		GetInsertPoints( pts1,pts2,num,newElPt,&newNum,i*scale );		
		*pSum	= newNum;
		newElPt = newElPt+*pSum;
		pSum++;
	}

	if (pts1)
	{
		delete []pts1;
	}
	if (pts2)
	{
		delete []pts2;
	}
    return 0;

}

int InterpolateContour1(PT_3D * ptArray1, int sum1, PT_3D * ptArray2, int sum2,
	PT_3D * ptArray, int* pSum, double lfDis, int nPart)
{
	int            i = 0;
	int            newNum;
	float          scale;
	//PT_3D        *array1=NULL,*array2=NULL;
	PT_3D        *newElPt = NULL, *tmpPt = NULL;

	if (ptArray1 == NULL || ptArray2 == NULL || ptArray == NULL || sum1 <= 0 || sum2 <= 0)
		return 1;

	int num = (sum1 + sum2) / 2;
	PT_3D *pts1 = new PT_3D[num];
	PT_3D *pts2 = new PT_3D[num];
	if (!pts1 || !pts2) return -1;

	GetEqualPoint(ptArray1, sum1, pts1, num);
	GetEqualPoint(ptArray2, sum2, pts2, num);

	// 整体内插时候需要 调整两段线的首尾点 2点线段无法处理
	if (0 == nPart)
	{
		double dis1 = GraphAPI::GGet2DDisOf2P(pts1[0], pts2[0]);
		double dis2 = GraphAPI::GGet2DDisOf2P(pts1[0], pts2[num - 1]);
		if (dis1 > dis2)
		{
			for (i = 0; i < num / 2; i++)
			{
				PT_3D t = pts2[i];
				pts2[i] = pts2[num - i - 1];
				pts2[num - i - 1] = t;
			}
		}
	}

	double z1 = pts1[0].z;
	double z2 = pts2[0].z;
	double z3 = min(z1, z2);
	double z4 = max(z1, z2);
	newElPt = ptArray;

	double curZ = z3 + lfDis;
	int curZ1 = (int)(curZ * 2 + 0.25);//四舍五入
	curZ = curZ1 / 2.0;
	while (curZ<z4)
	{
		scale = (curZ - z1) / (z2 - z1);

		GetInsertPoints(pts1, pts2, num, newElPt, &newNum, scale);
		*pSum = newNum;
		newElPt = newElPt + *pSum;
		pSum++;
		curZ += lfDis;
	}

	if (pts1)
	{
		delete[]pts1;
	}
	if (pts2)
	{
		delete[]pts2;
	}
	return 0;

}


void GetInsertPoints1(PT_3D* ptArray1,int sum1,PT_3D* ptArray2,int sum2,
						PT_3D* ptArray ,int* pSum, float scale, int range,int optimize)
{	
	int i,j;
	int insertNum=0;
	PT_3D    *tmpPt1=NULL,*tmpPt2=NULL;
    PT_3D    *newElPt=NULL,*tmpPt=NULL;
	int *newPtNo=NULL, *tmpPtNo=NULL, lastNo=-1;
	int order=0;
	double oldDis, *scaleArray=NULL,*disArray=NULL,*tmpDis=NULL;
	double k1,k2,curAngle;
	double adjustScale;
	newPtNo = (int*)malloc((sum1+sum2)*sizeof(int));
	if( newPtNo==NULL )
	{		
		return;
	}
	memset(newPtNo,0,(sum1+sum2)*sizeof(int));
	disArray = (double*)malloc(sum2*sizeof(double));
	if( disArray==NULL )
	{		
		if( newPtNo!=NULL )free(newPtNo);
		return;
	}
	memset(disArray,0,sum2*sizeof(double));
	scaleArray = (double*)malloc(sum1*sizeof(double));
	if( scaleArray==NULL )
	{	
		if( newPtNo!=NULL )free(newPtNo);
		if( disArray!=NULL )free(disArray);
		return;
	}
	memset(scaleArray,0,sum1*sizeof(double));
    for ( i=0,tmpPt1 = ptArray1,tmpPtNo=newPtNo; i<sum1; i++,tmpPt1++,tmpPtNo++ )
    {	
		double oldAngle=-1,dis;			
		int	   count=0,m=0,n=0,ret;
		int    start1=0, end1=0, len1=0, start2=0, end2=0, len2=0 ;
		oldDis=-1;	
		if( i<sum1-1 )
			ret= GetAngle( tmpPt1->x,tmpPt1->y,(tmpPt1+1)->x,(tmpPt1+1)->y,0x01,&k1,NULL );				
		else
			ret= GetAngle( (tmpPt1-1)->x,(tmpPt1-1)->y,tmpPt1->x,tmpPt1->y,0x01,&k1,NULL );		
		if( range<=0 )range=2;
		if( range>sum2 )range = sum2;		
		if( sum2>range )
		{
			if( i==0 || (i>0 && *(tmpPtNo-1)==-1) )
			{ 
				start1=0; end1=sum2; len1=sum2;
				start2=0; end2=0;	 len2=0;
			}
			else 
			{ 				
				start1=*(tmpPtNo-1)-range/2; end1=*(tmpPtNo-1)+range/2; len1=end1-start1;
				start2=0; end2=0;	 len2=0;
				if( start1<0 )
				{
					start2=0;			end2 = end1; len2 = end2-start2;
					start1=start1+sum2; end1 = sum2; len1 = end1-start1;
				}
				else if( end1>sum2 )
				{
					start2=start1;		end2 = sum2; len2 = end2-start2;
					start1=0;	   end1 = end1-sum2; len1 = end1-start1;
				}				
			}
		}
		else
		{
			start1=0; end1=sum2; len1=end1-start1;
			start2=0; end2=0;	 len2=0;
		}
		for( j=start1,tmpPt2 = ptArray2+start1,tmpDis=disArray; j<end1; j++,tmpPt2++,tmpDis++)
		{
			*tmpDis = (tmpPt1->x-tmpPt2->x)*(tmpPt1->x-tmpPt2->x)+
					  (tmpPt1->y-tmpPt2->y)*(tmpPt1->y-tmpPt2->y);
			*tmpPtNo++ = j;
		}		
		for( j=start2,tmpPt2 = ptArray2+start2; j<end2; j++,tmpPt2++,tmpDis++)
		{
			*tmpDis = (tmpPt1->x-tmpPt2->x)*(tmpPt1->x-tmpPt2->x)+
					  (tmpPt1->y-tmpPt2->y)*(tmpPt1->y-tmpPt2->y);
			*tmpPtNo++ = j;
		}		
		for( j=0,tmpPtNo =newPtNo+i,tmpDis=disArray; j<len1+len2; j++ )
		{		
			n=j;
			for( m=n+1; m<len1+len2; m++ )			
				if( tmpDis[n]>tmpDis[m] )n=m;			
			dis = tmpDis[j];
			tmpDis[j]=tmpDis[n];
			tmpDis[n]=dis;
			m   = tmpPtNo[j];
			tmpPtNo[j]=tmpPtNo[n];
			tmpPtNo[n]=m;			
		}	
		memset(tmpPtNo+1,0,sum2*sizeof(int));		
	}
	if( optimize==1 )
	{	
		for( i=0, tmpPtNo=newPtNo,order=0; i<sum1-1 && tmpPtNo<newPtNo+sum1-1; i++)
		{
			int* tmpNo = NULL;
			if( *tmpPtNo==-1 )continue;
			tmpNo = tmpPtNo+1;
			while( *tmpNo==-1 && tmpNo<=newPtNo+sum1-1 )tmpNo++;
			if( tmpNo>newPtNo+sum1-1 )break;
			if( *tmpPtNo<*tmpNo )order++;
			else if( *tmpPtNo>*tmpNo )order--;
			tmpPtNo=tmpNo; 
		}
		for( i=0, tmpPtNo=newPtNo; i<sum1-1 && tmpPtNo<newPtNo+sum1-1; i++)
		{
			int* tmpNo = NULL;
			while( *tmpPtNo==-1 && tmpPtNo<newPtNo+sum1-1 )tmpPtNo++;
			if( tmpPtNo>=newPtNo+sum1-1 )break;
			tmpNo = tmpPtNo+1;
			while( *tmpNo==-1 && tmpNo<=newPtNo+sum1-1 )tmpNo++;
			if( tmpNo>newPtNo+sum1-1 )break;
			if( order>0 )
			{
				if( *tmpPtNo>*tmpNo && *tmpPtNo-*tmpNo<sum1*0.6 )
				{
					if((*tmpNo)!=sum2-1)(*tmpNo)++;
					(*tmpPtNo)--;
				}
			}
			else
			{
				if( *tmpPtNo<=*tmpNo && *tmpNo-*tmpPtNo<sum1*0.6 )
				{
					(*tmpNo)--;
					if((*tmpPtNo)!=sum2-1)(*tmpPtNo)++;
				}
			}		
			tmpPtNo=tmpNo; 
		}
	}
	*pSum=0;
	for ( i=0,tmpPtNo=newPtNo,tmpPt1=ptArray1,tmpPt = ptArray; i<sum1;
		  i++,tmpPtNo++,tmpPt1++)
	{	
		adjustScale = scale;
		if( i==0 || i==sum1-1 )
		{
			int count=0;
			if( i==0 )
			{
				count = FixIntersectPoints( ptArray1,i,ptArray2,newPtNo,tmpPt,sum1,sum2,adjustScale );
				tmpPt += count;
				*pSum += count;
			}
			else if( i==sum1-1 )
			{
				if( *tmpPtNo==-1 )
				count = FixIntersectPoints( ptArray1,i,ptArray2,newPtNo,tmpPt,sum1,sum2,adjustScale );
				else
				count = FixIntersectPoints( ptArray1,i,ptArray2,newPtNo,tmpPt+1,sum1,sum2,adjustScale );
				*pSum += count;
			}
		}
		if( *tmpPtNo==-1 )continue;
		if( optimize==1 && lastNo>0 )
		{
			tmpPt1--;
			if( order>0 && *tmpPtNo-lastNo>1 && *tmpPtNo-lastNo<sum2/2 )
			{
				for( j=lastNo+1; j<*tmpPtNo; j++ )
				{
					tmpPt2=ptArray2 + j;
					tmpPt->x = tmpPt1->x*adjustScale + tmpPt2->x*(1-adjustScale);
					tmpPt->y = tmpPt1->y*adjustScale + tmpPt2->y*(1-adjustScale);
					tmpPt->z = tmpPt1->z*scale       + tmpPt2->z*(1-scale);
					(*pSum)++,tmpPt++;
				}
			}
			else if( order<0 && *tmpPtNo-lastNo<-1 && *tmpPtNo-lastNo>-sum2/2 )
			{
				for( j=lastNo-1; j>*tmpPtNo; j-- )
				{
					tmpPt2=ptArray2 + j;
					tmpPt->x = tmpPt1->x*adjustScale + tmpPt2->x*(1-adjustScale);
					tmpPt->y = tmpPt1->y*adjustScale + tmpPt2->y*(1-adjustScale);
					tmpPt->z = tmpPt1->z*scale       + tmpPt2->z*(1-scale);
					(*pSum)++,tmpPt++;
				}
			}
			tmpPt1++;
		}
		tmpPt2=ptArray2 + *tmpPtNo;
		tmpPt->x = tmpPt1->x*adjustScale + tmpPt2->x*(1-adjustScale);
		tmpPt->y = tmpPt1->y*adjustScale + tmpPt2->y*(1-adjustScale);
		tmpPt->z = tmpPt1->z*scale       + tmpPt2->z*(1-scale);
		(*pSum)++,tmpPt++;
		lastNo = *tmpPtNo;
	}
	if( newPtNo!=NULL )free(newPtNo);
	if( disArray!=NULL )free(disArray);
	if( scaleArray!=NULL )free(scaleArray);
    return;
}
int Interpolate1(PT_3D * ptArray1,int sum1, PT_3D * ptArray2,int sum2,
						 PT_3D * ptArray ,int* pSum, int nScale, int scanRange, int optimize,float tolerance)
{
	int            i=0;
	int            sum,curNum,newNum;
	float          scale;
	PT_3D        *array1=NULL,*array2=NULL;
    PT_3D        *newElPt=NULL,*tmpPt=NULL;
	if(ptArray1==NULL || ptArray2==NULL || sum1<=0 || sum2<=0 || nScale<=0)
		return INTPAPI_INVALIDE_PARAM;
	if(ptArray==NULL)
	{
		*pSum = (int)(nScale*sum1+nScale*(nScale+1)*0.5*sum2);
		return  INTPAPI_SUCESS;
	}
	if(scanRange<=0) scanRange = 20;
	if(optimize<0 || optimize>1) optimize = 0;
	if(tolerance<=0 ) tolerance = 0.5;
	array1	= ptArray1;
	array2	= ptArray2;
	newElPt = ptArray;
	scale   = ((float)1)/(nScale+1);
	curNum  = sum1;
	for ( i=nScale; i>0; i--)
	{		
		scale = ((float)1)/(nScale+1);
		sum = curNum;			
		GetInsertPoints1( array1,sum,array2,sum2,newElPt,&newNum,i*scale,scanRange, optimize );		
		*pSum	= newNum;
		newElPt = newElPt+*pSum;
		pSum++;
	}	
    return INTPAPI_SUCESS;
}