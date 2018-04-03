#pragma once
#include <osgViewer/Viewer>
#include <OpenThreads/ReadWriteMutex>
#include "qcomm.h"

namespace mm3dView
{
	/**
	* @brief CSelDefViewer �Զ������ά��������̳���osgviewer��������ķ���������д���Զ��߳�������м�������
	*/
	class CSelDefViewer : public osgViewer::Viewer
	{
	public:
		CSelDefViewer();

	protected:
		virtual ~CSelDefViewer();
		/**
		* @brief frame ��ά��Ⱦ��ÿһ֡����ģ����Ⱦÿһ֡����ͨ������������л���
		*/
		virtual void frame(double simulationTime = USE_REFERENCE_TIME);

	private:

		/**
		* @brief pushEvents �¼�
		*/
		std::deque<osg::ref_ptr<osgGA::Event>> pushEvents;

	};
}

