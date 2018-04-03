#include "stdafx.h"

#include "OsgbView.h"
#include <sstream>
#include "IViewer.h"
#include "resource.h"
#include "qcomm.h"
#include <numeric> 
#include "Resource.h"
#include "MsgSender.h"
#include "GeomParser.h"
#include "BufParser.h"
#include "FtrParser.h"
#include "osgdef.h"
#include "ICallBack.h"
#include "OsgbVecView.h"
#include "GrBuffer.h"
#include "MainFrm.h"
#include "DlgInputCommand.h"
#include "ChildFrm.h"
#include "ShowImgDlg.h"
#include "CoordCenter.h"
#include "Ori.h"
#include <algorithm>
#include <list>
#include "qcomm.h"
#include "OsgCoordCenter.h"
#include "RegDef2.h"
#include "DlgCommand.h"
#include "myAbsOriDlg.h"
#include <sstream>
#include <iostream>
#include <regex>



using namespace tr1;
using namespace tr1::placeholders;
using namespace mm3dView;
using namespace osgCall;
using namespace std;


// 从SmartView/res/resource.h 中复制
#define ID_HDLGBAR                      100
#define IDD_DLGBAR_IMGVIEW              10001

// 主要用于使用SmartView.dll中的资源
static AFX_EXTENSION_MODULE SmartViewDLL;

/**
* @brief registerCmd 注册命令
* @param pWorker 命令的工作者
* @param id  命令的id号
*/
template<typename T>
void registerCmd(CWorker* pWorker, int id)
{
	pWorker->RegisterCommand(id, T::Create);
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

// 分割字符串 
template<class _sT, class _cT, class getSizeFun>
void split(const _sT &in, const _cT split_cahr, vector<_sT> &res, getSizeFun fun)
{
	_sT str = "";
	for (int i = 0; i < fun(in); i++)
	{
		_cT c = in[i];
		if (split_cahr == c)
		{
			if (fun(str) > 0)
			{
				res.push_back(str);
			}
			str = "";
		}
		else
		{
			str += c;
		}
	}
	if (fun(str) > 0)
	{
		res.push_back(str);
	}
}
// COsgbView

IMPLEMENT_DYNCREATE(COsgbView, CBaseView)

#define REGPATH_VIEW_OSGB		_T("Config\\Options\\OsgbView")

COsgbView::COsgbView() :
	pMsgSender(new CMsgSender(this)),
	pFtrParser(new CFtrParser(this)),
	//showImgDlg(nullptr),
	mouseIn(false),
	hintState(0),
	openFileFlag(FALSE)
{
	// 初始话SmartViewDLL
	AfxInitExtensionModule(SmartViewDLL,
	GetModuleHandle(_T("SmartView.dll")));

	str_Tips.LoadString(IDS_OSGB_TIPS);
	str_On.LoadString(IDS_ON);
	str_Off.LoadString(IDS_OFF);
	std::vector<CString> vec;
	split(str_Tips, _T(';'), vec, [](const CString & i){return i.GetLength(); });
	vec_tips.resize(vec.size());
	for (int i = 0; i < vec.size(); i ++)
	{
		vec_tips[i] = vec[i].Left(vec[i].GetLength() - 3);
	}
	OnUpdate(nullptr, hc_UpdateOption, nullptr);
	bBoundSet = false;
	bIsFit = false;
	memset(m_strRegSection, 0, 256 * sizeof(TCHAR));
	strcpy(m_strRegSection, REGPATH_VIEW_OSGB);

	bCatchCorner = false;
	bLockH = false;
	bNeedSetLockH = false;
	bShowImg = false;
	bSkipOnceLeftClick = false;
	m_bViewVector = true;
	posgVectorView = nullptr;
	
	shared_ptr<CMyAbsOriDlg> temp(new CMyAbsOriDlg());
	mAbsOriDlg = temp;
	mAbsOriDlg->Create(IDD_DIALOG_DOM, NULL);
}

COsgbView::~COsgbView()
{
	if (bShowImg)
	{
		CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		pMain->OnOsgbviewImg();
	}
}

BEGIN_MESSAGE_MAP(COsgbView, CBaseView)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_MESSAGE(WM_LOAD_SCENE, &COsgbView::OnLoadScene)
	ON_MESSAGE(WM_RM_ALL_SCENE, &COsgbView::OnRmAllScene)
	ON_MESSAGE(WM_LF_CLK, &COsgbView::OnVecViewLfClk)
	ON_MESSAGE(WM_OSG_MOUSE_MV, &COsgbView::OnVecViewMouseMv)
	ON_MESSAGE(WM_RIGHT_CLK, &COsgbView::OnVecViewRightClk)
	ON_MESSAGE(WM_PROMPT_LOCK, &COsgbView::OnPromptLockMsg)
	ON_MESSAGE(WM_PROMPT_CATCH, &COsgbView::OnPromptCatch)
	ON_MESSAGE(WM_BACK, &COsgbView::OnBack)
	ON_MESSAGE(WM_PROMPT_OPERA, &COsgbView::OnPromptOpera)
	ON_WM_SETCURSOR()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(ID_SELECT, OnUpdateSelect)
	ON_COMMAND(ID_SELECT, &COsgbView::OnSelect)
// 	ON_MESSAGE(WM_TOP_VIEW, &COsgbView::OnTopView)
// 	ON_MESSAGE(WM_LEFT_VIEW, &COsgbView::OnLeftView)
// 	ON_MESSAGE(WM_RIGHT_VIEW, &COsgbView::OnRightView)
	ON_MESSAGE(WM_PAN_XY, &COsgbView::OnPanXy)
	ON_MESSAGE(WM_ZOOM_TO, &COsgbView::OnZoomTo)
	
	ON_COMMAND(ID_OSGBVIEW_TOP, &COsgbView::OnTopView)
	ON_COMMAND(ID_OSGBVIEW_LEFT, &COsgbView::OnLeftView)
	ON_COMMAND(ID_OSGBVIEW_RIGHT, &COsgbView::OnRightView)
	ON_COMMAND(ID_OSGBVIEW_IMGDLG, &COsgbView::OnChangeShowImage)
	ON_COMMAND(ID_OSGBVIEW_HOME, &COsgbView::OnHome)
	ON_COMMAND(ID_OSGBVIEW_SHIFTOPERAT, &COsgbView::OnShiftOperation)
	ON_COMMAND(ID_OSGBVIEW_LOCKH, &COsgbView::OnLockHeight)
	ON_COMMAND(ID_OSGBVIEW_CATCHCORNER, &COsgbView::OnCatchCorner)
	
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(WM_SHOW_IMG, &COsgbView::OnShowImg)
	ON_MESSAGE(WM_CHANGE_CURPT, &COsgbView::OnChangeCurpt)
	ON_MESSAGE(WM_LEFTBTDOUBLECLICK, &COsgbView::OnLeftBtDoubleClick)
	
	ON_WM_MOUSELEAVE()
	ON_COMMAND(ID_WHITE_MODEL, &COsgbView::OnWhiteModel)
	ON_COMMAND(IDC_BUTTON_VIEWOVERLAY, &COsgbView::OnViewoverlay)
	
	ON_WM_NCCALCSIZE()
END_MESSAGE_MAP()

// COsgbView diagnostics

#ifdef _DEBUG
void COsgbView::AssertValid() const
{
	CSceneView::AssertValid();
}

#ifndef _WIN32_WCE
void COsgbView::Dump(CDumpContext& dc) const
{
	CSceneView::Dump(dc);
}
#endif
#endif //_DEBUG

void COsgbView::showAbsOriDlg()
{
	mAbsOriDlg->ShowWindow(SW_SHOW);

	IViewer->fetchOriData();
}

void COsgbView::hideAbsOriDlg()
{
	mAbsOriDlg->ShowWindow(SW_HIDE);

	IViewer->clearOriPt();
}

double COsgbView::getScale()
{
	return IViewer->getScale();
}

double COsgbView::getScaleFromDist()
{
	return IViewer->getScaleFromDist();
}

void COsgbView::initOsgScene(string fileName)
{
	ary_filename.push_back(fileName);

	if (ary_filename.size() > 0)
	{
		this->Invalidate(TRUE);
	}
}

bool COsgbView::isLinePick(const double x, const double y) const
{
	return IViewer->isLinePick(x, y);
}


vector<double> COsgbView::getLinePick(const double x, const double y) const
{
	return IViewer->linePickPt(x, y);
}

CDlgDoc* COsgbView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDlgDoc)));
	return (CDlgDoc*)m_pDocument;
}

bool COsgbView::isSelect()
{
	bool callIsSel = bisSel;

	bisSel = false;
	return callIsSel;
}

void COsgbView::setSelect(const bool bisSelect)
{
	bisSel = bisSelect;
}

double COsgbView::getSelDist()
{
	CWorker* work = GetDocument();
	CSelection* mSel = work->GetSelection();

	double sView = work->m_coordwnd.m_lfScaleView;
	double sSearch = work->m_coordwnd.m_lfScaleSearch;
	double radius = mSel->GetSelectRadius();

	double selDist = radius / sSearch;
	return selDist;
}

Envelope COsgbView::getOsgbExtent()
{
	getScenceBound();
	bIsFit = false;
	return m_scenceBound;
}

