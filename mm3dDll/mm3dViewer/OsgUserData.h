#pragma once
#include <osg/Referenced>
#include <string>

namespace mm3dView
{
	/**
	* @brief COsgUserData 放在osg node节点中的数据类型
	*/
	class COsgUserData : public osg::Referenced
	{
	public:
		COsgUserData();

		/**
		* @brief idString 节点的唯一标识
		*/
		std::string idString;

		/**
		* @brief bIsLocal 是否是本地数据
		*/
		bool bIsLocal;

	protected:
		virtual ~COsgUserData();

	};
}

