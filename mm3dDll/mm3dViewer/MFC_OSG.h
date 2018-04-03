#pragma once
#include "IViewer.h"
#include <osgViewer/Viewer>
#include <osg/Node>
#include <osgGA/TerrainManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <string>
#include <deque>
#include "seldefViewer.h"
#include "SelfDefTerrainManipulator.h"

namespace osgDraw
{
	class IBaseDrawer;
}

namespace osgEvent
{
	class CBaseUserEvent;
	class IOsgEvent;
}

namespace osgCall
{
	class ICallBack;
}

namespace mm3dView
{
	class COsgModel;
	class CSelDefViewer;
	class CSelfStateManipulator;
	class COsgSyn;

	/**
	* @brief cOSG ��mfc����н�����ά��Ⱦ�������
	*/

	class MFCVIEWERAPI cOSG : public IOsgMfcViewer
	{
	public:
		cOSG(HWND hWnd);
		~cOSG();

		/**
		* @brief getScale ���ݸ߶Ȼ�ó߶�
		*/
		virtual double getScale();

		/**
		* @brief getScaleFromDist ���ݾ����ó߶�,�����ڻ��ư뾶
		*/
		virtual double getScaleFromDist();

		/**
		* @brief getScrren2WorldMat ���osg����Ļ������ľ���
		*/
		virtual double* getScrren2WorldMat();

		/**
		* @brief InitOSG ��ʼ��osg�����
		* @param modelname ��ʼ���ĳ�������
		*/
		virtual void InitOSG(std::string modelname);

		/**
		* @brief StartRendering ��ʼ������Ⱦ��������Ⱦ�߳�
		*/
		virtual HANDLE StartRendering();

		/**
		* @brief addScene  ����ļ���������
		* @param strModel  ģ��·����
		*/
		virtual void addScene(const std::string &strModel);

		/**
		* @brief removeAllScene  �����������������
		*/
		virtual void removeAllScene();

		/**
		* @brief SetSceneExtent ���ó�����Χ
		*/
		virtual void SetSceneExtent();

		/**
		* @brief home �ؼ�
		*/
		virtual void home();

		/**
		* @brief light ������Դ
		*/
		virtual void light();

		virtual bool shiftOperation();
		
		virtual bool shiftCatchCorner();

		virtual bool lockHeight(int x, int y);

		virtual bool updataFtrView(bool bShow);

		/**
		* @brief getViewer ��ȡ�����ָ��
		*/
		inline osgViewer::Viewer* getViewer()
		{
			return mViewer;
		}

		/**
		* @brief setCall ���ûص�
		* @param ic  �ص�
		*/
		virtual void setCall(const std::shared_ptr<osgCall::ICallBack> &ic);

		/**
		* @brief driveTo �ƶ�������
		* @brief x y ƽ���ά���ƶ�
		*/
		virtual void driveTo(const double x, const double y);

		/**
		* @brief panTo ƽ�Ƶ��㣨x,y),����������ǶȲ���
		* @brief x y ƽ���ά���ƶ�
		*/
		virtual void panTo(const double x, const double y);

		/**
		* @brief zoomTo ���ŵ����Զ�ά��ͬһ�߶�,����������ǶȲ���
		* @brief scale ���Զ�ά��ͬһ�߶�
		*/
		virtual void zoomTo(const double scale);

		/**
		* @brief isLinePick ͨ����ֱ��ײ��⣬�Ƿ��ܹ���õ�
		* @brief x y ƽ���ά����д�ֱ��ײ���
		*/
		virtual bool isLinePick(const double x, const double y);

