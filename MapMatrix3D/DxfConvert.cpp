// DxfConvert.cpp: implementation of the CDxfConvert class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "editbase.h"
#include "DxfConvert.h"
#include "license.h"
#include "dsm.h"
 
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDxfConvert::CDxfConvert()
{
	m_strInputFile = _T("");
	m_dXLL = 0.0;
	m_dXUR = 0.0;
	m_dZMin = 0.0;
	m_dYLL = 0.0;
	m_dYUR = 0.0;
	m_dZMax = 0.0;
	m_bNotUsed = FALSE;
	m_bSpotHeight = FALSE;
	m_bBreakLine = FALSE;
	m_bBoundary = FALSE;
	m_strOutputFile = _T("");
	m_dXLL2 = -999999;
	m_dXUR2 = 999999;
	m_dXSpace = 30.0;
	m_dYLL2 = -999999;
	m_dYUR2 = 999999;
	m_dYSpace = 30.0;
	m_LayerAttrib = 2;
	m_demRotationAngle = 0.0;
	bRunMode=1; 
 	tinLayer=NULL;
}

CDxfConvert::~CDxfConvert()
{

}  

BOOL CDxfConvert::setDemParam(KTExtents *ext, KTDEMParam *demParam)	
{		
	double d_tmp, Xur, Yur; 
	strcpy(demInfo->outputFile,m_strOutputFile);  
	demInfo->Xll=floor(demInfo->Xll/m_dXSpace)*m_dXSpace; 
	demInfo->Yll=floor(demInfo->Yll/m_dYSpace)*m_dYSpace; 
	demInfo->Xur=floor(demInfo->Xur/m_dXSpace)*m_dXSpace; 
	demInfo->Yur=floor(demInfo->Yur/m_dYSpace)*m_dYSpace; 

	if(demInfo->Xll>demInfo->Xur||demInfo->Yll>demInfo->Yur)
		return FALSE;
	demInfo->rotationAngle=m_demRotationAngle;
	demInfo->dx=(float)m_dXSpace;
	demInfo->dy=(float)m_dYSpace;

	demParam->cornerX = demInfo->Xll;
	demParam->cornerY = demInfo->Yll;
	
	demParam->rotAngle = demInfo->rotationAngle;
	
	demParam->cellW = demInfo->dx;
	demParam->cellH = demInfo->dy;

 	Xur=demInfo->Xur;
	Yur=demInfo->Yur;
	
	d_tmp  = (Xur-demParam->cornerX)*cos(demInfo->rotationAngle);
	d_tmp += (Yur-demParam->cornerY)*sin(demInfo->rotationAngle);	
	demParam->nrColumns = (int)floor(d_tmp/demInfo->dx + 0.05) + 1;
	
	d_tmp  = (Yur-demParam->cornerY)*cos(demInfo->rotationAngle);
	d_tmp -= (Xur-demParam->cornerX)*sin(demInfo->rotationAngle);		
	demParam->nrRows = (int)floor(d_tmp/demInfo->dy + 0.05) + 1;

	if(demParam->nrColumns<=0||demParam->nrRows<=0)
		return FALSE;
	return TRUE;
}

bool CDxfConvert::CheckDemLicense()
{
#ifndef TRIAL_VERSION	
	return CheckLicense(1);
#endif
	
	return false;

}

extern "C"  double		 KTHeight(KTModel *model, double x, double y);

bool CDxfConvert::Convert(float fBLX,float fBLY,float fTRX,float fTRY)
{
#if 0
	KTGetDXFExtents(m_strInputFile, &ext);
    demInfo->Xll=(int(fBLX)==-999999)?ext.minX:fBLX;
	demInfo->Yll=(int(fBLY)==-999999)?ext.minY:fBLY;
	demInfo->Xur=(int(fTRX)==999999)?ext.maxX:fTRX;
	demInfo->Yur=(int(fTRY)==999999)?ext.maxY:fTRY; 
	
	if(!setDemParam(&ext,&demParam))
	{
		AfxMessageBox("check parameter"); 
		return false;
	}
	tinLayer = KTGetDXFLayers(m_strInputFile);
	///if no dem license ,exit
	if(CheckDemLicense()==false)
	{
		tinLayer=NULL;
	}
	
	model = KTTriangulateDXF(m_strInputFile, tinLayer, TRUE);
	if(model==NULL)
	{
		KTFreeModel(model);
		AfxMessageBox( KTGetErrorString());
		return false;
	}
	/*	if (KTGenerateDEM(model, m_strOutputFile, &demParam) == -1) 
	{
	KTFreeModel(model);
	AfxMessageBox("error");//AfxMessageBox("KTGenerateDEM() error!");
	return false;
	}
	*/
	FILE * fp_dem=fopen(m_strOutputFile,"wt");
	fprintf(fp_dem,"%lf %lf 0.00  %lf %lf %d %d\n", double(demParam.cornerX),double(demParam.cornerY),double(demParam.cellW),double(demParam.cellH),demParam.nrColumns,demParam.nrRows);
    for(int i  =0 ;i<demParam.nrRows ;i++)
	{
		for(int j=0 ;j< demParam.nrColumns; j++)
		{
			double x = demParam.cornerX+ j*demParam.cellW;
			double y = demParam.cornerY+ i*demParam.cellH;
			double z = KTHeight( model,   x,   y);
			if(z >DemNoValues+1)
			{
				fprintf(fp_dem,"%lf ",z);
			}
			else
			{
				fprintf(fp_dem,"-9999.9 ");
			}
			if(0==(j+1)%10) fprintf(fp_dem,"\n");
			
		}
		fprintf(fp_dem,"\n");
	}
	fclose(fp_dem);
	
	KTFreeLayers(tinLayer);
	KTFreeModel(model);

#endif
	return true;
}
