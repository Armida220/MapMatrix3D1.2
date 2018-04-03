// UIParam.h: interface for the CUIParam class.
//
//////////////////////////////////////////////////////////////////////
 
#if !defined(AFX_PROCPARAM_H__C5685680_702B_4188_A0D3_073BD767B764__INCLUDED_)
#define AFX_PROCPARAM_H__C5685680_702B_4188_A0D3_073BD767B764__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

MyNameSpaceBegin


#define LAYERPARAMITEM_NOTEMPTY			0x01
#define LAYERPARAMITEM_LOCAL			0x02
#define LAYERPARAMITEM_NOTLOCAL			0x04
#define LAYERPARAMITEM_POINT_SINGLE		0x08
#define LAYERPARAMITEM_LINE_SINGLE		0x10
#define LAYERPARAMITEM_AREA_SINGLE		0x20
#define LAYERPARAMITEM_TEXT_SINGLE		0x40

#define LAYERPARAMITEM_NOTEMPTY_POINT	(LAYERPARAMITEM_NOTEMPTY|LAYERPARAMITEM_LOCAL|LAYERPARAMITEM_NOTLOCAL|LAYERPARAMITEM_POINT_SINGLE)
#define LAYERPARAMITEM_NOTEMPTY_LINE	(LAYERPARAMITEM_NOTEMPTY|LAYERPARAMITEM_LOCAL|LAYERPARAMITEM_NOTLOCAL|LAYERPARAMITEM_LINE_SINGLE)
#define LAYERPARAMITEM_NOTEMPTY_AREA	(LAYERPARAMITEM_NOTEMPTY|LAYERPARAMITEM_LOCAL|LAYERPARAMITEM_NOTLOCAL|LAYERPARAMITEM_AREA_SINGLE)
#define LAYERPARAMITEM_NOTEMPTY_TEXT	(LAYERPARAMITEM_NOTEMPTY|LAYERPARAMITEM_LOCAL|LAYERPARAMITEM_NOTLOCAL|LAYERPARAMITEM_TEXT_SINGLE)

#define LAYERPARAMITEM_NOTEMPTY_LINEAREA	(LAYERPARAMITEM_NOTEMPTY_LINE|LAYERPARAMITEM_NOTEMPTY_AREA)


#define LAYERPARAMITEM_POINT			(LAYERPARAMITEM_LOCAL|LAYERPARAMITEM_NOTLOCAL|LAYERPARAMITEM_POINT_SINGLE)
#define LAYERPARAMITEM_LINE				(LAYERPARAMITEM_LOCAL|LAYERPARAMITEM_NOTLOCAL|LAYERPARAMITEM_LINE_SINGLE)
#define LAYERPARAMITEM_AREA				(LAYERPARAMITEM_LOCAL|LAYERPARAMITEM_NOTLOCAL|LAYERPARAMITEM_AREA_SINGLE)
#define LAYERPARAMITEM_TEXT				(LAYERPARAMITEM_LOCAL|LAYERPARAMITEM_NOTLOCAL|LAYERPARAMITEM_TEXT_SINGLE)

#define LAYERPARAMITEM_LINEAREA				(LAYERPARAMITEM_LINE|LAYERPARAMITEM_AREA)


//界面参数对象；封装常用的界面参数；由主程序在界面上显示这些参数；
class EXPORT_EDITBASE CUIParam
{
public:
	struct Option
	{
		TCHAR desc[64];
		int value;
		int accelkey;
		int flag; //标志：0, 一般选项, 1, 默认选中项, 2, 结果选中项
	};
	enum
	{
		NoneType = 0,
		BoolType = 1,
		IntType = 2,
		FloatType = 3,
		DoubleType = 4,
		PointType = 5,
		StringType = 6,
		OptionType = 7,
		HotKeyType = 8,
		LayerNameType = 9,
		MutiLayerNameType = 10,
		ButtonType = 11,
		FontNameType = 12,
		MultiEditType = 13,
		CheckListType = 14,
		UsedLayerNameType = 15,
		FileNameType = 16,
		PathNameType = 17,
		ColorType = 18
	};

