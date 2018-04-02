#pragma once
#include <osg/Referenced>
#include <string>

namespace mm3dView
{
	/**
	* @brief COsgUserData ����osg node�ڵ��е���������
	*/
	class COsgUserData : public osg::Referenced
	{
	public:
		COsgUserData();

		/**
		* @brief idString �ڵ��Ψһ��ʶ
		*/
		std::string idString;

		/**
		* @brief bIsLocal �Ƿ��Ǳ�������
		*/
		bool bIsLocal;

	protected:
		virtual ~COsgUserData();

	};
}

