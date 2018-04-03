// EBFeature.h: interface for the CFeature class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EBFEATURE_H__1FA8F991_A2D7_4A3D_976B_04D3C6D4A411__INCLUDED_)
#define AFX_EBFEATURE_H__1FA8F991_A2D7_4A3D_976B_04D3C6D4A411__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Geometry.h"
#include "Permanent.h"

#define FIELDNAME_FTRDELETED		_T("FTRDELETED")
#define FIELDNAME_FTRID				_T("FTRID")
#define FIELDNAME_FTRCODE			_T("FTRCODE")
#define FIELDNAME_FTRVISIBLE		_T("VISIBLE")
#define FIELDNAME_FTRPURPOSE		_T("PURPOSE")
#define FIELDNAME_FTRDISPLAYORDER	_T("DISPLAYORDER")
#define FIELDNAME_FTRGROUPID		_T("FTRGROUPID")
#define FIELDNAME_FTRGROUPNAME		_T("FTRGROUPNAME")
#define FIELDNAME_FTRGROUPSELECT	_T("FTRGROUPSELECT")
#define FIELDNAME_FTRMODIFYTIME		_T("FTRMODIFYTIME")
#define FIELDNAME_FTRLAYERID        _T("FTRLAYERID")
#define FIELDNAME_FTRWORKSPACENAME  _T("WORKSPACENAME")
#define FIELDNAME_FTRMAPSHEETNAME   _T("FTRMAPSHEETNAME")

MyNameSpaceBegin

struct OUID
{
	OUID(){
		memset(v,0,sizeof(v));
	}
	OUID(GUID& t){
		memcpy(v,&t,sizeof(v));
	}
	GUID ToGUID(){
		GUID t;
		memcpy(&t,v,sizeof(t));
		return t;
	}
	void FromString(const char *lpstr)
	{
		const char *text = lpstr;
		BYTE *buf = (BYTE*)v;
		int i, num = sizeof(v);
		int len = strlen(lpstr);
		
		for( i=0; i<num; i++)
		{
			buf[i] = 0;
			if( i<len )
			{
				if( text[(i<<1)]>='a' )
					buf[i] |= ((text[(i<<1)]-_T('a')+0xa)<<4);
				else
					buf[i] |= ((text[(i<<1)]-_T('0'))<<4);
				
				if( text[(i<<1)+1]>='a' )
					buf[i] |= ((text[(i<<1)+1]-_T('a')+0xa));
				else
					buf[i] |= ((text[(i<<1)+1]-_T('0')));
			}
		}
	}
	CStringA ToString()const
	{
		char text[40] = {0};
		BYTE *buf = (BYTE*)v;
		int i, num = sizeof(v);
		for( i=0; i<num; i++)
		{
			if( buf[i]>=0xa0 )
				text[(i<<1)] = (buf[i]>>4)-0xa + _T('a');
			else
				text[(i<<1)] = (buf[i]>>4) + _T('0');
			
			if( (buf[i]&0xf)>=0xa )
				text[(i<<1)+1] = (buf[i]&0xf)-0xa + _T('a');
			else
				text[(i<<1)+1] = (buf[i]&0xf) + _T('0');
		}
		
		return CStringA(text);
	}
	BOOL operator ==(const OUID& t)const{
		return (v[0]==t.v[0]&&v[1]==t.v[1]&&v[2]==t.v[2]&&v[3]==t.v[3]);
	}
	BOOL IsNull()const{
		return (v[0]==0&&v[1]==0&&v[2]==0&&v[3]==0);
	}
	long v[4];
};

#define FTR_COLLECT				0x000000001
#define FTR_MARK				0x000000002
#define FTR_EDB					0x000000004

//EXPORT_EDITBASE CGeometry*  GCreateGeometry(int nGeoClass);
class EXPORT_EDITBASE CFeature : public CPermanent
{
	DECLARE_DYNAMIC(CFeature)
public:
	CFeature();
	virtual ~CFeature();
	static CPermanent* CreateObject(){
		return new CFeature;
	}
	virtual int  GetClassType()const;