	// 该结构体的内存管理由 CUIParam 负责，数据对象之间的拷贝总是为浅拷贝
	struct ParamItem
	{
		friend class CUIParam;
		ParamItem(){ type=NoneType; 
			accelKey = 0;
			bShow = true;
			memset(field,0,sizeof(field)); 
			memset(title,0,sizeof(title)); 
			memset(desc,0,sizeof(desc));
			memset(&data,0,sizeof(data));
			exParam = 0;
		}
		TCHAR field[32]; //字段名
		char accelKey;     // 字段别名
		bool bShow;
		int type; //值类型
		TCHAR title[64]; //显示给用户看的名称
		TCHAR desc[256]; //描述信息
		TCHAR file_filter[256]; //仅FileNameParam使用的文件名过滤参数
		union //数据
		{
			BOOL bValue;
			int  nValue;
			long lValue;
			float fValue;
			double lfValue;

			struct {
				double x,y,z;
			}point;

			CString *pStrValue;

			struct
			{
				CString *pFileName;
				CString *pFileFilter;
			}FileName;

			char chHotKey;
			CArray<Option,Option> *pOptions;
		}data;

		//扩展参数，比如选择图层时，图层对话框的初始标志
		DWORD exParam;

	public:
		ParamItem& operator=(const ParamItem& para)
		{
			CopyFrom(para);
			return *this;
		}
	private:
		// 这两个函数只供 CUIParam 对象使用，外界不得使用
		void Clear()
		{
			switch(type) 
			{
			case StringType:
			case LayerNameType:
			case UsedLayerNameType:
			case MutiLayerNameType:			 
			case FontNameType:
			case MultiEditType:
			case PathNameType:
				if( data.pStrValue )
				{
					delete data.pStrValue;
				}
				break;
			case FileNameType:
				if( data.FileName.pFileName )
				{
					delete data.FileName.pFileName;
				}
				if( data.FileName.pFileFilter )
				{
					delete data.FileName.pFileFilter;
				}
				break;
			case OptionType:
			case CheckListType:
				if( data.pOptions )
				{
					delete data.pOptions;
				}
				break;
			default:;
			}

			type=NoneType; 
			memset(field,0,sizeof(field)); 
			memset(title,0,sizeof(title)); 
			memset(desc,0,sizeof(desc));
			memset(&data,0,sizeof(data));
			accelKey = 0;
			bShow = true;
			exParam = 0;
		}

		void CopyFrom(const ParamItem& item)
		{
			Clear();
			accelKey = item.accelKey;
			bShow = item.bShow;
			type = item.type;
			memcpy(field,item.field,sizeof(field));
			memcpy(title,item.title,sizeof(title));
			memcpy(desc,item.desc,sizeof(desc));
			exParam = item.exParam;

			switch(type) {
			case NoneType:
				break;
			case IntType:
				data.nValue = item.data.nValue;
				break;
			case BoolType:
				data.bValue = item.data.bValue;
				break;
			case FloatType:
				data.fValue = item.data.fValue;
				break;
			case DoubleType:
				data.lfValue = item.data.lfValue;
				break;
			case PointType:
				data.point = item.data.point;
				break;
			case StringType:
			case LayerNameType:
			case UsedLayerNameType:
			case MutiLayerNameType:			 
			case FontNameType:
			case MultiEditType:
			case PathNameType:
				data.pStrValue = new CString(*item.data.pStrValue);
				break;
			case FileNameType:
				data.FileName.pFileName = new CString(*item.data.FileName.pFileName);
				data.FileName.pFileFilter = new CString(*item.data.FileName.pFileFilter);
				break;
			case OptionType:
			case CheckListType:
				data.pOptions = new CArray<Option,Option>;
				if( data.pOptions )data.pOptions->Copy(*item.data.pOptions);
				break;
			case HotKeyType:
				data.chHotKey = item.data.chHotKey;
				break;
			case ColorType:
				data.lValue = item.data.lValue;
				break;
			default:;
			}
		}
	};
	CUIParam();
	virtual ~CUIParam();

	void CopyFrom(const CUIParam *p);

	void SetOwnerID(LPCTSTR id, LPCTSTR title);
	
