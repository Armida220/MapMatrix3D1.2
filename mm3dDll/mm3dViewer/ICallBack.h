#pragma once
#include <memory>
#include <functional>
#include <vector>
/**
* @brief ICallBack osg�ص��ӿ�
*/

#ifdef CALLLIBDLL  
#define CALLAPI _declspec(dllexport)  
#else  
#define CALLAPI  _declspec(dllimport)  
#endif  



namespace osgCall
{
	/**
	* @brief Coord ��ά��������
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
		* @brief �ǳ�Ա����, ����+������accumulate�ȼӷ��㷨�Ľṹ�����, ���Զ�lhs����ת��
		* @param lhs +����ߵı���
		* @param rhs +���ұߵı���
		*/
		friend const Coord operator+(const Coord& lhs, const Coord& rhs)
		{
			return Coord(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
		}

		/**
		* @brief ��дoperator/���ڶ������Ľṹ����г���
		* @param lhs /����ߵı���
		* @param rhs /���ұߵı���
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
	* @brief osgFtr �ṹ����������д�vecview�е�����������osg��������������Ϣ
	*/
	struct osgFtr
	{
		/**
		* @brief isSel �������Ƿ�ѡ����
		*/
		bool isSel;
		
		/**
		* @brief isVisible �������Ƿ�ɼ�
		*/
		bool isVisible = true;

		/**
		* @brief vecCoord ���ж��������
		*/
		std::vector<Coord> vecCoord;

		/**
		* @brief type ����������
		*/
		int type;

		/**
		* @brief str ��������֣��������ı���Ϣ
		*/
		std::string str = "";

		/**
		* @brief textSize ���ִ�С
		*/
		double textSize = 0;
		/**
		* @brief nAlign ���ֶ���λ��
		*/
		int nAlign = 0x21;

		float	fTextAngle;				//�ı�ˮƽ������x��ļнǣ���ʱ�룩{ע���Ƕȵ�λΪ����}
		//float	fCharAngle;				//�����ַ�����ת�Ƕȣ���ʱ�룩,�����϶���{ע���Ƕȵ�λΪ����}


		/**
		* @brief isLocal �Ƿ��Ǳ���
		*/
		bool isLocal = false;

		/**
		* @brief tangent ���߷���
		*/
		double tangent;

		/**
		* @brief rgb ��ɫ
		*/
		Coord rgb = { 255, 0, 255 };

		double alpha = 1;
		/**
		* @brief mId Ψһ�ı�ʶ
		*/
		std::string mId = "";

		/**
		* @brief �ǳ�Ա����, ����==�����������ж��Ƿ���Ψһ��ʶ
		* @param lhs ==����ߵı���
		* @param rhs ==���ұߵı���
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
	* @brief CallBack �ص���
	*/
	class CALLAPI ICallBack
	{
	public:
		ICallBack();

		virtual ~ICallBack();

		/**
		* @brief callLfClk �ص�������
		* @param x x��������
		* @param y y��������
		* @param z z��������
		*/
		virtual void callLfClk(double x, double y, double z) = 0;

		/**
		* @brief callMouseMv �ص�����ƶ�
		* @param x x��������
		* @param y y��������
		* @param z z��������
		*/
		virtual void callMouseMv(double x, double y, double z) = 0;

		/**
		* @brief callRightClk �ص�����һ�
		* @param x x��������
		* @param y y��������
		* @param z z��������
		*/
		virtual void callRightClk(double x, double y, double z) = 0;

		/**
		* @brief promptLock ��ʾ����
		* @param bLock �Ƿ��Ѿ�������
		*/
		virtual void promptLock(bool bLock) = 0;

		/**
		* @brief promptCatch ��ʾ�Ƿ�׽�ڽǵ�
		* @param bCatch �Ƿ�׽�ڽǵ�
		*/
		virtual void promptCatch(bool bCatch) = 0;

		/**
		* @brief promptOpera ��ʾ�Ƿ���shift��ʽ���в���
		* @param bShiftOpera �Ƿ���shift��ʽ���в���
		*/
		virtual void promptOpera(bool bShiftOpera) = 0;

		/**
		* @brief callBkspace �ص�backspace����
		*/
		virtual void callBkspace() = 0;

		/**
		* @brief callPan �ص��϶�ģ��
		* @param x x��������
		* @param y y��������
		* @param z z��������
		*/
		virtual void callPan(double x, double y, double z) = 0;

		/**
		* @brief callZoom �ص�����ģ��
		* @param x x��������
		* @param y y��������
		* @param scale �߶�
		*/
		virtual void callZoom(double x, double y, double scale) = 0;

		/**
		* @brief callShowImg �ص����ݵ���ʾӰ��
		* @param x x��������
		* @param y y��������
		* @param z z��������
		*/
		virtual void callShowImg(double x, double y, double z) = 0;

		/**
		* @brief callChangeAbsOriDlg �ı���Զ���̵����
		* @param relaX x��������
		* @param relaY y��������
		* @param relaZ z��������
		*/
		virtual void callChangeAbsOriDlg(double relaX, double relaY, double relaZ) = 0;

		/**
		* @brief callFetchOriData ��ȡ��������
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
