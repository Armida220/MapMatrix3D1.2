#pragma once
#include <string>
#include <memory>
#include <vector>
#include "ICallBack.h"
#include <deque>
#include <windows.h>

#ifdef MFCVIEWERIBDLL  
#define MFCVIEWERAPI _declspec(dllexport)  
#else  
#define MFCVIEWERAPI  _declspec(dllimport)  
#endif  


namespace osgDraw
{
	class IBaseDrawer;
}

namespace osgEvent
{
	class CBaseUserEvent;
}

namespace osgCall
{
	class ICallBack;
}

namespace mm3dView
{

	/**
	* @brief IOsgMfcViewer ��mfc����н�����ά��Ⱦ��������ӿ�
	*/

	class MFCVIEWERAPI IOsgMfcViewer
	{
	public:
		/**
		* @brief getScale ���ݾ����ó߶�
		*/
		virtual double getScale() = 0;

		/**
		* @brief getScaleFromDist ���ݾ����ó߶�,�����ڻ��ư뾶
		*/
		virtual double getScaleFromDist() = 0;

		/**
		* @brief getScrren2WorldMat ���osg����Ļ������ľ���
		*/
		virtual double* getScrren2WorldMat() = 0;

		/**
		* @brief InitOSG ��ʼ��osg�����
		* @param modelname ��ʼ���ĳ�������
		*/
		virtual void InitOSG(std::string modelname) = 0;

		/**
		* @brief StartRendering ��ʼ������Ⱦ��������Ⱦ�߳�
		*/
		virtual HANDLE StartRendering() = 0;

		/**
		* @brief addScene  ����ļ���������
		* @param strModel  ģ��·����
		*/
		virtual void addScene(const std::string &strModel) = 0;

		/**
		* @brief removeAllScene  �����������������
		*/
		virtual void removeAllScene() = 0;

		/**
		* @brief SetSceneExtent ���ó�����Χ
		*/
		virtual void SetSceneExtent() = 0;

		/**
		* @brief home �ؼ�
		*/
		virtual void home() = 0;

		/**
		* @brief light ������Դ
		*/
		virtual void light() = 0;

		/**
		* @brief setCall ���ûص�
		* @param ic  �ص�
		*/
		virtual void setCall(const std::shared_ptr<osgCall::ICallBack> &ic) = 0;

		/**
		* @brief isLinePick ͨ����ֱ��ײ��⣬�Ƿ��ܹ���õ�
		* @brief x y ƽ���ά����д�ֱ��ײ���
		*/
		virtual bool isLinePick(const double x, const double y) = 0;