		/**
		* @brief addObj ����ά�����ݽ�����������
		* @param vecFtr ʸ������������
		*/
		virtual void addObj(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief removeObj ����ά�����ݽ����������
		* @param vecFtr ʸ������������
		*/
		virtual void removeObj(const std::vector<osgCall::osgFtr> &vecFtr);
		
		virtual void removeObj(const std::vector<std::string> &vecID);

		/**
		* @brief synData ����ά�����ݺ�vecview����ͬ��
		* @param vecFtr ʸ������������
		*/
		virtual void synData(const std::vector<osgCall::osgFtr> &vecFtr);


		/**
		* @brief updataDate ����ά�����ݺ�vecview���и���
		* @param vecFtr ʸ������������
		*/
		virtual void updataDate(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief synHighLightData ����ά�����ݺ�vecview���и���ͬ��
		* @param vecHighFtr ʸ������������
		* @param bIsOsgbAcitve osgbView�Ƿ��Ǽ���״̬, Ĭ���Ǽ����
		*/
		virtual void synHighLightData(const std::vector<osgCall::osgFtr> &vecHighFtr, bool bIsOsgbAcitve = true);

		/**
		* @brief clearHighLightData �����������
		*/
		virtual void clearHighLightData();

		/**
		* @brief addConstDragLine ���Ӹ��������
		* @param vecFtr ʸ������������
		*/
		virtual void addConstDragLine(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief setConstDragLine ���Ӹ��������
		* @param vecFtr ʸ������������
		*/
		virtual void setConstDragLine(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief setVariantDragLine ������ʱ�Ķ�̬��
		* @param vecFtr ʸ������������
		*/
		virtual void setVariantDragLine(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief addVariantDragLine ������ʱ�Ķ�̬��
		* @param vecFtr ʸ������������
		*/
		virtual void addVariantDragLine(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief UpdatesnapDraw ���ò�׽ʱ��ķ������ڱ�ʾ�õ㱻��׽��
		* @param vecFtr ʸ������������
		*/
		virtual void UpdatesnapDraw(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief addGraph ����tin��dem����,����Ľ���
		* @param vecFtr ʸ������������
		*/
		virtual void addGraph(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief delGraph ɾ��graph
		*/
		virtual void delGraph();

		/**
		* @brief clearDragLine �������
		*/
		virtual void clearDragLine();

		/**
		* @brief setCursor ��ά�ƶ�ʱ�����ù��
		* @brief x, y, z ����λ��
		*/
		virtual void setCursor(double x, double y, double z);

		/**
		* @brief clearGroupResult �������,
		* @param group ��Ҫ�����group
		*/
		void clearGroupResult(osg::ref_ptr<osg::Group> group) const;

		/**
		* @brief threadSafeClear �������,�����޸�Ϊ�̰߳�ȫ
		* @param group ��Ҫ�����group
		*/
		void threadSafeClear(osg::ref_ptr<osg::Group> group) const;

		/**
		* @brief linePickPt ͨ��ƽ��x,y ��������ֱ��ײ���
		* @param x y ƽ������
		*/
		virtual std::vector<double> linePickPt(const double x, const double y) const;

		/**
		* @brief viewAll ��ʾ����map�ڵ�
		*/
		virtual void viewAll(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief ViewHideAll ��������map�ڵ�
		*/
		virtual void ViewHideAll(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief viewLocal ֻ��ʾ����
		* @param vecFtr ʸ������������
		*/
		virtual void viewLocal(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief viewExternal ֻ��ʾ�ⲿ��
		* @param vecFtr ʸ������������
		*/
		virtual void viewExternal(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief getOsgbExtent ��ȡosgb��Χ�� ���ص�������
		* @param left ��Χ��ߣ���������
		* @param right ��Χ�ұߣ���������
		* @param top ��Χ���棬��������
		* @param bottom ��Χ���棬��������
		*/
		virtual void getOsgbExtent(double &left, double &right, double &top, double &bottom);

		/**
		* @brief topView  ��ά��ͼ�л������ӽ�
		*/
		virtual void topView();

		/**
		* @brief leftView  ��ά��ͼ�л������ӽ�
		*/
		virtual void leftView();

		/**
		* @brief rightView  ��ά��ͼ�л������ӽ�
		*/
		virtual void rightView();

		/**
		* @brief islineBlock  �ж��߶��Ƿ���ģ���ཻ���ڵ�
		* @brief x1 y1 z1  �߶ε���ʼ��
		* @brief x2 y2 z2  �߶ε��յ�
		*/
		virtual bool islineBlock(double x1, double y1, double z1,
			double x2, double y2, double z2);

		/**
		* @brief drawVaryPt  ���Ʋο���,��Ϊ��Ӱ�񻥶��ĵ�
		* @brief x y z ��ά��
		*/
		virtual bool drawVaryPt(double x, double y, double z);

		/**
		* @brief clearVaryGroup ���ǰ������ĸ������
		*/
		virtual void clearVaryGroup();

		/**
		* @brief startEvent ��������¼�
		* @param order  �¼���˳��
		*/
		virtual void startEvent(int order);

		/**
		* @brief absOri ���Զ���
		* @param oriInfo ���Զ�����Ϣ
		*/
		virtual std::string absOri(std::string oriInfo);

		/**
		* @brief driveToRelaPt ��λ����Ե�
		* @param relaX  ��Ե�x
		* @param relaY  ��Ե�y
		* @param relaZ  ��Ե�z
		*/
		virtual void driveToRelaPt(double relaX, double relaY, double relaZ);

		/**
		* @brief fetchOriData ��ȡ��������
		*/
		virtual void fetchOriData();

		/**
		* @brief showOriPt ��ʾ���Զ���̵���
		* @param oriRes �̵���
		*/
		virtual void showOriPt(std::string oriRes);

		/**
		* @brief predictOriPt Ԥ����Զ���̵�
		* @param oriRes �̵���
		*/
		virtual std::string predictOriPt(std::string oriRes);

		/**
		* @brief clearOriPt ����̵���
		*/
		virtual void clearOriPt();

		virtual void groundToScreen(double x, double y, double z, double & _x, double &_y);

		virtual bool screenToGround(double x, double y, double & _x, double &_y, double &_z);

	protected:
		/**
		* @brief InitManipulators ��ʼ���ٿ���
		*/
		virtual void InitManipulators(void);

		/**
		* @brief InitSceneGraph ��ʼ������
		*/
		virtual void InitSceneGraph(void);

		/**
		* @brief InitCameraConfig ��ʼ�����
		*/
		virtual void InitCameraConfig(void);

		/**
		* @brief setHomeCetner ���ûؼҵĵ�, ��Ĭ�ϵĸĳ�45�ȽǶ����¿�
		*/
		virtual void setHomeCetner();

		inline void Done(bool value) { mDone = value; }

		inline bool Done(void) { return mDone; }

		/**
		* @brief Render ������Ⱦ
		*/
		static void Render(void* ptr);

		/**
		* @brief initEvent ��ʼ���¼�
		*/
		virtual void initEvent();

	
	private:
		bool mDone;

		/**
		* @brief m_ModelName ģ��·����
		*/
		std::string m_ModelName;

		/**
		* @brief m_hWnd mfc����������ָ��
		*/
		HWND m_hWnd;

		/**
		* @brief mViewer �����ָ��
		*/
		osg::ref_ptr<CSelDefViewer> mViewer;
	
		/**
		* @brief terrain ģ�͵Ĳٿ����������εĽ��вٿأ��Ƚ��ʺ�������ͳ���
		*/
		osg::ref_ptr<CSelfDefTerrainManipulator> terrain;

		/**
		* @brief keyswitchManipulator �����л��ٿ����Ŀ�����
		*/
		osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator;

		/**
		* @brief curStateManipulator ��ǰ�����л�״̬�Ŀ�����
		*/
		CSelfStateManipulator* curStateManipulator;

		/**
		* @brief ic �ص��ӿ�
		*/
		std::shared_ptr<osgCall::ICallBack> icall;

		/**
		* @brief osgbModel osgģ��
		*/
		COsgModel* osgbModel;

		/**
		* @brief syn ����άͬ����
		*/
		COsgSyn* syn;

		/**
		* @brief pEvent �¼��ӿ�
		*/
		std::shared_ptr<osgEvent::IOsgEvent> pEvent;
	};

	class CRenderingThread : public OpenThreads::Thread
	{
	public:
		CRenderingThread(cOSG* ptr);
		virtual ~CRenderingThread();

		virtual void run();

	protected:
		cOSG* _ptr;
		bool _done;
	};
}





