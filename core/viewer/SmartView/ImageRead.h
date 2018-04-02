// ImageRead.h: interface for the CImageRead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGE_H__0C1DA878_DBA0_491F_8775_DCC55FFCCA1F__INCLUDED_)
#define AFX_IMAGE_H__0C1DA878_DBA0_491F_8775_DCC55FFCCA1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SmartViewDef.h"
#include "iseeirw.h"

#define SETZOOM_INVALID			0
#define SETZOOM_VALID			1
#define SETZOOM_NOCHANGE		2
#define SETZOOM_OUTTOP			4
#define SETZOOM_OUTBOTTOM		8

typedef int (CALLAGREEMENT *LPFUNC_InitCache)(const char*);
typedef int (CALLAGREEMENT *LPFUNC_ClearCache)();


extern CRITICAL_SECTION g_section;

//从影像左下角开始读
class EXPORT_SMARTVIEW CImageRead  
{	 
public:
	CImageRead();
	virtual ~CImageRead();
	virtual BOOL Load(LPCTSTR fileName);
	virtual BOOL IsValid();
	LPCTSTR GetImageName();
	int GetColorBitCount();
	void GetParamidRange(int *pnMin, int *pnMax);
	virtual CSize GetImageSize();
	virtual CSize GetImageOriginalSize();
	float GetCurReadZoom();
	virtual int  SetCurReadZoom(float zoomRate);
	virtual BOOL ReadRectBmp(CRect rect, HBITMAP hBmp);
	virtual void Close();

	void SetUISCacheDir(LPCTSTR dir);

protected:
	BOOL LoadImgDll(LPCTSTR fileName);
	BOOL PasteBits(HBITMAP hBmp, CRect rect, BYTE *pBuf, int nBufSize);

	LPSUBIMGBLOCK GetCurSubImgInfo();
	
	virtual void Process();
	char m_chFileName[_MAX_PATH];
	BOOL m_bLoad;

	HMODULE			m_hImgDll;
	LPIRWP_INFO		m_pDllInfo;
	ISADDR			m_IsAddr;	
	LPINFOSTR		m_pInfoStr;
	IRWP_API_GET_IMG_INFO	m_pInfoFunc;
	IRWP_API_READ_IMG		m_pReadFunc;
	IRWP_API_CLOSE_IMG		m_pCloseFunc;
	LPFUNC_InitCache		m_pInitCacheFunc;
	LPFUNC_ClearCache		m_pClearCacheFunc;
	
	int				m_nPyramid;
	CString			m_strUISCacheDir;
};

//typedef int (CALLAGREEMENT*FileClose)(const char fileName[100]);
//从影像左下角开始读
class EXPORT_SMARTVIEW CMutiImageRead  
{	 
public:
	CMutiImageRead();
	virtual ~CMutiImageRead();
	virtual BOOL Load(LPCTSTR lpszFileName);
	virtual BOOL IsValid();
	LPCTSTR GetImageName();
	int GetColorBitCount();
	void GetParamidRange(int *pnMin, int *pnMax);
	virtual CSize GetImageSize();
	virtual CSize GetImageOriginalSize();
	float GetCurReadZoom();
	virtual int  SetCurReadZoom(float zoomRate);
	virtual BOOL ReadRectBmp(CRect rect, HBITMAP hBmp);
	virtual void Close();
	int GetPyramid(){return m_nPyramid;}
	//	PImgInfo CloneInfo();
	
protected:
	BOOL LoadImgDll(LPCTSTR fileName);
	BOOL PasteBits(HBITMAP hBmp, CRect rect, BYTE *pBuf, int nBufSize);
	
	LPSUBIMGBLOCK GetCurSubImgInfo();
	
	virtual void Process();
	char m_chFileName[_MAX_PATH];
	BOOL m_bLoad;
	
	HMODULE			m_hImgDll;
	LPIRWP_INFO		m_pDllInfo;
	ISADDR			m_IsAddr;	
	LPINFOSTR		m_pInfoStr;
	IRWP_API_GET_IMG_INFO	m_pInfoFunc;
	IRWP_API_READ_IMG		m_pReadFunc;
	IRWP_API_CLOSE_IMG		m_pCloseFunc;
	int				m_nPyramid;
};