		/**
		* @brief addObj ����ά�����ݽ�����������
		* @param vecFtr ʸ������������
		*/
		virtual void addObj(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief removeObj ����ά�����ݽ����������
		* @param vecFtr ʸ������������
		*/
		virtual void removeObj(const std::vector<osgCall::osgFtr> &vecFtr) = 0;
		virtual void removeObj(const std::vector<std::string> &vecID) = 0;

		/**
		* @brief driveTo �ƶ�������
		* @brief x y ƽ���ά���ƶ�
		*/
		virtual void driveTo(const double x, const double y) = 0;

		/**
		* @brief panTo ƽ�Ƶ��㣨x,y),����������ǶȲ���
		* @brief x y ƽ���ά���ƶ�
		*/
		virtual void panTo(const double x, const double y) = 0;

		/**
		* @brief zoomTo ���ŵ����Զ�ά��ͬһ�߶�,����������ǶȲ���
		* @brief scale ���Զ�ά��ͬһ�߶�
		*/
		virtual void zoomTo(const double scale) = 0;

		/**
		* @brief synData ����ά�����ݺ�vecview����ͬ��
		* @param vecFtr ʸ������������
		*/
		virtual void synData(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief updataDate ����ά�����ݺ�vecview���и���
		* @param vecFtr ʸ������������
		*/
		virtual void updataDate(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief synHighLightData ����ά�����ݺ�vecview���и���ͬ��
		* @param vecHighFtr ʸ������������
		*/
		virtual void synHighLightData(const std::vector<osgCall::osgFtr> &vecHighFtr, bool bIsOsgbAcitve = true) = 0;

		/**
		* @brief clearHighLightData �����������
		*/
		virtual void clearHighLightData() = 0;

		/**
		* @brief addConstDragLine ���Ӹ��������
		* @param vecFtr ʸ������������
		*/
		virtual void addConstDragLine(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief setConstDragLine ���ø��������
		* @param vecFtr ʸ������������
		*/
		virtual void setConstDragLine(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief setVariantDragLine ������ʱ�Ķ�̬��
		* @param vecFtr ʸ������������
		*/
		virtual void setVariantDragLine(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief addVariantDragLine ������ʱ�Ķ�̬��
		* @param vecFtr ʸ������������
		*/
		virtual void addVariantDragLine(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief UpdatesnapDraw ���ò�׽ʱ��ķ������ڱ�ʾ�õ㱻��׽��
		* @param vecFtr ʸ������������
		*/
		virtual void UpdatesnapDraw(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief addGraph ����tin��dem����,����Ľ���
		* @param vecFtr ʸ������������
		*/
		virtual void addGraph(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief delGraph ɾ��graph
		*/
		virtual void delGraph() = 0;

		/**
		* @brief clearDragLine �������
		*/
		virtual void clearDragLine() = 0;

		/**
		* @brief linePickPt �������,�����޸�Ϊ�̰߳�ȫ
		* @param ƽ��xy ����
		*/
		virtual std::vector<double> linePickPt(const double x, const double y) const = 0;

		/**
		* @brief viewAll ��ʾ����map�ڵ�
		*/
		virtual void viewAll(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief ViewHideAll ��������map�ڵ�
		*/
		virtual void ViewHideAll(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief viewLocal ֻ��ʾ����
		* @param vecFtr ʸ������������
		*/
		virtual void viewLocal(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief viewExternal ֻ��ʾ�ⲿ��
		* @param vecFtr ʸ������������
		*/
		virtual void viewExternal(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief getOsgbExtent ��ȡosgb��Χ�� ���ص�������
		* @param left ��Χ��ߣ���������
		* @param right ��Χ�ұߣ���������
		* @param top ��Χ���棬��������
		* @param bottom ��Χ���棬��������
		*/
		virtual void getOsgbExtent(double &left, double &right, double &top, double &bottom) = 0;

		/**
		* @brief topView  ��ά��ͼ�л������ӽ�
		*/
		virtual void topView() = 0;

		/**
		* @brief leftView  ��ά��ͼ�л������ӽ�
		*/
		virtual void leftView() = 0;

		/**
		* @brief leftView  ��ά��ͼ�л������ӽ�
		*/
		virtual void rightView() = 0;

		/**
		* @brief islineBlock  �ж��߶��Ƿ���ģ���ཻ���ڵ�
		* @brief x1 y1 z1  �߶ε���ʼ��
		* @brief x2 y2 z2  �߶ε��յ�
		*/
		virtual bool islineBlock(double x1, double y1, double z1,
			double x2, double y2, double z2) = 0;

		/**
		* @brief drawVaryPt  ���Ʋο���,��Ϊ��Ӱ�񻥶��ĵ�
		* @brief x y z ��ά��
		*/
		virtual bool drawVaryPt(double x, double y, double z) = 0;

		/**
		* @brief clearVaryGroup ���ǰ������ĸ������
		*/
		virtual void clearVaryGroup() = 0;

		/**
		* @brief startEvent ��������¼�
		* @param order  �¼���˳��
		*/
		virtual void startEvent(int order) = 0;

		/**
		* @brief absOri ���Զ���
		* @param oriInfo ���Զ�����Ϣ
		*/
		virtual std::string absOri(std::string oriInfo) = 0;

		/**
		* @brief driveToRelaPt ��λ����Ե�
		* @param relaX  ��Ե�x
		* @param relaY  ��Ե�y
		* @param relaZ  ��Ե�z
		*/
		virtual void driveToRelaPt(double relaX, double relaY, double relaZ) = 0;

		/**
		* @brief showOriPt ��ʾ���Զ���̵���
		* @param oriRes �̵��� 
		*/
		virtual void showOriPt(std::string oriRes) = 0;

		/**
		* @brief predictOriPt Ԥ����Զ���̵�
		* @param oriRes �̵���
		*/
		virtual std::string predictOriPt(std::string oriRes) = 0;

		/**
		* @brief fetchOriData ��ȡ��������
		*/
		virtual void fetchOriData() = 0;

		/**
		* @brief clearOriPt ����̵���
		*/
		virtual void clearOriPt() = 0;
		
		virtual void groundToScreen(double x, double y, double z, double & _x, double &_y) = 0;
		
		virtual bool screenToGround(double x, double y, double & _x, double &_y, double &_z) = 0;

		virtual bool shiftOperation() = 0;

		virtual bool shiftCatchCorner() = 0;

		virtual bool lockHeight(int x, int y) = 0;

		virtual bool updataFtrView(bool bShow) = 0;

	};

	/**
	* @brief IOsgMfcViewerFactory ������ά��Ⱦ��������ӿڵĹ���
	*/

	class MFCVIEWERAPI IOsgMfcViewerFactory
	{
	public:
		/**
		* @brief create �����ӿ�
		* @param hWnd   ���
		*/
		static std::shared_ptr<IOsgMfcViewer> create(HWND hWnd);
	};

}

