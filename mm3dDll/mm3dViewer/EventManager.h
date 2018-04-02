#pragma once
#include <vector>
#include "BaseUserEvent.h"
#include "osgViewer/Viewer"
#include <osg/Node>

namespace osgEvent
{
	/**
	* @brief �л��¼�����������������¼���ӵ�ж�����¼����������¼��н����л�
	*/
	class CSwitchEventManager : public CBaseUserEvent
	{
	public:
		CSwitchEventManager(osgViewer::Viewer* viewer);

		/**
		* @brief addEvnet �����¼�
		* @param key  �¼������
		* @param name �¼�������
		* @param ce ���¼�
		*/
		virtual void addEvent(int key, std::string name, CBaseUserEvent *ce);

		/**
		* @brief selectEventByNum �������ѡ���¼�
		* @param num ���
		*/
		virtual void selectEventByNum(unsigned int num);

		/**
		* @brief setCtrlPtNo ���ÿ��Ƶ����
		* @param ctrlNo ���Ƶ����
		*/
		/*virtual void setCtrlPtNo(std::string ctrlNo);*/

		/**
		* @brief showOriPt ��ʾ�����
		* @param oriRes �̵���
		*/
		virtual void showOriPt(std::string oriRes);

		/**
		* @brief clearOriPt ��������
		*/
		virtual void clearOriPt();

		/**
		* @brief fetchOriData ��ȡ��������
		*/
		virtual void fetchOriData();
		
		virtual void shiftHeightLock(float x, float y);

		/**
		* @brief shiftCatchCorner �л���׽�ڽǵ�
		*/
		virtual void shiftCatchCorner();

		/**
		* @brief shiftOperation �ı佻�������ķ�ʽ
		*/
		virtual void shiftOperation();

	protected:
		bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	private:
		typedef std::pair<std::string, osg::ref_ptr<CBaseUserEvent>> NamedBaseEvent;
		typedef std::map<int, NamedBaseEvent> KeyMainMap;

		/**
		* @brief �¼�map����
		*/
		KeyMainMap _manips;

		/**
		* @brief ��ǰ�¼�
		*/
		osg::ref_ptr<CBaseUserEvent> _current;

	};

}
