//*******************************************************************
//	FArray.hpp（ FlexArray）
//	模块名称：灵活动态数组C++模板类 
//	version:1.0
//  说明：该链表化是在CSArray 2.0版本的基础上强化了数据插入与删除的修正版，
//	本代码提供对动态数组的支持，在内存中程序将数据分块存放，
//　避免了大块内存的申请，同时也和普通的双向链表不同的是本代码提供
//　了对内部数据的快速索引，大大提高了数据访问速度。
//	该链表的主要目标是大量数据的管理，提供一个快速的数据插入删除等接口．在使用它时
//	配置一套好的参数将大大提高程序的执行效率，
//新增优势功能：
//1 在实现上内部分为数组和内存池，在删除、压缩后回收的内存放回内存池可以重复利用避免内存频繁释放申请。
//2 提供了新的压缩方法，腾出的空间放回回收池，也可以独立删除数组或者回收池内存。
//3 因为删除操作不可避免的会有内存数据移动问题，因此在大批量删除时效率会很差，因此特别提供了一个批量删除的功能比如想把值＝0的单元全部删除
//该链表的执行效率大大高于
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FARRAY_H__B1D40C22_2698_4202_921E_36D447EA4199__INCLUDED_)
#define AFX_FARRAY_H__B1D40C22_2698_4202_921E_36D447EA4199__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef ASSERT
#define ASSERT(a) void()
#endif

#ifndef BOOL
#define BOOL int
#endif

//( Flex Array )
template<class T>
class CFArray  
{
	typedef struct _FARRAYNODE{
		struct _FARRAYNODE *	pPrevNode;	//前一节点
		T				   *	pData;		//存储实际数据数组的数据块的指针	
		WORD					dwUsed;		//被占用的空间数,初始时为0，最大值为该结点的长度
		struct _FARRAYNODE *	pNextNode;	//后一节点
	}FARRAYNODE;

public:
	CFArray(WORD nGrowBy=5)
	{
		m_pCurNode=m_pHeadNode=m_pTailNode=NULL;
		m_nCurIndex=-1;
		m_nCount=0;
		m_nEmpty=0;
		m_nGrowBy=nGrowBy;
		m_byZipKey=10;	//默认压缩阀值为10%
		m_bNeedCompack = false;

		m_pEmptyHeadNode =NULL;	//头节点
	    m_pEmptyEndNode  =NULL;
	}

	virtual ~CFArray()
	{
		RemoveAll();
	    //Free();
		//孙开敏修改于2005 2 21
	}

	//******************************************
	//	name:Add
	//	function:添加数据
	//	input:　T newElement-新数据
	//  return: 成功=数据索引号,失败=-1
	//	remark: 
	//******************************************
	int Add(T newElement)
	{
		if(m_nGrowBy==0)//
			return -1;
		if(m_pHeadNode==NULL)
		{//如果是空就产生第一个节点
			if(!(m_pHeadNode=NewNode())) return -1;
			*m_pHeadNode->pData = newElement;
			m_pHeadNode->dwUsed = 1;
			m_nCurIndex = 0;
			m_nCount    = 1;
			m_nEmpty    = m_nGrowBy - 1;
			m_pCurNode  = m_pTailNode = m_pHeadNode;
			//生成第一个节点
		}
		else
		{
			if(m_pTailNode->dwUsed==m_nGrowBy)
			{//如果最后一个节点已经没有空余使用则再插入一个节点
				FARRAYNODE *pNewNode=NewNode();
				if(!pNewNode) return -1;
				pNewNode->pPrevNode    = m_pTailNode;
				pNewNode->pNextNode    = NULL;// 孙开敏加于2005 2 21 原缺少此语句
				m_pTailNode->pNextNode = pNewNode;
				m_pTailNode = pNewNode;
				m_nEmpty   +=m_nGrowBy;
			}
			*(m_pTailNode->pData+m_pTailNode->dwUsed++)=newElement;
			//孙开敏修改于2005 2 21 原句为m_pTailNode->pData[m_pTailNode->dwUsed++]=newElement;
			m_nCount++;
			m_nEmpty--;//标注整个结构中还有多少空余单元可以使用
			m_nCurIndex = m_nCount-m_pTailNode->dwUsed;//使指针指向当前节点
			m_pCurNode  = m_pTailNode;
		}
		return m_nCount-1;
	}

