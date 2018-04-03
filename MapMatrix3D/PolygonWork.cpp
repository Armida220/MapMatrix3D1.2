// PolygonWork.cpp : 实现文件
//

#include "stdafx.h"
#include "PolygonWork.h"
#include "float.h"

// CPolygonWork

namespace PolygonIntersect
{

CPolygonWork::CPolygonWork()
{
	is_overlap = false;
	error_type=NONE;
}

CPolygonWork::~CPolygonWork()
{
	clear();
}


// CPolygonWork 成员函数
int CPolygonWork::point_position(tpoint point,int data_index)
{
	vector<tvertex> &datas=active_data[data_index];

	//
	int t=0;
	unsigned int pcount=datas.size();
	unsigned int pre=0,next=0,old=0,c=0;

	if(1)
	{
		CArray<tpoint,tpoint> arrPts;
		arrPts.SetSize(pcount);
		for(int i=0; i<pcount; i++)
		{
			arrPts[i] = datas[i];
		}

		int ret = GraphAPI::GIsPtInRegion(point,arrPts.GetData(),arrPts.GetSize());
		if(ret==0)
			return 1;
		if(ret==1)
			return 2;
		if(ret==2)
			return 4;
		return 3;
	}
}

//用最左边点的前后点对该点的斜率比较大小来确定顺逆时针方向
void CPolygonWork::right_direction(int data_index)
{
	vector<tvertex> &datas=active_data[data_index];
	//
	int data_size=datas.size();

	double minx=DBL_MAX,miny=DBL_MAX,maxx=-DBL_MAX,maxy=-DBL_MAX;
	int index_minx,index_miny,index_maxx,index_maxy;

	//获取此图形数据的范围；
	for(int i=0;i<data_size;++i)
	{
		if(datas[i].x<minx)
		{
			minx=datas[i].x;
			index_minx=i;
		}
		if(datas[i].x>maxx)
		{
			maxx=datas[i].x;
			index_maxx=i;
		}
		if(datas[i].y<miny)
		{
			miny=datas[i].y;
			index_miny=i;
		}
		if(datas[i].y>maxy)
		{
			maxy=datas[i].y;
			index_maxy=i;
		}
	}
	//通过x坐标最小的点来判断此图形的方向；
	int pre=(index_minx-1+data_size)%data_size;
	int next=(index_minx+1+data_size)%data_size;
	//当x坐标最小点为图形范围的左下角点时；
	if(index_minx==index_miny)
	{
		if(((datas[next].y-miny)/(datas[next].x-minx+PRECISION))<((datas[pre].y-miny)/(datas[pre].x-minx+PRECISION)))
		{
			return;
		}
	}
	//当x坐标最小点为图形范围的左上角点时；
	else if(index_minx==index_maxy)
	{
		if(((maxy-datas[next].y)/(datas[next].x-minx+PRECISION))>((maxy-datas[pre].y)/(datas[pre].x-minx+PRECISION)))
		{
			return;
		}
	}
	//当x坐标最小点在图形范围的左边界上时；
	else 
	{
		if((datas[next].y>datas[index_minx].y)&&(datas[pre].y>datas[index_minx].y))
		{
			if(((datas[next].x-minx)/(datas[next].y-datas[index_minx].y+PRECISION))>((datas[pre].x-minx)/(datas[pre].y-datas[index_minx].y+PRECISION)))
			{
				return;
			}
		}
		else if((datas[next].y<datas[index_minx].y)&&(datas[pre].y<datas[index_minx].y))
		{
			if(((datas[next].x-minx)/(-datas[next].y+datas[index_minx].y+PRECISION))<((datas[pre].x-minx)/(-datas[pre].y+datas[index_minx].y+PRECISION)))
			{
				return;
			}
		}
		else
		{
			if(datas[next].y<datas[pre].y)
			{
				return;
			}
		}
	}

	//如果图形数据方向不为正，则将其转换为正向；
	vector<tvertex> temp;
	for(i=data_size-1;i>=0;--i)
	{
		temp.push_back(datas[i]);
	}
	datas.clear();
	datas=temp;
	temp.clear();
}

void CPolygonWork::delete_rpoint(int backup_index,int data_index,double control_para)
{
	vector<tpoint> &data_backup=backup_data[backup_index];
	vector<tvertex> &datas=active_data[data_index];
	if(data_backup.size()<3)
	{
		error_type=EMPTY_DATA;
		return;
	}
	//
	unsigned int i=0,j=1;
	datas.clear();
	//去除图形的重复点；
	tvertex temp;
	temp.id=data_backup[0].id;
	temp.x=data_backup[0].x;
	temp.y=data_backup[0].y;
	temp.z=data_backup[0].z;
	temp.have_re_point=false;
	data_backup[i].have_re_point=false;
	datas.push_back(temp);
	while(j<data_backup.size())
	{
		data_backup[j].have_re_point=false;
		if(fabs(data_backup[j].x-data_backup[i].x)+fabs(data_backup[j].y-data_backup[i].y)<=control_para)
		{
			datas[datas.size()-1].have_re_point=true;
			data_backup[i].have_re_point=true;
			data_backup[j].have_re_point=true;
			++j;
		}
		else
		{
			temp.id=data_backup[j].id;
			temp.x=data_backup[j].x;
			temp.y=data_backup[j].y;
			temp.z=data_backup[j].z;
			datas.push_back(temp);
			i=j;
			++j;
		}
	}
	if(fabs(data_backup[i].x-data_backup[0].x)+fabs(data_backup[i].y-data_backup[0].y)<=control_para)
	{
		datas[0].have_re_point=true;
		data_backup[i].have_re_point=true;
		data_backup[0].have_re_point=true;
		datas.pop_back();
	}
	//
	if(datas.size()<3)
	{
		error_type=EMPTY_DATA;
		return;
	}
}

void CPolygonWork::graph_adjust(int data_s_index,int data_w_index,double control_para)
{
	vector<tvertex> &sgraph=active_data[data_s_index];
	vector<tvertex> &wgraph=active_data[data_w_index];
	//用于标记两图形各点当前被处理的状态；
	//-1 代表初始状态；
	//在于 0 的值代表与另一个图形中的点重合，这个值也是其索引值；
	//-2 代表该点的位置不再可调整；
	//-3 代表该点原来是个重合点，而现在变成了一个不再可调整的点；
	vector<int> state_w(wgraph.size(),-1);
	vector<int> state_s(sgraph.size(),-1);
	//
	//合并重合点(注意不要重复合并顶点)；
	for(int i=0;i<sgraph.size();++i)
	{
		for(int j=0;j<wgraph.size();++j)
		{
			if(state_w[j]==-1 && fabs(wgraph[j].x-sgraph[i].x)+fabs(wgraph[j].y-sgraph[i].y)<control_para)
			{
				wgraph[j].x=sgraph[i].x;
				wgraph[j].y=sgraph[i].y;
				state_w[j]=i;
				state_s[i]=j;
			}
		}
	}
	//
	{
		//清除由于合并点造成的图形 wgraph 中的重点；
		vector<tvertex> temp_graph;
		vector<unsigned int> indexlist;
		vector<bool> wlist(state_w.size(),true);
		//
		for(i=wgraph.size()-1;i>=0;--i)
		{
			if(!wlist[i] || state_w[i]==-1) 
				continue;
			//
			for( int j=0;j<i;++j)
			{
				if(!wlist[j] || state_w[j]==-1)
					continue;
				//
				if(state_w[j]==state_w[i])
				{
					wlist[j]=false;
					indexlist.push_back(j);
				}
			}
		}
		for(i=0;i<wgraph.size();++i)
		{
			if(wlist[i])
			{
				temp_graph.push_back(wgraph[i]);
			}
		}
		wgraph.clear();
		wgraph=temp_graph;
		temp_graph.clear();
		//重建 state_w ，并更新 state_s 中的索引；
		unsigned int windex=-1;
		vector<int> temp_w;
		for(i=0;i<state_w.size();++i)
		{
			if(wlist[i])
			{
				++windex;
				//
				temp_w.push_back(state_w[i]);
				//
				if(state_w[i]>=0 && state_s[state_w[i]]==i)
				{
					state_s[state_w[i]]=windex;
				}
				else
				{
					//异常情况；
				}
			}
		}
		//
		state_w.clear();
		state_w=temp_w;
		temp_w.clear();
		wlist.clear();
	}
	//点在线段上时处理；
// 	i=0;
// 	unsigned int t=0,j=0,k=0;
// 	while(i<wgraph.size())
// 	{
// 		//如果是重合的顶点或是不可调顶点；
// 		if(state_w[i]!=-1 || state_w[i]==-2)
// 		{
// 			++i;
// 			continue;
// 		}
// 		//
// 		t=(i+1)%wgraph.size();
// 		//
// 		unsigned int index_p=0,index_n=0;
// 		bool online=false;
// 		//
// 		double gap=DBL_MAX;
// 		//
// 		j=0;
// 		while(j<sgraph.size())
// 		{
// 			k=(j+1)%sgraph.size();
// 			//
// 			if(state_w[t]==-1)
// 			{
// 				double cx,cy,cz;
// 				if(can_cross(wgraph[i],wgraph[t],sgraph[j],sgraph[k],cx,cy,cz))
// 				{
// 					double dis1;
// 					if(can_projection(wgraph[i],sgraph[j],sgraph[k]) && !can_projection(wgraph[t],sgraph[j],sgraph[k]))
// 					{
// 						if(fabs(wgraph[i].x-cx)+fabs(wgraph[i].y-cy)<control_para/2)
// 						{
// 							goto L;
// 						}
// 						if(fabs(sgraph[j].x-cx)+fabs(sgraph[j].y-cy)<control_para/2)
// 						{
// 							++j;
// 							continue;
// 						}
// 						if(fabs(sgraph[k].x-cx)+fabs(sgraph[k].y-cy)<control_para/2)
// 						{
// 							++j;
// 							continue;
// 						}
// 						//
// 						dis1=pl_distance(wgraph[i],sgraph[j],sgraph[k]);
// 					}
// 					else if(!can_projection(wgraph[i],sgraph[j],sgraph[k]) && can_projection(wgraph[t],sgraph[j],sgraph[k]))
// 					{
// 						if(fabs(wgraph[t].x-cx)+fabs(wgraph[t].y-cy)<control_para/2)
// 						{
// 							++j;
// 							continue;
// 						}
// 						if(fabs(sgraph[j].x-cx)+fabs(sgraph[j].y-cy)<control_para/2)
// 						{
// 							++j;
// 							continue;
// 						}
// 						if(fabs(sgraph[k].x-cx)+fabs(sgraph[k].y-cy)<control_para/2)
// 						{
// 							++j;
// 							continue;
// 						}
// 						//
// 						dis1=pl_distance(wgraph[t],sgraph[j],sgraph[k]);
// 					}
// 					else
// 					{
// 						goto L;
// 					}
// 					if(dis1<control_para)
// 					{
// 						double dis2;
// 						if(can_projection(sgraph[j],wgraph[i],wgraph[t]))
// 						{
// 							dis2=pl_distance(sgraph[j],wgraph[i],wgraph[t]);
// 						}
// 						else
// 						{
// 							dis2=pl_distance(sgraph[k],wgraph[i],wgraph[t]);
// 						}
// 						if(dis2<control_para)
// 						{
// 							if(state_s[j]!=-2 && state_s[k]!=-2)
// 							{
// 								double aA=wgraph[i].y-wgraph[t].y;
// 								double aB=wgraph[t].x-wgraph[i].x;
// 								double aC=-(aB*wgraph[i].y+aA*wgraph[i].x);
// 
// 								double bA=sgraph[j].y-sgraph[k].y;
// 								double bB=sgraph[k].x-sgraph[j].x;
// 								double bC=-(sgraph[j].y*bB+sgraph[j].x*bA);
// 
// 								double A=(aA+bA)/2;
// 								double B=(aB+bB)/2;
// 								double C=(aC+bC)/2;
// 
// 								point_projection(wgraph[i],A,B,C);
// 								state_w[i]=-2;
// 								point_projection(wgraph[t],A,B,C);
// 								state_w[t]=-2;
// 								point_projection(sgraph[j],A,B,C);
// 								if(state_s[j]>=0)
// 								{
// 									wgraph[state_s[j]].x=sgraph[j].x;
// 									wgraph[state_s[j]].y=sgraph[j].y;
// 									state_w[state_s[j]]=-3;
// 									state_s[j]=-3;
// 								}
// 								else
// 								{
// 									state_s[j]=-2;
// 								}
// 								point_projection(sgraph[k],A,B,C);
// 								if(state_s[k]>=0)
// 								{
// 									wgraph[state_s[k]].x=sgraph[k].x;
// 									wgraph[state_s[k]].y=sgraph[k].y;
// 									state_w[state_s[k]]=-3;
// 									state_s[k]=-3;
// 								}
// 								else
// 								{
// 									state_s[k]=-2;
// 								}
// 
// 								break;
// 							}
// 							else
// 							{
// 								double bA=sgraph[j].y-sgraph[k].y;
// 								double bB=sgraph[k].x-sgraph[j].x;
// 								double bC=-(sgraph[j].y*bB+sgraph[j].x*bA);
// 
// 								point_projection(wgraph[i],bA,bB,bC);
// 								point_projection(wgraph[t],bA,bB,bC);
// 
// 								state_w[i]=-2;
// 								state_w[t]=-2;
// 								//
// 								if(state_s[j]>=0)
// 								{
// 									state_w[state_s[j]]=-3;
// 									state_s[j]=-3;
// 								}
// 								else
// 								{
// 									state_s[j]=-2;
// 								}
// 								if(state_s[k]>=0)
// 								{
// 									state_w[state_s[k]]=-3;
// 									state_s[k]=-3;
// 								}
// 								else
// 								{
// 									state_s[k]=-2;
// 								}
// 
// 								break;
// 							}	
// 						}
// 						else
// 						{
// 							goto L;
// 						}
// 					}
// 					else
// 					{
// 						++j;
// 						continue;
// 					}
// 				}
// 				else
// 				{
// 					goto L;
// 				}
// 			}
// 			else
// 			{
// 				//如果点可能落在线上;
// L:				if(can_projection(wgraph[i],sgraph[j],sgraph[k]))
// 				{
// 					double dis=pl_distance(wgraph[i],sgraph[j],sgraph[k]);
// 					if(dis<control_para)
// 					{
// 						online=true;
// 						//
// 						if(dis<gap)
// 						{
// 							gap=dis;
// 							index_p=j;
// 							index_n=k;
// 						}
// 					}
// 				}
// 			}
// 			//
// 			++j;
// 		}
// 		if(online)
// 		{
// 			point_projection(wgraph[i],sgraph[index_p],sgraph[index_n]);
// 			if(fabs(wgraph[i].x-sgraph[index_p].x)+fabs(wgraph[i].y-sgraph[index_p].y)<control_para/2)
// 			{
// 				wgraph[i].x=sgraph[index_p].x;
// 				wgraph[i].y=sgraph[index_p].y;
// 			}
// 			else if(fabs(wgraph[i].x-sgraph[index_n].x)+fabs(wgraph[i].y-sgraph[index_n].y)<control_para/2)
// 			{
// 				wgraph[i].x=sgraph[index_n].x;
// 				wgraph[i].y=sgraph[index_n].y;
// 			}
// 			state_w[i]=-2;
// 			//
// 			if(state_s[index_p]>=0)
// 			{
// 				state_w[state_s[index_p]]=-3;
// 				state_s[index_p]=-3;
// 			}
// 			else
// 			{
// 				state_s[index_p]=-2;
// 			}
// 			if(state_s[index_n]>=0)
// 			{
// 				state_w[state_s[index_n]]=-3;
// 				state_s[index_n]=-3;
// 			}
// 			else
// 			{
// 				state_s[index_n]=-2;
// 			}
// 		}
// 		//
// 		++i;
// 	}
// 	//当 sgraph 中的点在 wgraph 中的线段上时处理；
// 	for(i=0;i<sgraph.size();++i)
// 	{
// 		if(state_s[i]>=0 || state_s[i]==-3)
// 			continue;
// 		//
// 		unsigned int index_p=0,index_n=0;
// 		//
// 		bool online=false;
// 		//
// 		double gap=DBL_MAX;
// 		//
// 		for(unsigned int j=0;j<wgraph.size();++j)
// 		{
// 			unsigned int k=(j+1)%wgraph.size();	
// 
// 			//如果点可能落在线上;
// 			if(can_projection(sgraph[i],wgraph[j],wgraph[k]))
// 			{
// 				//求点到直线的距离；
// 				double dis=pl_distance(sgraph[i],wgraph[j],wgraph[k]);
// 				if(dis<control_para)
// 				{
// 					//判断是否小于上一个距离；
// 					if(dis<gap)
// 					{
// 						online=true;
// 						//
// 						gap=dis;
// 						//
// 						index_p=j;
// 						index_n=k;
// 					}
// 				}
// 			}
// 		}
// 		if(online)
// 		{
// 			if(state_s[i]==-1)
// 			{
// 				point_projection(sgraph[i],wgraph[index_p],wgraph[index_n]);
// 				state_s[i]=-2;
// 				if(state_w[index_p]>=0)
// 				{
// 					state_s[state_w[index_p]]=-2;
// 				}
// 				state_w[index_p]=-2;
// 				if(state_w[index_n]>=0)
// 				{
// 					state_s[state_w[index_n]]=-2;
// 				}
// 				state_w[index_n]=-2;
// 			}
// 			else if(state_s[i]==-2 && state_w[index_p]<=-2 && state_w[index_n]==-1)
// 			{
// 				point_projection(wgraph[index_n],wgraph[index_p],sgraph[i]);
// 				state_w[index_n]=-2;
// 			}
// 			else if(state_s[i]==-2 && state_w[index_p]<=-2 && state_w[index_n]>=0)
// 			{
// 				point_projection(wgraph[index_n],wgraph[index_p],sgraph[i]);
// 				//
// 				sgraph[state_w[index_n]].x=wgraph[index_n].x;
// 				sgraph[state_w[index_n]].y=wgraph[index_n].y;
// 				state_s[state_w[index_n]]=-3;
// 				//
// 				state_w[index_n]=-3;
// 			}
// 			else if(state_s[i]==-2 && state_w[index_p]==-1 && state_w[index_n]<=-2)
// 			{
// 				point_projection(wgraph[index_p],sgraph[i],wgraph[index_n]);
// 				state_w[index_p]=-2;
// 			}
// 			else if(state_s[i]==-2 && state_w[index_p]>=0 && state_w[index_n]<=-2)
// 			{
// 				point_projection(wgraph[index_p],sgraph[i],wgraph[index_n]);
// 				//
// 				sgraph[state_w[index_p]].x=wgraph[index_p].x;
// 				sgraph[state_w[index_p]].y=wgraph[index_p].y;
// 				state_s[state_w[index_p]]=-3;
// 				//
// 				state_w[index_p]=-3;
// 			}
// 			else if(state_s[i]==-2 && state_w[index_p]<=-2 && state_w[index_n]<=-2)
// 			{
// 				//此操作待定；
// 			}
// 		}
// 	}
}

int CPolygonWork::find_cross_point()
{
	vector<tvertex> &data_a=active_data[0];
	vector<tvertex> &data_b=active_data[1];
	//
	data_pt.clear();
	//
	int i=0,j=0,a_size=data_a.size(),b_size=data_b.size();
	for(i=0;i<a_size;++i)
	{
		int a_pre=i,a_next=(i+1)%a_size;
		//
		for(j=0;j<b_size;++j)
		{
			int b_pre=j,b_next=(j+1)%b_size;
			//首先排除不可能有交点的情况；
			double minax=0,maxax=0,minay=0,maxay=0;
			if(data_a[a_pre].x<=data_a[a_next].x)
			{
				minax=data_a[a_pre].x;
				maxax=data_a[a_next].x;
			}
			else
			{
				minax=data_a[a_next].x;
				maxax=data_a[a_pre].x;
			}
			if(data_a[a_pre].y<=data_a[a_next].y)
			{
				minay=data_a[a_pre].y;
				maxay=data_a[a_next].y;
			}
			else
			{
				minay=data_a[a_next].y;
				maxay=data_a[a_pre].y;
			}
			//
			double minbx=0,maxbx=0,minby=0,maxby=0;
			if(data_b[b_pre].x<=data_b[b_next].x)
			{
				minbx=data_b[b_pre].x;
				maxbx=data_b[b_next].x;
			}
			else
			{
				minbx=data_b[b_next].x;
				maxbx=data_b[b_pre].x;
			}
			if(data_b[b_pre].y<=data_b[b_next].y)
			{
				minby=data_b[b_pre].y;
				maxby=data_b[b_next].y;
			}
			else
			{
				minby=data_b[b_next].y;
				maxby=data_b[b_pre].y;
			}
			//
			if(minbx-maxax>=PRECISION || minax-maxbx>=PRECISION)
			{
				continue;
			}
			if(minby-maxay>=PRECISION || minay-maxby>=PRECISION)
			{
				continue;
			}
			//当图形B的点是在图形A的边上时；
			bool on_line=false;
			bool is_a_pre=false;
			bool is_a_next=false;
			//如果图形B的前一点在图形A上；
			//此点和 data_a 中当前检测边的前一个顶点重合；
			if((fabs(data_b[b_pre].x-data_a[a_pre].x)<PRECISION) && (fabs(data_b[b_pre].y-data_a[a_pre].y)<PRECISION))
			{
				tpoint temp;
				temp.x=data_b[b_pre].x;
				temp.y=data_b[b_pre].y;
				temp.z=data_a[a_pre].z;
				//
				temp.id=data_a[a_pre].id+"+"+data_b[b_pre].id;
				data_pt.push_back(temp);
				//
				tits its;
				its.its=data_pt.size()-1;
				its.visit=false;
				its.flag=1;
				data_a[a_pre].itslist.push_back(its);
				//
				is_a_pre=true;
				//
				its.flag=1;
				data_b[b_pre].itslist.push_back(its);
				//
				on_line=true;
			}
			//此点和 data_a 中当前检测边的后一个顶点重合；
			else  if((fabs(data_b[b_pre].x-data_a[a_next].x)<PRECISION) && (fabs(data_b[b_pre].y-data_a[a_next].y)<PRECISION))
			{
				is_a_next=true;
				//
				on_line=true;
			}
			//此点在 data_a 中当前检测边上；
			else if(isPointInLine(data_b[b_pre],data_a[a_pre],data_a[a_next]))
			{
				tpoint temp;
				temp.x=data_b[b_pre].x;
				temp.y=data_b[b_pre].y;
				if(interpolation_source_index==0)
				{
					GraphAPI::GGetPtZOfLine<tpoint>(&data_a[a_pre],&data_a[a_next],&temp);
				}
				else
				{
                    temp.z=data_b[b_pre].z;
				}
				//
				temp.id=data_a[a_pre].id+"-"+data_a[a_next].id+"+"+data_b[b_pre].id;
				data_pt.push_back(temp);
				//
				tits its;
				its.its=data_pt.size()-1;
				its.visit=false;
				its.flag=0;
				data_a[a_pre].itslist.push_back(its);
				//
				its.flag=1;
				data_b[b_pre].itslist.push_back(its);
				//
				on_line=true;
			}
			//如果图形B的后一个点在图形A上；
			if(fabs(data_b[b_next].x-data_a[a_pre].x)<PRECISION && fabs(data_b[b_next].y-data_a[a_pre].y)<PRECISION)
			{
				//
				is_a_pre=true;
				//
				on_line=true;
			}
			else  if(fabs(data_b[b_next].x-data_a[a_next].x)<PRECISION && fabs(data_b[b_next].y-data_a[a_next].y)<PRECISION)
			{
				is_a_next=true;
				//
				on_line=true;
			}
			else if(isPointInLine(data_b[b_next],data_a[a_pre],data_a[a_next]))
			{
				on_line=true;
			}
			//当图形A的点是在图形B的边上时；
			//如果图形A的前一点在图形B上；
			if((!is_a_pre) && isPointInLine(data_a[a_pre],data_b[b_pre],data_b[b_next]))
			{
				tpoint temp;
				temp.x=data_a[a_pre].x;
				temp.y=data_a[a_pre].y;
				if(interpolation_source_index==0)
				{
	                temp.z=data_a[a_pre].z;
				}
				else
				{
                    GraphAPI::GGetPtZOfLine<tpoint>(&data_b[a_pre],&data_b[a_next],&temp);
				}
				//
				temp.id=data_a[a_pre].id+"+"+data_b[b_pre].id+"-"+data_b[b_next].id;
				data_pt.push_back(temp);
				//
				tits its;
				its.its=data_pt.size()-1;
				its.visit=false;
				its.flag=1;
				data_a[a_pre].itslist.push_back(its);
				//
				its.flag=0;
				data_b[b_pre].itslist.push_back(its);
				//
				on_line=true;
			}
			//如果图形A的后一点在图形B上；
			if((!is_a_next) && isPointInLine(data_a[a_next],data_b[b_pre],data_b[b_next]))
			{
				on_line=true;
			}
			if(on_line)
			{
				continue;
			}
			//当图形A的边和图形B的边相交时；
			tpoint temp;
			if(can_cross(data_a[a_pre],data_a[a_next],data_b[b_pre],data_b[b_next],temp.x,temp.y,temp.z))
			{
				temp.id=data_a[a_pre].id+"-"+data_a[a_next].id+"+"+data_b[b_pre].id+"-"+data_b[b_next].id;
				data_pt.push_back(temp);
				//
				tits its;
				its.its=data_pt.size()-1;
				its.visit=false;
				its.flag=0;
				data_a[a_pre].itslist.push_back(its);
				//
				its.flag=0;
				data_b[b_pre].itslist.push_back(its);
			}
		}
	}

	if(data_pt.size()==0)
	{
		error_type=NO_INTERSECTION;
	}

	return data_pt.size();
}

void CPolygonWork::intersection_sort(int data_index)
{
	vector<tvertex> &datas=active_data[data_index];
	//
	for(unsigned int i=0;i<datas.size();++i)
	{
		if(datas[i].itslist.size()!=0)
		{
			vector<tits> templist;
			for(unsigned int j=0;j<datas[i].itslist.size();++j)
			{
				double mind=DBL_MAX;
				unsigned int index=0;
				for(unsigned int k=0;k<datas[i].itslist.size();++k)
				{
					if(datas[i].itslist[k].visit)
						continue;
					//
					unsigned int t=datas[i].itslist[k].its;
					double temp_min=fabs(data_pt[t].x-datas[i].x)+fabs(data_pt[t].y-datas[i].y);
					if(temp_min<mind)
					{
						mind=temp_min;
						index=k;
					}
				}
				datas[i].itslist[index].visit=true;
				//
				tits item=datas[i].itslist[index];
				item.visit=false;
				templist.push_back(item);
			}
			datas[i].itslist.clear();
			datas[i].itslist=templist;
		}
	}
}

void CPolygonWork::union_points(int dataf_index,int data_index)
{
	vector<tfpoint> &dataf=final_data[dataf_index];
	vector<tvertex> &datas=active_data[data_index];
	//
	dataf.clear();
	//
	for(unsigned int i=0;i<datas.size();++i)
	{
		unsigned t=(i-1+datas.size())%datas.size();
		//
		tfpoint temp;
		temp.con_index=-1;
		//此点即是顶点，也是交点；
		if(datas[i].itslist.size()!=0 && datas[i].itslist[0].flag==1)
		{
			temp.flag=3;
		}
		//此点是顶点；
		else
		{
			if(datas[t].itslist.size()!=0 )
			{
				unsigned int index=datas[t].itslist[datas[t].itslist.size()-1].its;
				if(fabs(datas[i].x-data_pt[index].x)<PRECISION*10 && fabs(datas[i].y-data_pt[index].y)<PRECISION*10)
				{
					if(datas[i].itslist.size()==0)
					{
						continue;
					}
					else
					{
						index=datas[i].itslist[0].its;
						datas[i].x=data_pt[index].x;
						datas[i].y=data_pt[index].y;
						datas[i].z=data_pt[index].z;
						datas[i].itslist[0].flag=1;
						temp.flag=3;
					}
				}
				else
				{
					goto L;
				}
			}
			else
			{
L:				if(datas[i].itslist.size()!=0 && datas[i].itslist[0].flag==0)
				{
					unsigned int index=datas[i].itslist[0].its;
					if(fabs(datas[i].x-data_pt[index].x)<PRECISION*10 && fabs(datas[i].y-data_pt[index].y)<PRECISION*10)
					{
						datas[i].x=data_pt[index].x;
						datas[i].y=data_pt[index].y;
						datas[i].z=data_pt[index].z;
						datas[i].itslist[0].flag=1;
						temp.flag=3;
					}
					else
					{
						temp.flag=2;
					}
				}
				else
				{
					temp.flag=2;
				}
			}
		}
		//	
		temp.id=datas[i].id;
		temp.visit=false;
		temp.x=datas[i].x;
		temp.y=datas[i].y;
		temp.z=datas[i].z;
		temp.have_re_point=datas[i].have_re_point;
		dataf.push_back(temp);
		if(datas[i].itslist.size()!=0)
		{
			unsigned int j=0;
			if(datas[i].itslist[0].flag==1)
			{
				j=1;
			}
			//普通交点；
			for(;j<datas[i].itslist.size();++j)
			{
				temp.con_index=0;
				temp.flag=0;
				temp.id=data_pt[datas[i].itslist[j].its].id;
				temp.visit=false;
				temp.x=data_pt[datas[i].itslist[j].its].x;
				temp.y=data_pt[datas[i].itslist[j].its].y;
				temp.z=data_pt[datas[i].itslist[j].its].z;
				dataf.push_back(temp);
			}
		}
	}
	//
	datas.clear();
	for(i=0;i<dataf.size();++i)
	{
		if(dataf[i].flag==2 || dataf[i].flag==3)
		{
			tvertex temp;
			temp.id=dataf[i].id;
			temp.x=dataf[i].x;
			temp.y=dataf[i].y;
			temp.z=dataf[i].z;
			temp.have_re_point=dataf[i].have_re_point;
			datas.push_back(temp);
		}
	}
}

void CPolygonWork::create_connection()
{
	vector<tfpoint> &data_fa=final_data[0];
	vector<tfpoint> &data_fb=final_data[1];
	//
	for(unsigned int i=0;i<data_fa.size();++i)
	{
		if(data_fa[i].flag==2)
		{
			data_fa[i].con_index=-1;
			continue;
		}
		//
		bool finded=false;
		//
		for(unsigned int j=0;j<data_fb.size();++j)
		{
			if(data_fb[j].flag==2)
			{
				data_fb[j].con_index=-1;
				continue;
			}
			if(data_fb[j].con_index>0)
			{
				continue;
			}
			//
			if(fabs(data_fa[i].x-data_fb[j].x)<PRECISION && fabs(data_fa[i].y-data_fb[j].y)<PRECISION)
			{
				data_fa[i].con_index=j;
				data_fb[j].con_index=i;
				finded=true;
				break;
			}
		}
		///此处注意；
		if(!finded)
		{
			data_fa[i].flag=2;
			data_fa[i].con_index=-1;
		}
	}
}

void CPolygonWork::check_p_in()
{
	vector<tfpoint> &data_a_final=final_data[0];
	vector<tfpoint> &data_b_final=final_data[1];
	//
	for(unsigned int i=0;i<data_a_final.size();++i)
	{
		unsigned int t=data_a_final[i].con_index;
		if((data_a_final[i].flag==3 && data_b_final[t].flag==3) || 
			(data_a_final[i].flag==3 && data_b_final[t].flag==0) ||
			(data_a_final[i].flag==0 && data_b_final[t].flag==3))
		{
			unsigned int j=(t-1+data_b_final.size())%data_b_final.size();
			unsigned int k=(t+1)%data_b_final.size();
			tpoint templ,tempr;
			templ.id="";
			templ.x=(data_b_final[j].x+data_b_final[t].x)/2;
			templ.y=(data_b_final[j].y+data_b_final[t].y)/2;
			templ.z=0;
			//
			tempr.id="";
			tempr.x=(data_b_final[k].x+data_b_final[t].x)/2;
			tempr.y=(data_b_final[k].y+data_b_final[t].y)/2;
			tempr.z=0;
			//
			int positionl=point_position(templ,0);
			int positionr=point_position(tempr,0);
			//
			if(positionl==4 && positionr==4)
			{
				j=(i-1+data_a_final.size())%data_a_final.size();
				k=(i+1)%data_a_final.size();
				templ.id="";
				templ.x=(data_a_final[j].x+data_a_final[i].x)/2;
				templ.y=(data_a_final[j].y+data_a_final[i].y)/2;
				templ.z=0;
				//
				tempr.id="";
				tempr.x=(data_a_final[k].x+data_a_final[i].x)/2;
				tempr.y=(data_a_final[k].y+data_a_final[i].y)/2;
				tempr.z=0;
				//
				positionl=point_position(templ,1);
				positionr=point_position(tempr,1);
				//
				if(positionl==4 && positionr==4)
				{
					data_a_final[i].flag=4;
					data_b_final[t].flag=4;
				}
				else if(positionr==3)
				{
					data_a_final[i].flag=1;
					data_b_final[t].flag=-1;
				}
			}
			else if(positionr==3)
			{
				data_a_final[i].flag=-1;
			}
		}
	}
}

void CPolygonWork::check_in_out(int fs_index,int fw_index,int data_index)
{
	vector<tfpoint> &data_fs=final_data[fs_index];
	vector<tfpoint> &data_fw=final_data[fw_index];
	//
	unsigned int i=0;
	//
	bool have_error=false;
	vector<unsigned int> errorlist;
	//
	for(;i<data_fs.size();++i)
	{
		unsigned int j=data_fs[i].con_index;
		unsigned int k=(j+1)%data_fw.size();
		if(data_fs[i].flag==3 ||  data_fs[i].flag==0)
		{
			if(data_fw[k].flag==2)
			{
				double position=point_position(data_fw[k],data_index);
				if(position==4)
				{
					//此交点为一般入点；
					data_fs[i].flag=1;
					continue;
				}
				if(position==3)
				{
					//此交点为出点；
					data_fs[i].flag=-1;
					continue;
				}
			}
			else 
			{
				tpoint temp;
				temp.x=(data_fw[j].x+data_fw[k].x)/2;
				temp.y=(data_fw[j].y+data_fw[k].y)/2;
				temp.z=0;
				double position=point_position(temp,data_index);
				if(position==4)
				{
					//此交点为一般入点；
					data_fs[i].flag=1;
					continue;
				}
				else if(position==3)
				{
					//此交点为出点；
					data_fs[i].flag=-1;
					continue;
				}
				else if(position==2 || position==1)
				{
					bool on_line=true;
					unsigned int t=i;
					int j0 = j;
					while(on_line)
					{
						if(data_fw[k].con_index==(t+1)%data_fs.size() || data_fw[k].con_index==(t-1+data_fs.size())%data_fs.size())
						{
							t=data_fw[k].con_index;
							j=k;
							k=(j+1)%data_fw.size();

							if( k==j0 )
							{
								error_type=OVERLAP_EXACT;
								return;
							}
							
							continue;
						}
						///////////////
						temp.x=(data_fw[j].x+data_fw[k].x)/2;
						temp.y=(data_fw[j].y+data_fw[k].y)/2;
						position=point_position(temp,data_index);
						if(position==3)
						{
							on_line=false;
							//
							//此交点为出点；
							data_fs[i].flag=-1;
						}
						else if(position==4)
						{
							on_line=false;
							//
							//此交点为入点；
							data_fs[i].flag=1;
						}
						else
						{
							on_line=false;
							//异常情况；
							have_error=true;
							data_fs[i].flag=2;
							//errorlist.push_back(i);
						}
						//
						t=i;
						j=data_fs[i].con_index;
						k=(j+1)%data_fw.size();
						while(data_fw[k].con_index==(t+1)%data_fs.size() || data_fw[k].con_index==(t-1+data_fs.size())%data_fs.size())
						{
							data_fs[t].flag=data_fs[i].flag;
							//
							t=data_fw[k].con_index;
							j=k;
							k=(j+1)%data_fw.size();
						}
						//
					}
				}
			}
		}
	}
}

void CPolygonWork::find_union()
{
	union_list.clear();

	if( error_type!=NONE)
		return;
	//
	vector<tfpoint> fpoints[2]={final_data[0],final_data[1]};
	if(is_overlap)
	{
        vector<tpoint> temp_list;
		for(int i=0;i<fpoints[0].size();++i)
		{
			tpoint temp;
			temp.have_re_point=fpoints[0][i].have_re_point;
			temp.id=fpoints[0][i].id;
			temp.x=fpoints[0][i].x;
			temp.y=fpoints[0][i].y;
			temp.z=fpoints[0][i].z;
			temp_list.push_back(temp);
		}
		union_list.push_back(temp_list);
		return;
	}
	//找第一个出点；
	vector<tpoint> pointlist;
	bool first=true;
	bool delete_out=false;
	unsigned int current=0,old_current=0;
L:	pointlist.clear();
	unsigned int index=0;
	unsigned int flag=index;
	current=old_current;
	while (old_current==0 && fpoints[current][index].flag!=-1)
	{
		index=(index+1)%fpoints[current].size();
		if(index==flag)
		{
			if(first)
			{
				old_current=1;
				current=1;
				break;
			}
			else
			{
				return;
			}
		}
	}
	//if(fpoints[old_current][0].flag!=-1 && index==flag)
	if(old_current==1 && fpoints[old_current][0].flag!=-1)
	{
		if(first)
		{
			first=false;
			//current=1;
		}
		index=0;
		flag=index;
		while (old_current==1 && fpoints[current][index].flag!=-1)
		{
			index=(index+1)%fpoints[current].size();
			if(index==flag)
			{
				return;
			}
		}
	}
	else
	{
		first=false;
	}
	//
	//去除多余的出点；
	if(!delete_out)
	{
		delete_out=true;
		//
		for(unsigned int i=0;i<fpoints[current].size();++i)
		{
			if(fpoints[current][i].flag==-1)
			{
				int _current=(current+1)%2;
				unsigned int j=fpoints[current][i].con_index;
				unsigned int k=(j-1+fpoints[_current].size())%fpoints[_current].size();
				unsigned int t=i;

				while((t+1)%fpoints[current].size()==fpoints[_current][k].con_index || 
					(t-1+fpoints[current].size())%fpoints[current].size()==fpoints[_current][k].con_index)
				{
					t=fpoints[_current][k].con_index;
					if(fpoints[current][t].flag==-1)
					{
						fpoints[current][t].flag=-3;
					}
					j=k;
					k=(j-1+fpoints[_current].size())%fpoints[_current].size();
				}
			}
		}
	}
	//
	//如果由该出点转到另一多边形的下一点对与此多边形也是出点，则放弃上一个第一出点；
	unsigned int _index=fpoints[current][index].con_index;
	unsigned int _current=(current+1)%2;
	_index=(_index+1)%fpoints[_current].size();
	index=(index-1+fpoints[current].size())%fpoints[current].size();
	flag=_index;
	while(fpoints[_current][_index].con_index==index && fpoints[current][fpoints[_current][_index].con_index].flag==-1)
	{
		fpoints[current][fpoints[_current][_index].con_index].flag=-3;
		_index=(_index+1)%fpoints[_current].size();
		index=(index-1+fpoints[current].size())%fpoints[current].size();
		if(_index==flag)
		{
			break;
		}
	}
	_index=(_index-1+fpoints[_current].size())%fpoints[_current].size();
	index=fpoints[_current][_index].con_index;
	//
	tpoint temp;
	temp.id=fpoints[current][index].id;
	temp.x=fpoints[current][index].x;
	temp.y=fpoints[current][index].y;
	temp.z=fpoints[current][index].z;
	pointlist.push_back(temp);
	//
	fpoints[current][index].visit=true;
	fpoints[current][index].flag=-3;
	//
	index=fpoints[current][index].con_index;
	current=(current+1)%2;
	fpoints[current][index].visit=true;
	//
	index=(index+1)%fpoints[current].size();
	//
	//循环遍历，直到回到起点；
	while(!fpoints[current][index].visit)
	{
		temp.id=fpoints[current][index].id;
		temp.x=fpoints[current][index].x;
		temp.y=fpoints[current][index].y;
		temp.z=fpoints[current][index].z;
		pointlist.push_back(temp);
		//
		if(fpoints[current][index].flag==-1)
		{
			if(current==old_current)
			{
				fpoints[current][index].flag=-3;
			}
			else
			{
				/*if(fpoints[current][index].con_index==-1)
				{
				return;
				}*/
				fpoints[current][index].flag=-3;
				if(fpoints[old_current][fpoints[current][index].con_index].flag==-1)
				{
					fpoints[old_current][fpoints[current][index].con_index].flag=-3;
				}
			}
			//
			index=fpoints[current][index].con_index;
			current=(current+1)%2;
			index=(index+1)%fpoints[current].size();
		}
		else
		{
			if(current!=old_current)
			{
				int _index=fpoints[current][index].con_index;
				if(_index!=-1)
				{
					if(fpoints[old_current][_index].flag==-1)
						fpoints[old_current][_index].flag=-3;
				}
			}
			//
			index=(index+1)%fpoints[current].size();
		}
	}
	//
	fpoints[current][index].visit=false;
	//
	index=fpoints[current][index].con_index;
	current=(current+1)%2;
	fpoints[current][index].visit=false;
	//
	union_list.push_back(pointlist);
	goto L;
}

void CPolygonWork::find_intersection()
{
	intersection_list.clear();

	if( error_type!=NONE)
		return;
	//
	vector<tfpoint> fpoints[2]={final_data[0],final_data[1]};
	if(is_overlap)
	{
        vector<tpoint> temp_list;
		for(int i=0;i<fpoints[0].size();++i)
		{
			tpoint temp;
			temp.have_re_point=fpoints[0][i].have_re_point;
			temp.id=fpoints[0][i].id;
			temp.x=fpoints[0][i].x;
			temp.y=fpoints[0][i].y;
			temp.z=fpoints[0][i].z;
			temp_list.push_back(temp);
		}
		intersection_list.push_back(temp_list);
		return;
	}
	//找第一个入点；
	vector<tpoint> pointlist;
	bool first=true;
	bool delete_in=false;
	unsigned int current=0,old_current=0;
L:	pointlist.clear();
	unsigned int index=0;
	unsigned int flag=index;
	current=old_current;
	while (old_current==0 && (fpoints[current][index].flag!=1 && fpoints[current][index].flag!=4))
	{
		index=(index+1)%fpoints[current].size();
		if(index==flag)
		{
			if(first)
			{
				old_current=1;
				current=1;
				break;
			}
			else
			{
				return;
			}
		}
	}
	//if((fpoints[current][index].flag!=1 && fpoints[current][index].flag!=4) && index==flag)
	if(old_current==1 && (fpoints[current][index].flag!=1 && fpoints[current][index].flag!=4))
	{
		if(first)
		{
			first=false;
			//current=1;
		}
		index=0;
		flag=index;
		while (old_current==1 && (fpoints[current][index].flag!=1 && fpoints[current][index].flag!=4))
		{
			index=(index+1)%fpoints[current].size();
			if(index==flag)
			{
				return;
			}
		}
	}
	else
	{
		first=false;
	}
	//去除多余的入点；
	if(!delete_in)
	{
		delete_in=true;
		//
		for(unsigned int i=0;i<fpoints[current].size();++i)
		{
			if(fpoints[current][i].flag==1)
			{
				int _current=(current+1)%2;
				unsigned int j=fpoints[current][i].con_index;
				unsigned int k=(j-1+fpoints[_current].size())%fpoints[_current].size();
				unsigned int t=i;
				while((t+1)%fpoints[current].size()==fpoints[_current][k].con_index || 
					(t-1+fpoints[current].size())%fpoints[current].size()==fpoints[_current][k].con_index)
				{
					t=fpoints[_current][k].con_index;
					if(fpoints[current][t].flag==1)
					{
						fpoints[current][t].flag=3;
					}
					j=k;
					k=(j-1+fpoints[_current].size())%fpoints[_current].size();
				}
			}
		}
	}
	//
	tpoint temp;
	temp.id=fpoints[current][index].id;
	temp.x=fpoints[current][index].x;
	temp.y=fpoints[current][index].y;
	temp.z=fpoints[current][index].z;
	pointlist.push_back(temp);
	//
	fpoints[current][index].visit=true;
	fpoints[current][index].flag=3;
	//
	index=fpoints[current][index].con_index;
	current=(current+1)%2;
	fpoints[current][index].visit=true;
	//
	index=(index+1)%fpoints[current].size();
	//
	//循环遍历，直到回到起点；
	while(!fpoints[current][index].visit)
	{
		temp.id=fpoints[current][index].id;
		temp.x=fpoints[current][index].x;
		temp.y=fpoints[current][index].y;
		temp.z=fpoints[current][index].z;
		pointlist.push_back(temp);
		//
		if(fpoints[current][index].flag==1 || fpoints[current][index].flag==4)
		{
			if(current==old_current)
			{
				fpoints[current][index].flag=3;
			}
			else
			{
				fpoints[current][index].flag=3;
				if(fpoints[old_current][fpoints[current][index].con_index].flag==1)
				{
					fpoints[old_current][fpoints[current][index].con_index].flag=3;
				}
			}
			//
			index=fpoints[current][index].con_index;
			current=(current+1)%2;
			index=(index+1)%fpoints[current].size();
		}
		else
		{
			if(current!=old_current)
			{
				int _index=fpoints[current][index].con_index;
				if(_index!=-1)
				{
					if(fpoints[old_current][_index].flag==1)
						fpoints[old_current][_index].flag=3;
				}
			}
			//
			index=(index+1)%fpoints[current].size();
		}
	}
	//
	fpoints[current][index].visit=false;
	//
	index=fpoints[current][index].con_index;
	current=(current+1)%2;
	fpoints[current][index].visit=false;
	//
	intersection_list.push_back(pointlist);
	goto L;
}

void CPolygonWork::find_XOR(int fa_index,int fb_index,int data_index)
{	
	vector<tfpoint> fpoints[2]={final_data[fa_index],final_data[fb_index]};
	//
	xor_list.clear();

	if( error_type!=NONE)
		return;

	if(is_overlap)
	{
        vector<tpoint> temp_list;
		for(int i=0;i<fpoints[0].size();++i)
		{
			tpoint temp;
			temp.have_re_point=fpoints[0][i].have_re_point;
			temp.id=fpoints[0][i].id;
			temp.x=fpoints[0][i].x;
			temp.y=fpoints[0][i].y;
			temp.z=fpoints[0][i].z;
			temp_list.push_back(temp);
		}
		xor_list.push_back(temp_list);
		return;
	}
	//
	//找到 data_fa 的第一个入点；
	vector<tpoint> pointlist;
	//
	bool delete_in=false;
	bool add_in=false;
	//
L:	pointlist.clear();
	unsigned int index=0;
	unsigned int flag=index;
	int current=0;
	while(fpoints[current][index].flag!=1 && fpoints[current][index].flag!=4)
	{
		index=(index+1)%fpoints[current].size();
		if(index==flag)
		{
			return;
		}
	}
	//如果由该出点转到另一多边形的下一点对与此多边形也是入点，则放弃上一个入点；
	int _current=1;
	int _index=fpoints[current][index].con_index;
	unsigned int j=(_index+1)%fpoints[_current].size();
	j=fpoints[_current][j].con_index;
	unsigned int k=(index+1)%fpoints[current].size();
	while(k==j && (fpoints[current][k].flag==1 || fpoints[current][k].flag==4))
	{
		index=k;
		//
		_index=fpoints[current][index].con_index;
		j=(_index+1)%fpoints[_current].size();
		j=fpoints[_current][j].con_index;
		k=(index+1)%fpoints[current].size();
	}
	//去除 data_fa 中多余的入点；
	if(!delete_in)
	{
		delete_in=true;
		//
		for(unsigned int i=0;i<fpoints[current].size();++i)
		{
			if(fpoints[current][i].flag==1)
			{
				_current=1;
				j=fpoints[current][i].con_index;
				k=(j-1+fpoints[_current].size())%fpoints[_current].size();
				unsigned int t=i;
				while((t+1)%fpoints[current].size()==fpoints[_current][k].con_index || 
					(t-1+fpoints[current].size())%fpoints[current].size()==fpoints[_current][k].con_index)
				{
					t=fpoints[_current][k].con_index;
					if(fpoints[current][t].flag==1)
					{
						fpoints[current][t].flag=3;
					}
					j=k;
					k=(j-1+fpoints[_current].size())%fpoints[_current].size();
				}
			}
		}
	}

	//
	tpoint temp;
	temp.id=fpoints[current][index].id;
	temp.x=fpoints[current][index].x;
	temp.y=fpoints[current][index].y;
	temp.z=fpoints[current][index].z;
	pointlist.push_back(temp);
	//
	fpoints[current][index].flag=3;
	fpoints[current][index].visit=true;
	//
	index=fpoints[current][index].con_index;
	current=1;
	//
	//增加 data_fb 中的入点；
	if(!add_in)
	{
		add_in=true;
		//
		for(unsigned int i=0;i<fpoints[current].size();++i)
		{
			if(fpoints[current][i].flag==-1)
			{
				int _current=0;
				j=fpoints[current][i].con_index;
				k=(j-1+fpoints[_current].size())%fpoints[_current].size();
				tpoint temp;
				temp.id="";
				temp.x=(fpoints[_current][j].x+fpoints[_current][k].x)/2;
				temp.y=(fpoints[_current][j].y+fpoints[_current][k].y)/2;
				temp.z=0;
				if(point_position(temp,data_index)==3)
				{
					fpoints[current][i].flag=4;
				}
			}
		}
	}
	//
	fpoints[current][index].visit=true;
	index=(index+1)%fpoints[current].size();
	//
	//循环遍历，直到回到起点；
	while(!fpoints[current][index].visit)
	{
		temp.id=fpoints[current][index].id;
		temp.x=fpoints[current][index].x;
		temp.y=fpoints[current][index].y;
		temp.z=fpoints[current][index].z;
		pointlist.push_back(temp);
		//
		if(current==1)
		{
			if(fpoints[current][index].flag==1)
			{
				fpoints[current][index].flag=3;
				//
				index=fpoints[current][index].con_index;
				current=0;
				//
				if(fpoints[current][index].flag==1)
				{
					fpoints[current][index].flag=3;
				}
				//
				index=(index-1+fpoints[current].size())%fpoints[current].size();
			}
			else if(fpoints[current][index].flag==4)
			{

				index=fpoints[current][index].con_index;
				current=0;
				//
				if(fpoints[current][index].flag!=4)
				{
					index=(index-1+fpoints[current].size())%fpoints[current].size();
				}
				else
				{
					fpoints[current][index].flag=3;
					index=fpoints[current][index].con_index;
					current=1;
					index=(index+1+fpoints[current].size())%fpoints[current].size();
				}
			}
			else
			{
				int _index=fpoints[current][index].con_index;
				if(_index!=-1)
				{
					if(fpoints[0][_index].flag==1)
						fpoints[0][_index].flag=3;
				}
				index=(index+1)%fpoints[current].size();
			}
		}
		else
		{
			if(fpoints[current][index].flag==1 || fpoints[current][index].flag==4)
			{
				fpoints[current][index].flag=3;
				index=fpoints[current][index].con_index;
				//
				current=1;
				index=(index+1)%fpoints[current].size();
			}
			else
			{
				index=(index-1+fpoints[current].size())%fpoints[current].size();
			}
		}
	}
	//
	fpoints[current][index].visit=false;
	//
	index=fpoints[current][index].con_index;
	current=(current+1)%2;
	fpoints[current][index].visit=false;
	//
	xor_list.push_back(pointlist);
	goto L;
}

void CPolygonWork::set_overlap(double op)
{
	overlap_para=op;
	delete_rpoint(0,0,overlap_para);
	delete_rpoint(1,1,overlap_para);
	graph_adjust(0,1,overlap_para);
}

void CPolygonWork::overlap_polygons()
{
	int i=0;
	int index,other_index;
	if(final_data[0].size()<=final_data[1].size())
	{
		index=0;
		other_index=1;
	}
	else
	{
		index=1;
		other_index=0;
	}
	is_overlap=true;
    for(i=0;i<final_data[index].size();++i)
	{
		int j=(i+1)%final_data[index].size();
		if(final_data[index][i].flag==2 || final_data[index][j].flag==2)
		{
			is_overlap=false;
			return;
		}
		tpoint temp;
		temp.x=(final_data[index][i].x+final_data[index][j].x)/2;
		temp.y=(final_data[index][i].y+final_data[index][j].y)/2;
		int pos=point_position(temp,other_index);
		if(pos!=1 && pos!=2)
		{
			is_overlap=false;
			return;
		}
	}
}

void CPolygonWork::initialize()
{
	if(error_type!=NONE)
		return;

	interpolation_source_index=0;
	//使图形数据按正向存储；
	right_direction(0);
	right_direction(1);
	//找两多边形的交点；
	find_cross_point();
	//对图形A的交点排序；
	intersection_sort(0);
	//对图形B的交点排序；
	intersection_sort(1);
	//合并图形A中的交点和顶点；
	union_points(0,0);
	//合并图形B中的交点和顶点；
	union_points(1,1);
	//创建图形A和图形B的连接关系；
	create_connection();
	//判断两多边形是否完全重合；
    overlap_polygons();
	if(is_overlap)   
	{
		error_type=OVERLAP_EXACT;
		return;
	}
	//查找图形A和图形B中特殊的入点；
	check_p_in();
	//判断图形A中各点的出入情况；
	check_in_out(0,1,0);
	//判断图形B中各点的出入情况；
	check_in_out(1,0,1);
}

}