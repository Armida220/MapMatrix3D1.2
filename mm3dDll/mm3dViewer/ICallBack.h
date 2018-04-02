#pragma once
#include <memory>
#include <functional>
#include <vector>
/**
* @brief ICallBack osg回调接口
*/

#ifdef CALLLIBDLL  
#define CALLAPI _declspec(dllexport)  
#else  
#define CALLAPI  _declspec(dllimport)  
#endif  



namespace osgCall
{
	/**
	* @brief Coord 三维几何坐标
	*/
	struct Coord
	{
	public:
		double x;
		double y;
		double z;

		Coord(double tx, double ty, double tz)
			:x(tx), y(ty), z(tz)
		{
		}

		/**
		* @brief 非成员函数, 重载+号用于accumulate等加法算法的结构体求和, 可以对lhs进行转型
		* @param lhs +号左边的变量
		* @param rhs +号右边的变量
		*/
		friend const Coord operator+(const Coord& lhs, const Coord& rhs)
		{
			return Coord(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
		}

		/**
		* @brief 重写operator/用于对上述的结构体进行除法
		* @param lhs /号左边的变量
		* @param rhs /号右边的变量
		*/
		Coord& operator /(double rhs)
		{
			return Coord(x / rhs, y / rhs, z / rhs);
		}

		Coord& operator -=(const Coord& rhs)
		{
			return Coord(x - rhs.x, y - rhs.y, z - rhs.z);
		}

		Coord& operator -(const Coord& rhs)
		{
			return Coord(x - rhs.x, y - rhs.y, z - rhs.z);
		}


		bool operator == (const Coord& rhs) const
		{
			return 
				(x == rhs.x) &&
				(y == rhs.y) &&
				(z == rhs.z);
		}
	};

	/**
	* @brief osgFtr 结构体包含了所有从vecview中的特征解析成osg的特征的所有信息
	*/
	struct osgFtr
	{
		/**
		* @brief isSel 该特征是否被选中了
		*/
		bool isSel;
		
		/**
		* @brief isVisible 该特征是否可见
		*/
		bool isVisible = true;

		/**
		* @brief vecCoord 所有顶点的坐标
		*/
		std::vector<Coord> vecCoord;

		/**
		* @brief type 特征的类型
		*/
		int type;

		/**
		* @brief str 如果是文字，包含的文本信息
		*/
		std::string str = "";

		/**
		* @brief textSize 文字大小
		*/
		double textSize = 0;
		/**
		* @brief nAlign 文字对齐位置
		*/
		int nAlign = 0x21;

		float	fTextAngle;				//文本水平基线与x轴的夹角（逆时针）{注：角度单位为度数}
		//float	fCharAngle;				//单个字符的旋转角度（逆时针）,绕左上顶点{注：角度单位为度数}


		/**
		* @brief isLocal 是否是本地
		*/
		bool isLocal = false;

		/**
		* @brief tangent 切线方向
		*/
		double tangent;

		/**
		* @brief rgb 颜色
		*/
		Coord rgb = { 255, 0, 255 };

		double alpha = 1;
		/**
		* @brief mId 唯一的标识
		*/
		std::string mId = "";

		/**
		* @brief 非成员函数, 重载==号用于用于判断是否是唯一标识
		* @param lhs ==号左边的变量
		* @param rhs ==号右边的变量
		*/
		friend bool operator == (const osgFtr& lhs, const osgFtr& rhs)
		{
			return lhs.mId == rhs.mId;
		}

	};

	typedef std::tr1::function<void(double, double, double)> Fun;
	typedef std::tr1::function<void(double, double, double, bool)> Fun1;
	typedef std::tr1::function<void()> Fun2;
	typedef std::tr1::function<bool()> Fun3;
	typedef std::tr1::function<void(bool)> Fun4;
	typedef std::tr1::function<void(int)> Fun5;
	typedef std::tr1::function<void(double, double, double, int)> Fun6;

	/**
	* @brief CallBack 回调类
	*/
	class CALLAPI ICallBack
	{
	public:
		ICallBack();

		virtual ~ICallBack();

		/**
		* @brief callLfClk 回调鼠标左击
		* @param x x方向坐标
		* @param y y方向坐标
		* @param z z方向坐标
		*/
		virtual void callLfClk(double x, double y, double z) = 0;

		/**
		* @brief callMouseMv 回调鼠标移动
		* @param x x方向坐标
		* @param y y方向坐标
		* @param z z方向坐标
		*/
		virtual void callMouseMv(double x, double y, double z) = 0;

		/**
		* @brief callRightClk 回调鼠标右击
		* @param x x方向坐标
		* @param y y方向坐标
		* @param z z方向坐标
		*/
		virtual void callRightClk(double x, double y, double z) = 0;

		/**
		* @brief promptLock 提示锁定
		* @param bLock 是否已经锁定了
		*/
		virtual void promptLock(bool bLock) = 0;

		/**
		* @brief promptCatch 提示是否捕捉内角点
		* @param bCatch 是否捕捉内角点
		*/
		virtual void promptCatch(bool bCatch) = 0;

		/**
		* @brief promptOpera 提示是否以shift方式进行操作
		* @param bShiftOpera 是否以shift方式进行操作
		*/
		virtual void promptOpera(bool bShiftOpera) = 0;

		/**
		* @brief callBkspace 回调backspace按键
		*/
		virtual void callBkspace() = 0;

		/**
		* @brief callPan 回调拖动模型
		* @param x x方向坐标
		* @param y y方向坐标
		* @param z z方向坐标
		*/
		virtual void callPan(double x, double y, double z) = 0;

		/**
		* @brief callZoom 回调缩放模型
		* @param x x方向坐标
		* @param y y方向坐标
		* @param scale 尺度
		*/
		virtual void callZoom(double x, double y, double scale) = 0;

		/**
		* @brief callShowImg 回调根据点显示影像
		* @param x x方向坐标
		* @param y y方向坐标
		* @param z z方向坐标
		*/
		virtual void callShowImg(double x, double y, double z) = 0;

		/**
		* @brief callChangeAbsOriDlg 改变绝对定向刺点情况
		* @param relaX x方向坐标
		* @param relaY y方向坐标
		* @param relaZ z方向坐标
		*/
		virtual void callChangeAbsOriDlg(double relaX, double relaY, double relaZ) = 0;

		/**
		* @brief callFetchOriData 获取定向数据
		*/
		virtual void callFetchOriData() = 0;
		
		virtual void callDBC(double x, double y, double z, int button) = 0;

	}; 

	class CALLAPI ICallFactory
	{
	public:
		static std::shared_ptr<ICallBack> create(const Fun &funLfClk, const Fun &funMouseMv, 
			const Fun &funRightClk, const Fun4 &funPromptLock, const Fun4 &funPromptCatch,
			const Fun2& funBack, const Fun4 &funPromptOpera, const Fun &funPan, const Fun &funZoom, 
			const Fun &funShowImg, const Fun6 & funDBL, const Fun &funChangeAbsOriDlg, const Fun2 &funFetchOriData);
	};

}
