// DxfConvert.h: interface for the CDxfConvert class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXFCONVERT_H__F0DDD3AA_4921_4599_AD3A_FCCF361DB785__INCLUDED_)
#define AFX_DXFCONVERT_H__F0DDD3AA_4921_4599_AD3A_FCCF361DB785__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <math.h>

#define DXF	0x611
#define UR	0x612
#define USGS	0x613
#define KF_C_AND_G	'&'	/* C&G			*/
#define KF_ARCINFO	'A'	/* ArcInfo		*/
#define KF_MTI_FIXED	'B'	/* Abacus/MTI		*/
#define KF_CLM		'C'	/* CLM			*/
#define KF_DIGICAD	'D'	/* Digicad		*/
#define KF_FIELD_SDR33	'F'	/* Field sdr33		*/
#define KF_GEOTOP	'G'	/* Geotop		*/
#define KF_IGRAPH_ICS	'I'	/* Intergraph  ICS	*/
#define KF_GEOCOMP	'J'	/* GEOCOMP pts/strings	*/
#define KF_CSV		'K'	/* csv KEAYS format	*/
#define KF_CIVILSOFT	'L'	/* Civilsoft		*/
#define KF_MTI		'M'	/* MTI			*/
#define KF_SDR_COORDINA	'O'	/* Coordina SDR.MAP	*/
#define KF_PACSOFT	'P'	/* Pacsoft		*/
#define KF_SDR_ROADING	'R'	/* Roading SDR		*/
#define KF_STRADA	'S'	/* Strada		*/
#define KF_AUTOCOGO	'T'	/* AutoCogo		*/
#define KF_SUS25	'U'	/* SUS25		*/
#define KF_VIRTUOZO_DEM	'V'	/* VirtuoZo DEM		*/
#define KF_WILDSOFT	'W'	/* WILDSOFT		*/
#define KF_XYZ		'X'	/* XYZ			*/
#define KF_KINGS	'Z'	/* Kings New Zealand	*/
#define KF_MOSS_GENIO	'3'	/* MOSS 3D GENIO	*/
#define KF_CIVILCAD44	'4'	/* CivilCAD 4.4		*/
#define KF_CIVILCAD53	'5'	/* CivilCAD 5.3		*/

#define LF_NOTUSED		0x0
#define LF_CONTOURABLE	0x1
#define LF_BREAKLINES	0x2
#define LF_BOUNDARY	0x4

 typedef struct _dem_info {

	char inputFile[256];
	
	char outputFile[256];
	
	int  inputFormat;	
	
	double Xll, Yll, Xur, Yur;
	
	float dx, dy;		
	
	double rotationAngle;

} DEM_INFO;



int		 KFTranslate(
			const char *urFilename,
			const char *extFilename,
			char format,
			int export
);
/*
const char	*KFGetErrorString(void);

   void tin2VirDEM(void);  
   void usgs2VirDEM(void);
 */  

typedef struct kt_layer_list_s	KTLayerList;
typedef struct kt_dem_param_s	KTDEMParam;
typedef struct kt_model_s	KTModel;	/* Opaque. */
typedef struct kt_extents_s	KTExtents;



//##ModelId=3A569A890027
struct kt_layer_list_s {
	//##ModelId=3A569A890033
	KTLayerList	*next;
	//##ModelId=3A569A890029
	char		*layerName;
	//##ModelId=3A569A890028
	int		 flags;
};

//##ModelId=3A569A8A00F1
struct kt_dem_param_s {
	double		cornerX, cornerY;
	//##ModelId=3A569A8A00FB
	double		rotAngle;		/* In radians. */
	double		cellW, cellH;
	int		nrColumns, nrRows;
};

//##ModelId=3A569A890009
struct kt_extents_s {
	double minX, minY, minZ;
	double maxX, maxY, maxZ;
};

/*
	typedef struct _extents_s {
		double minX, minY, minZ;
		double maxX, maxY, maxZ;
	} USGS_EXT;
	


	typedef struct _dem_param {
		double		cornerX, cornerY;
		double		rotAngle;	
		double		cellW, cellH;
		int		nrColumns, nrRows;
	}DEM_PARAM;

typedef struct usgs_model	USGS_MODEL;	
*/
extern "C" {

	KTLayerList	*KTGetDXFLayers(const char *dxfFilename);
	KTLayerList	*KTGetURLayers(const char *urFilename);
	void		 KTFreeLayers(KTLayerList *layers);
	int		 KTGetDXFExtents(const char *dxfFilename, KTExtents *ext);
	int		 KTGetURExtents(const char *urFilename, KTExtents *ext);

	KTModel		*KTTriangulateDXF(
				const char *dxfFilename, const KTLayerList *layers,
				int isTemporary
	);
	KTModel		*KTTriangulateUR(
				const char *urFilename, const KTLayerList *layers,
				int isTemporary
	);
	double		 KTHeight(KTModel *model, double x, double y);
	int		 KTGetModelExtents(const KTModel *model, KTExtents *ext);
	void		 KTFreeModel(KTModel *model);

	int		 KTGenerateDEM(
				KTModel *model, const char *demFilename,
				const KTDEMParam *demParam
	);

	const char	*KTGetErrorString(void);
}

	typedef struct _dem_param {
		double		cornerX, cornerY;
		double		rotAngle;		
		double		cellW, cellH;
		int		nrColumns, nrRows;
	}DEM_PARAM;

class CDxfConvert  
{
public:
	CDxfConvert();
	virtual ~CDxfConvert();
 	bool Convert(float fBLX,float fBLY,float fTRX,float fTRY);
	 bool  CheckDemLicense();
 	BOOL DemParamAdjust();
	bool bRunMode; 
	DEM_INFO  demInfo[1]; 
	KTLayerList *tinLayer; 
	KTExtents ext; 
	KTModel *model; 
	KTDEMParam demParam;  
	BOOL setDemParam(KTExtents *ext, KTDEMParam *demParam);  
	CString m_strInputTitle;  
	CString	m_strInputFile; 
	int	m_dXLL; 
	int	m_dXUR; 
	int	m_dZMin; 
	int	m_dYLL; 
	int	m_dYUR; 
	int	m_dZMax; 
	BOOL	m_bNotUsed; 
	BOOL	m_bSpotHeight; 
	BOOL	m_bBreakLine; 
	BOOL	m_bBoundary; 
	CString	m_strOutputFile; 
	int	m_dXLL2; 
	int	m_dXUR2; 
	double	m_dXSpace; 
	int	m_dYLL2; 
	int	m_dYUR2; 
	double	m_dYSpace; 
	CListBox m_layerListBox; 
	int		m_LayerAttrib; 
	double	m_demRotationAngle; 
};

#endif // !defined(AFX_DXFCONVERT_H__F0DDD3AA_4921_4599_AD3A_FCCF361DB785__INCLUDED_)
