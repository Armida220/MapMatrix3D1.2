#include "StdAfx.h"
#include "mm3d_v8handler.h"
#include <string>
#include "IOsgDomGenerator.h"
#include "IOsgDsmGene.h"
#include "IOsgClip.h"
#include "ScriptExecutor.h"
#include "mm3dDll.h"
#include <sstream> 

using namespace std;

extern Cmm3dDllApp theApp;

extern vector<string> split_black_space(string s);
/**
* @brief replace_all 将字符串中的所有出现的字符替换成另一个字符
* @param s 字符串
* @param t 要被替换的字符
* @param w 用来替换的字符
*/
void replace_all(std::string& s, std::string const & t, std::string const & w)
{
	string::size_type pos = s.find(t), t_size = t.size(), r_size = w.size();
	while (pos != std::string::npos){ // found   
		s.replace(pos, t_size, w);
		pos = s.find(t, pos + r_size);
	}
}

//模板函数：将string类型变量转换为常用的数值类型（此方法具有普遍适用性）  
template <class Type>
Type stringToNum(const string& str)
{
	istringstream iss(str);
	Type num;
	iss >> num;
	return num;
}

CMM3DV8Handler::CMM3DV8Handler(CefRefPtr<CefBrowser> browser)
	: mBrowser(browser)
{
}

CMM3DV8Handler::~CMM3DV8Handler(void)
{
}

bool CMM3DV8Handler::Execute(const CefString& func_name,
							 CefRefPtr<CefV8Value> object,
							 const CefV8ValueList& arguments,
							 CefRefPtr<CefV8Value>& retval,
							 CefString& exception)
{
	if (arguments.size() == 0)
	{
		if (func_name == _T("openSceneDir"))
		{
			CFileDialog dialog(TRUE, NULL, NULL, OFN_HIDEREADONLY, (LPCTSTR)_TEXT("3D model Files (*.osgb, *.osg)|*.osgb;*.osg;|All Files (*.*)|*.*||"), NULL);

			if (dialog.DoModal() == IDOK)
			{
				string pathName = dialog.GetPathName();
				replace_all(pathName, "\\", "\/");
				string selFileName = "setSceneFile('" + pathName + "');";
				CScriptExecutor::getInstance()->excuteScript(selFileName);
			}
		}
		else if (func_name == _T("openClipDir"))
		{
			CFileDialog dialog(FALSE, NULL, NULL, OFN_HIDEREADONLY, (LPCTSTR)_TEXT("3D model Files (*.osgb, *.osg)|*.osgb;*.osg;|All Files (*.*)|*.*||"), NULL);

			if (dialog.DoModal() == IDOK)
			{
				string pathName = dialog.GetPathName();

				if (dialog.GetFileExt() == _T(""))
				{
					pathName += ".osgb";
				}

				replace_all(pathName, "\\", "\/");
				string selFileName = "setOutFile('" + pathName + "');";
				CScriptExecutor::getInstance()->excuteScript(selFileName);
			}
		}
		else if (func_name == _T("openOutDir"))
		{
			CFileDialog dialog(FALSE, NULL, NULL, OFN_HIDEREADONLY, (LPCTSTR)_TEXT("dom File (*.tif)|*.tif;|All Files (*.*)|*.*||"), NULL);

			if (dialog.DoModal() == IDOK)
			{
				string pathName = dialog.GetPathName();

				if (dialog.GetFileExt() == _T(""))
				{
					pathName += ".tif";
				}
				
				replace_all(pathName, "\\", "\/");
				string saveFileName = "setOutFile('" + pathName + "');";
				CScriptExecutor::getInstance()->excuteScript(saveFileName);
			}
		}
		else if (func_name == _T("closeWindow"))
		{
			SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CLOSE, 0, 0);
		}
	}
	else if (arguments.size() == 3)
	{
		if (func_name == _T("produceDom"))
		{
			string inFile = arguments[0]->GetStringValue();
			string outFile = arguments[1]->GetStringValue();
			string strRes = arguments[2]->GetStringValue();
			double res = stringToNum<double>(strRes);
			replace_all(inFile, "\/", "\\");
			replace_all(outFile, "\/", "\\");
			IOsgDomGeneratorFactory::create()->generateDOM(inFile, outFile, res);
		}
		else if (func_name == _T("produceDsm"))
		{
			string inFile = arguments[0]->GetStringValue();
			string outFile = arguments[1]->GetStringValue();
			string strRes = arguments[2]->GetStringValue();
			replace_all(inFile, "\/", "\\");
			replace_all(outFile, "\/", "\\");
			double res = stringToNum<double>(strRes);
			IOsgDsmGeneFactory::create()->generateDSM(inFile, outFile, res);
		}
		else if (func_name == _T("clipScene"))
		{
			string inFile = arguments[0]->GetStringValue();
			string outFile = arguments[1]->GetStringValue();
			string coord = arguments[2]->GetStringValue();
			vector<string> coords = split_black_space(coord);
			vector<osg::Vec3d> pol;

			for (int i = 0; i < coords.size(); i = i + 2)
			{
				double x = stringToNum<double>(coords[i]);
				double y = stringToNum<double>(coords[i + 1]);
				pol.push_back(osg::Vec3d(x, y, 0));
			}

			replace_all(inFile, "\/", "\\");
			replace_all(outFile, "\/", "\\");
			IOsgClipFactory::create()->accurateClip(inFile, outFile, pol);

		}
		
		
	}

	return true;
}


