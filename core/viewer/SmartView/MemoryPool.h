#ifndef MEMORYPOOL_HEAD_20090226
#define MEMORYPOOL_HEAD_20090226

#ifdef new
#undef new
#define _REDEF_NEW
#endif

#ifndef _INC_NEW
#include <new.h>
#endif

#define printf(a) ;

template<class TYPE>
void MyConstructElements(TYPE* pElements, int nCount)
{	
	// first do bit-wise zero initialization
	memset((void*)pElements, 0, nCount * sizeof(TYPE));
	
	// then call the constructor(s)
	for (; nCount--; pElements++)
		::new((void*)pElements) TYPE;
}

template<class TYPE>
void MyDestructElements(TYPE* pElements, int nCount)
{	
	// call the destructor(s)
	for (; nCount--; pElements++)
		pElements->~TYPE();
}


template<class TYPE>
class CMemoryPool;


template<class TYPE>
class CSmallMemoryPool
{
	friend CMemoryPool<TYPE>;
protected:
	struct MyNode
	{
		TYPE data;
		MyNode* pNext;
		CSmallMemoryPool *pPool;
	};
	
	struct MyPlex     // warning variable length structure
	{
		MyPlex* pNext;

#if (_AFX_PACKING >= 8)
		DWORD dwReserved[1];    // align on 8 byte boundary
#endif
		// BYTE data[maxNum*elementSize];
		
		void* data() { return this+1; }
		
		static MyPlex* PASCAL Create(MyPlex*& pHead, UINT nMax, UINT cbElement)
		{
			MyPlex* p = (MyPlex*) new BYTE[sizeof(MyPlex) + nMax * cbElement];
			// may throw exception
			p->pNext = pHead;
			pHead = p;  // change head (adds in reverse order for simplicity)
			return p;
		}
		
		void FreeDataChain()       // free this one and links
		{
			MyPlex* p = this;
			while (p != NULL)
			{
				BYTE* bytes = (BYTE*) p;
				MyPlex* pNext = p->pNext;
				delete[] bytes;
				p = pNext;
			}
		}
	};
	
public:
	// Construction
	CSmallMemoryPool(int nBlockSize = 1024, int nMaxBlockNum = 10)
	{
		MyNode test;
		m_nPosOffset = ((long)&test)-((long)&test.data);

		m_nCount = 0;
		m_pNodeHead = m_pNodeTail = m_pNodeFree = NULL;
		m_pBlocks = NULL;
		m_nBlockSize = nBlockSize;
		m_nMaxBlockNum = nMaxBlockNum;
	}
	~CSmallMemoryPool()
	{
		// destroy elements
		MyNode* pNode;
		//for (pNode = m_pNodeHead; pNode != NULL; pNode = pNode->pNext)
		//	MyDestructElements<TYPE>(&pNode->data, 1);
		
		m_nCount = 0;
		m_pNodeHead = m_pNodeTail = m_pNodeFree = NULL;
		m_pBlocks->FreeDataChain();
		m_pBlocks = NULL;
	}

	TYPE *Alloc()
	{
		if (m_pNodeFree == NULL)
		{
			if( m_nCount>=m_nBlockSize*m_nMaxBlockNum )
				return NULL;
			
			// add another block
			MyPlex* pNewBlock = MyPlex::Create(m_pBlocks, m_nBlockSize,
				sizeof(MyNode));
			
			// chain them into free list
			MyNode* pNode = (MyNode*) pNewBlock->data();
			// free in reverse order to make it easier to debug
			pNode += m_nBlockSize - 1;
			for (int i = m_nBlockSize-1; i >= 0; i--, pNode--)
			{
				pNode->pNext = m_pNodeFree;
				m_pNodeFree = pNode;
			}
		}
		
		MyNode* pNode = m_pNodeFree;
		m_pNodeFree = m_pNodeFree->pNext;
		m_nCount++;
		
		pNode->pPool = this;
		//MyConstructElements<TYPE>(&pNode->data, 1);
		return &pNode->data;
	}

