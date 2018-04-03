// PlugBase.h: interface for the CPlugBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLUGBASE_H__EA4B3EF4_EA2D_4291_A89C_8B2FFBD1AFD3__INCLUDED_)
#define AFX_PLUGBASE_H__EA4B3EF4_EA2D_4291_A89C_8B2FFBD1AFD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Command.h "
#include "PluginAPI.h "
#include "AccessPlugin.h "
#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持

#define  NULL_ID					-1  //在工具条按钮名资源字符串ID或命令菜单项字符串ID设为此值，可去掉相应界面项
#define  PLUG_TYPE_INVALID			-1
#define  PLUG_TYPE_CMD				0	//对应菜单或工具条的插件
#define  PLUG_TYPE_DOC				1	//定制文档类型的插件
#define  PLUG_TYPE_FILE				2   //文件导入导出


#define  PLUGFUNC_TYPE_CREATE			0x01   //Command的派生类的Create函数
#define  PLUGFUNC_TYPE_CREATE_COLLECT   0x03   // 采集命令
#define  PLUGFUNC_TYPE_CREATE_EDIT      0x05   // 编辑命令
#define  PLUGFUNC_TYPE_SELECT		    0x0D   // 选择命令
#define  PLUGFUNC_TYPE_PROCESS		    0x10   //一般命令响应函数



MyNameSpaceBegin


typedef void (*LPPROC_DOCMAP)();

typedef LONG_PTR FUNC_PTR;

class EXPORT_EDITBASE CPlugBase 
{
public:
	CPlugBase();	
	virtual ~CPlugBase();
	//插件对象生成之后的初始化处理
	virtual void Init(){};
	//返回插件的名称，在宿主程序菜单栏中与整个插件对应的popup菜单字符串
	virtual CString GetPlugName(){return CString();};
	//返回插件类型PLUG_TYPE_CMD或PLUG_TYPE_DOC
	inline int GetPlugType(){return m_nType;}
	//命令项的个数
	virtual int GetItemCount() = 0;
	//工具条的标题
	virtual CString GetToolBarTitle() = 0; 
	//工具条菜单界面位图的ID（位图资源）
	virtual UINT GetToolBarBmpResID() = 0;
	// 插件命令项ID
	virtual const UINT * GetItemCommandIDs() = 0;
	//对应各个命令的菜单项的字符串
	virtual const UINT * GetItemMenuStrResIDs() = 0;
	//各个工具栏命令对应的提示字符串ID
	virtual const UINT * GetItemNamesResIDs() = 0;
	//各个命令对应的加速键
	virtual const char ** GetItemShortCuts() = 0;
	//各个命令对应的命令行
	virtual const char ** GetItemCmdLines() = 0;
	//各个命令对应的函数指针
	virtual const FUNC_PTR* GetFuncPtr() = 0;
	//各个命令对应的函数的类型
	virtual const int * GetFuncTypes() = 0;
	//可在此处释放插件所占用的资源
	virtual void Release() = 0;

	// 定制停靠窗口
	virtual CDockablePane* GetDockingControlBar() { return NULL; };
	// 停靠窗口标题
	virtual CString GetDockingControlBarTitle() { return CString();};
	// 停靠窗口类型，左上(0)，左下(1)，右上(2)，右下(3)，底部(4)，分别与主界面对应位置窗口合并
	virtual int GetDockingControlBarType() { return -1; };

	//加载插件数据
	virtual void LoadPluginData(DocId docId) {};
protected:
	int m_nType;
	
};

//命令类型
class EXPORT_EDITBASE CPlugCmds: public  CPlugBase
{	
public:
	CPlugCmds();	
	virtual ~CPlugCmds();

protected:	
};
 
// 文档定制类型
class EXPORT_EDITBASE CPlugDocs:public CPlugBase
{
public:
	CPlugDocs();
	virtual~CPlugDocs();
	//文档存储文件的后缀名
	virtual CString GetExtString()=0;
	//范围管理持久化的Access对象
	virtual CAccessPlugin *GetAccessObj()=0;

protected:
private:
};

//导入导出
class EXPORT_EDITBASE CPlugFiles: public  CPlugBase
{	
public:
	CPlugFiles();	
	virtual ~CPlugFiles();
//	virtual void ProcessData() = 0;
	
protected:	
};

MyNameSpaceEnd
#endif // !defined(AFX_PLUGBASE_H__EA4B3EF4_EA2D_4291_A89C_8B2FFBD1AFD3__INCLUDED_)