void COsgbView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	int id = (int)lHint;

	switch (lHint)
	{
		case hc_AddObject:
			{
				CFeature *pFtr = (CFeature*)pHint;

				if (!pFtr)
					break;
				addObj(pFtr);
				break;
			}
		case hc_DelObject:
			{
				CFeature *pFtr = (CFeature*)pHint;

				if (!pFtr)
					break;
				removeObj(pFtr);

				break;
			}
		case hc_DelAllObjects:
			{
				break;
			}
		case hc_SelChanged:
			{
				//高亮同步
				synHighLigh();
				break;
			}
		case hc_Refresh:
			{
				synchronize();
				synHighLigh();
				break;
			}
		case hc_ClearDragLine:
			{
				ClearDragLine();
				break;
			}
		case hc_SetConstDragLine:
			{
				const GrBuffer *pBuf = (const GrBuffer*)pHint;
				SetConstDragLine(pBuf);
				break;
			}
		case hc_AddConstDragLine:
			{
				const GrBuffer *pBuf = (const GrBuffer*)pHint;
				AddConstDragLine(pBuf);
			}
			break;
		case hc_AddVariantDragLine:
			{
				const GrBuffer *pBuf = (const GrBuffer*)pHint;
				AddVariantDragLine(pBuf);
				break;
			}
		case hc_SetVariantDragLine:
			{
				const GrBuffer *pBuf = (const GrBuffer*)pHint;
				SetVariantDragLine(pBuf);
				break;
			}
		case hc_UpdatesnapDraw:
			{
				const SNAPITEM *t0 = (const SNAPITEM*)pHint;
				updateSnap(t0);
			
				break;
			}
		case hc_UpdateObjectDisplay:
			{
				UpdateDispyParam  * param = (UpdateDispyParam  *)pHint;
				if (param)
				{
					updateObjectVisibal(param);
				}
				else
				{
					synchronize();
					synHighLigh();
				}
				break;
			}
		case hc_eventStart:
			{
				int *order = (int*)pHint;
				IViewer->startEvent(*order);
				break;
			}
		case hc_UpdateAllObjects:
		case hc_UpdateAllObjects_Visible:
		case hc_UpdateAllObjects_VectorView:
		{
			synchronize();
			synHighLigh();

			break;
		}
		case hc_UpdateLayerDisplay:
		{
			updateLayerVisibal((const UpdateDispyParam*)pHint);
			break;
		}
		case hc_SetCursorType:
		{
			SetCursorType((long)pHint);
			break;
		}
		case hc_AddGraph:
		{
			const GrBuffer *pBuf = (const GrBuffer*)pHint;
			addGraph(pBuf);
			break;
		}
		case hc_DelGraph:
		{
			delGraph();
			break;
		}
		case hc_handMove:
		{
			COsgbVecView* pView = getOsgbVecView();
			if (pView)
			{
				const PT_3D *t0 = (const PT_3D*)pHint;
				panTo(t0->x, t0->y);
			}
			break;
		}
		case hc_zoomScale:
		{
			COsgbVecView* pView = getOsgbVecView();

			if (pView)
			{
				double scale = pView->getScale();
				zoomTo(scale);
			}
			const PT_3D *t0 = (const PT_3D*)pHint;
			if (t0 && pView)
			{
				panTo(t0->x, t0->y);
			}
			break;
		}
		case hc_SetCrossPos:
		{
			bool senderIsVVN =pSender && 
				(pSender->GetRuntimeClass() == RUNTIME_CLASS(CVectorView_new));
			const PT_3D *t0 = (const PT_3D*)pHint;
			if (!MouseIn())
			{
				double _x, _y;

				GetClientRect(osgFrameRect);
				IViewer->groundToScreen(t0->x, t0->y, t0->z, _x, _y);
				m_cursorL.SetPos(_x - 12, osgFrameRect.Height() - _y - 13);
				GetDC()->DrawIcon(_x - 12, osgFrameRect.Height() - _y - 13, m_cursorL.GetSysCursor());
			}
// 			if ((!MouseIn() && (hintState & HINT_SETCROSS)) || !senderIsVVN)
// 			{
// 				panTo(t0->x, t0->y);
// 			}		
			break;
		}
		case hc_SetOsgCrossPos:
		{
			if (!MouseIn() && (hintState & HINT_SETOSGCROSS))
			{
				const PT_3D *t0 = (const PT_3D*)pHint;
				panTo(t0->x, t0->y);
			}
			break;
		}
		case hc_SetAnchorPoint:
		{
			const PT_3D *t0 = (const PT_3D*)pHint;
			panTo(t0->x, t0->y);

			break;
		}
		case hc_UpdateDataColor:{
			CDlgDataSource * pDS = (CDlgDataSource *)pHint;
			if (pDS)
			{
				std::vector<osgCall::osgFtr> vecFtr;
				pFtrParser->getDataSourceInfo(pDS, vecFtr);
				IViewer->updataDate(vecFtr);
			}
			break;
		}
		case hc_UpdateOption:
		{
			m_clrSnap = AfxGetApp()->GetProfileInt(REGPATH_SNAP, REGITEM_SNAPCOLOR, m_clrSnap);
			break;
		}
		case hc_absOri:
		{
			string *oriInfo = (string*)pHint;
			std::string reportStr = IViewer->absOri(*oriInfo);

			mAbsOriDlg->reloadReportHtml(reportStr);

			break;
		}
		case hc_goto:
		{
			string* gotoInfo = (string*)pHint;
			vector<string> strs;
			split(*gotoInfo, _T(';'), strs, [](const string & i){return i.length(); });
			
			if (strs.size() == 3)
			{
				double relaX = stringToNum<double>(strs[0]);
				double relaY = stringToNum<double>(strs[1]);
				double relaZ = stringToNum<double>(strs[2]);

				IViewer->driveToRelaPt(relaX, relaY, relaZ);
			}
			break;
		}
		case hc_showOriData:
		{
			string* oriData = (string*)pHint;
			IViewer->showOriPt(*oriData);
			break;
		}
		case hc_predictOriData:
		{
			string* oriData = (string*)pHint;
			string oriPt = IViewer->predictOriPt(*oriData);

			mAbsOriDlg->setVmData(oriPt);

			break;
		}
		//case hc_
	}
}


void COsgbView::getScenceBound()
{
	if (!bBoundSet)
	{
		double left = 0; double right = 0; double top = 0; double bottom = 0;
		IViewer->getOsgbExtent(left, right, top, bottom);

		PT_2D rect[2] = { PT_2D(left, top), PT_2D(right, bottom) };
		m_scenceBound.CreateFromPts(rect, 2);
		bBoundSet = true;
	}
}

bool COsgbView::isFit()
{
	getScenceBound();
	if (bIsFit) return true;
	CWorker * pWorker = GetWorker();
	if (!pWorker) return false;
	int nds = pWorker->GetDataSourceCount();
	if (nds <= 0) return false;
	Envelope e;
	for (int i = 0; i < nds; i++)
	{
		Envelope e0 = pWorker->GetDataSource(i)->GetBound();
		e.Union(&e0);
	}
	bIsFit = m_scenceBound.bIntersect(&e);
	return bIsFit;
}

void COsgbView::OnDraw(CDC* pDC)
{
	if (!ary_filename.empty())
	{
		string strModel = ary_filename[0];
		bBoundSet = false;
		bIsFit = false;
		if (openFileFlag == TRUE)
		{
			ary_filename.clear();

			//增加场景节点
			addScene(strModel);

			//数据同步
			synchronize();
			//高亮同步
			synHighLigh();

			Sleep(1000);

			//显示初始视角
			IViewer->home();
			IViewer->light();
		}
		else if (openFileFlag == FALSE)
		{
			openFileFlag = TRUE;
			ary_filename.clear();

			//初始化模型
			IViewer->InitOSG(strModel);
			//设置范围
			IViewer->SetSceneExtent();

			//启动线程
			mThreadHandle = IViewer->StartRendering();

			//数据同步
			int sn = synchronize();
			//高亮同步
			synHighLigh();
			
			if (sn < 10)
				Sleep(100);
			//关闭光源
			IViewer->light();
		}
		getInitRect();
		hideImgDlg();

	}
	
	
}

BOOL COsgbView::LoadDocData()
{
	return TRUE;
}

void COsgbView::addScene(const string &strModel) const
{
	IViewer->addScene(strModel);
}

void COsgbView::removeAllScene() const
{
	IViewer->removeAllScene();
}

void COsgbView::setNoDragCur()
{
	::SetCursor(m_cursorL.GetSysCursor());
}

void COsgbView::setDragCur()
{
	::SetCursor(m_hHandCursor);
}

void COsgbView::SetCursorType(long type)
{
	switch (type)
	{
	case CURSOR_DISABLE:
		EnableCrossSelMarkVisible(FALSE);
		EnableCrossCrossVisible(FALSE);
		break;
	case CURSOR_NORMAL:
		EnableCrossSelMarkVisible(TRUE);
		EnableCrossCrossVisible(TRUE);
		break;
	case CURSOR_SELECT:
		EnableCrossSelMarkVisible(TRUE);
		EnableCrossCrossVisible(FALSE);
		break;
	case CURSOR_DRAW:
		EnableCrossSelMarkVisible(FALSE);
		EnableCrossCrossVisible(TRUE);
		break;
	default:;
	}

	UpdateCross();
}

void COsgbView::addObj(CFeature* pFtr)
{
// 	CFtrLayer* layer = GetDocument()->GetDlgDataSource()->GetFtrLayerOfObject(pFtr);
// 	COLORREF clr = layer->GetColor();
// 	double r = GetRValue(clr) / 255.0;
// 	double g = GetGValue(clr) / 255.0;
// 	double b = GetBValue(clr) / 255.0;
// 	PT_3D rgb(r, g, b);
// 
// 	COsgbVecView* posgVectorView = getOsgbVecView();
// 
// 	if (posgVectorView == nullptr)
// 	{
// 		return;
// 	}
// 
// 	GrBuffer* buf = posgVectorView->AddObj(pFtr);
// 	
// 	if (buf == nullptr)
// 	{
// 		return;
// 	}
// 
// 	OUID id = pFtr->GetID();
// 	string idStr = id.ToString();
// 	const char *code = pFtr->GetCode();
// 	BOOL bLocal = (code == NULL || strlen(code) <= 0);

	vector<osgFtr> vecFtr;
// 	CBufParser parser(idStr, rgb, bLocal);
// 	parser.getBufData(buf, vecFtr);
// 	delete buf;
// 	buf = nullptr;

	pFtrParser->getAllFtrInfo(pFtr, vecFtr);
	IViewer->addObj(vecFtr);
	
	return;
	
}

void COsgbView::removeObj(CFeature* pFtr)
{
	OUID id = pFtr->GetID();
	string idStr = id.ToString();
	if ((!m_hideIDS.empty()) && isHide(idStr))
	{
		m_hideIDS.erase(idStr);
	}
	vector<string> vecFtr;
	vecFtr.push_back(idStr);
	auto iter = m_map_featerID_WhiteModelID.find(idStr);
	if (iter != m_map_featerID_WhiteModelID.end())
	{
		vecFtr.push_back(iter->second.id);
		m_map_featerID_WhiteModelID.erase(iter);
	}
	IViewer->removeObj(vecFtr);
}


int COsgbView::synchronize()
{
	CDlgDoc* dlg = GetDocument();
	int dataSourceCount = dlg->GetDlgDataSourceCount();
	vector<osgFtr> vecFtr;
	for (int i = 0; i < dataSourceCount; i++)
	{

		CDlgDataSource* dataSource = dlg->GetDlgDataSource(i);
		pFtrParser->getDataSourceInfo(dataSource, vecFtr);
	}
	
	synData(vecFtr);
	return vecFtr.size();
}

