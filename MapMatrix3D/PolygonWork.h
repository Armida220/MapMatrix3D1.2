#pragma once
#ifndef POLYGON_H
#define POLYGON_H

#include "SmartViewFunctions.h"

#include <vector>
using std::vector;
// CPolygonWork 命令目标

namespace PolygonIntersect
{

//图形中点的数据结构；
struct tpoint3d
{
	double x;      
	double y;
	double z;
};

class tpoint 
{
public:
	CString id;    
	double x;      
	double y;
	double z;
	//
	bool have_re_point;
};

//在图形中表示交点的数据结构;
class tits
{
public:
	int its;    //存放交点列表 vector<tpoint> data_pt 的序号
	int flag;   //交点类型：0, 普通交点，1，即是顶点也是交点
	bool visit;
};

//图形中顶点的数据结构；
class tvertex :public tpoint
{
public:
	//该点后面的交点列表；
	vector<tits> itslist;
};


//最终用于表示图形A和图形B关系的数据结构；
class tfpoint :public tpoint
{
public:
	int flag;       //求出入点之前它表示交点类型（2顶点、0交点、3即是顶点也是交点），出入点的标记，1，入点，-1，出点，4，对两个多边形来说都是入点
	int con_index;  //另一个多边形中该点的索引，如果不是交点，应为-1
	bool visit;
};



const double PRECISION=0.000000000001;

class CPolygonWork : public CObject
{
public:
	enum ErrorType{
		NONE=0L,
		EMPTY_DATA,
		OVERLAP_EXACT,
		NO_INTERSECTION
	};

