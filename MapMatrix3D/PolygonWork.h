#pragma once
#ifndef POLYGON_H
#define POLYGON_H

#include "SmartViewFunctions.h"

#include <vector>
using std::vector;
// CPolygonWork ����Ŀ��

namespace PolygonIntersect
{

//ͼ���е�����ݽṹ��
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

//��ͼ���б�ʾ��������ݽṹ;
class tits
{
public:
	int its;    //��Ž����б� vector<tpoint> data_pt �����
	int flag;   //�������ͣ�0, ��ͨ���㣬1�����Ƕ���Ҳ�ǽ���
	bool visit;
};

//ͼ���ж�������ݽṹ��
class tvertex :public tpoint
{
public:
	//�õ����Ľ����б�
	vector<tits> itslist;
};


//�������ڱ�ʾͼ��A��ͼ��B��ϵ�����ݽṹ��
class tfpoint :public tpoint
{
public:
	int flag;       //������֮ǰ����ʾ�������ͣ�2���㡢0���㡢3���Ƕ���Ҳ�ǽ��㣩�������ı�ǣ�1����㣬-1�����㣬4���������������˵�������
	int con_index;  //��һ��������иõ��������������ǽ��㣬ӦΪ-1
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
	//�������ݣ�
	vector<tpoint> backup_data[2];
	//ʵ�ʴ���ʱ�õ�����(�ݲ�������ݣ���
	vector<tvertex> active_data[2];
	//���ռ���ʱ�õ����ݣ��ϲ��˶���ͽ��������ݣ���
	vector<tfpoint> final_data[2];
	//�������������Ƿ���ȫ�غϣ�
	bool is_overlap;
	//initialize()�Ƿ�ɹ�
	bool is_initialize_ok;
private:
	//�ַ����ָ��;
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
	//�ж�ͼ�������Ƿ��ǰ�����洢��������ǣ����������´洢��
	void right_direction(int data_index);
	//ȥ��ͼ���������ظ��ĵ㣻
	void delete_rpoint(int backup_index,int data_index,double control_para);
	//��һ��ͼ��Ϊ���ȣ�������ͼ�εĶ���������е�����
	void graph_adjust(int data_s_index,int data_w_index,double control_para);
	//��ͼ��A��ͼ��B�Ľ��㣻
	int find_cross_point();	
	//��ͼ���еĽ��㰴�������򣨸��ݽ��㵽�����������жϣ���
	void intersection_sort(int data_index);
	//�ϲ�ͼ���еĽ����ԭʼ�㣻
	void union_points(int dataf_index,int data_index);
	//����ͼ��A �� ͼ��B �����ӹ�ϵ��
	void create_connection();
	//�ж���������Ƿ���ȫ�غϣ�
	void overlap_polygons();
	//�����ͼ������������(�������໥Ϊ���)��
	void check_p_in();
	//���� data_fs, �ж� data_fw �еĸ�����ĳ��������
	void check_in_out(int fs_index,int fw_index,int data_index);
public:
	//��ӵ㣻
	//���� gindex Ϊ����ε������ţ�Ŀǰֻ��Ϊ 0 �� 1 ����ʾ�ڸ�����λ��ǵڶ�������Σ�
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
	//������������ַ������Ի�õ���ԭʼ�����е�λ�ã�
	//�ַ����ĸ�ʽΪ "An-Am+Bn-Bm" ���� A��B ���ǵڼ�������Σ�n��m Ϊ�ཻ�߶ε��������������ֵ��
	//����ֵΪһ��4ά������ǰ����ֵΪ��һ��������е�������
	//������ֵΪ�ڶ���������е�������
	//Ĭ��ֵΪ -1 ��ʾ����Ӧԭʼ������еĵ㣻
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
	//�����ص���Ʋ�����
	void set_overlap(double op);
	//
	//��ʼ�����㣻
	void initialize();
	//ѡ��߳��ڲ�����Դ��
	void set_interpolation_source(int index) {interpolation_source_index=index;}
	int get_interpolation_source() {return interpolation_source_index;}
	//������A�Ͷ����B�Ĳ�����
	void find_union();
	//������A�Ͷ����B�Ľ�����
	void find_intersection();
	//��������εĲ�����fb_indexӦ����data_index��ͬ
	void find_XOR(int fa_index,int fb_index,int data_index);
public:
	//������������εĽ�������ݣ�
	vector<tpoint> data_pt;
	//���沢�����ݣ�
	vector< vector<tpoint> > union_list;
	//���潻�����ݣ�
	vector< vector<tpoint> > intersection_list;
	//���油�����ݣ�
	vector< vector<tpoint> > xor_list;
public:
	//�жϵ��Ƿ���ͼ���ڲ���
	int point_position(tpoint point,int data_index);
	//
	inline bool isPointInLine(tpoint& point, tpoint& pa, tpoint& pb)
	{
		return GraphAPI::GIsPtInLine(pa,pb,point);
	}
	//����ͶӰ��ֱ����(ֱ��������ȷ������
	inline void point_projection(tpoint &point,tpoint &pa,tpoint &pb)
	{
		//ˮƽֱ��ʱ��
		if(fabs(pa.y-pb.y)<=PRECISION)
		{
			point.y=(pa.y+pb.y)/2;
			return;
		}
		//��ֱֱ��ʱ��
		if(fabs(pa.x-pb.x)<=PRECISION)
		{
			point.x=(pa.x+pb.x)/2;
			return;
		}
		//һ��ֱ��ʱ��
		double aA=pa.y-pb.y;
		double aB=pb.x-pa.x;
		double aC=-(aB*pa.y+aA*pa.x);

		double bA=pb.x-pa.x;
		double bB=pb.y-pa.y;
		double bC=-(point.y*bB+point.x*bA);

		point.x=(aB*bC-bB*aC)/(aA*bB-bA*aB+PRECISION);
		point.y=(aC*bA-bC*aA)/(aA*bB-bA*aB+PRECISION);
	}
	//����ͶӰ��ֱ����(ֱ���ɱ�׼����ȷ������
	inline void point_projection(tpoint &point,double aA,double aB,double aC)
	{
		//ˮƽֱ��ʱ��
		if(fabs(aA)<=PRECISION)
		{
			point.y=-aC/aB;
			return;
		}
		//��ֱֱ��ʱ��
		if(fabs(aB)<=PRECISION)
		{
			point.x=-aC/aA;
			return;
		}
		//һ��ֱ��ʱ��
		double bA=aB;
		double bB=-aA;
		double bC=-(point.y*bB+point.x*bA);

		point.x=(aB*bC-bB*aC)/(aA*bB-bA*aB+PRECISION);
		point.y=(aC*bA-bC*aA)/(aA*bB-bA*aB+PRECISION);
	}
	//�ж������߶��Ƿ����ཻ�������ཻ�������佻�㣻
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