void COsgbView::synHighLigh()
{
	
	CDlgDataSource * dlgDataSource = GetDocument()->GetDlgDataSource();
	double scale = dlgDataSource->GetSymbolDrawScale();

	vector<osgFtr> vecSelFtr;
	CSelection* mSel = GetDocument()->GetSelection();
	int numSel = 0;
	const LONG_PTR* lptr = mSel->GetSelectedObjs(numSel);

	for (int i = 0; i < numSel; i++)
	{
		CFeature* pFtr = HandleToFtr(lptr[i]);
		pFtrParser->getAllFtrInfo(pFtr, vecSelFtr, true);
	}

	CView* pActiveView = GetActiveView();

	if (pActiveView == nullptr)
	{
		return;
	}

	if (pActiveView->IsKindOf(RUNTIME_CLASS(COsgbView)))
	{
		//同步高亮点、线、面
		IViewer->synHighLightData(vecSelFtr, true);
	}
	else
	{
		//同步高亮点、线、面
		IViewer->synHighLightData(vecSelFtr, true);

		COsgbVecView* pView = getOsgbVecView();

		if (pView)
		{
			if (!vecSelFtr.empty())
			{
				double minx, maxx, miny, maxy;
				bool initMinMax = false;
				
				for ( const osgFtr & osgFtr : vecSelFtr)
				{
					for (const Coord & c : osgFtr.vecCoord)
					{
						const double &_x = c.x;
						const double &_y = c.y;
						if (!initMinMax)
						{
							initMinMax = true;
							minx = maxx = _x;
							miny = maxy = _y;
							continue;
						}

						if (_x > maxx) maxx = _x;
						if (_y > maxy) maxy = _y;

						if (_x < minx) minx = _x;
						if (_y < miny) miny = _y;
					}
				}
				if (initMinMax)
				{
					IViewer->panTo((minx + maxx) / 2, (miny + maxy)/2);
				}
				else
				{
					PT_3D pt = pView->m_gCurPoint;
					IViewer->panTo(pt.x, pt.y);
				}
			}
		}
	}
}

void COsgbView::SetConstDragLine(const GrBuffer *pBuf)
{
	if (!pBuf || !pBuf->HeadGraph())
	{
		vector<osgFtr> vecFtr;
		//同步画点、线、面
		IViewer->setConstDragLine(vecFtr);
		return;
	}

	if (!pBuf->HeadGraph())
		return;

	vector<osgFtr> vecFtr;

	string idStr = "";
	PT_3D curlayerClr = getCurLayerColor();

	//解析buf
	CBufParser parser(idStr, curlayerClr);
	parser.getBufData(pBuf, vecFtr);

	//同步画点、线、面
	IViewer->setConstDragLine(vecFtr);
}

void COsgbView::AddConstDragLine(const GrBuffer *pBuf)
{
	if (!pBuf)return;
	if (!pBuf->HeadGraph())
		return;

	vector<osgFtr> vecFtr;
	string idStr = "";
	PT_3D curlayerClr = getCurLayerColor();

	//解析buf
	CBufParser parser(idStr, curlayerClr);
	parser.getBufData(pBuf, vecFtr);

	//同步画点、线、面
	IViewer->addConstDragLine(vecFtr);
}

void COsgbView::SetVariantDragLine(const GrBuffer *pBuf)
{
	if (!pBuf || !pBuf->HeadGraph())
	{
		vector<osgFtr> vecFtr;
		IViewer->setVariantDragLine(vecFtr);
		return;
	}

	vector<osgFtr> vecFtr;
	string idStr = "";
	PT_3D dragLineClr = getDragLineClr();

	//解析buf
	CBufParser parser(idStr, dragLineClr);
	parser.getBufData(pBuf, vecFtr);

	//同步画点、线、面
	IViewer->setVariantDragLine(vecFtr);
}

void COsgbView::AddVariantDragLine(const GrBuffer *pBuf)
{
	if (!pBuf)return;
	if (!pBuf->HeadGraph())
		return;

	vector<osgFtr> vecFtr;
	string idStr = "";
	PT_3D dragLineClr = getDragLineClr();

	//解析buf
	CBufParser parser(idStr, dragLineClr);
	parser.getBufData(pBuf, vecFtr);

	//同步画点、线、面
	IViewer->addVariantDragLine(vecFtr);
}

void COsgbView::UpdatesnapDraw(const GrBuffer* pBuf)
{
	if (!pBuf || !pBuf->HeadGraph())
	{
		vector<osgFtr> vecFtr;
		IViewer->UpdatesnapDraw(vecFtr);
		return;
	}

	vector<osgFtr> vecFtr;
	string idStr = "";

	PT_3D curlayerClr = getCurLayerColor();
	//解析buf
	CBufParser parser(idStr, curlayerClr);
	parser.getBufData(pBuf, vecFtr);

	//同步画点、线、面
	IViewer->UpdatesnapDraw(vecFtr);
}

void COsgbView::addGraph(const GrBuffer* pBuf)
{
	if (!pBuf)return;
	if (!pBuf->HeadGraph())
		return;

	vector<osgFtr> vecFtr;
	string idStr = "";
	PT_3D curlayerClr = getCurLayerColor();

	//解析buf
	CBufParser parser(idStr, curlayerClr);
	parser.getBufData(pBuf, vecFtr);

	//同步画点、线、面
	IViewer->addGraph(vecFtr);
}

void COsgbView::delGraph()
{
	IViewer->delGraph();
}

void COsgbView::ClearDragLine()
{
	IViewer->clearDragLine();
}

void COsgbView::driveTo(double cx, double cy)
{
	IViewer->driveTo(cx, cy);
}

void COsgbView::panTo(double cx, double cy)
{
	IViewer->panTo(cx, cy);
}

void COsgbView::zoomTo(double scale)
{
	IViewer->zoomTo(scale);
}

GrBuffer COsgbView::makeCellwithCoord(const CellDef &cell, const PT_3D &trans, const double scale)
{
	GrBuffer buf;
	buf.AddBuffer(cell.m_pgr, 0);

	//设置偏移量和缩放比例，db[0]是x方向缩放比例，db[5]是y方向缩放比例
	//db[10]是z方向缩放比例
	double db[16] = { 0 };
	db[0] = scale; db[5] = scale; db[10] = scale;
	db[3] = trans.x; db[7] = trans.y; db[11] = trans.z;
	buf.Transform(db);

	return buf;
}

PT_3D COsgbView::getCurLayerColor()
{
	CDlgDoc* dlgDoc = GetDocument();
	CDlgDataSource* dataSource = dlgDoc->GetDlgDataSource();
	CFtrLayer* curlayer = dataSource->GetCurFtrLayer();
	COLORREF curClr = curlayer->GetColor();
	double r = GetRValue(curClr) / 255.0;
	double g = GetGValue(curClr) / 255.0;
	double b = GetBValue(curClr) / 255.0;

	return PT_3D(r, g, b);
}

PT_3D COsgbView::getDragLineClr()
{
	COsgbVecView* pOsgVecView =	getOsgbVecView();
	if (!pOsgVecView) return PT_3D(255, 255, 255);
	COLORREF clrDragLine = pOsgVecView->getDragLineClr();

	double r = GetRValue(clrDragLine) / 255.0;
	double g = GetGValue(clrDragLine) / 255.0;
	double b = GetBValue(clrDragLine) / 255.0;

	return PT_3D(r, g, b);
}

//
//// COsgbView message handlers

void COsgbView::OnInitialUpdate()
{
	ASSERT(GetDocument()->IsKindOf(RUNTIME_CLASS(CDlgDoc)));
	CDlgDoc* pDoc = dynamic_cast<CDlgDoc*>(GetDocument());

	//CBaseView::OnInitialUpdate();
	IViewer = IOsgMfcViewerFactory::create(m_hWnd);
	openFileFlag = FALSE;

	typedef function<void(double, double, double)> fun;
	typedef function<void(double, double, double, bool)> fun1;
	typedef function<void()> fun2;
	typedef function<bool()> fun3;
	typedef function<void(bool)> fun4;
	typedef function<void(double, double, double, int)> fun6;
	
	fun funLfClk = bind(&CMsgSender::sendMsgLfClk, pMsgSender, _1, _2, _3);
	fun funMouseMv = bind(&CMsgSender::sendMsgMouseMove, pMsgSender, _1, _2, _3);
	fun funRightClk = bind(&CMsgSender::sendMsgRightClk, pMsgSender, _1, _2, _3);
	fun4 funPromptLock = bind(&CMsgSender::sendMsgPromptLock, pMsgSender, _1);
	fun4 funPromptCatch = bind(&CMsgSender::sendMsgPromptCatch, pMsgSender, _1);
	//fun2 funKeyDownBack = bind(&CMsgSender::sendMsgBkspace, pMsgSender);
	fun2 funKeyDownBack = bind(&COsgbView::bkSpcaeLastOps, this);
	fun4 funPromptOpera = bind(&CMsgSender::sendMsgPromptOpera, pMsgSender, _1);
	fun funPan = bind(&CMsgSender::sendMsgPan, pMsgSender, _1, _2, _3);
	fun funZoom = bind(&CMsgSender::sendMsgZoom, pMsgSender, _1, _2, _3);
	fun funShowImg = bind(&CMsgSender::sendMsgShowImg, pMsgSender, _1, _2, _3);
	fun6 funDBC = bind(&CMsgSender::sendMsgDoubleClick, pMsgSender, _1, _2, _3, _4);
	fun funChangeAbsOriDlg = bind(&CMsgSender::sendMsgChangeAbsOriDlg, pMsgSender, _1, _2, _3);
	fun2 funFetchOriData = bind(&CMsgSender::sendMsgFetchOriData, pMsgSender);

	shared_ptr<osgCall::ICallBack> ic = osgCall::ICallFactory::create(funLfClk,
		funMouseMv, funRightClk, funPromptLock, funPromptCatch, funKeyDownBack,
		funPromptOpera, funPan, funZoom, funShowImg, funDBC, funChangeAbsOriDlg, funFetchOriData);
	IViewer->setCall(ic);

	// TODO: Add your specialized code here and/or call the base class
	LoadDocData();
	registerCmd<CSetWhiteModelCommand>(GetWorker(), ID_WHITE_MODEL);
//	GetWorker()->RegisterCommand( , CSetWhiteModelCommand::Create);

}

afx_msg LRESULT COsgbView::OnLoadScene(WPARAM wParam, LPARAM lParam)
{
	LoadDocData();
	return 0;
}

afx_msg LRESULT COsgbView::OnRmAllScene(WPARAM wParam, LPARAM lParam)
{
	removeAllScene();
	return 0;
}

afx_msg LRESULT COsgbView::OnVecViewLfClk(WPARAM wParam, LPARAM lParam)
{
	bisSel = false;
	//获得的点信息
	stPoint* sp = (stPoint*)(wParam);
	double* osgMat = nullptr;// IViewer->getScrren2WorldMat();

	PT_3DEX pt;
	pt.x = sp->mX;
	pt.y = sp->mY;
	pt.z = sp->mZ;

	char strMsg[MAX_PATH];
	sprintf_s(strMsg, "osgbview grd pt: %lf %lf %lf\n", pt.x, pt.y, pt.z);
	_logop(strMsg);

	//使用继承的vecview 里面修改了一些东西
	COsgbVecView* vecView = bSkipOnceLeftClick ? nullptr : getOsgbVecView();
	bSkipOnceLeftClick = false;
	if (vecView != nullptr)
	{
		vecView->leftClk(&pt, osgMat);
		if (bShowImg)
			SendMessage(WM_SHOW_IMG, (WPARAM)&vecView->m_gCurPoint, 1);
	}

	return 0;
}