	void Free(TYPE *p)
	{
		MyNode* pNode = (MyNode*)(m_nPosOffset + (long)p);

		//MyDestructElements<TYPE>(&pNode->data, 1);
		pNode->pNext = m_pNodeFree;
		pNode->pPool = NULL;
		m_pNodeFree = pNode;
		m_nCount--;
	}

	BOOL IsFreeAll()
	{
		return (m_nCount==0);
	}
	
protected:
	int m_nPosOffset;

	MyNode* m_pNodeHead;
	MyNode* m_pNodeTail;
	int m_nCount;
	MyNode* m_pNodeFree;
	struct MyPlex* m_pBlocks;
	int m_nBlockSize;
	int m_nMaxBlockNum;
};


template<class TYPE>
class CMemoryPool
{
public:
	// Construction
	CMemoryPool()
	{
		CSmallMemoryPool<TYPE>::MyNode test;
		m_nPosOffset = ((long)&test)-((long)&test.data);

		m_pBuf = NULL;
		m_nLen = 0;
		m_nUsed = 0;
		m_nCur = 0;
	}
	~CMemoryPool()
	{
		if( m_pBuf!=NULL )
		{
			for( int i=0; i<m_nUsed; i++)
				delete m_pBuf[i];
			delete[] m_pBuf;
		}
	}

	TYPE *Alloc()
	{
		TYPE *p = NULL;

		if( m_nCur>=0 && m_nCur<m_nUsed )
		{
			TYPE *p = m_pBuf[m_nCur]->Alloc();
			if( p!=NULL )
				return p;
		}

		for( int i=1; i<m_nUsed; i++)
		{
			p = m_pBuf[i]->Alloc();
			if( p!=NULL )
			{
				m_nCur = i;
				return p;
			}
		}

		if( !ReallocBuf() )
			return NULL;

		CSmallMemoryPool<TYPE> *pPool = new CSmallMemoryPool<TYPE>;
		if( pPool!=NULL )
		{
			p = pPool->Alloc();

			m_pBuf[m_nUsed] = pPool;
			m_nCur = m_nUsed;
			m_nUsed++;
		}

		return p;
	}

	void Free(TYPE *p)
	{
		CSmallMemoryPool<TYPE>::MyNode* pNode = (CSmallMemoryPool<TYPE>::MyNode*)(m_nPosOffset + (long)p);
		CSmallMemoryPool<TYPE> *pPool = pNode->pPool;
		pPool->Free(p);

		if( pPool->IsFreeAll() )
		{
			delete pPool;
			for( int i=0; i<m_nUsed; i++)
			{
				if( pPool==m_pBuf[i] )
				{
					if( (m_nUsed-1)>i )
					{
						memmove(m_pBuf+i,m_pBuf+i+1,(m_nUsed-1-i)*sizeof(CSmallMemoryPool<TYPE>*));
					}
					break;
				}
			}
			m_nUsed--;
		}
	}
protected:
	BOOL ReallocBuf()
	{
		if( m_nUsed>=m_nLen )
		{
			CSmallMemoryPool<TYPE> **pNew = ::new CSmallMemoryPool<TYPE>*[m_nLen+256];
			if( pNew!=NULL )
			{
				memcpy(pNew,m_pBuf,sizeof(CSmallMemoryPool<TYPE>*)*m_nUsed);
				delete[] m_pBuf;
				m_pBuf = pNew;

				m_nLen += 256;
				return TRUE;
			}
		}
		else
			return TRUE;

		return FALSE;
	}
	
protected:
	CSmallMemoryPool<TYPE> **m_pBuf;
	int m_nLen;
	int m_nUsed;
	int m_nCur;

	int m_nPosOffset;
};

#ifdef USE_MEMPOOL 

#define DECLARE_MEMPOOL(classname) static void *operator new(size_t size); static void operator delete(void *p);
#define IMPLEMENT_MEMPOOL(classname) CMemoryPool<classname> g##classname##Pool;\
void *classname::operator new(size_t size)\
{\
	return g##classname##Pool.Alloc();\
}\
void classname::operator delete(void *p)\
{\
	g##classname##Pool.Free((classname*)p);\
}

#else 

#define DECLARE_MEMPOOL(classname) 
#define IMPLEMENT_MEMPOOL(classname) 

#endif 

#endif