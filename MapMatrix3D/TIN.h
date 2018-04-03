
#ifndef TIN_H_
#define TIN_H_

#include "Triangulation.h"

namespace triangulation
{
	
	
	struct point_3d
	{
		double x, y, z;
		//
		int constraint_id;
		int flag;
		unsigned long user_param1;
		unsigned long user_param2;
	};
	
	struct edge_3d
	{
		point_3d sp;
		point_3d ep;
	};
	
	struct triangle_3d
	{
		point_3d pa, pb, pc;
	};
	
	class DELAUNAY_API  ITIN2D
	{
	public:
		static ITIN2D* NEWTIN2DHandler();
	public:
		virtual ~ITIN2D()
		{}
		
		virtual bool init() = 0;
		//need delete the repeated points;
		virtual bool insertPoints(point_3d* ppoints, unsigned int point_count) = 0;
		virtual bool insertPoint(point_3d point) = 0;
		virtual bool deletePoint(point_3d point) = 0;
		virtual bool insertSegment(const point_3d& pa, const point_3d& pb) = 0;
		virtual bool deleteSegment(const point_3d& pa, const point_3d& pb) = 0;
		
		virtual void refreshTriangleInfo() = 0;
		virtual unsigned int getTriangleCount() = 0;
		//�˷����ɻ�ȡ������꼰�������ԣ�
		virtual triangle_3d getTriangle(unsigned int index) = 0;
		
		virtual unsigned int getEdgeCount() = 0;
		//�˷���ֻ�ܻ�ȡ�ߵ����꣬���ܵõ���ĸ������ԣ�
		virtual edge_3d getEdge(int index) = 0;
		//�˷����ɻ�ȡ������꼰�������ԣ�
		//�˷����õ��������ε� pa pb �ֱ��� �ߵ����˵㣬 pc ����һ��;
		virtual void getTriangleContainEdge(triangle_3d(&res)[2], int edge_index) = 0;
		virtual bool isConstraint(int edge_index) = 0;
		
		virtual void clear() = 0;
		//
		void setTolerance(double tolerance = 0.00001)
		{
			TOLERANCE = tolerance;
		}
	protected:
		double TOLERANCE;
		
	};
	
	
}



#endif