class CThreadMutiImageRead;
class EXPORT_SMARTVIEW CRoatableMutiImageRead : public CMutiImageRead
{
	friend CThreadMutiImageRead;
public:
	CRoatableMutiImageRead();
	virtual ~CRoatableMutiImageRead();
	virtual CSize GetImageSize();
	virtual BOOL ReadRectBmp(CRect rect, HBITMAP hBmp);
	
	virtual void SetRotate(float ang);
	inline float GetRotateAnge()const{
		return m_fAngle;
	}
	BOOL IsRotated();
	
protected:
	BYTE *GetRotateBuf(int size);
	
protected:
	float m_fAngle;
	BYTE *m_pRotateBuf;
	DWORD m_nBufSize;
};


class EXPORT_SMARTVIEW CRoatableImageRead : public CImageRead
{
public:
	CRoatableImageRead();
	virtual ~CRoatableImageRead();
	virtual CSize GetImageSize();
	virtual BOOL ReadRectBmp(CRect rect, HBITMAP hBmp);
	
	virtual void SetRotate(float ang);
	inline float GetRotateAnge()const{
		return m_fAngle;
	}
	BOOL IsRotated();
	
protected:
	BYTE *GetRotateBuf(int size);
	
protected:
	float m_fAngle;
	BYTE *m_pRotateBuf;
	DWORD m_nBufSize;
};



class EXPORT_SMARTVIEW CThreadImageRead : public CRoatableImageRead
{
public:
	enum
	{
		threadDead = 0,
		threadPause = 1,
		threadRun = 2
	};
	CThreadImageRead();
	virtual ~CThreadImageRead();

public:
	//override, insert some codes for synchronization between multiple threads
	virtual int  SetCurReadZoom(float zoomRate);
	virtual void Close();
	virtual BOOL ReadRectBmp(CRect rect, HBITMAP hBmp);

public:
	//control the read thread
	BOOL InitReadThread();
	void DestroyReadThread();
	void BeginReadThread();
	void EndReadThread();
	inline BOOL IsReadThreadStarted(){ return m_nThreadCtrl==2; }

	void Lock();
	void Unlock();

	//set and get read parameters and data
	void SetReadRect(CRect rect, BOOL bSkipRead=FALSE, int nSkipRate=1);
	inline CRect GetReadRect(){ return m_rcBlock; }
	BOOL GetReadBmp(HBITMAP hBmp);
	void SetReadBmpInfo(int w, int h, int bitcount=0);

	//the kernel function for the read thread
	static DWORD WINAPI ReadThread(LPVOID pParam);

	BOOL ReadRectBmp(CRect rect, HBITMAP hBmp, BOOL bSkipRead, int nSkipRate);

	//set the handle of the window receiving messages from read thread 
	void SetThreadCallback(HWND hWnd, UINT nMsg);
private:
	//read a line of image data
	BOOL ReadBlockLine(int nCurLine);

private:
	HANDLE m_hReadThread;
	CRITICAL_SECTION m_section;
	int m_nThreadCtrl;
	HANDLE m_hEvent;

	CRect m_rcBlock;
	BOOL m_bSkipRead;
	int	m_nCurRead,m_nSkipReadRate;
	HBITMAP m_hBlockBmp,m_hLineBmp;
	BYTE *m_pBlockBits, *m_pLineBits;
	BITMAPINFO* m_pBmpInfo;

	HWND m_hWndCallback;
	UINT m_nMsgCallback;
};


class EXPORT_SMARTVIEW CRoatableMutiImageReadEx : public CRoatableMutiImageRead
{
public:
	CRoatableMutiImageReadEx();
	~CRoatableMutiImageReadEx();
	BOOL Attach();
	BOOL DeAttach();
	inline BOOL IsAttach() {return m_bIsAttach;}
	virtual int  SetCurReadZoom(float zoomRate);
	float GetZoomRateEx();	

protected:
	virtual void Process();

protected:
	float m_lfZoomRate; //
	BOOL  m_bIsAttach;
};