	void AddParam(LPCTSTR field, bool value, LPCTSTR title, LPCTSTR desc=NULL, char accelkey=0, bool bShow = true);
	void AddParam(LPCTSTR field, int  value, LPCTSTR title, LPCTSTR desc=NULL, char accelkey=0, bool bShow = true);
	void AddParam(LPCTSTR field, long  value, LPCTSTR title, LPCTSTR desc=NULL, char accelkey=0, bool bShow = true);
	void AddParam(LPCTSTR field, float value, LPCTSTR title, LPCTSTR desc=NULL, char accelkey=0, bool bShow = true);
	void AddParam(LPCTSTR field, double value, LPCTSTR title, LPCTSTR desc=NULL, char accelkey=0, bool bShow = true);
	void AddParam(LPCTSTR field, PT_3D point, LPCTSTR title, LPCTSTR desc=NULL, char accelkey=0, bool bShow = true);
	void AddParam(LPCTSTR field, LPCTSTR str, LPCTSTR title, LPCTSTR desc=NULL, char accelkey=0, bool bShow = true);
	void AddParam(LPCTSTR field, char key, LPCTSTR title, LPCTSTR desc=NULL);
	void AddLayerNameParam(LPCTSTR field, LPCTSTR str, LPCTSTR title, LPCTSTR desc=NULL, DWORD checkFlags=0, char accelkey=0, bool bShow = true);
	void AddLayerNameParamEx(LPCTSTR field, LPCTSTR str, LPCTSTR title, LPCTSTR desc=NULL, DWORD checkFlags=0, char accelkey=0, bool bShow = true);
	void AddButtonParam(LPCTSTR field, LPCTSTR title, LPCTSTR desc=NULL);
	void AddFontNameParam(LPCTSTR field, LPCTSTR str, LPCTSTR title, LPCTSTR desc=NULL, char accelkey=0, bool bShow = true);
	void AddMultiEditParam(LPCTSTR field, LPCTSTR str, LPCTSTR title, LPCTSTR desc=NULL, char accelkey=0, bool bShow = true);
	void AddColorParam(LPCTSTR field, long  value, LPCTSTR title, LPCTSTR desc=NULL, char accelkey=0, bool bShow = true);

	void AddUsedLayerNameParam(LPCTSTR field, LPCTSTR str, LPCTSTR title, LPCTSTR desc=NULL, char accelkey=0, bool bShow = true);
	void AddFileNameParam(LPCTSTR field, LPCTSTR str, LPCTSTR filter, LPCTSTR title, LPCTSTR desc=NULL, char accelkey=0, bool bShow = true);
	void AddPathNameParam(LPCTSTR field, LPCTSTR str, LPCTSTR title, LPCTSTR desc=NULL, char accelkey=0, bool bShow = true);
	
	void BeginOptionParam(LPCTSTR field, LPCTSTR title, LPCTSTR desc=NULL, char accelkey=0, bool bShow = true);
	void AddOption(LPCTSTR desc, int value, int accelkey, BOOL bDefOpt);
	void EndOptionParam();

	void SetOptionDefault(LPCTSTR field, int value);

	void BeginCheckListParam(LPCTSTR field, LPCTSTR title, LPCTSTR desc=NULL, char accelkey=0, bool bShow = true);
	void AddCheckItem(LPCTSTR desc, int value);
	void EndCheckListParam();

	void AddLineTypeParam(LPCTSTR field, int nDefPenCode, LPCTSTR title, LPCTSTR desc=NULL, char accelkey=0, bool bShow = true);

	int GetParam(LPCTSTR field, CUIParam::ParamItem& item)const;
	int GetParamByTitle(LPCTSTR title, CUIParam::ParamItem& item)const;

	void DelParam(LPCTSTR field);

	CString m_strID;
	CString m_strTitle;
	CString m_strTip;
	CArray<ParamItem,ParamItem> m_arrItems;
	int m_nRetItem;

private:
	ParamItem m_optItemInAdding;
};


MyNameSpaceEnd

#endif // !defined(AFX_PROCPARAM_H__C5685680_702B_4188_A0D3_073BD767B764__INCLUDED_)
