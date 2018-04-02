#include "stdafx.h"
#include "BufParser.h"
#include <string>
#include "Permanent.h"
#include "ICallBack.h"
#include "GrBuffer.h"


using namespace std;
using namespace osgCall;

void replace(string & str, char s, char d)
{
	string n;
	n.reserve(str.size());
	for (char _ : str)
	{
		if (_ == s)
		{
			if (d != 0)
				n += d;
		}
		else
			n += _;
	}
	str = n;
}

CBufParser::CBufParser(string idStr, PT_3D layerClr, bool bIsLocal)
	: mIdStr(idStr), mLayerClr(layerClr), mbLocal(bIsLocal)
{
}

void CBufParser::getBufData(const GrBuffer *pBuf, vector<osgFtr> &vecFtr, bool fillPolygon)
{
	//遍历坐标点 并转换坐标
	GrBuffer allBuf;
	allBuf.CopyFrom(pBuf);
	GrBuffer tempBuf;

	allBuf.GetAllGraph(&tempBuf);

	const Graph *gr = tempBuf.HeadGraph();
	if (!gr) return;

	//DWORD clr = gr->color;

	auto getRgb = [&](DWORD clr){
		double r = GetRValue(clr) / 255.0;
		double g = GetGValue(clr) / 255.0;
		double b = GetBValue(clr) / 255.0;

		if (r == 0 && g == 0 && b == 0 || r == 1 && g == 1 && b == 1)
		{
			PT_3D clr(mLayerClr);
			return clr;
		}
		else
		{
			PT_3D color(r, g, b);
			return color;
		}

	};

	while (gr)
	{
		PT_3D color = getRgb(gr->color);
		if (IsGrPoint(gr))
		{
			const GrPoint *cgr = (const GrPoint*)gr;
			PT_3D pt0 = cgr->pt;

			addPt(pt0, color, vecFtr);
		}
		else if (IsGrPointString(gr) || IsGrPointStringEx(gr))
		{
			const GrPointString *cgr = (const GrPointString*)gr;
			const GrVertexList *pList = &(cgr->ptlist);

			addPtString(pList, color, vecFtr);
		}
		else if (IsGrLineString(gr))
		{
			const GrLineString *cgr = (const GrLineString*)gr;
			const GrVertexList *pList = &(cgr->ptlist);
			
			addLine(pList, color, vecFtr);
		}
		else if (IsGrPolygon(gr))
		{
			if (fillPolygon)
			{
				const GrPolygon *cgr = (const GrPolygon*)gr;

				addPolygon(cgr, color, vecFtr);
			}
		}
		else if (IsGrText(gr))
		{
			const GrText *cgr = (const GrText*)gr;
			PT_3D location = cgr->pt;
			string text = cgr->text;
			replace(text, '\r', 0);
			addText(location, text, color, cgr->settings, vecFtr);
		}

		gr = gr->next;
	}
}



void CBufParser::getBufDataToWhiteModel(const GrBuffer *pBuf, std::vector<osgCall::osgFtr> &vecFtr, double bh)
{
	GrBuffer allBuf;
	allBuf.CopyFrom(pBuf);
	GrBuffer tempBuf;

	allBuf.GetAllGraph(&tempBuf);

	const Graph *gr = tempBuf.HeadGraph();
	if (!gr) return;
	Coord rgb(mLayerClr.x, mLayerClr.y, mLayerClr.z);
	while (gr)
	{
		if (IsGrLineString(gr))
		{
			const GrLineString *cgr = (const GrLineString*)gr;
			const GrVertexList *pList = &(cgr->ptlist);
			std::vector<osgCall::osgFtr> tempFtr;
			addLine(pList, mLayerClr, tempFtr);
			if (tempFtr.size() > 0)
			{
				int ssize = tempFtr.size();
				for (int i = 0; i < ssize; i++)
				{
					osgFtr & last = tempFtr[i];
					auto & vecCoord = last.vecCoord;
					std::vector<Coord> newvecCoord;
					newvecCoord.reserve(vecCoord.size());
					newvecCoord.push_back(vecCoord[0]);
					for (int i = 1; i < vecCoord.size(); i++)
					{
						if (newvecCoord.back() == vecCoord[i]) continue;
						newvecCoord.push_back(vecCoord[i]);
					}
					vecCoord = newvecCoord;
					for (int i = 0; i < newvecCoord.size(); i++)
					{
						Coord & coord = newvecCoord[i];
						coord.z = bh;
					}
					osgFtr line;
					line.vecCoord = newvecCoord;
					line.type = CLS_GEOCURVE;
					line.mId = mIdStr;
					line.rgb = rgb;
					tempFtr.push_back(line);
				}
				

				vecFtr.insert(vecFtr.end(), tempFtr.begin(), tempFtr.end());
			}

		}
		gr = gr->next;
	}
}