afx_msg LRESULT COsgbView::OnVecViewMouseMv(WPARAM wParam, LPARAM lParam)
{
	
	//使用继承的vecview 里面修改了一些东西
	COsgbVecView* vecView = getOsgbVecView();

	if (vecView != nullptr)
	{
		//获得的点信息
		stPoint* sp = (stPoint*)(wParam);

		//PT_3DEX pt;
		PT_3D pt;
		pt.x = sp->mX;
		pt.y = sp->mY;
		pt.z = sp->mZ;

		vecView->mouseMove(&pt);
	}

	return 0;
}

afx_msg LRESULT COsgbView::OnVecViewRightClk(WPARAM wParam, LPARAM lParam)
{
	if (GetKeyState(VK_CONTROL) < 0)
	{
		
	}
	else
	{
		//获得的点信息
		stPoint* sp = (stPoint*)(wParam);

		PT_3DEX pt;
		pt.x = sp->mX;
		pt.y = sp->mY;
		pt.z = sp->mZ;

		//使用继承的vecview 里面修改了一些东西
		COsgbVecView* vecView = getOsgbVecView();

		if (vecView != nullptr)
		{
			vecView->rightClk(&pt);
		}
	}

	//增加提示
// 	CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
// 	CString strPrompt = pMain->m_wndOutputBar.m_wndInEdit.GetPrompt();
	
	/*
	if (strPrompt != "")
	{
		// = _T("锁定高程(1);左视角(2);右视角(3);顶视角(4);捕捉内角点(5);切换操作方式(6);前方交会界面(7);重置视角(8)");
		strPrompt += str_Tips;
		pMain->m_wndOutputBar.m_wndInEdit.SetPrompt(strPrompt);
	}
	*/
	return 0;
}

afx_msg LRESULT COsgbView::OnPromptLockMsg(WPARAM wParam, LPARAM lParam)
{
	bool bLock = (bool)wParam;
	m_ctrlBarHorz.SetButtonState2(ID_OSGBVIEW_LOCKH, TRUE, bLock);
	if (bLock == bLockH && bLockH == false)
	{
		CString str;
		str.LoadString(IDS_PICKHEIGHT_FAILE);
		OutputMessage(str);
		return 0; 
	}

	CString str = vec_tips[0] + _T(" ");
	if (bLock && bNeedSetLockH) bSkipOnceLeftClick = true;
	bNeedSetLockH = false;
	bLockH = bLock;
	if (bLock)
	{
		//增加提示
		OutputMessage(str + str_On);
	}
	else
	{
		OutputMessage(str + str_Off);
	}
	
	return 0;
}

afx_msg LRESULT COsgbView::OnPromptCatch(WPARAM wParam, LPARAM lParam)
{
	bool bIsCatch = (bool)wParam;
	CString str = vec_tips[4] + _T(" ");
	bCatchCorner = bIsCatch;
	if (bIsCatch)
	{
		//增加提示
		OutputMessage(str + str_On);
	}
	else
	{
		OutputMessage(str + str_Off);
	}
	m_ctrlBarHorz.SetButtonState2(ID_OSGBVIEW_CATCHCORNER, TRUE, bIsCatch);

	return 0;
}

afx_msg LRESULT COsgbView::OnBack(WPARAM wParam, LPARAM lParam)
{
	//使用继承的vecview 里面修改了一些东西
	COsgbVecView* vecView = getOsgbVecView();

	if (vecView != nullptr)
	{
		vecView->SendMessage(WM_KEYDOWN, VK_BACK, 1);
	}

	return 0;
}

afx_msg LRESULT COsgbView::OnPromptOpera(WPARAM wParam, LPARAM lParam)
{
	bool bShiftOpera = (bool)wParam;
	CString str;
	if (bShiftOpera)
	{
		str.LoadString(IDS_OPERATOR_SHIFT);
	}
	else
	{
		str.LoadString(IDS_OPERATOR_MOUSEPRESS);
	}
	OutputMessage(str);

	return 0;
}

afx_msg LRESULT COsgbView::OnPanXy(WPARAM wParam, LPARAM lParam)
{
	
	stPoint* pt = (stPoint*)wParam;

	COsgbVecView* pView = getOsgbVecView();

	if (pView)
	{
		PT_3D xyz(pt->mX, pt->mY, pt->mZ);
		pView->DriveToXyz(&xyz, true);

		//CDlgDoc *pDoc = GetDocument();
		//pDoc->UpdateAllOtherViews(hc_SetCrossPos, (CObject*)&xyz);
	}

	return 0;
}

afx_msg LRESULT COsgbView::OnZoomTo(WPARAM wParam, LPARAM lParam)
{
	stPoint* pt = (stPoint*)wParam;

	COsgbVecView* pView = getOsgbVecView();

	if (pView)
	{
		pView->adjustScale(pt->mX, pt->mY);

		PT_3D xyz(pt->mX, pt->mY, 0);
		CDlgDoc *pDoc = GetDocument();
		pDoc->UpdateAllOtherViews(hc_SetCrossPos, (CObject*)&xyz);
	}

	return 0;
}

#define CMDFUN(fun, ifun)  afx_msg void COsgbView::fun(){ifun;}

CMDFUN(OnTopView, IViewer->topView())
CMDFUN(OnLeftView, IViewer->leftView())
CMDFUN(OnRightView, IViewer->rightView())

afx_msg LRESULT COsgbView::OnTopView(WPARAM wParam, LPARAM lParam)
{
	IViewer->topView();

	return 0;
}


afx_msg LRESULT COsgbView::OnLeftView(WPARAM wParam, LPARAM lParam)
{
	IViewer->leftView();

	return 0;
}

afx_msg LRESULT COsgbView::OnRightView(WPARAM wParam, LPARAM lParam)
{
	IViewer->rightView();
	return 0;
}

void COsgbView::OnChangeShowImage()
{
	COsgbVecView* vecView = getOsgbVecView();

	if (vecView)
	{
		CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		pMain->OnOsgbviewImg();
		bShowImg = pMain->m_showImgBar.IsVisible();
		m_ctrlBarHorz.SetButtonState2(ID_OSGBVIEW_IMGDLG, TRUE, bShowImg);

		/*pMain->m_showImgBar.*/
// 		if (bShowImg == false)
// 		{
// 			//关闭对话框
// 			hideImgDlg();
// 		}
// 		else
// 		{
// 			//显示对话框
// 			swImgDlg();
// 		}

	}
}

void COsgbView::OnHome()
{
	COsgbVecView* vecView = getOsgbVecView();
	IViewer->home();
	if (vecView)
	{
		vecView->OnZoomFit();
	}
}

void COsgbView::OnShiftOperation()
{
	IViewer->shiftOperation();
}

void COsgbView::OnLockHeight()
{
	if (!bLockH && bNeedSetLockH) {
		CString str = vec_tips[0] + _T(" ") + str_Off;
		OutputMessage(str);
		return;
	}
	CPoint   point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	lockHeight(point.x, point.y);
	
}

void COsgbView::OnCatchCorner()
{
	IViewer->shiftCatchCorner();
}

void COsgbView::lockHeight(int x, int y)
{
	CRect rc;
	GetClientRect(rc);
	y = rc.Height() - y - 1;
	if (!rc.PtInRect(CPoint(x, y)) && !bLockH) { 
		bNeedSetLockH = true; 
		CString str;
		str.LoadString(IDS_LOCKHEIGHT_TIPS);
		OutputMessage(str);
		return; 
	}
	if (bLockH) bNeedSetLockH = false;
	IViewer->lockHeight(x, y);
}

void COsgbView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (bNeedSetLockH)
	{
		lockHeight(point.x, point.y);
		return;
	}
	if (GetKeyState(VK_SHIFT) < 0)
	{
		setDragCur();
	}
	else
	{
		setNoDragCur();
	}
	
	CView::OnLButtonDown(nFlags, point);
}

void COsgbView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	TRACKMOUSEEVENT   tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_LEAVE;
	tme.dwHoverTime = HOVER_DEFAULT;
	tme.hwndTrack = m_hWnd;
	_TrackMouseEvent(&tme);
	mouseIn = true;
	if (GetKeyState(VK_SHIFT) < 0)
	{
		setDragCur();
	}
	else
	{
		setNoDragCur();
	}

	CView::OnMouseMove(nFlags, point);
}



