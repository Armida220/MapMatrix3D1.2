#pragma once

//#include"CGFL.h"
//#include "vector3d.h"
#include "GeoCurve.h"
#include <vector>

/*
从离散点形式导入的矩形格网追踪等高线
2005/11/7 by ChengXiaoQing 
*/


namespace geom
{
	class LineString;
}

class GridContour
{
public:
	GridContour();
	virtual ~GridContour();
	int   Init(double* x, double *y, double *z, int iNum, float intervalx, float intervaly, float cntinterval);

	void  LocalTrack(int row, int col, int r, int c, float x0, float y0, float * h, double dem_dx, double dem_dy,
		int in, unsigned char *shw, unsigned char * svw, float iz, int flip);
	CGeoCurve*  GetLine(int iNum);
	int   GetContourNumber();
	int		m_iIndexInterval;
	bool m_bLasMode;
private:
	std::vector<CGeoCurve*> contours;
	int m_iLineNum = 0;
	void   TraceContours(float * Grid, unsigned char *HFlags, unsigned char *VFlags, int Row, int Col, double h, int Grid_DX, int Grid_DY, int * i0, int * j0, int * end, int * np, float * x, float *y);
	void   SetFlags(float * Grid, unsigned char *HFlags, unsigned char *VFlags, short Row, short Col, double Height);
	void    SearchOneLine(float iz, float * h, unsigned char * sh, unsigned char * sv, int row, int col);
	void   PrimaryTrack(float iz, float * h, unsigned char * sh, unsigned char * sv, int row, int col, double * pdem);
	int    MarkPass(float iz, float * h, int nx, int ny, double dem_dx, double dem_dy, int * in, unsigned char * shw, unsigned char * svw, int * row, int * col, float * x0, float * y0, float * x, float * y, int * np, int * end_search);
	int    LineTrace(int pas, int * in, float * h, float * x, float * y, int * np, unsigned char * shw, unsigned char * svw, int * row, int * col, float z1, float z2, float z3, float z4, double dem_dx, double dem_dy, int nx, int ny, float * x0, float * y0);
	void   write_cnt(int iNum, float * x, float *y, float z);
	void  Flip(int np);

	double m_lfBaseX;
	double m_lfBaseY;
	float m_lfInterval;

	// 给PrimaryTrack函数使用的缓冲
	float	*scnt_x;
	float	*scnt_y;
};

