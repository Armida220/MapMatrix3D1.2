#include "stdafx.h"
#include "FtrParser.h"
#include "GeomParser.h"
#include <vector>
#include "ICallBack.h"
#include "BufParser.h"
#include "OsgbVecView.h"
#include "OsgbView.h"
#include "GeoCurve.h"
#include "GeoText.h"
using namespace std;
using namespace osgCall;

CFtrParser::CFtrParser(COsgbView * p) :
pGeomParser(new CGeomParser()), m_pPara(p)
{
}


CFtrParser::~CFtrParser()
{
}


void CFtrParser::getFtrLyrData(CFtrLayer* ptLayer, vector<osgFtr> &vecFtr, CDlgDataSource* pDS)
{
	int objCnt = ptLayer->GetObjectCount();
	if (objCnt <= 0) return;
	
	if (!m_pPara) return;
	
	COLORREF clr = ptLayer->GetColor();
	double r = GetRValue(clr) / 255.0;
	double g = GetGValue(clr) / 255.0;
	double b = GetBValue(clr) / 255.0;
	PT_3D rgb(r, g, b);
	for (int i = 0; i < objCnt; i++)
	{
		CFeature* feature = ptLayer->GetObject(i);

		if (feature == NULL)
		{
			continue;
		}

		getAllFtrInfo(feature, ptLayer, pDS, vecFtr,  rgb);

	}
	
}