int COsgbView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	//if (CBaseView::OnCreate(lpCreateStruct) == -1)
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	CWnd *pWnd = GetParent();
	m_ctrlBarHorz.Create(pWnd, IDD_DLGBAR_IMGVIEW,
		WS_CHILD | WS_VISIBLE | CBRS_BOTTOM, ID_HDLGBAR);
	m_ctrlBarHorz.SetMsgWnd(m_hWnd);

	// 本地资源 id 相同 图标来自IDR_TOOLBAR_OSGBVIEW
	{
		// 改变顺序同时改变IDR_TOOLBAR_OSGBVIEW中的顺序
		UINT ids[] = {
			ID_OSGBVIEW_TOP,
			ID_OSGBVIEW_LEFT,
			ID_OSGBVIEW_RIGHT,
			ID_OSGBVIEW_HOME,
			ID_OSGBVIEW_SHIFTOPERAT,
			ID_OSGBVIEW_LOCKH,
			ID_OSGBVIEW_CATCHCORNER,
			ID_OSGBVIEW_IMGDLG,
			ID_WHITE_MODEL
		};

		int btnNum = sizeof(ids) / sizeof(ids[0]);
		CBitmap bmp;
		bmp.LoadBitmap(IDR_TOOLBAR_OSGBVIEW);
		BITMAP pbmp;
		bmp.GetBitmap(&pbmp);
		std::vector<char> buffer(pbmp.bmHeight * pbmp.bmWidthBytes, 0);
		bmp.GetBitmapBits(pbmp.bmHeight * pbmp.bmWidthBytes, buffer.data());
		for (int i = 0; i < btnNum; i++)
		{
			int linewidth = 16 * pbmp.bmBitsPixel / 8;
			int sx = i * linewidth;
			CBitmap _bmp;
			std::vector<char> bmBits(15 * 16 * pbmp.bmBitsPixel / 8, 0);
			for (int r = 0; r < 15; r++)
			{
				memcpy(bmBits.data() + r * linewidth,
					buffer.data() + pbmp.bmWidthBytes * r + sx,
					linewidth);
			}
			_bmp.CreateBitmap(16, 15, pbmp.bmPlanes, pbmp.bmBitsPixel, bmBits.data());
			HBITMAP hBmp = (HBITMAP)_bmp;
			m_ctrlBarHorz.AddButtons(ids + i, &hBmp, 1);
			_bmp.DeleteObject();
		}
		bmp.DeleteObject();
		for (int i = 0; i < btnNum; i++)
		{
			CMFCButton *pBtn = (CMFCButton*)m_ctrlBarHorz.GetButton(ids[i]);
			if (pBtn)
			{
				CString str;
				str.LoadString(ids[i]);
				pBtn->SetTooltip(str);
			}
		}
	}

	// SmartView 的资源
	{
		UINT CtrlButID[] =
		{
			IDC_BUTTON_CURSOR
		};

		UINT CtrlButBmpID[] =
		{
			IDC_BUTTON_CURSOR
		};

		UINT CtrlButTipID[] =
		{
			6
		};
		int btnNum = 0;
		UINT ids[sizeof(CtrlButID) / sizeof(CtrlButID[0])];
		UINT bmps[sizeof(CtrlButID) / sizeof(CtrlButID[0])];
		for (int i = 0; i < sizeof(CtrlButID) / sizeof(CtrlButID[0]); i++)
		{
			if (m_cmdUseFlags[i])
			{
				ids[btnNum] = CtrlButID[i];	bmps[btnNum] = CtrlButBmpID[i];
				btnNum++;
			}
		}
		m_ctrlBarHorz.AddButtons(ids, bmps, btnNum);

		for (i = 0; i < sizeof(CtrlButID) / sizeof(CtrlButID[0]); i++)
		{
			if (m_cmdUseFlags[i])
			{
				CMFCButton *pBtn = (CMFCButton*)m_ctrlBarHorz.GetButton(CtrlButID[i]);
				if (pBtn)
				{
					CString str;
					UINT uID = CtrlButTipID[i];
					TCHAR tmpstr[256];
					::LoadString(SmartViewDLL.hResource, uID, tmpstr, sizeof(tmpstr) - 1);
					str = tmpstr;
					pBtn->SetTooltip(str);
				}
			}
		}
	}

	// 本地资源 id 不同 图标为独立资源
	{
		UINT ToolBtnIds2[] = {
			IDC_BUTTON_VIEWOVERLAY
		};

		UINT ToolBtnBmps2[] = {
			IDB_BUTTON_VIEWOVERLAY
		};

		UINT ToolButTips2[] = {
			IDS_TIPS_VIEWOVERLAY
		};

		CBitmap bmp;
		for (int i = 0; i < sizeof(ToolBtnIds2) / sizeof(ToolBtnIds2[0]); i++)
		{
			bmp.LoadBitmap(ToolBtnBmps2[i]);
			HBITMAP hBmp = (HBITMAP)bmp;
			m_ctrlBarHorz.AddButtons(ToolBtnIds2 + i, &hBmp, 1);
			bmp.DeleteObject();
		}

		for (i = 0; i < sizeof(ToolBtnIds2) / sizeof(ToolBtnIds2[0]); i++)
		{
			CMFCButton *pBtn = (CMFCButton*)m_ctrlBarHorz.GetButton(ToolBtnIds2[i]);
			if (pBtn)
			{
				CString str;
				str.LoadString(ToolButTips2[i]);
				pBtn->SetTooltip(str);
			}
		}
		m_ctrlBarHorz.SetButtonState2(IDC_BUTTON_VIEWOVERLAY, TRUE, TRUE);

	}
	

	TCHAR shapeName[_MAX_PATH] = { 0 };
	GetModuleFileName(SmartViewDLL.hModule, shapeName, _MAX_FNAME);
	TCHAR *pos = _tcsrchr(shapeName, _T('\\'));
	if (pos)pos[1] = _T('\0');
	_tcscat(shapeName, _T("CrossShape.dat"));
	m_cursorL.Load(ConvertTCharToChar(shapeName));

	int index = AfxGetApp()->GetProfileInt(m_strRegSection, _T("CursorType"), 0);
	COLORREF color = AfxGetApp()->GetProfileInt(m_strRegSection, _T("CursorColor"), 255);
	m_cursorL.SetCursorType(index);
	m_cursorL.SetCursorColor(color);
	m_cursorL.EnableSelMarkVisible(true);

	//m_hCurCursor = m_cursorL.GetSysCursor();

	m_hHandCursor = LoadCursor(NULL, IDC_HAND);

	CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	bShowImg = pMain->m_showImgBar.IsVisible();
	m_ctrlBarHorz.SetButtonState2(ID_OSGBVIEW_IMGDLG, TRUE, bShowImg);

	return 0;
}

void COsgbView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CSceneViewEx::OnLButtonUp(nFlags, point);
	CView::OnLButtonUp(nFlags, point);
}

void COsgbView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	return;
}

void COsgbView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (GetKeyState(VK_SHIFT) < 0)
	{
		setDragCur();
	}
	else
	{
		setNoDragCur();
	}

	if (GetKeyState(VK_CONTROL) < 0)
	{
		/*showPopMenu();*/
	}

	CView::OnRButtonDown(nFlags, point);
}

void COsgbView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//

	CView::OnRButtonUp(nFlags, point);
}

void COsgbView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	setDragCur();
	CView::OnMButtonDown(nFlags, point);
}

void COsgbView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CView::OnMButtonUp(nFlags, point);
}

void COsgbView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	COsgbVecView* vecView = getOsgbVecView();
#if 0

	//当是1 5 6的时候不向vectorview视图发送按键消息
	if (
		nChar == '1' ||
		nChar == '5' ||
		nChar == '6'
		)
	{
		
	}
	else if (		nChar == '2')
	{
		SendMessage(WM_LEFT_VIEW, nChar, nFlags);
	}
	else if (		nChar == '3')
	{
		SendMessage(WM_RIGHT_VIEW, nChar, nFlags);
	}
	else if (		nChar == '4' )
	{
		SendMessage(WM_TOP_VIEW, nChar, nFlags);
	}
	else if (		nChar == '8')
	{
		IViewer->home();
		if (vecView)
		{
			vecView->OnZoomFit();
		}
	}
	else if (nChar == '7')
	{
		COsgbVecView* vecView = getOsgbVecView();

		if (vecView)
		{
			bool bShowImg = vecView->changeShowImg();

			if (bShowImg == false)
			{
				//关闭对话框
				hideImgDlg();
			}
			else
			{
				//显示对话框
				swImgDlg();
			}
		}
	}
	else if (nChar == '9')
	{
		
	}
	else if (nChar == '0')
	{
		hintState++;
		if (hintState > 2)
		{
			hintState = 0;
		}
		CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		CString str;
		CString lt;
		lt.LoadString(IDS_LINKTYPE);
		str.Format(_T("%s: %d \n"), lt, hintState);
		OutputMessage(str);
	}
	else
#endif
	if (nChar == VK_BACK){}
	else if (nChar == VK_TAB) {}
	else  if (vecView != nullptr)
	{
		vecView->PostMessageA(WM_KEYDOWN, nChar, nFlags);
		//vecView->OnKeyDown(nChar, nRepCnt, nFlags);
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}



void COsgbView::OnPaint()
{ // device context for painting
	// TODO: Add your message handler code here
	// Do not call CSceneView::OnPaint() for painting messages
	CView::OnPaint();
}



void COsgbView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	CView::OnTimer(nIDEvent);
}


void COsgbView::OnSize(UINT nType, int cx, int cy)
{
	static BOOL m_bIsSelfSizing = FALSE;
	if (m_bIsSelfSizing)
	{
		CView::OnSize(nType, cx, cy);
	}
	else
	{
		m_bIsSelfSizing = TRUE;
		CRect rcClient;
		GetWindowRect(rcClient);
		GetParent()->ScreenToClient(rcClient);
		//GetClientRect(rcClient);
		int x = rcClient.left;
		int y = rcClient.top;
		cx = rcClient.Width();
		cy = rcClient.Height();
		::MoveWindow(m_hWnd, x, y, cx, cy - m_ctrlBarHorz.GetButtonsHeight(), TRUE);

		::MoveWindow(m_ctrlBarHorz.GetSafeHwnd(), x, y + cy - m_ctrlBarHorz.GetButtonsHeight(),
			cx, m_ctrlBarHorz.GetButtonsHeight(), TRUE);

		m_ctrlBarHorz.AdjustLayout();
		m_bIsSelfSizing = FALSE;
		// TODO: Add your message handler code here
	}
}


void COsgbView::OnUpdateSelect(CCmdUI* pCmdUI)
{
	int id = GetWorker()->GetCurrentCommandId();
	if (id == 0)
	{
		if (GetWorker()->IsSelectorOpen() && GetWorker()->GetSelectorMode() != SELMODE_POLYGON && GetWorker()->GetSelectorMode() != SELMODE_RECT)
			pCmdUI->SetCheck(TRUE);
		else
			pCmdUI->SetCheck(FALSE);
	}
	else
		pCmdUI->SetCheck(FALSE);
}


BOOL COsgbView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_SPACE:
		{
			COsgbVecView* vecView = getOsgbVecView();

			if (vecView != nullptr)
			{
				vecView->SendMessage(WM_COMMAND, MAKEWPARAM(ID_STEREO_SETXYZ, 0), 0);
			}

			break;
		}
		}
	}

	return CView::PreTranslateMessage(pMsg);
}

void COsgbView::ViewAll()
{
	vector<osgFtr> vecFtr = parseVecFtr();
	IViewer->viewAll(vecFtr);
	
	if (isOsgbViewActive() == false)
	{
		return;
	}
		
	COsgbVecView* vecView = getOsgbVecView();

	if (vecView != nullptr)
	{
		vecView->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_TYPE_ALL, 0), 0);
	}
}

void COsgbView::ViewHideAll()
{
	vector<osgFtr> vecFtr = parseVecFtr();
	IViewer->ViewHideAll(vecFtr);

	if (isOsgbViewActive() == false)
	{
		return;
	}

	COsgbVecView* vecView = getOsgbVecView();

	if (vecView != nullptr)
	{
		vecView->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_TYPE_SHOWORHIDE, 0), 0);
	}
}

void COsgbView::ViewExternal()
{
	vector<osgFtr> vecFtr = parseVecFtr();
	IViewer->viewExternal(vecFtr);

	if (isOsgbViewActive() == false)
	{
		return;
	}
	
	COsgbVecView* vecView = getOsgbVecView();

	if (vecView != nullptr)
	{
		vecView->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_TYPE_EXTERNAL, 0), 0);
	}
}

void COsgbView::ViewLocal()
{
	vector<osgFtr> vecFtr = parseVecFtr();
	IViewer->viewLocal(vecFtr);

	if (isOsgbViewActive() == false)
	{
		return;
	}

	COsgbVecView* vecView = getOsgbVecView();

	if (vecView != nullptr)
	{
		vecView->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_TYPE_LOCAL, 0), 0);
	}
}