	//******************************************
	//	name:AddBatch
	//	function:批量添加数据
	//	input:　T *pElement-源数组指针
	//			int count-数组大小
	//  return: BOOL　TRUE－成功；FALSE－失败
	//	remark: 
	//******************************************
	BOOL AddBatch(T *pElement,int count)
	{
		for(int i=0;i<count;i++)
			if(Add(pElement[i])==-1)
				return FALSE;
			return TRUE;
	}


	//******************************************
	//	name:Copy
	//	function:数据复制
	//	input:　CFArray & src-源动态数组
	//  return: 
	//	remark: 使用前请先确保两个对象有相同的数据类型
	//******************************************
	void Copy(CFArray &src )
	{
		T *pt;
		RemoveAll();
		int size=src.GetSize();
		SetSize(size);
		for(int i=0;i<m_nCount;i++)
		{
			pt=src.GetPtAt(i);
			SetAt(*pt,i);
		}
	}

	//******************************************
	//	name:GetAt
	//	function:获取数组指定位置的数据
	//	input:　int index-指定位置
	//  return: T 数据
	//	remark: 
	//******************************************
	T GetAt(DWORD index)
	{
		FARRAYNODE      *pDest = NULL;
		ASSERT(index>=0&&index<m_nCount);
		pDest = GetDestSegEntry( index );
		return ((T *)pDest->pData)[index-m_nCurIndex];
	}
	
	//******************************************
	//	name:GetPtAt
	//	function:获取数组指定位置的数据的指针
	//	input:　int index-指定位置
	//  return: T 数据
	//	remark: 提供对内部数据的直接访问，小心使用!!
	//******************************************
	T *GetPtAt(DWORD index)
	{
		FARRAYNODE      *pDest = NULL;
		ASSERT(index>=0&&index<m_nCount);
		pDest = GetDestSegEntry( index );
		return ((T *)pDest->pData)+index-m_nCurIndex;
	}
	
	T operator[](DWORD index){ return GetAt(index);}
	
	//******************************************
	//	name:GetSize
	//	function:获取数组的数据容量
	//	input:　
	//  return: int 数据容量
	//	remark: 
	//******************************************
	int GetSize(){return m_nCount;}
	
	void SetCompackKey(BYTE byKey){ m_byZipKey=byKey;}
	//******************************************
	//	name:SetAt
	//	function:修改数组指定位置的数据
	//	input:　T newElement-新数据
	//			int index-指定索引号
	//  return: BOOL TURE-成功；FALSE－失败
	//	remark: 
	//******************************************
	BOOL SetAt(DWORD index,T &newElement)
	{
		FARRAYNODE      *pDest = NULL;
		if(index<0||index>m_nCount-1)
			return FALSE;
		pDest=GetDestSegEntry(index);
		*(pDest->pData+index-m_nCurIndex) = newElement;
		return TRUE;
	}

	//******************************************
	//	name:InsertAt
	//	function:在数组指定位置插入一个新数据
	//	input:　int index-指定索引号
	//			T newElement-待插入的数据
	//  return: BOOL TURE-成功；FALSE－失败
	//	remark: 
	//******************************************
	int InsertAt(DWORD index,T newElement)
	{
		if(index > m_nCount)
			return -1;
		if(index == m_nCount) 
			return Add(newElement);

		FARRAYNODE *pDest=GetDestSegEntry(index);
		//首先找到目标单元所在的节点
	
		//如果目标节点已经用完那么只需在后面插入一个新的节点，
		//然后把index位置及其后面的单元值后放入新插入节点，新值赋值给index处单元
		
		//如果目标节点没有用完那么只需把index及其后面的单元后移新值赋值给index处单元
		if(pDest->dwUsed == m_nGrowBy)
		{			
			FARRAYNODE *pAddNode =NewNode();
			if(!pAddNode) return -1;
			
			pAddNode->dwUsed = pDest->dwUsed-(WORD)(index-m_nCurIndex);
			memcpy(pAddNode->pData,
				pDest->pData+index-m_nCurIndex,
				sizeof(T)*pAddNode->dwUsed);

			*(pDest->pData+index-m_nCurIndex) = newElement;
			pDest->dwUsed=(WORD)(index-m_nCurIndex+1);

			FARRAYNODE *pNext=pDest->pNextNode;
			pDest->pNextNode=pAddNode;
			pAddNode->pPrevNode=pDest;
			if(pNext)
			{
				pNext->pPrevNode=pAddNode;
				pAddNode->pNextNode=pNext;
			}
			else
			{
				m_pTailNode = pAddNode;			
				pAddNode->pNextNode = NULL;
			}
			m_nEmpty+=m_nGrowBy-1;
		}
		else
		{
			memmove(pDest->pData+index-m_nCurIndex+1,
				    pDest->pData+index-m_nCurIndex,
			 	    (pDest->dwUsed-(index-m_nCurIndex))*sizeof(T));

			*(pDest->pData+index-m_nCurIndex)=newElement;

			pDest->dwUsed++;
			m_nEmpty--;
		}
		m_nCount++;
		if(IsNeedCompack( )) Compack();
		return index;
	}

