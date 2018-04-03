// TextureObj.h: interface for the CTextureObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTUREOBJ_H__1A24647E_58A0_4A24_80F1_ADCE2B2A694D__INCLUDED_)
#define AFX_TEXTUREOBJ_H__1A24647E_58A0_4A24_80F1_ADCE2B2A694D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


struct Quad 
{
	bool intersect(const CRect* rect )
	{
		return (rect->PtInRect(CPoint(x0,y0))|| 
		    	rect->PtInRect(CPoint(x1,y1))||
				rect->PtInRect(CPoint(x2,y2))||
				rect->PtInRect(CPoint(x3,y3)));

	}
	double x0,y0,x1,y1,x2,y2,x3,y3;
};
class tex_object
{
public:
	// set managed to true if you want the class to cleanup objects in the destructor
	tex_object(UINT tgt, bool managed) 
		: target(tgt), valid(false), texture(0), manageObjects(managed) {}
	
	virtual ~tex_object()
	{ 
		if (manageObjects)
			del();
	}

	void enable();
	void disable();
	void parameter(UINT pname, UINT i);
	void parameter(UINT pname, float f);
	void parameter(UINT pname, const int * ip);
	void parameter(UINT pname, float * fp);

	void bind();
	void del();
	bool is_valid() const;
	void gen();
	UINT target;
	bool valid;
	bool manageObjects;
	UINT texture;
};

class tex_object_2d : public tex_object
{
public:
	tex_object_2d();
	virtual ~tex_object_2d();
	void define(int w, int h, int format, void* pBits);
	void setbits(int x, int y, int w, int h, void *pBits, int format);
	void subcopy( int xoff, int yoff, int x, int y, int wid, int hei);
	int width, height;
};

class tex_image : public tex_object_2d
{
public:
	tex_image() : filled(false){}
	virtual ~tex_image(){}

	void cutto(tex_image& a);

	//显示纹理
	void display(double x[4], double y[4], BOOL bLinear, BOOL bGPU);
	void display_alpha(double x[4], double y[4]);

	void display(double x0[4], double y0[4], double x[4], double y[4], BOOL bLinear, BOOL bGPU);

	//设置纹理位图
	void setviewbmp(HBITMAP hBmp);
	void setviewbmp_BGRA(HBITMAP hBmp);

	BOOL copyToBmp(HBITMAP hBmp);
	
	bool filled;

	BOOL bmptotexture(HBITMAP hBmp, int x, int y, int xTar, int yTar,
		int nTarWidth, int nTarHeight, int format );
};

#endif // !defined(AFX_TEXTUREOBJ_H__1A24647E_58A0_4A24_80F1_ADCE2B2A694D__INCLUDED_)
