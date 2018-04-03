#ifndef _SMARTVIEWBASETYPE_HEAD_
#define _SMARTVIEWBASETYPE_HEAD_


MyNameSpaceBegin

struct PT_2D 
{
	PT_2D(){ x=0; y=0; }
	PT_2D(double a, double b){ x=a; y=b; }
	double x;
	double y;
};

struct PT_3D : public PT_2D
{
	PT_3D(){ x=0; y=0; z=0; }
	PT_3D(double a, double b, double c){ x=a; y=b; z=c; }
	double z;
};

struct PT_4D : public PT_3D
{
	PT_4D(){ x=0; y=0; z=0; yr=0; }
	PT_4D(double a, double b, double c){ x=a; y=b; z=c; yr=b; }
	PT_4D(double a, double b, double c, double d){ x=a; y=b; z=c; yr=d; }
	PT_4D(const PT_3D& pt){ x=pt.x; y=pt.y; z=pt.z; yr=pt.y; }
	PT_3D To3D()
	{
		return PT_3D(x,y,z);
	}
	PT_2D ToL2D()
	{
		return PT_2D(x,y);
	}
	PT_2D ToR2D()
	{
		return PT_2D(z,yr);
	}
	double yr;
};

#define TAH_LEFT                      0x01
#define TAH_MID						  0x02
#define TAH_RIGHT                     0x04
#define TAH_CLEAR					  0xfffffff8

#define TAV_TOP                       0x08
#define TAV_MID						  0x10
#define TAV_BOTTOM                    0x20
#define TAV_CLEAR					  0xffffffc7

#define   SHRUGN	0				//���ʼ�
#define   SHRUGL	1
#define   SHRUGR	2 
#define   SHRUGU	3
#define   SHRUGD	4      



enum placeType
{
	singPt = 0,				//����
	mutiPt = 1,				//���
	byLineH = 2,			//ˮƽ
	byLineV = 3,			//��ֱ
	byLineGridH = 4,		//����ˮƽ
	byLineGridV = 5			//���ȴ�ֱ
};


struct EXPORT_SMARTVIEW TEXT_SETTINGS0
{
	TEXT_SETTINGS0();
	float	fHeight;				//�ַ��߶�
	double	fWidScale;				//�ַ������ƽ����ȵı�ֵ
	double	fCharIntervalScale;	//�ַ������ƽ����ȵı�ֵ
	double	fLineSpacingScale;		//�м�౶�� 
	double	fTextAngle;			//�ı�ˮƽ������x��ļнǣ���ʱ�룩{ע���Ƕȵ�λΪ����}
	double	fCharAngle;			//�����ַ�����ת�Ƕȣ���ʱ�룩һ��Ϊ90�ȣ�ʵ�������ߴ�ֱ�ֲ��ı�{ע���Ƕȵ�λΪ����}
	int		nPlaceType;				//�ַ����з�ʽ�����硰����Ԫ�ء�������ԭ�㡱��
	int		nAlignment;				//���뷽ʽ9��
	int     nInclineType;			//��б��ʽ5��
	double	fInclineAngle;			//��б�Ƕ�{ע���Ƕȵ�λΪ����}
	int		nOtherFlag;				//�Ӵ֣��»��ߵȱ�־
	TCHAR strFontName[LF_FACESIZE];			//��������

	BOOL	IsBold()const;
	BOOL	IsUnderline()const;
	void	SetBold(BOOL bValue);
	void	SetUnderline(BOOL bValue);
};



struct EXPORT_SMARTVIEW tagTextSettings
{
	tagTextSettings();

	void CopyFrom(const TEXT_SETTINGS0 *setting)
	{
		fHeight = setting->fHeight;
		fWidScale = setting->fWidScale;
		fCharIntervalScale = setting->fCharIntervalScale;
		fLineSpacingScale = setting->fLineSpacingScale;
		fTextAngle = setting->fTextAngle;
		fCharAngle = setting->fCharAngle;
		nAlignment = setting->nAlignment;
		nInclineType = setting->nInclineType;
		fInclineAngle = setting->fInclineAngle;
		nOtherFlag = setting->nOtherFlag;
		_tcscpy(tcFaceName,setting->strFontName);
	}

	void CopyTo(TEXT_SETTINGS0 *setting)const
	{
		setting->fHeight = fHeight;
		setting->fWidScale = fWidScale;
		setting->fCharIntervalScale = fCharIntervalScale;
		setting->fLineSpacingScale = fLineSpacingScale;
		setting->fTextAngle = fTextAngle;
		setting->fCharAngle = fCharAngle;
		setting->nAlignment = nAlignment;
		setting->nInclineType = nInclineType;
		setting->fInclineAngle = fInclineAngle;
		setting->nOtherFlag = nOtherFlag;
		_tcscpy(setting->strFontName,tcFaceName);
	}

	BOOL IsStandardFont()const
	{
		return (fWidScale==1.0f && fInclineAngle==0.0f && fCharAngle==0.0f && fTextAngle==0.0f && fCharIntervalScale==0.0f );
	}

	BOOL	IsBold()const;
	BOOL	IsUnderline()const;
	void	SetBold(BOOL bValue);
	void	SetUnderline(BOOL bValue);