void DrawArrow1(PT_3D pt0, PT_3D pt1, double scale, COLORREF color, GrBuffer *pBuf)
{
	double k = 1.0 / scale;
	double cosa = 1, sina = 0, dz = 0;
	double dis = GraphAPI::GGet2DDisOf2P(pt0, pt1);
	if (dis > 1e-4)
	{
		cosa = (pt1.x - pt0.x) / dis;
		sina = (pt1.y - pt0.y) / dis;
		dz = (pt1.z - pt0.z) / dis;
	}

	PT_3D pt2, pt3, pt4, pt5;
	pt2.x = pt0.x + k*cosa * 20; pt2.y = pt0.y + k*sina * 20; pt2.z = pt0.z + k*dz * 20;
	pt3.x = pt0.x + k*cosa * 35; pt3.y = pt0.y + k*sina * 35; pt3.z = pt0.z + k*dz * 35;
	pt4.x = pt2.x - k*sina * 5;  pt4.y = pt2.y + k*cosa * 5;  pt4.z = pt2.z;
	pt5.x = pt2.x + k*sina * 5;  pt5.y = pt2.y - k*cosa * 5;  pt5.z = pt2.z;

	pBuf->BeginLineString(color, 0.0);
	pBuf->MoveTo(&pt0);
	pBuf->LineTo(&pt2);
	pBuf->End();

	pBuf->BeginPolygon(color);
	pBuf->MoveTo(&pt3);
	pBuf->LineTo(&pt4);
	pBuf->LineTo(&pt5);
	pBuf->LineTo(&pt3);
	pBuf->End();
}
GrBuffer* FeatureToBuffer(CFeature* pFtr, CDlgDataSource* pDS)
{
	CGeometryBase *pObj = pFtr->GetGeometry();
	COLORREF hiColor = RGB(255, 0, 255);
	GrBuffer *pBuf = new GrBuffer();
	float scale = pDS->GetSymbolDrawScale();
	if (pObj->IsKindOf(RUNTIME_CLASS(CGeoText)))
	{
		((CGeoText*)pObj)->Draw(pBuf, scale);

	}
	else if (pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
	{
		CGeometry *pNewObj = pFtr->GetGeometry()->Clone();
		((CGeoSurface*)pNewObj)->EnableFillColor(TRUE, hiColor);
		((CGeoSurface*)pNewObj)->SetTransparency(50);
		pNewObj->Draw(pBuf, scale);
		delete pNewObj;
	}
	else
	{
		pFtr->GetGeometry()->Draw(pBuf, scale);
	}

	pBuf->SetAllColor(hiColor);
	pBuf->SetAllLineWidth(FALSE, 2);
	CArray<PT_3DEX, PT_3DEX> arrPts;
	pFtr->GetGeometry()->GetShape(arrPts);
	//绘制起点箭头
	if (0)
	{
		
		if (arrPts.GetSize() > 1)
		{

			CArray<PT_3DEX, PT_3DEX> arrPts2;
			PT_3D pt0 = arrPts[0];
			PT_3D pt1 = pt0;
			if (pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
			{
				((CGeoCurveBase*)pObj)->GetShape()->GetPts(arrPts2);
				for (int k = 1; k < arrPts2.GetSize(); k++)
				{
					if (!GraphAPI::GIsEqual2DPoint(&arrPts2[k], &arrPts2[k - 1]))
					{
						pt1 = arrPts2[k];
						break;
					}
				}
			}

			DrawArrow1(pt0, pt1, scale, hiColor, pBuf);

			if (pObj->IsKindOf(RUNTIME_CLASS(CGeoDCurve)))
			{
				arrPts2.RemoveAll();
				((CGeoDCurve*)pObj)->GetAssistShape(arrPts2);
				CGeoCurve obj;
				obj.CreateShape(arrPts2.GetData(), arrPts2.GetSize());
				arrPts2.RemoveAll();
				obj.GetShape()->GetPts(arrPts2);
				if (arrPts2.GetSize() > 1)
				{
					pt0 = arrPts2[0];
					pt1 = pt0;

					for (int k = 1; k < arrPts2.GetSize(); k++)
					{
						if (!GraphAPI::GIsEqual2DPoint(&arrPts2[k], &arrPts2[k - 1]))
						{
							pt1 = arrPts2[k];
							break;
						}
					}

					DrawArrow1(pt0, pt1, scale, RGB(128, 0, 128), pBuf);
				}
			}
		}
	}
	if (pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) || arrPts.GetSize()>1)
	{
		hiColor = RGB(0, 255, 255);
		pBuf->BeginPointString(hiColor, 1, 1);
		PT_3D  pts;
		for (int i = 0; i < arrPts.GetCount(); i++)
		{
			pts.x = arrPts[i].x;
			pts.y = arrPts[i].y;
			pts.z = arrPts[i].z;
			pBuf->PointString(&pts, TRUE);
		}

	}

	return pBuf;
}
void CFtrParser::getAllFtrInfo(CFeature* pFtr, CFtrLayer *pLayer, CDlgDataSource* pDS, 
	vector<osgFtr> &vecFtr,
	const PT_3D & rgb, bool isSelect)
{
	if (!m_pPara) return;
	auto pVecView = m_pPara->getOsgbVecView();
	bool isNoHatch = pVecView?pVecView->isNoHatch():true;
	if (!pDS)
	{
		CDlgDoc* dlg = m_pPara->GetDocument();

		pDS = dlg->GetDlgDataSourceOfFtr(pFtr);
	}
	if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)))
	{
		((CGeoMultiSurface*)pFtr->GetGeometry())->NormalizeDirection();
	}
	GrBuffer* buf = isSelect ? FeatureToBuffer(pFtr, pDS) : nullptr;

	if (buf == nullptr)
	{
		buf = new GrBuffer;
		
		if ( pLayer->IsSymbolized())
		{
			if (pDS)
			{
				pDS->DrawFeature(pFtr, buf, pFtr->GetGeometry()->IsSymbolizeText(), 0);
			}
		}
		else
		{
			if (pDS)
			{
				pFtr->Draw(buf, pDS->GetSymbolDrawScale());
			}
		}
	}
	OUID id = pFtr->GetID();
	string idStr = id.ToString();
	const char *code = pFtr->GetCode();
	bool bLocal = (code == NULL || strlen(code) <= 0);

	vector<osgFtr> tempFtr;
	CBufParser parser(idStr, rgb, bLocal);
	parser.getBufData(buf, tempFtr, !isNoHatch);
	delete buf;
	buf = nullptr;
	
	bool visible = pFtr->IsVisible();
	for (auto &temp : tempFtr)
	{
		temp.isVisible = visible;
		temp.isLocal = bLocal;
	}
	if (!isSelect)
		getFtrWhiteModel(pFtr, m_pPara->m_map_featerID_WhiteModelID, tempFtr);
 	vecFtr.insert(vecFtr.end(), tempFtr.begin(), tempFtr.end());
	
}

void CFtrParser::getAllFtrInfo(CFeature* pFtr, std::vector<osgCall::osgFtr> &vecFtr, bool isSelect)
{
	if (!m_pPara) return;
	CDlgDoc* dlg = m_pPara->GetDocument();

	CDlgDataSource* pDS = dlg->GetDlgDataSourceOfFtr(pFtr);
	CFtrLayer *layer = pDS->GetFtrLayerOfObject(pFtr);
	
	if (!layer) return;
	COLORREF clr = layer->GetColor();
	double r = GetRValue(clr) / 255.0;
	double g = GetGValue(clr) / 255.0;
	double b = GetBValue(clr) / 255.0;
	PT_3D rgb(r, g, b);
	getAllFtrInfo(pFtr,layer, pDS, vecFtr, rgb, isSelect);
	
}