class EXPORT_SMARTVIEW CThreadMutiImageRead
{
public:
	enum
	{
		threadDead = 0,
		threadPause = 1,
		threadRun = 2
	};
	CThreadMutiImageRead();	
	virtual ~CThreadMutiImageRead();
	void SetImgParmasArr(const void *pImgParArr = NULL);
	void SetArrSkipInfos(const void *pArrSkipInfos);
//	void SetImgPyramidsRef(CArray<int ,int&> *pPyramids);
	BOOL AttachImage(LPCTSTR fileName, BOOL bInit = FALSE);
	BOOL InsertImage(int idx, LPCTSTR fileName);
	BOOL Detach(LPCTSTR fileName);
	CSize GetImageSize(LPCTSTR fileName);
	CSize GetImageOriginalSize(LPCTSTR fileName);
	
	int   GetParamidRange(int idx,int * pnMin, int * pnMax);
//	PImgInfo GetCurCloneImgInfo();
	void  SetRateChangeFlag(BOOL flag = TRUE);
	
public:
	//override, insert some codes for synchronization between multiple threads
	//设置缩放比，设置合适的返回影像名保存在str中
	int  SetImgZoomRate(int idx, float zoomRate);			//只在初始化时调用

	float  GetCurReadZoom(LPCTSTR fileName);
	float  GetCurReadZoom(int idx);

	void Close();
	BOOL ReadCurRectBmp(CRect rect, HBITMAP hBmp);
	BOOL ReadRectBmp(int idx ,CRect rect, HBITMAP hBmp, BOOL bSkipRead, int nSkipRate);
//	BOOL ReadRectBmp(const char fileName[100],CRect& rect, HBITMAP hBmp);
	
public:
	//control the read thread
	BOOL InitReadThread();
	void DestroyReadThread();
	void BeginReadThread();
	void EndReadThread();
	inline BOOL IsReadThreadStarted(){ return m_nThreadCtrl==2; }
	
	void Lock();
	void Unlock();
	
	//set and get read parameters and data
	void SetReadImgInfo(const CArray<int,int> &imgIdx ,const CArray<CRect,CRect> &oldimgRect,const CArray<CRect,CRect> &newimgRect);
	void ClearReadImgInfo();
	void SetCurReadRect(CRect rect, BOOL bSkipRead, int nSkipRate);
/*	inline CRect GetReadRect(){ return m_rcBlock; }*/
	inline HBITMAP GetCurHbitmap(){return m_hBlockBmp;}
	BOOL GetReadBmp(CArray<HBITMAP,HBITMAP> &arrhBmp);
	void SetCurReadBmpInfo(int w, int h, int bitcount=0);
	void SetReadBmpInfo(int w, int h, int bitcount=0);
	void ClearAllReadBmpInfo();
//	void SetReadBmpInfo(const char fileName[100], int w, int h, int bitcount=0);
	
	//the kernel function for the read thread
	static DWORD WINAPI ReadThread(LPVOID pParam);
	
	//set the handle of the window receiving messages from read thread 
	void SetThreadCallback(HWND hWnd, UINT nMsg);
private:
//	void UpdateCurImgSkipParam();
	//read a line of image data
	BOOL ReadCurBlockLine(int nCurLine);
	
private:
	CArray<CRoatableMutiImageReadEx*, CRoatableMutiImageReadEx*> m_arrRecentImageReads;//保存最近的十个影像的读取信息
	CArray<CRoatableMutiImageReadEx*, CRoatableMutiImageReadEx*> m_arrImageReads;
	BOOL m_bChangeImageRate;
	HANDLE m_hReadThread;
	CRITICAL_SECTION m_section;
	int m_nThreadCtrl;
	HANDLE m_hEvent;
	const void *m_pImgParArr;

	const void *m_pArrSkipInfos;
	CArray<int,int> m_arrImgIdx;
	CArray<CRect,CRect> m_arrOldImgRect;
	CArray<CRect,CRect> m_arrNewImgRect;
	CRect m_rcBlock;
	BOOL m_bSkipRead;
	int	m_nCurRead,m_nSkipReadRate;
	HBITMAP m_hBlockBmp,m_hLineBmp;
	BYTE *m_pBlockBits, *m_pLineBits;
	BITMAPINFO* m_pBmpInfo;
	CArray<HBITMAP,HBITMAP> m_arrHbitmap;

	HWND m_hWndCallback;
	UINT m_nMsgCallback;
//	float m_lfZoom;
	
};


#endif // !defined(AFX_IMAGE_H__0C1DA878_DBA0_491F_8775_DCC55FFCCA1F__INCLUDED_)