	//******************************************
	//	name:RemoveAt
	//	function:删除数组中指定索引号中包含的数据
	//	input:　int index-指定索引号
	//  return: BOOL TURE-成功；FALSE－失败
	//	remark: 
	//******************************************
	BOOL RemoveAt( DWORD index )
	{
		if(index<0||index>=m_nCount)
			return FALSE;
		FARRAYNODE *pDest=GetDestSegEntry(index);
		if(pDest==NULL) return true;//add by sunkm 2005/2/20

		m_nCount--;

		if(pDest->dwUsed==1)
		{
			FARRAYNODE *pPrev=pDest->pPrevNode;
			FARRAYNODE *pNext=pDest->pNextNode;
			if( !pPrev ) 
			{
				m_pHeadNode=pNext;
				if(m_pHeadNode) 
					m_pHeadNode->pPrevNode=NULL;
			}
			else
				pPrev->pNextNode=pNext;
			if( !pNext )
			{
				m_pTailNode = pPrev;
				if(m_pTailNode)
					m_pTailNode->pNextNode=NULL;
			}
			else
				pNext->pPrevNode=pPrev;
			
			if(pDest==m_pCurNode)
			{
				if(pPrev)
				{
					m_pCurNode  = pPrev;
					m_nCurIndex = m_nCurIndex-pPrev->dwUsed;
				}
				else
				{
					m_pCurNode  = m_pHeadNode;
					m_nCurIndex = m_pHeadNode?0:-1;
				}
				//孙开敏加于2005 2 21 原来写法（如下）使得当前指针指向头，效率不好
				//改为指向删除点的前一个点
				//m_pCurNode=m_pHeadNode;
				//m_nCurIndex=m_pHeadNode?0:-1;
			}
			CallBack(pDest);
			m_nEmpty-=(m_nGrowBy-1);
		}
		else
		{
			memmove(pDest->pData+index-m_nCurIndex,
				    pDest->pData+index-m_nCurIndex+1,
				    sizeof(T)*(pDest->dwUsed-(index-m_nCurIndex+1)));
			pDest->dwUsed--;
			m_nEmpty++;
			if( IsNeedCompack() ) Compack();
		}
		return TRUE;
	}
	////
	//在压缩后或者改变大小后还有节点内单元dwUsed＝0时会把这些节点回收到一个专门地方
	//可以重复利用，节约系统内存申请释放的开销
	// by sunkm
	////
	int CallBack(FARRAYNODE *begin,FARRAYNODE *end)//为了效率，返回值并不是真正的节点数量
	{
		if(begin==NULL|| end ==NULL)
			return 0;
		if(m_pEmptyHeadNode==NULL)
		{
			m_pEmptyHeadNode = begin;
			m_pEmptyHeadNode->pPrevNode=NULL;
			m_pEmptyEndNode  = end;
			m_pEmptyEndNode->pNextNode=NULL;
			return 1;
		}
		begin->pPrevNode           = m_pEmptyEndNode;
		m_pEmptyEndNode->pNextNode = begin;		
		end->pNextNode             = NULL;
		return 1;
	}
	int CallBack(FARRAYNODE *node)
	{
		if(node==NULL)
			return 0;		
		if(m_pEmptyHeadNode==NULL)
		{
			m_pEmptyHeadNode = node;
			m_pEmptyHeadNode->pPrevNode=NULL;
			m_pEmptyEndNode  = node;
			m_pEmptyEndNode->pNextNode =NULL;
			return 1;
		}
		node->pPrevNode            = m_pEmptyEndNode;
		m_pEmptyEndNode->pNextNode = node;		
		node->pNextNode            = NULL;
		m_pEmptyEndNode            = node;
		return 1;
	}	
	//******************************************
	//	name:SetSize()
	//	function:设置数据的容量
	//	input:　int size -数据的容量
	//  return: BOOL TURE-成功；FALSE－失败
	//	remark:只允许扩大容量
	//******************************************
	BOOL SetSize(DWORD size)
	{
		FARRAYNODE *pNewNode=NULL;
		if(m_nCount>=size)//减小长度
		{
			//当长度减小时，找到size－1对应的节点，然后把后面的节点串begin－end之间的都回收。
			//并且把m_pTailNode指向pDest；而且隐含了m_pCurNode和m_nCurIndex指向pDest节点
			FARRAYNODE *pDest = GetDestSegEntry( size - 1 );
			FARRAYNODE *begin = pDest->pNextNode;
			FARRAYNODE *end   = m_pTailNode;

			pDest->dwUsed = size - m_nCurIndex;
			m_pTailNode   = pDest;		
			pDest ->pNextNode = NULL;	
			CallBack(begin,end);
			return FALSE;
		}
		else//增加长度
		{
			//因为原来最后一个节点中也许还有剩余单元可以利用，因此			
			//计算需要增加几个节点
			if(m_pTailNode)
			{
				if(m_nGrowBy - m_pTailNode->dwUsed >= size - m_nCount)
				{//如果最后一个节点中剩余单元个数大于新增单元个数，只需修改参数不需真正增加节点					
					m_pTailNode->dwUsed+=(WORD)(size-m_nCount);//把新使用的单元个数计数进来
					m_nEmpty-=(size-m_nCount);
					m_nCount=size;
					return TRUE;
				}
				else
				{//否则把后面的剩余部分利用起来				
					m_nEmpty+=m_nGrowBy-m_pTailNode->dwUsed;
					m_nCount+=m_nGrowBy-m_pTailNode->dwUsed;
					m_pTailNode->dwUsed=m_nGrowBy;
				}
			}
			//运行到这里的时候要么最后一个节点已经使用完，要么原来数组为空
			int sumOfNewNode = (size-m_nCount+m_nGrowBy-1)/m_nGrowBy;
			int start = 0;
			if(m_pTailNode==NULL)
			{
				FARRAYNODE *pAddNode   =NewNode();	
				if(!pAddNode) return FALSE;
				//向内存池申请新节点
				pAddNode->dwUsed       = m_nGrowBy;
				pAddNode->pPrevNode    = NULL;
				pAddNode->pNextNode    = NULL;
				m_pCurNode = m_pTailNode = m_pHeadNode = pAddNode;
				m_nCurIndex=0;			
				start=1;
			}			
			for(int i = start ; i < sumOfNewNode ; i++)
			{
				FARRAYNODE *pAddNode   =NewNode();		
				if(!pAddNode) return FALSE;
				//向内存池申请新节点
				pAddNode->dwUsed       = m_nGrowBy;//这个
				//新插入的节点中各单元认为是已经被使用的
				//此处要注意，新插入的节点中已经保存了上次使用的值
				m_pTailNode->pNextNode = pAddNode;
				pAddNode->pPrevNode    = m_pTailNode;
				pAddNode->pNextNode = NULL;
				m_pTailNode =pAddNode;
			}
			int left=( m_nGrowBy * sumOfNewNode - (size-m_nCount));//?
			m_pTailNode->dwUsed= m_nGrowBy-left;
			//最后一个节点不一定都用完因此要按实际长度来设置
			m_nEmpty=m_nEmpty+left;
			//因此最后一个节点中剩余的单元个数也要记入m_nEmpty			
			m_nCount = size;			
		}//孙开敏修改于2005 2 21 
		
		/*
		if(m_pTailNode)
		{
			if((WORD)(size-m_nCount)<=m_nGrowBy-m_pTailNode->dwUsed)
			{//neet not to enlarge the buffer
				m_pTailNode->dwUsed+=(WORD)(size-m_nCount);
				m_nEmpty-=size-m_nCount;
				m_nCount=size;
				return TRUE;
			}else if(m_pTailNode->dwUsed!=m_nGrowBy)
			{//fill the tail node to full
				m_nEmpty+=m_nGrowBy-m_pTailNode->dwUsed;
				m_nCount+=m_nGrowBy-m_pTailNode->dwUsed;
				m_pTailNode->dwUsed=m_nGrowBy;
			}
		}
		int newsegs=(size-m_nCount+m_nGrowBy-1)/m_nGrowBy;
		for(int i=0;i<newsegs;i++)
		{
			pNewNode=NewNode();
			if(!pNewNode) return FALSE;
			pNewNode->dwUsed=(i<newsegs-1)?m_nGrowBy:(WORD)(size-m_nCount);
			if(!m_pHeadNode)
			{
				m_pHeadNode=m_pTailNode=m_pCurNode=pNewNode;
				m_nCurIndex=0;
			}
			ASSERT(m_pTailNode);
			m_pTailNode->pNextNode=pNewNode;
			pNewNode->pPrevNode=m_pTailNode;
			m_pTailNode=pNewNode;
			m_nCount+=pNewNode->dwUsed;
		}*/
		return TRUE;
	}
	
