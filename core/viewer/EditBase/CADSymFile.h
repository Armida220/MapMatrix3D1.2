// CADSymFile.h: interface for the CCADLinFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CADSYMFILE_H__1BA817FE_032A_4263_A80D_00153150192E__INCLUDED_)
#define AFX_CADSYMFILE_H__1BA817FE_032A_4263_A80D_00153150192E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GrBuffer.h"


MyNameSpaceBegin

struct CAD_LT_SEGMENT
{
	CAD_LT_SEGMENT(){
		type = 0;
	}

	void InitDash(){
		type = 0;
		data.dash_len = 0;
	}
	void InitShape(){
		type = 1;
		data.shape.shape_name[0] = 0;
		data.shape.shapefile_name[0] = 0;
		data.shape.relative_ang = 0;
		data.shape.absolute_ang = 0;
		data.shape.scale = 1.0f;
		data.shape.dx = 0;
		data.shape.dy = 0;
	}
	void InitText(){
		type = 2;
		data.text.content[0] = 0;
		data.text.style[0] = 0;
		data.text.relative_ang = 0;
		data.text.absolute_ang = 0;
		data.text.scale = 1.0f;
		data.text.dx = 0;
		data.text.dy = 0;
	}

	double GetLen()const{
		if( type==0 )
			return data.dash_len;
		else
			return 0;
	}

	int type;  //0=dash_len,1=shape,2=text
	union
	{
		double dash_len;
		struct 
		{
			char shape_name[128];
			char shapefile_name[128];
			float relative_ang;
			float absolute_ang;
			float scale;
			float dx;
			float dy;
		} shape;

		struct
		{
			char content[128];
			char style[128];
			float relative_ang;
			float absolute_ang;
			float scale;
			float dx;
			float dy;
		} text;
	}data;
};

struct CAD_LTDEF
{
	CAD_LTDEF()
	{
		name[0] = descr[0] = 0;
		align = segnum = 0;
	}

	char name[64];
	char descr[128];
	int align;
	int segnum;
	CAD_LT_SEGMENT segs[100];
};

struct CAD_SHPDEF
{
	CAD_SHPDEF()
	{
		filename[0] = 0;
		name[0] = 0;
		no = datalen = 0;
		bytes = 0;
	}
	int no;
	char filename[128];
	char name[128];
	int bytes;
	int datalen;
	char data[1000];
	char data_str[1000][8];
};

class EXPORT_EDITBASE CCADShxFile
{
public:
	CCADShxFile();
	virtual ~CCADShxFile();
	void Clear();
	BOOL Open(const char *fileName);
	BOOL OpenSHX(const char *fileName);
	BOOL OpenSHP(const char *fileName);
	int GetCount();
	const CAD_SHPDEF *GetItem(int idx);
	const CAD_SHPDEF *GetItem(const char *filename, const char *name);

	BOOL OpenBin(const char *fileName);
	BOOL SaveBin(const char *fileName);
public:
	void AddItem(CAD_SHPDEF *def);

public:
	int m_nShapeNum, m_nShapeMemLen;
	CAD_SHPDEF *m_pShapeDefs;

	CString m_strPath;
};


class EXPORT_EDITBASE CCADShxFileManager
{
public:
	CCADShxFileManager();
	virtual ~CCADShxFileManager();

	static CCADShxFileManager *GetObject();

	void SetPath(LPCTSTR path);
	const CAD_SHPDEF *GetShape(const char *file_name, const char *shp_name);

	CString m_strPath;
	CArray<CCADShxFile*,CCADShxFile*> m_arrPFiles;
};




class EXPORT_EDITBASE CCADLinFile  
{
public:
	CCADLinFile();
	virtual ~CCADLinFile();
	void Clear();
	BOOL Open(const char *fileName);
	int GetCount();
	const CAD_LTDEF *GetItem(int idx);
	const CAD_LTDEF *GetItem(const char *name);

	void GetShapeFileNames(CStringArray& arrNames);

	BOOL OpenBin(const char *fileName);
	BOOL SaveBin(const char *fileName);
private:
	void AddItem(CAD_LTDEF *def);

private:
	int m_nLTNum, m_nLTMemLen;
	CAD_LTDEF *m_pLTDefs;
};



struct EXPORT_EDITBASE CAD_HATCHDEF
{
	CAD_HATCHDEF()
	{
		name[0] = descr[0] = 0;
		segnum = 0;
	}
	char name[64];
	char descr[128];
	int segnum;
	struct HATCH_SEG
	{
		HATCH_SEG()
		{
			ang = xo = yo = dx = dy = 0;
			segnum = 0;
		}
		double ang;
		double xo, yo;
		double dx, dy;
		int segnum;
		double segs[10];
	} segs[100];
};

class EXPORT_EDITBASE CCADPatFile  
{
public:
	CCADPatFile();
	virtual ~CCADPatFile();
	void Clear();
	BOOL Open(const char *fileName);
	int GetCount();
	const CAD_HATCHDEF *GetItem(int idx);
	const CAD_HATCHDEF *GetItem(const char *name);

	BOOL OpenBin(const char *fileName);
	BOOL SaveBin(const char *fileName);
	
private:
	void AddItem(CAD_HATCHDEF *def);
	
private:
	int m_nHatchNum, m_nHatchMemLen;
	CAD_HATCHDEF *m_pHatchDefs;
	
};


class EXPORT_EDITBASE CCADSymbolLib
{
public:
	CCADSymbolLib();
	virtual ~CCADSymbolLib();

	BOOL Load(LPCTSTR path);
	BOOL Import(LPCTSTR path);
	BOOL Save(LPCTSTR path);

	CCADLinFile *GetLin();
	CCADShxFile *GetShx();
	CCADPatFile *GetPat();

protected:
	void Create();
	void Clear();

protected:
	CCADLinFile *m_pLinFile;
	CCADShxFile *m_pShxFile;
	CCADPatFile *m_pPatFile;
};

void EXPORT_EDITBASE DeleteCADSymbolFile(LPCTSTR path);

void EXPORT_EDITBASE LinToGrBuffer(const CAD_LTDEF *pDef, CCADSymbolLib *pCadSymLib, CArray<PT_3DEX,PT_3DEX>& arrPts, float line_width, float sym_scale, GrBuffer *pBuf);
void EXPORT_EDITBASE PatToGrBuffer(const CAD_HATCHDEF *pDef, CArray<PT_3DEX,PT_3DEX>& arrPts, float line_width, float sym_scale, double xoff, double yoff, GrBuffer* buf);
void EXPORT_EDITBASE ShpToGrBuffer(const CAD_SHPDEF *pDef, GrBuffer *pBuf);


MyNameSpaceEnd

#endif // !defined(AFX_CADSYMFILE_H__1BA817FE_032A_4263_A80D_00153150192E__INCLUDED_)
