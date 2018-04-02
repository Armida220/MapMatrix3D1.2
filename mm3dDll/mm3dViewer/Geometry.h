#pragma once
#include <cmath>
#include <vector>

namespace CL
{	
	#define EPSILON 0.000001

	//��άdoubleʸ��
	typedef struct  Vector2d
	{
		double x,y;
		
		Vector2d() 
		{ 
			x = 0.0;
			y = 0.0;
		}
		Vector2d(double dx, double dy) 
		{ 
			x = dx;
			y = dy; 
		}
		void Set(double dx, double dy) 
		{ 
			x = dx;
			y = dy; 
		}

		//ʸ����ģ
		double Mod() const
		{
			return sqrt(x * x + y * y);
		}

		// ʸ�����
		double Dot(const Vector2d& v) const
		{
			return x * v.x + y * v.y;
		}
		//�������
		double Cross(const Vector2d& rhs) const
		{
			return (x * rhs.y - y * rhs.x);
		}

		//��������	
		Vector2d operator + (const Vector2d& v) const
		{
			return Vector2d(x + v.x, y + v.y) ;
		}	
		Vector2d operator - (const Vector2d& v) const
		{
			return Vector2d(x - v.x, y - v.y) ;
		}
		bool operator == (const Vector2d& rhs)
		{			
			return ((fabs(x - rhs.x) < EPSILON) && (fabs(y - rhs.y) < EPSILON));			
		}
		bool operator != (const Vector2d& rhs)
		{			
			return ((fabs(x - rhs.x) > EPSILON) || (fabs(y - rhs.y) > EPSILON));			
		}
		bool operator < (const Vector2d& rhs) 
		{
			if (fabs(x - rhs.x) < EPSILON)
			{
				return y < rhs.y;
			}			
			else
			{
				return x < rhs.x;
			}
		}	
	}Vec2d;

	//��άdoubleʸ��
	typedef struct Vector3d
	{
		double x,y,z; 

		Vector3d() 
		{ 
			x=0.0;
			y=0.0;
			z=0.0;
		}
		Vector3d(double dx,double dy,double dz) 
		{ 
			x=dx;
			y=dy;
			z=dz; 
		}
		void Set(double dx, double dy,double dz) 
		{ 
			x = dx;
			y = dy; 
			z = dz;
		}
		
		//ʸ������
		Vector3d Scalar(double c) const
		{
			return Vector3d(c*x,c*y,c*z) ;
		}		

		//ʸ����ģ
		double Mod() const
		{
			return sqrt(x * x + y * y + z * z);
		}

		// ʸ�����
		double Dot(const Vector3d& v) const
		{
			return x * v.x + y * v.y + z * v.z ;
		}

		// ʸ�����
		Vector3d Cross(const Vector3d& v) const
		{		
			return Vector3d(y * v.z - z * v.y,  z * v.x - x * v.z,  x * v.y - y * v.x) ;
		} 		

		//��������	
		Vector3d operator + (const Vector3d& v) const
		{
			return Vector3d(x + v.x, y + v.y, z + v.z) ;
		}	
		Vector3d operator - (const Vector3d& v) const
		{
			return Vector3d(x - v.x, y - v.y, z - v.z) ;
		}
 		bool operator == (const Vector3d& rhs)
 		{			
 			return ((fabs(x - rhs.x)<EPSILON) && (fabs(y - rhs.y)<EPSILON) && (fabs(z - rhs.z)<EPSILON));			
 		}
 		bool operator != (const Vector3d& rhs)
 		{			
 			return ((fabs(x - rhs.x)>EPSILON) || (fabs(y - rhs.y)>EPSILON) || (fabs(z - rhs.z)>EPSILON));			
 		}
 		bool operator < (const Vector3d& rhs) 
 		{
			if (fabs(x - rhs.x)<EPSILON)
			{
				if (fabs(y - rhs.y)<EPSILON)
				{
					return z < rhs.z;
				}
				else
				{
					return y < rhs.y;
				}				
			}			
			else
			{
				return x < rhs.x;
			} 			
 		}	
	}Vec3d;

	//�жϵ��ڶ������
	bool Point_In_Polygon_2D(double x, double y, const std::vector<Vec2d> &POL);

	//�ж����ڶ������
	bool Line_In_Polygon_2D(const std::vector<CL::Vec2d> &lines, const std::vector<CL::Vec2d> &POL);
	
};