	//******************************************
	//	name:SetGrowBy()
	//	function:设置数据增长幅度
	//	input:　
	//  return: 
	//	remark: 在初始化时使用
	//******************************************
	void SetGrowBy(WORD nGrowBy)
	{
		ASSERT(m_nCount==0);
		m_nGrowBy=nGrowBy;
	}

	//******************************************
	//	name:RemoveAll()
	//	function:清空对象中的数据
	//	input:　
	//  return: BOOL TURE-成功；FALSE－失败
	//	remark:
	//******************************************
	BOOL RemoveAll()
	{
		if(m_pHeadNode)
			Free();	
		if(m_pEmptyHeadNode)
			FreePool();
		return TRUE;
	}

	//*********************************************
	//  name:Compack
	//	function :压缩数据
	//	remark:将链表中各结点段的空余空间进行整理，释放多余的节点
	//*********************************************
	void Compack()
	{
		FARRAYNODE *pTmp1,*pTmp2=NULL;
		//pTemp1为里面有空余单元的节点
		//pTemp2为里面有已经在使用单元的节点，pTemp2中已经在使用的单元可以用来填充pTemp1中的空位
		if(m_pHeadNode==m_pTailNode) return;
		pTmp1=FindNotFullNode(m_pHeadNode,m_pTailNode);
		if(pTmp1)//while(pTmp1) sunkm changed 2005 2 21
		{
			if(pTmp2==NULL)	
				pTmp2=pTmp1->pNextNode;
			ASSERT(pTmp2);
			while(pTmp1->dwUsed!=(DWORD)m_nGrowBy&&pTmp2)
			{
				if(pTmp2->dwUsed <= (m_nGrowBy-pTmp1->dwUsed))
				{
					//当pTemp2中已用单元不够或者恰好用来填充pTemp1中的空位时，pTemp2可以回收
					//the node can free
					memcpy(pTmp1->pData+pTmp1->dwUsed,
						pTmp2->pData,
						sizeof(T)*pTmp2->dwUsed);
					pTmp1->dwUsed+=pTmp2->dwUsed;
					if(m_pTailNode==pTmp2)
					{//如果pTemp2是末尾节点，则回收后可以结束
						m_pTailNode=pTmp2->pPrevNode;
						m_pTailNode->pNextNode=NULL;
						CallBack(pTmp2);
						pTmp2=NULL;
						break ;//孙开敏加于2005 2 21 
						        //因为pTemp2=NULL说明pTemp1中的空余空间已经找不到单元来填充，压缩结束
					}
					else
					{
						//当pTemp2中的元素全部用来填充pTemp1中的空位还没有填满pTemp1时pTemp2就指向下一个节点
						//一般来说pTemp2应该始终是pTemp1的下一个节点
						pTmp2->pPrevNode->pNextNode = pTmp2->pNextNode;
						pTmp2->pNextNode->pPrevNode = pTmp2->pPrevNode;
						FARRAYNODE *p=pTmp2->pNextNode;
						CallBack(pTmp2);
						pTmp2=p;

						if(pTmp1->dwUsed==m_nGrowBy)//孙开敏加于2005 2 21 原来算法有问题
						{
							pTmp1=FindNotFullNode(pTmp1,m_pTailNode);
							if(pTmp1)
								pTmp2=pTmp1->pNextNode;
							else
								break ;//如果找不到有空位的节点说明结束
							//如果pTemp1中的空位正好被填充完，则需要从后面的节点中继续找需要填充的
						}
					}
				}
				else
				{
					//当pTemp2中已用单元够用来填充pTemp1中的空位而且有剩余时，
					//pTemp2变为下一个要填充的节点，因此pTemp2变为pTemp1，pTemp2为下一个节点
					DWORD dwMoveItems=m_nGrowBy-pTmp1->dwUsed;
					memcpy(pTmp1->pData+pTmp1->dwUsed,
						pTmp2->pData,
						sizeof(T)*dwMoveItems);
					memmove(pTmp2->pData,
						pTmp2->pData+dwMoveItems,
						sizeof(T)*(pTmp2->dwUsed-dwMoveItems));
					pTmp2->dwUsed-=(WORD)dwMoveItems;
					pTmp1->dwUsed=m_nGrowBy;

					//孙开敏加于2005 2 21
					pTmp1 = pTmp2;
					pTmp2 = pTmp1->pNextNode;
					//pTemp2变为下一个要填充的节点，因此pTemp2变为pTemp1，pTemp2为下一个节点					
				}				
			}//while			
			/*
			FARRAYNODE *pFind=FindNotFullNode(pTmp1,pTmp2);
			if(!pFind)
			{
				pTmp1=FindNotFullNode(pTmp1,m_pTailNode);
			}else
			{
				pTmp1=pFind;
			}
			pTmp2=NULL;
			*///孙开敏删除于2005 2 21
		}//while		
		m_pCurNode=m_pHeadNode;//avoid the current node been deleted
		m_nCurIndex=0;
		if(m_pTailNode)
			m_nEmpty=m_nGrowBy-m_pTailNode->dwUsed;
		else
			m_nEmpty=0;
	}

protected:
	bool m_bNeedCompack;