void CFtrParser::getDataSourceInfo(CDlgDataSource* dataSource, std::vector<osgCall::osgFtr> &vecFtr)
{
	if (!dataSource) return;
	int layerCnt = dataSource->GetFtrLayerCount();
	std::vector<osgCall::osgFtr> tempFtr;
	for (int i = 0; i < layerCnt; i++)
	{
		CFtrLayer* layer = dataSource->GetFtrLayerByIndex(i);

		if (layer == NULL || layer->GetObjectCount() <= 0)
		{
			continue;
		}
		getFtrLyrData(layer, tempFtr, dataSource);
	}
	COLORREF clrMono;
	BOOL bEnable;
	dataSource->GetMonoColor(&bEnable, &clrMono);
	if (bEnable)
	{
		double r = GetRValue(clrMono) / 255.0;
		double g = GetGValue(clrMono) / 255.0;
		double b = GetBValue(clrMono) / 255.0;
		PT_3D color(r, g, b);
		if (!m_pPara->m_map_featerID_WhiteModelID.empty())
		{
			std::set<std::string> wmidset;
			for (const auto & i : m_pPara->m_map_featerID_WhiteModelID)
			{
				wmidset.insert(i.second.id);
			}
			for (osgFtr & ftr : tempFtr)
			{
				std::string & id = ftr.mId;
				if (wmidset.find(id) != wmidset.end()) continue;

				ftr.rgb.x = color.x;
				ftr.rgb.y = color.y;
				ftr.rgb.z = color.z;
			}
		}
		else
		{
			for (osgFtr & ftr : tempFtr)
			{
				ftr.rgb.x = color.x;
				ftr.rgb.y = color.y;
				ftr.rgb.z = color.z;
			}
		}
	}
	vecFtr.insert(vecFtr.end(), tempFtr.begin(), tempFtr.end());
}

void CFtrParser::getFtrLyrDataOnlyID(CFtrLayer* pLayer, std::vector<osgCall::osgFtr> &vecFtr)
{
	int objCnt = pLayer->GetObjectCount();
	if (objCnt <= 0) return;

	if (!m_pPara) return;

	std::vector<osgCall::osgFtr> tempFtr;
	tempFtr.reserve(objCnt);
	for (int i = 0; i < objCnt; i++)
	{
		CFeature* pFtr = pLayer->GetObject(i);

		if (pFtr == NULL)
		{
			continue;
		}
		osgFtr of;
		OUID id = pFtr->GetID();
		string idStr = id.ToString();
		of.mId = idStr;
		tempFtr.push_back(of);
	}
	vecFtr.insert(vecFtr.end(), tempFtr.begin(), tempFtr.end());

}


void CFtrParser::getFtrWhiteModel(CFeature* feature,
	const sid_wmid & featerID_WhiteModelID,
	std::vector<osgCall::osgFtr> &vecFtr)
{
	if (featerID_WhiteModelID.empty()) return;
	CDlgDoc * dlg = m_pPara->GetDocument();
	GrBuffer* buf = new GrBuffer;
	std::vector<osgCall::osgFtr> tempFtr;

	CFeature * pFtr = feature;
	auto pGeo = pFtr->GetGeometry();
	int gType = pGeo->GetClassType();
	string sid = pFtr->GetID().ToString();
	auto iter = featerID_WhiteModelID.find(sid);
	if (iter == featerID_WhiteModelID.end()) return;
	string id = iter->second.id;
	double bh = iter->second.h;
	switch (gType)
	{
	case CLS_GEOCURVE:
	case CLS_GEOSURFACE:
	case CLS_GEOMULTISURFACE:
	{
		CDlgDataSource* pDS = dlg->GetDlgDataSourceOfFtr(pFtr);
		buf->DeleteAll();
		if (pDS)
		{
			pFtr->Draw(buf, pDS->GetSymbolDrawScale());
		}
		CBufParser bp(id, PT_3D(1, 1, 1));
		bp.getBufDataToWhiteModel(buf, tempFtr, bh);
		CArray<PT_3DEX, PT_3DEX> pts;
		pGeo->GetShape(pts);
		for (int i = 0; i < pts.GetSize(); i++)
		{
			PT_3DEX & pt = pts[i];
			osgFtr line;
			line.vecCoord.push_back(Coord(pt.x, pt.y, pt.z));
			line.vecCoord.push_back(Coord(pt.x, pt.y, bh));
			line.type = CLS_GEOCURVE;
			line.mId = id;
			line.rgb = Coord(1, 1, 1);
			tempFtr.push_back(line);
		}
	}
	break;
	default:
		break;
	}
	if (!tempFtr.empty())
		vecFtr.insert(vecFtr.end(), tempFtr.begin(), tempFtr.end());

}

void CFtrParser::getFtrWhiteModel(const FTR_HANDLE * pftr, int num, const sid_wmid & featerID_WhiteModelID, std::vector<osgCall::osgFtr> &vecFtr)
{
	if (featerID_WhiteModelID.empty()) return;
	std::vector<osgCall::osgFtr> tempFtr;
	for (int i = 0; i < num; i ++)
	{
		getFtrWhiteModel(HandleToFtr(pftr[i]), featerID_WhiteModelID, tempFtr);
	}
	if (!tempFtr.empty())
		vecFtr.insert(vecFtr.end(), tempFtr.begin(), tempFtr.end());
}