	float	fHeight;				//�ַ��߶�
	float	fWidScale;				//�ַ������ƽ����ȵı�ֵ
	float	fCharIntervalScale;		//�ַ������ƽ����ȵı�ֵ
	float	fLineSpacingScale;		//�м�౶�� 
	float	fTextAngle;				//�ı�ˮƽ������x��ļнǣ���ʱ�룩{ע���Ƕȵ�λΪ����}
	float	fCharAngle;				//�����ַ�����ת�Ƕȣ���ʱ�룩,�����϶���{ע���Ƕȵ�λΪ����}
	int		nAlignment;				//���뷽ʽ9��
	int     nInclineType;			//��б��ʽ5��
	float	fInclineAngle;			//��б�Ƕ�{ע���Ƕȵ�λΪ����}
	int		nOtherFlag;				//�Ӵ֣��»��ߵȱ�־
	TCHAR	tcFaceName[LF_FACESIZE];//��������
};

typedef struct tagTextSettings TextSettings;

enum penCODE	
{
	penNone		= 0,
	penMove     = 1,
	penLine		= 2,
	penStream 	= 3,
	penArc  	= 4,
	penSpline 	= 5,
	pen3PArc    = 6
}; 

enum ptType
{
	ptNone      = 0,
	ptSpecial   = 1  // ������
};


//���Ƶ������: �գ���ȵ㣬����㣬������
enum ctrlPointType
{
	ctrlType_None = 0,
	ctrlType_Width = 1,
	ctrlType_Dir = 2,
	ctrlType_Scale = 3
};


struct PT_3DEX : public PT_3D
{
	PT_3DEX(){ x=0; y=0; z=0; pencode=penNone; wid=0; type=ptNone;}
	PT_3DEX(double lfx, double lfy, double lfz, int pen, float w=0, short tp=ptNone){ x=lfx; y=lfy; z=lfz; pencode=pen; wid=w; type=tp;}
	PT_3DEX(const PT_3D& pt, int pen, float w=0, short tp=ptNone){ x=pt.x; y=pt.y; z=pt.z; pencode=pen; wid=w; type=tp;}
	short pencode;
	short type;
	float wid;
};

struct LINE_2D 
{
	PT_2D sp;
	PT_2D ep;
};

struct LINE_3D 
{
	PT_3D sp;
	PT_3D ep;
};

struct INDEX
{
	long h[4];
	INDEX(long h0=-1, long h1=-1, long h2=-1, long h3=-1){
		h[0] = h0; h[1] = h1; h[2] = h2; h[3] = h3;
	}
	BOOL IsNULL(){
		return (h[0]==-1&&h[1]==-1&&h[2]==-1&&h[3]==-1);
	}
	BOOL operator==(const INDEX& other){
		return (h[0]==other.h[0]&&h[1]==other.h[1]&&
			h[2]==other.h[2]&&h[3]==other.h[3]);
	}
};

//����Ķ������� 
struct GrVertexAttr
{
	DWORD color:24;
	DWORD isMarked:1;
};

//����������б�
struct EXPORT_SMARTVIEW GrVertexAttrList
{
	GrVertexAttrList(){
		pts=0; nlen=nuse=0;
	}
	//ȥ������ռ�
	void CutSpace();
	void CopyFrom(const GrVertexAttrList *list);

	//�����б�
	GrVertexAttr *pts;

	//�����ڴ泤�Ⱥ͵�ǰʹ����
	DWORD nlen:16;
	DWORD nuse:15;
};


MyNameSpaceEnd


#define COPY_2DPT(pt1,pt2)		(pt1).x=(pt2).x, (pt1).y=(pt2).y
#define COPY_3DPT(pt1,pt2)		(pt1).x=(pt2).x, (pt1).y=(pt2).y, (pt1).z=(pt2).z
#define DIST_2DPT(pt1,pt2)		sqrt(((pt1).x-(pt2).x)*((pt1).x-(pt2).x)+((pt1).y-(pt2).y)*((pt1).y-(pt2).y))
#define DIST_3DPT(pt1,pt2)		sqrt(((pt1).x-(pt2).x)*((pt1).x-(pt2).x)+((pt1).y-(pt2).y)*((pt1).y-(pt2).y)+((pt1).z-(pt2).z)*((pt1).z-(pt2).z))


#define CONVERT_XYZ(xs,ys,zs,m,xd,yd,zd) \
	xd = xs*m[0] + ys*m[1] + zs*m[2] + m[3];\
	yd = xs*m[4] + ys*m[5] + zs*m[6] + m[7];\
zd = xs*m[8] + ys*m[9] + zs*m[10] + m[11];

#define CONVERT_PT2D(s,m,d) \
	d.x = s.x*m[0] + s.y*m[1] + m[2];\
	d.y = s.x*m[3] + s.y*m[4] + m[5];

#define CONVERT_PT3D(s,m,d) \
	d.x = s.x*m[0] + s.y*m[1] + s.z*m[2] + m[3];\
	d.y = s.x*m[4] + s.y*m[5] + s.z*m[6] + m[7];\
d.z = s.x*m[8] + s.y*m[9] + s.z*m[10] + m[11];


#define INDEX_0(pidx)		((pidx)->h[0])
#define INDEX_1(pidx)		((pidx)->h[1])
#define INDEX_2(pidx)		((pidx)->h[2])
#define INDEX_3(pidx)		((pidx)->h[3])

#define NULL_HANDLE					0
#define IsNULLObjHandle(h)			((h)==NULL_HANDLE)

#define VERTEX_DEPTH				-1.0f
#define TEXTURE_DEPTH				-0.9f

#endif