	void NeedCompack(bool bNeed)
	{
		m_bNeedCompack = bNeed;
	}
	//********************************************
	//	name:IsNeedCompack
	//	function:判断是否需要对数据进行压缩
	//*********************************************
	virtual BOOL IsNeedCompack()
	{
		return m_bNeedCompack;//孙开敏加于2005 2 21 压缩与否由使用者自行设置参数控制
		if(m_nEmpty<m_nGrowBy) return FALSE;
		return ((m_nEmpty-m_nGrowBy)*100>m_nCount*m_byZipKey);
	}
	//******************************************
	//	name:Free()
	//	function:释放对象占用的空间，
	//	input:
	//  return:void
	//	remark:内部使用,外部要清空对象请使用RemoveAll()接口
	//******************************************
public:
	virtual void Free()
	{
		FARRAYNODE *temp1,*temp2;
		temp1=m_pHeadNode;
		while(temp1!=NULL)
		{
			temp2=temp1->pNextNode;
			FreeNode(temp1);
			temp1=temp2;
		}
		m_pCurNode=m_pHeadNode=m_pTailNode=NULL;
		m_nCurIndex=-1;
		m_nCount=0;
		m_nEmpty=0;
	}
public:
	//******************************************
	//	name:Free()
	//	function:释放对象回收池占用的空间，
	//	input:
	//  return:void
	//	remark:内部使用,外部要清空对象请使用RemoveAll()接口
	//******************************************
	virtual void FreePool()
	{
		FARRAYNODE *temp1,*temp2;
		temp1=m_pEmptyHeadNode;
		int xx=0;
		while(temp1!=NULL)
		{
			temp2=temp1->pNextNode;
			FreeNode(temp1);
			temp1=temp2;
			xx++;
		}
		m_pEmptyHeadNode=m_pEmptyEndNode=NULL;		
	}
private:

	//************************************************
	//	FreeNode
	//	释放该结点占用的内存
	//************************************************
	void FreeNode(FARRAYNODE *pNode)
	{
		delete []pNode->pData;
		delete pNode;
	}
	//*********************************************
	//	查找有空闲空间的结点
	//*********************************************
	FARRAYNODE *FindNotFullNode(FARRAYNODE *pBegin,FARRAYNODE *pEnd)
	{
		FARRAYNODE *pRet=pBegin;
		if(!pEnd) pEnd=m_pTailNode;
		while(pRet&&pRet->dwUsed==m_nGrowBy&&pRet!=pEnd)
			pRet=pRet->pNextNode;
		if(pRet==m_pTailNode||pRet==pEnd)
			pRet=NULL;
		return pRet;
	}

	//*******************************************
	//	name NewNode
	//	function:为一个新结点分配空间
	//********************************************
	FARRAYNODE *NewNode()
	{
		FARRAYNODE *pRet;
		if(m_pEmptyHeadNode)
		{
			pRet = m_pEmptyHeadNode;
			m_pEmptyHeadNode = m_pEmptyHeadNode->pNextNode;			
			if(m_pEmptyHeadNode == NULL)
				m_pEmptyEndNode =NULL;			
			else
				m_pEmptyHeadNode->pPrevNode=NULL;

		}
		else
		{
			pRet=new FARRAYNODE;	
			if(!pRet) return NULL;
			pRet->pData=new T[m_nGrowBy];
			if(pRet->pData==NULL)
			{
				delete pRet;
				return NULL;
			}
		}		
		pRet->pPrevNode=pRet->pNextNode=NULL;
		pRet->dwUsed=0;		
		return pRet;
	}
	//******************************************
	//	name:GetDestSegEntry()
	//  function:获取数据所在链表的节点指针
	//	input:	int index -数据索引
	//  return: FARRAYNODE * -数据所在链表的节点指针
	//	remark:内部使用，
	//******************************************
	FARRAYNODE * GetDestSegEntry(DWORD index)
	{
		FARRAYNODE * ret=NULL;
		int			i = 0;
		DWORD		offset=0;
		if(index < m_nCurIndex)// dest pData is in before cur pData segment 
		{
			if(m_nCurIndex>2*index)
			{	//find the seg from head;
				ret=m_pHeadNode;
				while(offset+ret->dwUsed<=index)
				{
					offset+=ret->dwUsed;
					ret=ret->pNextNode;
				}
			}
			else	
			{	//find the seg from cur seg;
				ret=m_pCurNode;
				offset=m_nCurIndex;
				while(offset>index)
				{
					ret=ret->pPrevNode;
					offset-=ret->dwUsed;
				}
			}
			m_nCurIndex=offset;
		}
		else if(index >= (m_nCurIndex+m_pCurNode->dwUsed))
		{
			//if(m_nCurIndex+m_nCount<2*index)
			if(m_nCurIndex+m_nCount>2*index)//孙开敏修改于2005 2 21			
			{//find the seg from cur
				ret=m_pCurNode;
				offset=m_nCurIndex;
				//while(offset+ret->dwUsed<=index)
				while(ret&&offset+ret->dwUsed<=index)///changed by sunkm 2005/2/20
				{
					offset+=ret->dwUsed;
					ret=ret->pNextNode;
				}
			}else
			{//find the seg from tail
				ret=m_pTailNode;
				offset=m_nCount-ret->dwUsed;;
				while(offset>index)
				{
					ret=ret->pPrevNode;
					offset-=ret->dwUsed;
				}
			}
			m_nCurIndex=offset;
		}else
		{//in cur pData seg
			ret=m_pCurNode;
		}
		m_pCurNode=ret;
		return ret;
	}

/////////////////////////////////////////////////////////////////////
//  private data
	DWORD m_nCurIndex;	//当前节点的第一个元素的索引号
	DWORD m_nCount;		//数组对象包括的数据数
	WORD  m_nGrowBy;		//每次增长的尺寸
	DWORD m_nEmpty;		//空出的空间数量

	BYTE m_byZipKey;	//压缩阀值
	FARRAYNODE * m_pCurNode;	//链表中当前节点的指针，在数据检索时确定从向个方向搜索
	FARRAYNODE * m_pHeadNode;	//头节点
	FARRAYNODE * m_pTailNode;	//尾结点
    
	FARRAYNODE * m_pEmptyHeadNode;	//头节点
	FARRAYNODE * m_pEmptyEndNode;	//尾结点


};

#endif // !defined(AFX_SARRAY_H__B1D40C22_2698_4202_921E_36D447EA4199__INCLUDED_)