COsgbVecView* COsgbView::getOsgbVecView()
{
	if (!isFit()) return nullptr;
	if (posgVectorView) return posgVectorView;

	POSITION pos = m_pDocument->GetFirstViewPosition();
	while (pos)
	{
		CView* pView = m_pDocument->GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CVectorView_new)))
		{
			//看上去是动态转换，但是实际上是静态转换，转换后只能调用非虚函数
			posgVectorView = DYNAMIC_DOWNCAST(COsgbVecView, pView);
			return posgVectorView;
		}
	}

	return nullptr;
}


bool COsgbView::isOsgbViewActive()
{
	CView* pView = GetActiveView();

	if (pView->IsKindOf(RUNTIME_CLASS(COsgbView)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

vector<osgFtr> COsgbView::parseVecFtr()
{
	CDlgDoc* dlg = GetDocument();
	CDlgDataSource* dataSource = dlg->GetDlgDataSource();
	int layerCnt = dataSource->GetFtrLayerCount();

	vector<osgFtr> vecFtr;

	for (int i = 0; i < layerCnt; i++)
	{
		CFtrLayer* layer = dataSource->GetFtrLayerByIndex(i);

		if (layer == NULL || layer->GetObjectCount() <= 0)
		{
			continue;
		}

		pFtrParser->getFtrLyrData(layer, vecFtr);
	}

	return vecFtr;
}
/*
VEC_BUFFER COsgbView::parseSymbolBuf()
{
	COsgbVecView* posgVectorView = nullptr;
	CView* pView = nullptr;
	POSITION pos = m_pDocument->GetFirstViewPosition();
	while (pos)
	{
		pView = m_pDocument->GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(COsgbVecView)))
		{
			posgVectorView = DYNAMIC_DOWNCAST(COsgbVecView, pView);
			break;
		}
	}

	CDlgDoc* dlg = GetDocument();
	CDlgDataSource* dataSource = dlg->GetDlgDataSource();
	int layerCnt = dataSource->GetFtrLayerCount();

	VEC_BUFFER vecBuf;

	for (int i = 0; i < layerCnt; i++)
	{
		CFtrLayer* layer = dataSource->GetFtrLayer(i);

		if (layer == NULL)
		{
			continue;
		}

		int objCnt = layer->GetObjectCount();

		for (int i = 0; i < objCnt; i++)
		{
			CFeature* feature = layer->GetObject(i);

			if (feature == nullptr)
			{
				continue;
			}

			CFtrLayer* objLayer = dataSource->GetFtrLayerOfObject(feature);
			COLORREF clr = objLayer->GetColor();

			GrBuffer* buf = new GrBuffer();
			posgVectorView->Callback_DrawFeature(dataSource, layer, feature, SYMBOLIZE_ALL, buf);
			OUID id = feature->GetID();

			CString str = id.ToString();
			string idStr = str;

			const char *code = feature->GetCode();
			BOOL bLocal = (code == NULL || strlen(code) <= 0);

			stGrBuffer stBuf(idStr, buf, clr, bLocal);
			vecBuf.push_back(stBuf);
		}

	}

	return vecBuf;
}
*/

void COsgbView::updateSnap(const SNAPITEM* st)
{
	if (st == nullptr)
	{
		GrBuffer buf;
		UpdatesnapDraw(&buf);
	}
	else
	{
		const PT_3D * pt0 = &(st->pt);
		const PT_3D & t0 = *pt0;
		double scale = getScaleFromDist();

		GrBuffer buf;
		double r = GetDocument()->m_snap.GetSnapRadius() * scale;
		double _r = r / 2.0;
		buf.BeginLineString(m_clrSnap, 1, FALSE);
		switch (st->nSnapMode)
		{ 
		
		case CSnap::modeNearPoint:{
			buf.MoveTo(&PT_3D(t0.x - _r, t0.y + _r, t0.z));
			buf.LineTo(&PT_3D(t0.x + _r, t0.y + _r, t0.z));
			buf.LineTo(&PT_3D(t0.x, t0.y, t0.z));
			buf.LineTo(&PT_3D(t0.x - _r, t0.y + _r, t0.z));
			break;
		}
		case CSnap::modePolar: _r /= 2.0;
		case CSnap::modeIntersect:{
			buf.MoveTo(&PT_3D(t0.x - _r, t0.y - _r, t0.z));
			buf.LineTo(&PT_3D(t0.x + _r, t0.y + _r, t0.z));
			buf.MoveTo(&PT_3D(t0.x + _r, t0.y - _r, t0.z));
			buf.LineTo(&PT_3D(t0.x - _r, t0.y + _r, t0.z));
			break;
		}
		case CSnap::modeMidPoint:{
			buf.MoveTo(&PT_3D(t0.x - _r, t0.y, t0.z));
			buf.LineTo(&PT_3D(t0.x + _r, t0.y, t0.z));
			double _r1 = r / 12.0;
			buf.MoveTo(&PT_3D(t0.x - _r1, t0.y - _r1, t0.z));
			buf.LineTo(&PT_3D(t0.x - _r1, t0.y + _r1, t0.z));
			buf.LineTo(&PT_3D(t0.x + _r1, t0.y + _r1, t0.z));
			buf.LineTo(&PT_3D(t0.x + _r1, t0.y - _r1, t0.z));
			buf.LineTo(&PT_3D(t0.x - _r1, t0.y - _r1, t0.z));

			break;
		}
		case CSnap::modePerpPoint:{
			PT_3D  t1;
			t1.x = t0.x - _r;
			t1.y = t0.y - _r;
			t1.z = t0.z;
			
			buf.MoveTo(&t1);
			t1.y = t0.y + _r;
			buf.LineTo(&t1);
			t1.x = t0.x + _r;
			buf.LineTo(&t1);

			break;
		}
		case CSnap::modeCenterPoint:{
			buf.MoveTo(&PT_3D(t0.x - _r, t0.y - _r, t0.z));
			buf.LineTo(&PT_3D(t0.x - _r, t0.y + _r, t0.z));
			buf.LineTo(&PT_3D(t0.x + _r, t0.y + _r, t0.z));
			buf.LineTo(&PT_3D(t0.x + _r, t0.y - _r, t0.z));
			buf.LineTo(&PT_3D(t0.x - _r, t0.y - _r, t0.z));
			
			
			_r *= 0.8;
			//double _r1 = _r * 0.8;
			buf.MoveTo(&PT_3D(t0.x - _r, t0.y - _r, t0.z));
			buf.LineTo(&PT_3D(t0.x - _r, t0.y + _r, t0.z));
			buf.LineTo(&PT_3D(t0.x + _r, t0.y + _r, t0.z));
			buf.LineTo(&PT_3D(t0.x + _r, t0.y - _r, t0.z));
			buf.LineTo(&PT_3D(t0.x - _r, t0.y - _r, t0.z));
// 			_r *= 0.9;
// 			buf.MoveTo(&PT_3D(t0.x - _r, t0.y - _r, t0.z));
// 			buf.LineTo(&PT_3D(t0.x - _r, t0.y + _r, t0.z));
// 			buf.LineTo(&PT_3D(t0.x + _r, t0.y + _r, t0.z));
// 			buf.LineTo(&PT_3D(t0.x + _r, t0.y - _r, t0.z));
// 			buf.LineTo(&PT_3D(t0.x - _r, t0.y - _r, t0.z));
			break;
		}
		case CSnap::modeTangPoint:{
			double _r1 = 0.2 * _r;
			PT_3D  t1;
			t1.x = t0.x - _r;
			t1.y = t0.y + _r1;
			t1.z = t0.z;
			buf.MoveTo(&t1);
			t1.x = t0.x + _r;
			buf.LineTo(&t1);
			t1.x = t0.x - _r;
			t1.y = t0.y - _r;
			buf.MoveTo(&t1);
			t1.x = t0.x; 
			t1.y = t0.y;
			buf.LineTo(&t1);
			t1.x = t0.x + _r1 + _r; 
			t1.y = t0.y - _r1 - _r;
			buf.LineTo(&t1);

			break;
		}
		
		case CSnap::modeEndPoint:
		{
			float offlen = r*1.414 / 2.0;
			PT_3D  t1;
			t1.z = t0.z;
			t1.x = t0.x - offlen; t1.y = t0.y;
			buf.MoveTo(&t1);

			t1.x = t0.x; t1.y = t0.y + offlen;
			buf.LineTo(&t1);

			t1.x = t0.x + offlen; t1.y = t0.y;
			buf.LineTo(&t1);

			t1.x = t0.x; t1.y = t0.y - offlen;
			buf.LineTo(&t1);

			t1.x = t0.x - offlen; t1.y = t0.y;
			buf.LineTo(&t1);

			break;
		}
		case CSnap::modeGrid:
		case CSnap::modeKeyPoint:
		default:
		{
			buf.MoveTo(&PT_3D(t0.x - _r, t0.y - _r, t0.z));
			buf.LineTo(&PT_3D(t0.x - _r, t0.y + _r, t0.z));
			buf.LineTo(&PT_3D(t0.x + _r, t0.y + _r, t0.z));
			buf.LineTo(&PT_3D(t0.x + _r, t0.y - _r, t0.z));
			buf.LineTo(&PT_3D(t0.x - _r, t0.y - _r, t0.z));
			break;
		}
		}
		
		buf.End();

		UpdatesnapDraw(&buf);
	}

}

vector<COri> COsgbView::selectImgByPt(PT_3D *pt)
{
	// 影像索引和用于排序的参数
	struct idx_p
	{
		size_t idx;
		double p;
		bool operator<(const  idx_p & e) const {
			return p < e.p;
		}
	};
	vector<COri> vecOri;
	list<idx_p> vecIA;
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	CProjectViewBar* prjBar = pMain->getPrjViewBar();
	CVisualModelMapCtrl* mapCtrl = prjBar->getModelMapCtrl();
	CDrawGraphStatic* graphStatic = mapCtrl->getGraphStatic();
	CoreObject coreObj = graphStatic->getCoreObj();
	
	for (int i = 0; i < coreObj.iStripNum; i++)
	{
		STRIP strip = coreObj.strip[i];
		CString prjPath = coreObj.pp.strPrjPath;

		for (size_t j = 0; j < strip.iImageNum; j++)
		{
			IMAGE img = strip.image[j];
			CString strImgId = img.strImageID;

			CCoordCenter cc;
			cc.Init(coreObj, "", strImgId);
			Coordinate coord;
			coord.x = pt->x;
			coord.y = pt->y;
			coord.z = pt->z;
			coord.iType = GROUND;

			Coordinate coord2;
			coord2.iType = SCANNER;
			cc.Convert(coord, coord2);

			//如果投影在影像上，说明这个影像id可以作为前方交汇的影像
			if (coord2.lx >= 1 && coord2.lx <= img.ipara.iWidth
				&& coord2.ly >= 1 && coord2.ly <= img.ipara.iHeight)
			{
				//对照片进行筛选
				/*IViewer->islineBlock()*/
				double eyeX = img.aop.line_para[0];
				double eyeY = img.aop.line_para[1];
				double eyeZ = img.aop.line_para[2];

				double objX = pt->x;
				double objY = pt->y;
				double objZ = pt->z;

				if (IViewer->islineBlock(eyeX, eyeY, eyeZ, objX, objY, objZ))
				{
					continue;
				}

				string id = img.ie.strFileName;
				COri ori;
				ori.imgName = id;
				ori.imgX = coord2.lx;
				ori.imgY = img.ipara.iHeight - coord2.ly;
				
				vecOri.push_back(ori);
				size_t idx = vecOri.size() - 1;
				double hw = img.ipara.iWidth / 2;
				double hh = img.ipara.iHeight / 2;
				
				// 排序条件

				// 标记点距图像中心距离
				double toCenter = hypot((coord2.lx - hw) / hw, (coord2.ly - hh) / hh);
				double p1 = toCenter / 1.414;
				//物点与视点在zy平面上的距离
				double disInPanel = hypot(objX - eyeX, objY - eyeY);
				// 视线与水平面的夹角
				double ang = atan2(fabs(eyeZ - objZ), disInPanel);
				double p2 = ang / PI;

				double p = p1 + p2;
				vecIA.push_back({idx,p});
			}
			
		}
	}

	vector<COri> vecFinalOri;

	if (vecOri.size() >= 2)
	{
		vecIA.sort();
		vecFinalOri.reserve(vecOri.size());
		for (auto & ia : vecIA)
		{
			vecFinalOri.push_back(vecOri[ia.idx]);
		}
		//vecFinalOri.swap(vecOri);
		//vecFinalOri.push_back(vecOri[mid]);
		//vecFinalOri.push_back(vecOri[midRight]);
	}

	return vecFinalOri;
}

void COsgbView::getInitRect()
{
	//获取osgbview的窗口范围
	GetParentFrame()->GetWindowRect(osgFrameRect);

	COsgbVecView* pView = getOsgbVecView();

	if (pView == nullptr)
	{
		return;
	}

	//获取矢量视图范围
	pView->GetParentFrame()->GetWindowRect(vecFrameRect);
}

void COsgbView::updateLayerVisibal(const UpdateDispyParam * udp)
{
  	if (udp->type == UpdateDispyParam::typeVISIBLE){
  		EditBase::CFtrLayer * layer = (EditBase::CFtrLayer *)udp->handle;
  		bool bVIsible = udp->data.bVisible;
  		if (layer == NULL)
  		{
  			return;
  		}
  		CDlgDoc* dlgDoc = GetDocument();
		
  		CDlgDataSource* dataSource = dlgDoc->GetDlgDataSource();
  
  		vector<osgFtr> vecFtr;
  
  		if (bVIsible){
			pFtrParser->getFtrLyrData(layer, vecFtr);
			viewAll(vecFtr);
  		}
  		else{
			pFtrParser->getFtrLyrDataOnlyID(layer, vecFtr);
			hideAll(vecFtr);
  		}
  	}
  	else if (udp->type == UpdateDispyParam::typeCOLOR)
	{
 		EditBase::CFtrLayer * layer = (EditBase::CFtrLayer *)udp->handle;
 		
		vector<osgFtr> vecFtr;
		pFtrParser->getFtrLyrData(layer, vecFtr);
// 		COLORREF curClr = udp->data.lColor;
// 		double r = GetRValue(curClr) / 255.0;
// 		double g = GetGValue(curClr) / 255.0;
// 		double b = GetBValue(curClr) / 255.0;
// 		osgCall::Coord col(r, g, b);
// 		for (auto & ftr : vecFtr)
// 		{
// 			ftr.rgb = col;
// 		}
		IViewer->updataDate(vecFtr);
	}
}

void COsgbView::updateObjectVisibal(const UpdateDispyParam * udp)
{
	CFeature* pFtr = HandleToFtr(udp->handle);
	if (!pFtr) return;
	vector<osgFtr> vecFtr;
	pFtrParser->getAllFtrInfo(pFtr, vecFtr);
	if (udp->type == UpdateDispyParam::typeVISIBLE)
	{
		if (udp->data.bVisible)
		{
			vector<osgFtr> vecFtr;
			pFtrParser->getAllFtrInfo(pFtr, vecFtr);
			viewAll(vecFtr);
		}
		else
		{
			vector<osgFtr> vecFtr(1);
			vecFtr[0].mId = pFtr->GetID().ToString();
			hideAll(vecFtr);
		}
	}
	else if (udp->type == UpdateDispyParam::typeCOLOR)
	{
		vector<osgFtr> vecFtr;
		pFtrParser->getAllFtrInfo(pFtr, vecFtr);
		COLORREF curClr = udp->data.lColor;
		double r = GetRValue(curClr) / 255.0;
		double g = GetGValue(curClr) / 255.0;
		double b = GetBValue(curClr) / 255.0;
		osgCall::Coord col(r, g, b);
		for (auto & ftr : vecFtr)
		{
			ftr.rgb = col;
		}
		IViewer->updataDate(vecFtr);
	}
}

bool COsgbView::isHide(const std::string & id)
{
	return m_hideIDS.find(id) != m_hideIDS.end();
}

void COsgbView::hideAll(const std::vector<osgFtr> & vecFtr)
{
	for (const osgFtr & ftr:vecFtr)
	{
		m_hideIDS.insert(ftr.mId);
	}
	IViewer->ViewHideAll(vecFtr);
}

void COsgbView::viewAll(const std::vector<osgFtr> & vecFtr)
{
	for (const osgFtr & ftr : vecFtr)
	{
		if (isHide(ftr.mId))
			m_hideIDS.erase(ftr.mId);
	}
	IViewer->viewAll(vecFtr);
}

void COsgbView::synData(std::vector<osgCall::osgFtr> & vecFtr)
{
	if (!m_hideIDS.empty())
	{
		for (osgFtr & ftr : vecFtr)
		{
			if (isHide(ftr.mId))
				ftr.isVisible = false;
		}
	}
	IViewer->synData(vecFtr);
}

afx_msg LRESULT COsgbView::OnShowImg(WPARAM wParam, LPARAM lParam)
{
	PT_3D* pt = (PT_3D*)wParam;
	int type = (int)lParam;

	//打开按空三成果筛选的两张影像
	vecImgOri = selectImgByPt(pt);
	if (!bShowImg) OnChangeShowImage();
	m_gCurPoint = *pt;
	CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	pMain->m_showImgBar.showImage(vecImgOri, type, this);

	return 0;
}

void COsgbView::changeFakePt(string imgName, double x, double y)
{
	COri ori;
	ori.imgName = imgName;
	ori.imgX = x;
	ori.imgY = y;

	vector<COri>::iterator vec_it = find(vecImgOri.begin(), vecImgOri.end(), ori);

	if (vec_it != vecImgOri.end())
	{
		vec_it->imgX = ori.imgX;
		vec_it->imgY = ori.imgY;

		//重新计算前方交汇, 用线程安全的方式
		pMsgSender->sendMsgAdjustFakePt();

		//设置控件check状态
		//showImgDlg->setCheck(0);

		CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		pMain->m_showImgBar.setCheck(0);
	}
	else
	{
		_logop("无法找到匹配影像");
	}
}

void COsgbView::reCalFront(string imgName, double x, double y)
{
	COri ori;
	ori.imgName = imgName;
	ori.imgX = x;
	ori.imgY = y;

	vector<COri>::iterator vec_it = find(vecImgOri.begin(), vecImgOri.end(), ori);

	if (vec_it != vecImgOri.end())
	{
		vec_it->imgX = ori.imgX;
		vec_it->imgY = ori.imgY;

		//重新计算前方交汇, 用线程安全的方式
		pMsgSender->sendMsgChangeCurPt();

		//设置控件check状态
		CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		pMain->m_showImgBar.setCheck(1);

		_logop("重新前方交汇");
	}
	else
	{
		_logop("无法找到匹配影像");
	}
	
}

void COsgbView::bkSpcaeLastOps(/*int i*/)
{
	if (bShowImg)
	{
		CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		
		pMsgSender->sendMsgBkspace();
		pMain->m_showImgBar.setCheck(0);
		

	}
	else
	{
		pMsgSender->sendMsgBkspace();

	}
	
	
}

void COsgbView::UpdateImgVisible()
{
	CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	bShowImg = pMain->m_showImgBar.IsVisible();
	m_ctrlBarHorz.SetButtonState2(ID_OSGBVIEW_IMGDLG, TRUE, bShowImg);
}

afx_msg LRESULT COsgbView::OnChangeCurpt(WPARAM wParam, LPARAM lParam)
{
	int type = (int)wParam;
	reFrontIntersect(type);
	return 0;
}

LRESULT COsgbView::OnLeftBtDoubleClick(WPARAM wParam, LPARAM lParam)
{
	auto p = getOsgbVecView();
	if (p)
	{
		stPoint* pt = (stPoint*)wParam;
		PT_3D pt1;
		pt1.x = pt->mX;
		pt1.y = pt->mY;
		pt1.z = pt->mZ;
		p->leftDBClk(&pt1);
		//GetWorker()->LButtonDblClk(pt1, 1);
	}
	return 0;
}

// 格式化路径 全为小写 斜杠全用slash
void reformat_path_string(std::string & path, const char slash = '\\')
{
	if (!path.empty())
	{
		int length = path.length();
		bool lastIsSlash = false;
		std::string npath;
		npath.reserve(path.length() + 1);
		for (int i = 0; i < length; i++)
		{
			char c = path[i];
			if (c == '\\' || c == '/')
			{
				if (!lastIsSlash)
					npath += slash;
				lastIsSlash = true;
			}
			else
			{
				lastIsSlash = false;
				if (c >= 'A' && c <= 'Z')
					c = c + 'a' - 'A';
				npath += c;
			}
		}
		path.swap(npath);
	}
}

// 从后向前相同项的数量
int v_same_number_from_back(const vector<string> &v0, const vector<string> &v1)
{
	int s = 0;
	int i0 = v0.size() - 1;
	int i1 = v1.size() - 1;
	for (; i0 >= 0 && i1 >= 0; --i0, --i1)
	{
		if (v0[i0] == v1[i1])
			++s;
		else
			break;
	}
	return s;
}

const std::string getId(CoreObject & coreobj, const std::string & _imageName)
{
	const char slash = '\\';
	string imageName = _imageName;
	reformat_path_string(imageName, slash);
	struct _2_index
	{
		int _0;
		int _1;
		bool operator < (const _2_index & a)const
		{
			if (_0 == a._0) return _1 < a._1;
			else return _0 < a._0;
		}
	};
	std::map<_2_index, string> idx_path;
	_2_index _idx;
	// 路径完全对的上
	for (int i = 0; i < coreobj.iStripNum; i++)
	{
		_idx._0 = i;
		for (int j = 0; j < coreobj.strip[i].iImageNum; j++)
		{
			_idx._1 = j;
			string path(coreobj.strip[i].image[j].ie.strFileName);
			reformat_path_string(path, slash);
			idx_path[_idx] = path;
			if (path == imageName.c_str())
			{
				return coreobj.strip[i].image[j].strImageID;
			}
		}
	}


	// 部分目录对的上   aaa/b/c.xx  ddd/b/c.xx  
	vector<string> v_res;
	// 分割路径
	split(imageName, slash, v_res, [](const string & i){return i.size(); });
	_idx._0 = -1;
	_idx._1 = -1;
	int max_same = 0;
	for (auto & p : idx_path)
	{
		vector<string> res;
		split(p.second, slash, res, [](const string & i){return i.size(); });
		// 
		int s = v_same_number_from_back(res, v_res);
		if (s > max_same)
		{
			max_same = s;
			_idx = p.first;
		}
	}
	if (max_same != 0)
	{
		return coreobj.strip[_idx._0].image[_idx._1].strImageID;
	}

	// 全都对不上
	int pos = imageName.rfind(slash);
	int length = imageName.length() - 1;
	return imageName.substr(pos + 1, length);
}

void COsgbView::reFrontIntersect(int type)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;

	if (pMain == nullptr)
	{
		_logop("获取main失败");
		return;
	}

	CProjectViewBar* prjBar = pMain->getPrjViewBar();
	CVisualModelMapCtrl* mapCtrl = prjBar->getModelMapCtrl();
	CDrawGraphStatic* graphStatic = mapCtrl->getGraphStatic();
	CoreObject coreObj = graphStatic->getCoreObj();
	int imgH = coreObj.strip->image->ipara.iHeight;

#if 0
	Coordinate sumCoord;
	int numImg = 5;

	if (vecImgOri.size() < numImg)
	{
		numImg = vecImgOri.size();
	}
	
	for (int i = 0; i < numImg - 1; i++)
	{
		COri lOri = vecImgOri[i];
		COri rOri = vecImgOri[i + 1];

		//获得左影像id
		int pos = lOri.imgName.rfind("\\");
		int length = lOri.imgName.length() - 1;
		string lImgId = lOri.imgName.substr(pos + 1, length);

		//获得右影像id
		pos = rOri.imgName.rfind("\\");
		length = rOri.imgName.length() - 1;
		string rImgId = rOri.imgName.substr(pos + 1, length);

		{
			COsgCoordCenter cc;
			cc.InitByTwoImg(coreObj, lImgId.c_str(), rImgId.c_str());

			Coordinate imgCoord, grdCoord;
			imgCoord.lx = lOri.imgX; imgCoord.ly = imgH - lOri.imgY;
			imgCoord.rx = rOri.imgX; imgCoord.ry = imgH - rOri.imgY;
			imgCoord.iType = SCANNER; grdCoord.iType = GROUND;

			//获取地面坐标
			cc.Convert(imgCoord, grdCoord);

			//求和
			sumCoord.x += grdCoord.x;
			sumCoord.y += grdCoord.y;
			sumCoord.z += grdCoord.z;
		}
		
	}
	Coordinate aveCoord;
	aveCoord.x = sumCoord.x / (numImg - 1);
	aveCoord.y = sumCoord.y / (numImg - 1);
	aveCoord.z = sumCoord.z / (numImg - 1);
#else
	
	
	COri lOri = vecImgOri[pMain->m_showImgBar.lIdx()];
	COri rOri = vecImgOri[pMain->m_showImgBar.rIdx()];
	Coordinate aveCoord;
	//获得左影像id
	//int pos = lOri.imgName.rfind("\\");
	//int length = lOri.imgName.length() - 1;
	//string lImgId = lOri.imgName.substr(pos + 1, length);

	string lImgId = getId(coreObj,lOri.imgName);

	//获得右影像id
	//pos = rOri.imgName.rfind("\\");
	//length = rOri.imgName.length() - 1;
	//string rImgId = rOri.imgName.substr(pos + 1, length);
	string rImgId = getId(coreObj, rOri.imgName);

	{
		COsgCoordCenter cc;
		cc.InitByTwoImg(coreObj, lImgId.c_str(), rImgId.c_str());

		Coordinate imgCoord, grdCoord;
		imgCoord.lx = lOri.imgX; imgCoord.ly = imgH - lOri.imgY;
		imgCoord.rx = rOri.imgX; imgCoord.ry = imgH - rOri.imgY;
		imgCoord.iType = SCANNER; grdCoord.iType = GROUND;

		//获取地面坐标
		cc.Convert(imgCoord, grdCoord);

		aveCoord.x = grdCoord.x;
		aveCoord.y = grdCoord.y;
		aveCoord.z = grdCoord.z;
	}
#endif


	//改变当前点
	COsgbVecView* pView = getOsgbVecView();

	if (pView)
	{
		//改变新点
		PT_3D pt(aveCoord.x, aveCoord.y, aveCoord.z);

		if (type == 1)
		{
			IViewer->clearVaryGroup();
			pView->changeCurPt(&pt);
		}
		else if (type == 0)
		{
			IViewer->drawVaryPt(pt.x, pt.y, pt.z);
		}

		//计算误差并输出
		double errX = abs(m_gCurPoint.x - pt.x);
		double errY = abs(m_gCurPoint.y - pt.y);
		double errZ = abs(m_gCurPoint.z - pt.z);

		//增加提示
		char str[MAX_PATH];
		sprintf_s(str, "error: %lf %lf %lf\n", errX, errY, errZ);
		CString promptTxt = str;

		//输出到输出窗体中
		OutputMessage(promptTxt);

	}

}


void COsgbView::initSwImgDlg()
{
	//设置对话框范围
	CRect dlgRect;
	dlgRect.left = osgFrameRect.left;
	dlgRect.right = vecFrameRect.right;
	dlgRect.top = osgFrameRect.top + 3 * osgFrameRect.Height() / 4;
	dlgRect.bottom = osgFrameRect.bottom;

// 	shared_ptr<CShowImgDlg> dlg(new CShowImgDlg(dlgRect,this));
// 	showImgDlg = dlg;
// 	showImgDlg->Create(IDD_DIALOG_IMGVIEW);
// 	showImgDlg->initBrowser();
}

void COsgbView::swImgDlg()
{
// 	if (showImgDlg == nullptr)
// 	{
// //		return;
// 		//获取初始化区域
// 		getInitRect();
// 
// 		//初始化图片对话框
// 		initSwImgDlg();
// 
// 	}
// 
// 	showImgDlg->ShowWindow(SW_SHOW);
// 
// 	bShowImg = true;
// 	//调整视图到原始位置
// 	adjustViewToShowImg();
// 	m_ctrlBarHorz.SetButtonState2(ID_OSGBVIEW_IMGDLG, TRUE, bShowImg);

}

void COsgbView::hideImgDlg()
{
// 	if (showImgDlg != nullptr)
// 	{
// 		showImgDlg->ShowWindow(SW_HIDE);
// 		//return;
// 	}
// 	bShowImg = false;
// 	m_ctrlBarHorz.SetButtonState2(ID_OSGBVIEW_IMGDLG, TRUE, bShowImg);
// 	//调整视图到原始位置
// 	adjustViewToOrigin();
}

void COsgbView::adjustViewToOrigin()
{
	//重新调整osgbView恢复到最开始
	GetParentFrame()->MoveWindow(0, 0, osgFrameRect.Width(), osgFrameRect.Height(), TRUE);

	//重新调整矢量视图恢复到最开始
	COsgbVecView* pView = getOsgbVecView();

	if (pView == nullptr)
	{
		return;
	}

	pView->GetParentFrame()->MoveWindow(osgFrameRect.Width(), 0, vecFrameRect.Width(), vecFrameRect.Height(), TRUE);
}

void COsgbView::adjustViewToShowImg()
{
	//移动osgbiew到新位置
	CRect newOsgRect = osgFrameRect;
	newOsgRect.bottom -= newOsgRect.Height() / 4;
	GetParentFrame()->MoveWindow(0, 0, newOsgRect.Width(), newOsgRect.Height(), TRUE);

	//移动vecView到新位置
	CRect newVecRect = vecFrameRect;
	newVecRect.bottom -= newVecRect.Height() / 4;

	COsgbVecView* pView = getOsgbVecView();

	if (pView == nullptr)
	{
		return;
	}

	//移动窗口
	pView->GetParentFrame()->MoveWindow(newOsgRect.Width(), 0, newVecRect.Width(), newVecRect.Height(), TRUE);

}

void COsgbView::OnMouseLeave()
{
	mouseIn = false;
	
	auto * p =  getOsgbVecView();
	if (p)
	{
		p->EnableCrossVisible(FALSE);
		p->UpdateCross();
		p->FinishUpdateDrawing();
	}
	CView::OnMouseLeave();
}

void COsgbView::GroundToClient(PT_3D *pt0, PT_4D *pt1)
{
	IViewer->groundToScreen(pt0->x, pt0->y, pt0->z, pt1->x, pt1->y);
	CRect rc;
	GetClientRect(rc);
	pt1->y = rc.Height() - pt1->y;
}

void COsgbView::whiteModel(const FTR_HANDLE * pftr, int num, double h, bool badd)
{
	std::vector<std::string> ids;
	ids.resize(num);
	for (int i = 0; i < num; i++)
	{
		
		ids[i] = HandleToFtr(pftr[i])->GetID().ToString();
	}

	// 删除原有的
	std::vector<std::string> wids;
	wids.reserve(num);
	for (auto & id : ids)
	{
		auto iter = m_map_featerID_WhiteModelID.find(id);
		if (iter != m_map_featerID_WhiteModelID.end())
		{
			wids.push_back(iter->second.id);
			m_map_featerID_WhiteModelID.erase(iter);
		}
	}
	if (!wids.empty())
	{
		IViewer->removeObj(wids);
	}
	if (!badd) return; // 只是删除
	// 设置白模的id
	wids.resize(num);
	for (int i = 0; i < num; i++)
	{
		wids[i] = ids[i] + "_wm";
		m_map_featerID_WhiteModelID.insert(
			std::pair<std::string, wmID_h>(ids[i], { wids[i], h })
			);
	}
	std::vector<osgCall::osgFtr> vecFtr;
	pFtrParser->getFtrWhiteModel(pftr, num, m_map_featerID_WhiteModelID, vecFtr);
	IViewer->addObj(vecFtr);
}



void COsgbView::OnWhiteModel()
{
	GetWorker()->StartCommand(ID_WHITE_MODEL);
}


void COsgbView::OnViewoverlay()
{
	m_bViewVector = !m_bViewVector;
	IViewer->updataFtrView(m_bViewVector);

	m_ctrlBarHorz.SetButtonState2(IDC_BUTTON_VIEWOVERLAY, TRUE, m_bViewVector);

}

#include "FBHelperFunc.h"

void COsgbView::OutputMessage(LPCTSTR str)
{
 	GOutPut(str);
}