	BOOL CreateGeometry(int nGeoClass);
	virtual BOOL WriteTo(CValueTable& tab)const;
	virtual BOOL ReadFrom(CValueTable& tab,int idx = 0);
	virtual CGeometry * SetGeometry(CGeometry *pGeo);
	virtual CGeometry * GetGeometry()const;

	virtual CFeature* Clone()const;
	virtual BOOL CopyFrom(const CFeature *pObj);

	OUID GetID()const{ return m_id; }
	void SetID(OUID id){ m_id=id; }
	
	BOOL IsDeleted()const{ return m_bDeleted; }
	void SetToDeleted(BOOL bDel){ m_bDeleted=bDel; }

	void SetCode(const char* code);
	const char* GetCode() const {  return m_strCode; }

	BOOL IsVisible()const{ return m_bVisible; }
	void EnableVisible(BOOL bVisible) { m_bVisible=bVisible; }

	int GetPurpose()const{ return m_nPurpose; }
	void SetPurpose(int nPurpose) { m_nPurpose = nPurpose; }

	int GetLayerID() { return m_layerid; }
	void SetLayerID(int id) { m_layerid=id; }
	//为了提高程序查找速度而提供AppFlag系列的打标函数；m_nAppFlag 与 m_bDeleted 共用4个字节，不占用额外内存空间，且不保存到磁盘；
	//应该只在函数内部级别使用 SetAppFlag 和GetAppFlag，在退出函数时，应该将标志清零，避免影响其他地方使用了SetAppFlag的代码
	int GetAppFlag()const{ return m_nAppFlag; }
	void SetAppFlag(int nFlag) { m_nAppFlag = nFlag; }

	//为了支持多重flag同时使用，而不互相干扰，提供下面两个函数（支持24重设置）；
	//nBit范围在0~23，SetAppFlagBit中的flag为0或者1，GetAppFlagBit返回为0或者1
	int GetAppFlagBit(int nBit)const;
	void SetAppFlagBit(int nBit, int flag);

	double GetDisplayOrder() const{ return m_fDisplayOrder; }
	void SetDisplayOrder(double fOrder) { m_fDisplayOrder = fOrder; }

	void GetObjectGroup(CUIntArray &group) const{ group.Copy(m_arrGroupID); }
	BOOL AddObjectGroup(UINT id);
	BOOL RemoveObjectGroup(UINT id);
	BOOL IsInObjectGroup(UINT id)const;

	long GetModifyTime()const;
	void SetCurrentModifyTime();

	virtual void Draw(GrBuffer *pBuf, float fDrawScale=1.0)const;
	
protected:
	CGeometry *m_pGeometry;	
	OUID m_id;

	BOOL m_bDeleted:1; //临时标记，不用保存到磁盘
	BOOL m_bVisible:1;
	// 用途（采集、入库、符号）
	int m_nPurpose:6;
	int	 m_nAppFlag:24; //临时标记，供内部程序使用；不用保存到磁盘
	
	int m_nUserID;  // 预留
	double	 m_fDisplayOrder;  // 显示顺序
	char m_strCode[16]; // 标识码
	long m_nModifyTime;//修改时间
	CUIntArray m_arrGroupID;// 组号
	int m_layerid;//层ID
};

typedef CFeature *CPFeature;

typedef CArray<CFeature*,CFeature*> CFtrArray;
typedef CArray<CGeometry*,CGeometry*> CGeoArray;

#define FTR_HANDLE LONG_PTR

inline FTR_HANDLE FtrToHandle(CFeature *p){
	return (FTR_HANDLE)p;
}

inline CFeature* HandleToFtr(FTR_HANDLE h){
	return (CFeature*)h;
}

typedef CArray<FTR_HANDLE,FTR_HANDLE> CFtrHArray;


MyNameSpaceEnd

#endif // !defined(AFX_EBFEATURE_H__1FA8F991_A2D7_4A3D_976B_04D3C6D4A411__INCLUDED_)