	CPolygonWork();
	virtual ~CPolygonWork();
	ErrorType GetError(){ return error_type;
	}
private:
	double overlap_para;
	int interpolation_source_index;
	ErrorType error_type;
private:
	//备份数据；
	vector<tpoint> backup_data[2];
	//实际处理时用的数据(容差处理后的数据）；
	vector<tvertex> active_data[2];
	//最终计算时用的数据（合并了顶点和交点后的数据）；
	vector<tfpoint> final_data[2];
	//标记两个多边形是否完全重合；
	bool is_overlap;
	//initialize()是否成功
	bool is_initialize_ok;
private:
	//字符串分割函数;
	int str_segmentation(CString input,char dec,CStringArray &output)
	{
		input.TrimLeft();
		input.TrimRight();
		CString temp=input+dec;
		while(temp.GetLength()>0)
		{
			temp.TrimLeft();
			int pos=temp.Find(dec);
			if (pos==0)
			{
				temp.Right(temp.GetLength()-1);
				continue;
			}
			CString item=temp.Left(pos);
			item.TrimLeft();
			output.Add(item);
			temp=temp.Right(temp.GetLength()-pos-1);
		}
		return output.GetSize();
	}
protected:
	//判断图形数据是否是按正向存储，如果不是，则按正向重新存储；
	void right_direction(int data_index);
	//去除图形数据中重复的点；
	void delete_rpoint(int backup_index,int data_index,double control_para);
	//以一个图形为优先，对两个图形的顶点坐标进行调整；
	void graph_adjust(int data_s_index,int data_w_index,double control_para);
	//找图形A和图形B的交点；
	int find_cross_point();	
	//对图形中的交点按正向排序（根据交点到顶点距离最短判断）；
	void intersection_sort(int data_index);
	//合并图形中的交点和原始点；
	void union_points(int dataf_index,int data_index);
	//创建图形A 和 图形B 的连接关系；
	void create_connection();
	//判断两多边形是否完全重合；
	void overlap_polygons();
	//检查两图形中特殊的入点(即两点相互为入点)；
	void check_p_in();
	//根据 data_fs, 判断 data_fw 中的各交点的出入情况；
	void check_in_out(int fs_index,int fw_index,int data_index);
public:
	//添加点；
	//参数 gindex 为多边形的索引号，目前只能为 0 或 1 ，表示第个多边形或是第二个多边形；
	inline void add_point(int gindex,double x,double y,double z)
	{
		tpoint temp;
		temp.x=x;
		temp.y=y;
		temp.z=z;
		temp.id.Format("%d",backup_data[gindex].size());
		temp.id=char(65+gindex)+temp.id;
		backup_data[gindex].push_back(temp);
	}
    //
	inline void add_data(int gindex,tpoint3d *datas,int num)
	{
		for(int i=0;i<num;++i)
		{
			tpoint temp;
			temp.x=(datas+i)->x;
			temp.y=(datas+i)->y;
			temp.z=(datas+i)->z;
			temp.id.Format("%d",backup_data[gindex].size());
			temp.id=char(65+gindex)+temp.id;
			backup_data[gindex].push_back(temp);
		}
	}
	//
	inline tpoint *get_backup_point(int gindex,int n)
	{
		return &backup_data[gindex][n];
	}
	//
	inline tvertex *get_active_point(int gindex,int n)
	{
		return &active_data[gindex][n];
	}
	//
	inline tfpoint *get_final_point(int gindex,int n)
	{
		return &final_data[gindex][n];
	}
	//
	inline int get_bpoint_count(int gindex)
	{
		return backup_data[gindex].size();
	}
	//
	inline int get_apoint_count(int gindex)
	{
		return active_data[gindex].size();
	}
	//
	inline int get_fpoint_count(int gindex)
	{
		return final_data[gindex].size();
	}
	//
	void clear()
	{
		backup_data[0].clear();
		backup_data[1].clear();
		active_data[0].clear();
		active_data[1].clear();
		final_data[0].clear();
		final_data[1].clear();
		data_pt.clear();
		union_list.clear();
		intersection_list.clear();
		xor_list.clear();
	}
	//解析点的索引字符串，以获得点在原始数据中的位置；
	//字符串的格式为 "An-Am+Bn-Bm" 其中 A、B 标是第几个多边形，n、m 为相交线段的两个顶点的索引值；
	//返回值为一个4维向量；前两个值为第一个多边形中的索引；
	//后两个值为第二个多边形中的索引；
	//默认值为 -1 表示不对应原始多边形中的点；
	void get_result_point_index(const tpoint& pt, int index[4])
	{
		 CString str_index = pt.id;
		 index[0] = index[1] = index[2] = index[3] = -1;
		 CStringArray temp;
		 str_segmentation(str_index,'+',temp);
		 for(int k=0;k<temp.GetSize();++k)
		 {
			 CStringArray temp1;
			 str_segmentation(temp[k],'-',temp1);
			 for(int i=0;i<temp1.GetSize();++i)
			 {
				 char *buf=(LPTSTR)(LPCTSTR)temp1[i];
				 if(*(buf+0)=='A')
				 {
					 *(buf+0)='0';
					 index[i]=StrToInt(buf);
				 }
				 else if(*(buf+0)=='B')
				 {
					 *(buf+0)='0';
					 index[2+i]=StrToInt(buf);
				 }
			 }
		 }
	}
public:
	//设置重点控制参数；
	void set_overlap(double op);
	//
	//初始化运算；
	void initialize();
	//选择高程内插数据源；
	void set_interpolation_source(int index) {interpolation_source_index=index;}
	int get_interpolation_source() {return interpolation_source_index;}
	//求多边形A和多边形B的并集；
	void find_union();
	//求多边形A和多边形B的交集；
	void find_intersection();
	//求两多边形的补集；fb_index应该与data_index相同
	void find_XOR(int fa_index,int fb_index,int data_index);
public:
	//保存两个多边形的交点的数据；
	vector<tpoint> data_pt;
	//保存并集数据；
	vector< vector<tpoint> > union_list;
	//保存交集数据；
	vector< vector<tpoint> > intersection_list;
	//保存补集数据；
	vector< vector<tpoint> > xor_list;
public:
	//判断点是否在图形内部；
	int point_position(tpoint point,int data_index);
	//
	inline bool isPointInLine(tpoint& point, tpoint& pa, tpoint& pb)
	{
		return GraphAPI::GIsPtInLine(pa,pb,point);
	}
	//将点投影到直线上(直线由两点确定）；
	inline void point_projection(tpoint &point,tpoint &pa,tpoint &pb)
	{
		//水平直线时；
		if(fabs(pa.y-pb.y)<=PRECISION)
		{
			point.y=(pa.y+pb.y)/2;
			return;
		}
		//垂直直线时；
		if(fabs(pa.x-pb.x)<=PRECISION)
		{
			point.x=(pa.x+pb.x)/2;
			return;
		}
		//一般直线时；
		double aA=pa.y-pb.y;
		double aB=pb.x-pa.x;
		double aC=-(aB*pa.y+aA*pa.x);

		double bA=pb.x-pa.x;
		double bB=pb.y-pa.y;
		double bC=-(point.y*bB+point.x*bA);

		point.x=(aB*bC-bB*aC)/(aA*bB-bA*aB+PRECISION);
		point.y=(aC*bA-bC*aA)/(aA*bB-bA*aB+PRECISION);
	}
	//将点投影到直线上(直线由标准参数确定）；
	inline void point_projection(tpoint &point,double aA,double aB,double aC)
	{
		//水平直线时；
		if(fabs(aA)<=PRECISION)
		{
			point.y=-aC/aB;
			return;
		}
		//垂直直线时；
		if(fabs(aB)<=PRECISION)
		{
			point.x=-aC/aA;
			return;
		}
		//一般直线时；
		double bA=aB;
		double bB=-aA;
		double bC=-(point.y*bB+point.x*bA);

		point.x=(aB*bC-bB*aC)/(aA*bB-bA*aB+PRECISION);
		point.y=(aC*bA-bC*aA)/(aA*bB-bA*aB+PRECISION);
	}
	//判断两条线段是否能相交，若能相交，则求其交点；
	inline bool can_cross(tpoint &p1,tpoint &p2,tpoint &p3,tpoint &p4,double &x,double &y,double &z)
	{
		double t0,t1;
		if( !GraphAPI::GGetLineIntersectLineSeg(p1.x,p1.y,p2.x,p2.y,p3.x,p3.y,p4.x,p4.y,&x,&y,&t0,&t1) )
		{
			x=-1;
			y=-1;
			z=0;
			return false;			
		}
		if(interpolation_source_index==0)
		{
             z=p1.z+t0*(p2.z-p1.z);
		}
		else
		{
			z=p3.z+t1*(p4.z-p3.z);
		}
		return true;
	}
};

}

#endif
