#ifndef E00IO_H
#define E00IO_H
#include <vector>
using namespace std;

enum E00FieldType
{
	E00String = 0,
	E00Integer,
	E00Double
};

typedef struct _point {
	int id;
	double x, y;
	void *ptr; //记录额外与此对象关联的内存块
} shapePointObject;

typedef struct _arc {
	int id;
	int elements;
	double *x, *y;
	bool isPartofPolygon;
	void *ptr; //记录额外与此对象关联的内存块
} shapeArcObject;

typedef struct _polygon {
	int elements;
	double xmin,xmax,ymin,ymax;
	int *arcs;
	void *ptr; //记录额外与此对象关联的内存块
} shapePolygonObject;

typedef struct _text {
	char text[128];
	double height;
	double x,y;
	void *ptr; //记录额外与此对象关联的内存块
} shapeTextObject;

typedef struct _shape {
	shapePointObject *points;
	shapeArcObject *arcs;
	shapePolygonObject *polygons;
	shapeTextObject * texts;
	int pointsCount, arcsCount, polygonsCount, textsCount;
	char attributeName[50];
} strArcInfo;

struct E00FieldInfo
{
	char		fieldName[64];				//字段名称
	E00FieldType	fieldType;				//字段类型
};

typedef vector<E00FieldInfo> E00FieldInfoArray;

/************************************************************************/
/* mode==0->只读
	mode==1->写                                                                     */
/************************************************************************/
FILE * openE00File(const char *fileName,int mode);
void closeE00File(FILE *archivo);
void readARCSection (FILE *, strArcInfo *, int precision);
void readCNTSection (FILE *, strArcInfo *);
void readLABSection (FILE *, strArcInfo *, int precision);
void readLOGSection (FILE *, strArcInfo *);
void readPALSection (FILE *, strArcInfo *, int precision);
void readPRJSection (FILE *, strArcInfo *);
void readSINSection (FILE *, strArcInfo *);
void readTOLSection (FILE *, strArcInfo *);
void readTXTSection (FILE *, strArcInfo *, int precision);
void readTX6Section (FILE *, strArcInfo *, int precision);
void readTX7Section (FILE *, strArcInfo *, int precision);
void readRXPSection (FILE *, strArcInfo *);
void readRPLSection (FILE *, strArcInfo *);
void readIFOSection (FILE *, strArcInfo *);
/************************************************************************/
/* flag==0->点的属性
flag==1->线的属性
flag==2->面的属性
flag==3->注记的属性                                                     */
/************************************************************************/
void readFieldsInfo(FILE *,E00FieldInfoArray &arry, vector<int> &widths, int precision, int flag);
int readFieldValueAsInteger(FILE *archivo,int width);
double readFieldValueAsDouble(FILE *archivo,int width);
string readFieldValueAsString(FILE *archivo,int width);
/************************************************************************/
/* flag==0->只写入不属于面的弧段
flag==1->只写入属于面的弧段
flag==2->写入所有弧段                                                   */
/************************************************************************/
void writeARCSection (FILE *archivo, strArcInfo *arcinfo, int precision,int flag);
void writeCNTSection (FILE *, strArcInfo *);
void writeLABSection (FILE *, strArcInfo *, int precision);
void writeLOGSection (FILE *, strArcInfo *);
void writePALSection (FILE *, strArcInfo *, int precision);
void writePRJSection (FILE *, strArcInfo *);
void writeSINSection (FILE *, strArcInfo *);
void writeTOLSection (FILE *, strArcInfo *);
void writeTXTSection (FILE *, strArcInfo *, int precision);
void writeTX6Section (FILE *, strArcInfo *, int precision);
void writeTX7Section (FILE *, strArcInfo *);
void writeRXPSection (FILE *, strArcInfo *);
void writeRPLSection (FILE *, strArcInfo *);

/************************************************************************/
/* flag==0->写点的属性
flag==1->写线的属性
flag==2->写面的属性
flag==3->写注记的属性   以下属性操作函数是这样的                                                                  */
/************************************************************************/
void beginWriteIFOSection(FILE *, strArcInfo *, int precision);

void writeAATIFOField(FILE *, const E00FieldInfoArray *fieldArray, strArcInfo *, int precision, int flag);
void writeBNDIFOField(FILE *,  const E00FieldInfoArray *fieldArray, strArcInfo *, int precision, int flag);//外包
void writePATIFOField(FILE *,  const E00FieldInfoArray *fieldArray, strArcInfo *, int precision, int flag);//点或面
void writeTATTEXTField(FILE *,  const E00FieldInfoArray *fieldArray, strArcInfo *, int precision, int flag);//注记

void sprintfIntegerBlank(string &str);
void sprintfDoubleBlank(string &str,int precision);
void sprintfIntegerValue(string &str, int value);
void sprintfDoubleValue(string &str, double value, int precision);//外包
void sprintfStringValue(string &str, char value[128]);//点或面
void writeIFOFieldValue(FILE *,string &str);

void endWriteIFOSection(FILE *, strArcInfo *, int precision, int flag);

void readIFORecords (FILE *, char *, int, int);

char *readString (char *, char *, int , int);
void readDoublePrecision (FILE *, int, double *, double *);
void writeDoublePrecision (FILE *archivo, int number, double *x, double *y);
void readSinglePrecision (FILE *, int, double *, double *);
void writeSinglePrecision (FILE *, int, double *, double *);
shapeArcObject findARC(strArcInfo *, int id);

#endif
