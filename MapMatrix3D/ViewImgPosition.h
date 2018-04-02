// ViewImgPosition.h : main header file for the EDITBASE application
//

#if !defined(AFX_VIEWIMGPOSITION_H__EEEEFFFF_0982_4669_B5C1_D65DAAA065A2__INCLUDED_)
#define AFX_VIEWIMGPOSITION_H__EEEEFFFF_0982_4669_B5C1_D65DAAA065A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Matrix.h"
#include "UndoAction.h"

//影像的TFW参数是以影像左上角作为原点，
//影像在矢量视图中的映射参数是以影像的左下角作为原点

struct ViewImgPosition
{
	enum OriginType
	{
		Corner = 0,
		Center = 1
	};
	enum ParamType
	{
		ImageType = 0,
		MatrixType = 1
	};
	ViewImgPosition()
	{
		nType = ImageType;
		lfImgLayOX = lfImgLayOY = lfImgMatrix[1] = lfImgMatrix[2] = 0;
		lfImgMatrix[0] = lfImgMatrix[3] = 1.0;
		lfPixelSizeX = lfPixelSizeY = 1.0;
		nPixelBase = Corner;
		szImg.cx = szImg.cy = 0;
		is_visible = TRUE;
	}

	void ConvertToImageType()
	{
		if( nType==MatrixType )
		{
			ViewImgPosition item = *this;

			if( nPixelBase==Corner )
			{
				lfImgLayOX = lfDataMatrix[2] + szImg.cy*item.lfDataMatrix[1];
				lfImgLayOY = lfDataMatrix[5] + szImg.cy*item.lfDataMatrix[3];
				
				lfImgMatrix[0] = lfDataMatrix[0];
				lfImgMatrix[1] = lfDataMatrix[1];
				lfImgMatrix[2] = -lfDataMatrix[3];
				lfImgMatrix[3] = -lfDataMatrix[4];
			}
			else
			{
				lfImgLayOX = lfDataMatrix[2] + (0.5)*item.lfDataMatrix[0] + (szImg.cy-0.5)*item.lfDataMatrix[1];
				lfImgLayOY = lfDataMatrix[5] + (0.5)*item.lfDataMatrix[2] + (szImg.cy-0.5)*item.lfDataMatrix[3];
				
				lfImgMatrix[0] = lfDataMatrix[0];
				lfImgMatrix[1] = lfDataMatrix[1];
				lfImgMatrix[2] = -lfDataMatrix[3];
				lfImgMatrix[3] = -lfDataMatrix[4];
			}

			lfPixelSizeX = lfPixelSizeY = sqrt(fabs(matrix_modulus(lfImgMatrix,2)));

			nType = ImageType;
		}
	}

	void ConvertToMatrixType()
	{
		if( nType==ImageType )
		{
			ViewImgPosition item = *this;
			
			matrix_toIdentity(lfDataMatrix,3);

			if( nPixelBase==Corner )
			{
				lfDataMatrix[2] = item.lfImgLayOX + szImg.cy*item.lfImgMatrix[1];
				lfDataMatrix[5] = item.lfImgLayOY + szImg.cy*item.lfImgMatrix[3];
				
				lfDataMatrix[0] = item.lfImgMatrix[0];
				lfDataMatrix[1] = item.lfImgMatrix[1];
				lfDataMatrix[3] = -item.lfImgMatrix[2];
				lfDataMatrix[4] = -item.lfImgMatrix[3];
			}
			else
			{
				lfDataMatrix[2] = item.lfImgLayOX + (-0.5)*item.lfImgMatrix[0] + (szImg.cy-0.5)*item.lfImgMatrix[1];
				lfDataMatrix[5] = item.lfImgLayOY + (-0.5)*item.lfImgMatrix[2] + (szImg.cy-0.5)*item.lfImgMatrix[3];
				
				lfDataMatrix[0] = item.lfImgMatrix[0];
				lfDataMatrix[1] = item.lfImgMatrix[1];
				lfDataMatrix[3] = -item.lfImgMatrix[2];
				lfDataMatrix[4] = -item.lfImgMatrix[3];
			}

			nType = MatrixType;
		}
	}

	CSize szImg;
	char fileName[_MAX_PATH];
	int nType; //0，表示使用 tfw影像参数；1，表示使用矩阵参数 lfDataMatrix
	double lfImgLayOX, lfImgLayOY;
	double lfImgMatrix[4];
	double lfPixelSizeX, lfPixelSizeY;
	int nPixelBase; //0，表示以左下角像素的左下角为原点，1，表示以左下角像素的像素中心为原点
	double lfDataMatrix[9];
	BOOL is_visible;
};


class CUndoAdjustImagePosition: public CUndoAction
{
public:
	CUndoAdjustImagePosition(CEditor *p, LPCTSTR name);
	virtual ~CUndoAdjustImagePosition();
	
	virtual void Undo();
	virtual void Redo();
	virtual void CopyFrom(const CUndoAction *pObj);
	
	virtual void Commit();
	virtual void DestoryNewObjs();
	
	DECLARE_CLONE(CUndoAdjustImagePosition)
		
		ViewImgPosition oldImgPos;
	ViewImgPosition newImgPos;
};

#endif