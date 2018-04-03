// sunwayFunction.h: interface for the sunwayFunction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUNWAYFUNCTION_H__C90580C8_8A55_4A17_BA89_636161481E03__INCLUDED_)
#define AFX_SUNWAYFUNCTION_H__C90580C8_8A55_4A17_BA89_636161481E03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "afxtempl.h"

//几何特征类型
enum LineTypeEnum
{
		e_Point =0,		//点
		e_Beeline =1,	//直线
		e_Curve =2,		//曲线
		e_Arc =3,		//圆弧
		e_Circle =4,	//圆
		e_EllipseArc = 5,   //椭圆弧
		e_Ellipse = 6,      //椭圆
		e_Spline = 7,       //光滑曲线
		e_Polyline = 8,     //多义线
		
		e_Dir_Point  = 9,  //有向点
		e_Label = 10,      //标志点  
		e_Node = 11,       //结点(仅对点地物有效)
		e_Group = 12,      //组代理
};

//空间点
enum SSPointInfoEnum
{
	e_SS_Survey_Point    = 0x0001,	  //实测的或计算的
	e_SS_Height_Mark     = 0x0002,	  //高程注记否
	e_SS_Enter_Dtm       = 0x0004,   //参加建模否
	e_SS_New_Point       = 0x0008,   //新产生的点或旧点
	e_SS_Turn_Point      = 0x0010,   //转向点或一般点
	e_SS_Break_Point     = 0x0020,   //断点
	e_SS_LType_Line      = 0x0040,   //直线
	e_SS_LType_Curve     = 0x0080,   //曲线
	e_SS_LType_Arc       = 0x00c0,   //圆弧
	e_SS_Flag_Point1     = 0x0100,   //标志1, 为方便内部计算而设置的标志位，不存数据库，使用后应清空
	e_SS_Flag_Point2     = 0x0200,   //标志2
	e_SS_Flag_Point3     = 0x0400,   //标志3
	e_SS_Flag_Point4     = 0x0800,   //标志4
	e_SS_Attach_Point    = 0x1000,   //依附点
	e_SS_Formicate_Point = 0x2000,   //群集点
	e_SS_Smooth_Point    = 0x4000,   //平滑点
	e_SS_Framework_Point = 0x8000,   //特征点
	e_SS_Unknown_Point   = 0xf000,   //未知点
};

//对齐方式
enum AlignmentEnum
{
	e_Centre = 0,	//中心
	e_LeftTop,		//左上角
	e_LeftBottom,	//左下角
	e_LeftCentre,	//左中
	e_RightTop,		//右上角
	e_RightCentre,	//右中
	e_RightBottom,	//右下角
	e_TopCentre,	//上中
	e_BottomCentre,	//下中
	e_UnknownPos    //未知
};

class AFX_EXT_CLASS CSSPoint3D
{
public:
	double x;
	double y;
	double z;
	short  nType;    //SSPointInfoEnum
	CString strName;

	CSSPoint3D()
	{
		x=y=z=0;
		nType = 0;
		strName=_T("");
	}
};

typedef CArray<CSSPoint3D, CSSPoint3D&> SSPoint3DLIST;

//根据模板创建EDB工程文件
__declspec( dllimport ) BOOL CreateSunwayEDBFile(CString strTemplateFile, CString strEdbFile);
//保存点列信息到字节流
__declspec( dllimport ) BOOL StoreEdbGraphics(SSPoint3DLIST &__Points, COleVariant& var);
//从字节流获取点列信息
__declspec( dllimport ) BOOL RestoreEdbGraphics(SSPoint3DLIST &__Points, COleVariant& var);


#endif // !defined(AFX_SUNWAYFUNCTION_H__C90580C8_8A55_4A17_BA89_636161481E03__INCLUDED_)