void CBufParser::addPt(const PT_3D& pt0, const PT_3D& color, vector<osgFtr> &vecFtr)
{
	Coord xyz(pt0.x, pt0.y, pt0.z);

	vector<Coord> coords;
	coords.push_back(xyz);
	osgFtr pt;
	pt.vecCoord = coords;
	pt.type = CLS_GEOPOINT;
	pt.rgb.x = color.x;
	pt.rgb.y = color.y;
	pt.rgb.z = color.z;
	pt.mId = mIdStr;

	vecFtr.push_back(pt);
}

void CBufParser::addPtString(const GrVertexList *list, const PT_3D& color, vector<osgFtr> &vecFtr)
{
	for (int i = 0; i < list->nuse; i++)
	{
		GrVertex pt0 = list->pts[i];
		addPt(pt0, color, vecFtr);
	}
}

void CBufParser::addLine(const GrVertexList *list, const PT_3D& color, vector<osgFtr> &vecFtr)
{
	vector<Coord> coords;

	for (int i = 0; i < list->nuse; i++)
	{
		GrVertex pt0 = list->pts[i];
		long code = pt0.code;

		/*部分符号库不需要moveto作为起点而是以特定值作为起点*/
		/*把之前的线保存，以该点为新的起点*/
		if (IsGrPtCodeMoveTo(&pt0))
		{
			if (coords.size() > 0)
			{
				osgFtr line;
				line.vecCoord = coords;
				line.type = CLS_GEOCURVE;
				line.mId = mIdStr;

				Coord rgb(color.x, color.y, color.z);
				line.rgb = rgb;
				vecFtr.push_back(line);
			}

			coords.clear();
		}

		Coord xyz(pt0.x, pt0.y, pt0.z);
		coords.push_back(xyz);
	}

	//最后一段线保存
	if (coords.size() > 0)
	{
		osgFtr line;
		line.vecCoord = coords;
		line.type = CLS_GEOCURVE;
		line.mId = mIdStr;

		Coord rgb(color.x, color.y, color.z);
		line.rgb = rgb;

		vecFtr.push_back(line);
	}
}

void CBufParser::addPolygon(const GrPolygon *cgr, const PT_3D& color, vector<osgFtr> &vecFtr)
{
	vector<Coord> coords;
	const GrVertexList *list = &(cgr->ptlist);

	for (int i = 0; i < list->nuse; i++)
	{
		PT_3D pt0 = list->pts[i];
		Coord xyz(pt0.x, pt0.y, pt0.z);

		coords.push_back(xyz);
	}

	osgFtr poly;
	poly.vecCoord = coords;
	poly.type = CLS_GEOSURFACE;
	poly.mId = mIdStr;

	Coord rgb(color.x, color.y, color.z);
	poly.rgb = rgb;
	poly.alpha = 1.0 - cgr->index / 255.0;
	vecFtr.push_back(poly);
}

void CBufParser::addText(const PT_3D& pt0, const string &text, const PT_3D& color, const TextSettings & setting, vector<osgFtr> &vecFtr)
{
	vector<Coord> coords;
	coords.push_back(Coord(pt0.x, pt0.y, pt0.z));

	osgFtr textFtr;
	textFtr.str = text;
	textFtr.type = CLS_GEOTEXT;
	textFtr.vecCoord = coords;
	textFtr.textSize = setting.fHeight;
	textFtr.nAlign = setting.nAlignment;
	textFtr.fTextAngle = setting.fTextAngle;
	textFtr.mId = mIdStr;
	Coord rgb(color.x, color.y, color.z);
	textFtr.rgb = rgb;
	vecFtr.push_back(textFtr);
}

