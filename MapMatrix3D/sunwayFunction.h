// sunwayFunction.h: interface for the sunwayFunction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUNWAYFUNCTION_H__C90580C8_8A55_4A17_BA89_636161481E03__INCLUDED_)
#define AFX_SUNWAYFUNCTION_H__C90580C8_8A55_4A17_BA89_636161481E03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "afxtempl.h"

//������������
enum LineTypeEnum
{
		e_Point =0,		//��
		e_Beeline =1,	//ֱ��
		e_Curve =2,		//����
		e_Arc =3,		//Բ��
		e_Circle =4,	//Բ
		e_EllipseArc = 5,   //��Բ��
		e_Ellipse = 6,      //��Բ
		e_Spline = 7,       //�⻬����
		e_Polyline = 8,     //������
		
		e_Dir_Point  = 9,  //�����
		e_Label = 10,      //��־��  
		e_Node = 11,       //���(���Ե������Ч)
		e_Group = 12,      //�����
};

//�ռ��
enum SSPointInfoEnum
{
	e_SS_Survey_Point    = 0x0001,	  //ʵ��Ļ�����
	e_SS_Height_Mark     = 0x0002,	  //�߳�ע�Ƿ�
	e_SS_Enter_Dtm       = 0x0004,   //�μӽ�ģ��
	e_SS_New_Point       = 0x0008,   //�²����ĵ��ɵ�
	e_SS_Turn_Point      = 0x0010,   //ת����һ���
	e_SS_Break_Point     = 0x0020,   //�ϵ�
	e_SS_LType_Line      = 0x0040,   //ֱ��
	e_SS_LType_Curve     = 0x0080,   //����
	e_SS_LType_Arc       = 0x00c0,   //Բ��
	e_SS_Flag_Point1     = 0x0100,   //��־1, Ϊ�����ڲ���������õı�־λ���������ݿ⣬ʹ�ú�Ӧ���
	e_SS_Flag_Point2     = 0x0200,   //��־2
	e_SS_Flag_Point3     = 0x0400,   //��־3
	e_SS_Flag_Point4     = 0x0800,   //��־4
	e_SS_Attach_Point    = 0x1000,   //������
	e_SS_Formicate_Point = 0x2000,   //Ⱥ����
	e_SS_Smooth_Point    = 0x4000,   //ƽ����
	e_SS_Framework_Point = 0x8000,   //������
	e_SS_Unknown_Point   = 0xf000,   //δ֪��
};

//���뷽ʽ
enum AlignmentEnum
{
	e_Centre = 0,	//����
	e_LeftTop,		//���Ͻ�
	e_LeftBottom,	//���½�
	e_LeftCentre,	//����
	e_RightTop,		//���Ͻ�
	e_RightCentre,	//����
	e_RightBottom,	//���½�
	e_TopCentre,	//����
	e_BottomCentre,	//����
	e_UnknownPos    //δ֪
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

//����ģ�崴��EDB�����ļ�
__declspec( dllimport ) BOOL CreateSunwayEDBFile(CString strTemplateFile, CString strEdbFile);
//���������Ϣ���ֽ���
__declspec( dllimport ) BOOL StoreEdbGraphics(SSPoint3DLIST &__Points, COleVariant& var);
//���ֽ�����ȡ������Ϣ
__declspec( dllimport ) BOOL RestoreEdbGraphics(SSPoint3DLIST &__Points, COleVariant& var);


#endif // !defined(AFX_SUNWAYFUNCTION_H__C90580C8_8A55_4A17_BA89_636161481E03__INCLUDED_)
