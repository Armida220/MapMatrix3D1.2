// UIFPropList.cpp : implementation file
//

#include "stdafx.h"
#include "PropList.h"
#include "treectrl.h"
#include "LayersMgrDlg.h"
#include "FtrsMgrDlg.h"
#include "SmartViewFunctions.h"
#include "FtrLayer.h "
#include "LayersMgrDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_HEADER		1
#define ID_SCROLL_VERT	2
#define ID_SCROLL_HORZ	21
#define ID_COLOR_POPUP  3
#define ID_COMBOBOX_POPUP  100

#define TEXT_MARGIN		4

#define BITMAPWIDTH		16

//用于复制字符串的函数
static void CopyStr(char** szDest,const char* szSource)
{
	if (!szDest || !szSource)
		return;

	*szDest = new char[strlen(szSource)+1];
	memset((*szDest),0,strlen(szSource)+1);
	strcpy(*szDest,szSource);
}


CString VarToString(_variant_t& var)
{
	CString text;
	switch(var.vt) 
	{
	case VT_UI1:
		text.Format("%i",var.bVal);
		break;
	case VT_UI2:
		text.Format("%i",var.uiVal);
		break;
	case VT_UI4:
		text.Format("%i",var.ulVal);
		break;
	case VT_UINT:
		text.Format("%i",var.uintVal);
		break;
	case VT_I1:
		text.Format("%c",var.cVal);
		break;
	case VT_I2:
		text.Format("%i",var.iVal);
		break;	
	case VT_I4:
		text.Format("%d",(int)var.lVal);
		break;
	case VT_INT:
		text.Format("%d",(int)var.intVal);
		break;
	case VT_BOOL:
		if( (bool)var )
			text = CString("1");
		else
			text = CString("0");
		break;
	case VT_R4:
		text.Format("%f",(float)var.fltVal);
		break;
	case VT_R8:
		text.Format("%.4f",(double)var.dblVal);
		break;
	case VT_BSTR:
		text = (LPCTSTR)(_bstr_t)var;
		break;
	default:
		break;
	}

	return text;
}


double VarToDouble(_variant_t& var, BOOL *pRetOK)
{
	double v = 0;
	BOOL bOK = TRUE;
	switch(var.vt) 
	{
	case VT_UI1:
		v = (int)var.bVal;
		break;
	case VT_UI2:
		v = (int)var.uiVal;
		break;
	case VT_UI4:
		v = (int)var.ulVal;
		break;
	case VT_UINT:
		v = (int)var.uintVal;
		break;
	case VT_I1:
		v = (int)var.cVal;
		break;
	case VT_I2:
		v = (int)var.iVal;
		break;	
	case VT_I4:
		v = (int)var.lVal;
		break;
	case VT_INT:
		v = (int)var.intVal;
		break;
	case VT_BOOL:
		v = (int)var.boolVal;
		break;
	case VT_R4:
		v = (float)var.fltVal;
		break;
	case VT_R8:
		v = (double)var.dblVal;
		break;
	case VT_BSTR:
		{
			bOK = FALSE;
			char buf[1000] = {0};
			CString buf2 = (LPCTSTR)var.bstrVal;
			buf2.TrimLeft();
			buf2.TrimRight();
			strncpy(buf,buf2,sizeof(buf)-1);
			if(strlen(buf)>0)
			{
				char *pos = NULL;
				double v2 = strtod(buf,&pos);
				if(pos!=NULL && strlen(pos)==0)
				{
					v = v2;
					bOK = TRUE;
				}
			}			
		}
		break;
	default:
		bOK = FALSE;
	}

	if( pRetOK )*pRetOK = bOK;
	
	return v;
}

BOOL StringToVar(LPCTSTR str, int valuetype, _variant_t& var,BOOL bAllowEmpty = FALSE)
{
	if(!bAllowEmpty&&strlen(str)<=0 )
		return FALSE;

	char *pos = NULL;
	switch( valuetype )
	{
	case VT_I2:
		{
			short v = (short)strtol(str,&pos,10);
			if( strlen(pos)==0 )
				var = v;
		}
		break;
	case VT_I4:
		{
			long v = (long)strtol(str,&pos,10);
			if( strlen(pos)==0 )
				var = v;
		}
		break;
	case VT_R4:
		{
			float v = (float)strtod(str,&pos);
			if( strlen(pos)==0 )
				var = v;
		}
		break;
	case VT_R8:
		{
			double v = (double)strtod(str,&pos);
			if( strlen(pos)==0 )
				var = v;
		}
		break;
	case VT_BSTR:
		var = str;
		break;
	default: return FALSE;
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CLVLPropItem

IMPLEMENT_DYNAMIC(CLVLPropItem, CObject)

CLVLPropItem::CLVLPropItem()
{	
	Init ();
}
//******************************************************************************************
void CLVLPropItem::Init ()
{
	m_pWndInPlace = NULL;
	m_pWndList = NULL;
	m_bInPlaceEdit = FALSE;
	m_bInColorSel  = FALSE;
	m_Rect.SetRectEmpty ();

	m_dwData		= 0;

	m_bIsInput		= false;
	m_bReadOnlyRefCol = FALSE;
}
//*******************************************************************************************
CLVLPropItem::~CLVLPropItem()
{
	RemoveAllValues();

	OnDestroyWindow ();
}
//******************************************************************************************
void CLVLPropItem::OnDestroyWindow ()
{
	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->DestroyWindow ();
		delete m_pWndInPlace;
		m_pWndInPlace = NULL;
	}

	if(m_WndComboBox.GetSafeHwnd()!=NULL)
		m_WndComboBox.ShowWindow(SW_HIDE);
}
//*******************************************************************************************
BOOL CLVLPropItem::HitTest (CPoint point)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);

	if (m_Rect.PtInRect (point))
	{
		return TRUE;
	}

	return FALSE;
}

//*******************************************************************************************
void CLVLPropItem::Redraw ()
{
	ASSERT_VALID (this);

	if (m_pWndList != NULL)
	{
		ASSERT_VALID (m_pWndList);
		m_pWndList->RedrawWindow (m_Rect);
	}
}
//*******************************************************************************************
BOOL CLVLPropItem::IsSelected () const
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);

	return m_pWndList->IsInSelection(this);
}

void CLVLPropItem::RemoveAllValues()
{
	int nsz = m_PropValues.GetSize();
	for( int i=0; i<nsz; i++)
	{
		_variant_t *p = m_PropValues.GetAt(i);
		if( p )delete p;
		m_PropValues.SetAt(i,NULL);
	}
	m_PropValues.RemoveAll();
}

void CLVLPropItem::SetColumnCount(int num)
{
	RemoveAllValues();
	m_PropValues.SetSize(num);
	m_arrValuesReadOnly.SetSize(num);
	for( int i=0; i<num; i++)
	{
		_variant_t *p = new _variant_t;
		m_PropValues.SetAt(i,p);
		m_arrValuesReadOnly.SetAt(i,FALSE);
	}
	
}

//设置值
//******************************************************************************************
void CLVLPropItem::SetValue (const _variant_t& varValue,int index)
{
	ASSERT_VALID (this);

	CLVLPropColumn item = m_pWndList->GetColumn(index);

	if( varValue.vt!=VT_EMPTY && item.ValueType!=varValue.vt&&!(m_pWndList->GetFilterItem()==this&&strcmp(item.FieldName,FIELDNAME_LAYUSED)==0) )
	{
		if (m_pWndList->GetFilterItem()==this)//如果当前需要设置的值为过滤器
		{
			BOOL bInPlaceEdit = m_bInPlaceEdit;
			if (bInPlaceEdit)
			{
				OnEndEdit();
			}
			
			*m_PropValues[index] = varValue;

		}
		else
		{
			ASSERT (FALSE);
			return;

		}
	
	}



	BOOL bInPlaceEdit = m_bInPlaceEdit;
	if (bInPlaceEdit)
	{
		OnEndEdit();
	}

	*m_PropValues[index] = varValue;
	Redraw ();
}


_variant_t CLVLPropItem::GetValue (LPCTSTR field)const
{
	int icol = m_pWndList->GetColumnIndexByField(field);
	return *m_PropValues[icol];
}

void CLVLPropItem::SetColReadOnly(int col, BOOL bReadOnly)
{
	int size = m_arrValuesReadOnly.GetSize();
	if (col >= 0 && col < size)
	{
		m_arrValuesReadOnly.SetAt(col,bReadOnly);
	}
}

BOOL CLVLPropItem::IsColReadOnly(int col)
{
	int size = m_arrValuesReadOnly.GetSize();
	if (col >= 0 && col < size)
	{
		return m_arrValuesReadOnly.GetAt(col);
	}

	return FALSE;
}

//*******************************************************************************************
void CLVLPropItem::SetOwnerList (CLVLPropList* pWndList)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pWndList);

	m_pWndList = pWndList;
}
//*******************************************************************************************
void CLVLPropItem::Repos (int x, int& y)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);

	m_Rect.SetRectEmpty ();

	m_Rect = CRect (
	m_pWndList->m_rectList.left-x,
	y, 
	m_pWndList->m_rectList.right+x, 
	y + m_pWndList->m_nRowHeight);
	
	m_pWndList->GetHeaderCtrl().GetItemRect(0,&m_RectName);
	m_RectName.top = m_Rect.top;
	m_RectName.bottom = m_Rect.bottom;

	y += m_pWndList->m_nRowHeight;
}


//开始真正填充列表框里的值
//******************************************************************************************
void CLVLPropItem::OnDrawValue (CDC* pDC)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);

	if (IsSelected ()&&this!=m_pWndList->GetCurLayerPropItem())
	{
		CRect rectFill = m_Rect;
		if (m_pWndList->m_bFocused && !m_bIsInput)
		{
			CBrush br(RGB(200,200,200));
			rectFill.DeflateRect(0,1,0,1);
			pDC->FillRect (rectFill,&br);
		}
	}
	else if( this==m_pWndList->GetFilterItem() )
	{
		CRect rectFill = m_Rect;
		{
			CBrush br(RGB(255,255,160));
			rectFill.DeflateRect(0,1,0,1);
			pDC->FillRect (rectFill,&br);
		}
	}
	else if (this==m_pWndList->GetCurLayerPropItem())
	{
		CRect rectFill = m_Rect;
		if (!m_pWndList->IsInSelection(this))
		{
			CBrush br(RGB(157,223,255));
			rectFill.DeflateRect(0,1,0,1);
			pDC->FillRect (rectFill,&br);
		}
		else
		{
			CBrush br(RGB(107,203,205));
			rectFill.DeflateRect(0,1,0,1);
			pDC->FillRect (rectFill,&br);	
		}
	}

	CRect rectValue = m_RectName;
	
	CRgn rgnClipVal;
	CRect rectValClip = rectValue;
	rectValClip.bottom = min (rectValClip.bottom, m_pWndList->m_rectList.bottom);
	
	rgnClipVal.CreateRectRgnIndirect (&rectValClip);
	pDC->SelectClipRgn (&rgnClipVal);

	if (m_bIsInput)					//表示是输入过滤条件的条目
	{
		CBrush br(::GetSysColor(COLOR_INFOBK));
		pDC->FillRect(m_RectName,&br);
	}
	
	//画行名称
	pDC->DrawText(m_ItemName,m_RectName,DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
	
	pDC->SelectClipRgn (NULL);

	int nCol = m_pWndList->GetColumnCount();//获取有多少列数据

	for(int i = 0 ; i < nCol; i++) 
	{
		CLVLPropColumn col = m_pWndList->GetColumn(i);//获取每一列的属性
		if (m_pWndList->GetFilterItem()==this&&strcmp(col.FieldName,FIELDNAME_LAYUSED)==0)
		{
			col.Type = CLVLPropColumn::CHECK;
		}
		CRect rectValue = col.Rect&m_Rect;
		
		CRgn rgnClipVal;
		CRect rectValClip = rectValue;
		rectValClip.bottom = min (rectValClip.bottom, m_pWndList->m_rectList.bottom);
		
		rgnClipVal.CreateRectRgnIndirect (&rectValClip);
		pDC->SelectClipRgn (&rgnClipVal);
		
//		if(!IsSelected() || m_bIsInput)
		{
			CRect rc = rectValue;
			
			if(col.bFrame)
			{
				CBrush bs(RGB(128,128,128));
				rc = rectValue;
				rc.left = rc.right-1;
				pDC->FrameRect(&rc,&bs);

				rc = rectValue;
				rc.top = rc.bottom-1;
				pDC->FrameRect(&rc,&bs);
			}
//			CBrush br(col.DefaultColor);
//			pDC->FillRect(rt,&br);
		}

		switch(col.Type)
		{
		case CLVLPropColumn::EDIT:
			{
				CString lEditValue = VarToString(*m_PropValues[i]);
				if (lEditValue == _T("-2"))//-2为空值状态 即全选 -1已被bylayer占用
				{
					lEditValue ="";
					*m_PropValues[i] = (_variant_t)(LPCTSTR)lEditValue;
				}
				pDC->DrawText(lEditValue,rectValue,DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
			}
			break;
		case CLVLPropColumn::COLOR:
			{
				CRect rectColor = rectValue;
				rectColor.right = rectColor.left + rectColor.Height ();
				rectColor.DeflateRect (1, 1);

				CRect rectText = rectValue;
				rectText.left  = rectColor.right+1;

				BOOL bFtrsColorByLayer = FALSE;
				CManageBaseDlg *pDlg = (CManageBaseDlg*)m_pWndList->GetParent();
				if (pDlg->GetManageType() == ManageFtr)
				{
					bFtrsColorByLayer = ((CFtrsMgrDlg*)pDlg)->IsFtrscolorByLayer(m_dwData);
				}
				
				COLORREF ct = (long)*m_PropValues[i];
				if (bFtrsColorByLayer)
				{
					ct = ((FeatureItem*)m_dwData)->pFtrLayer->GetColor();
				}
				
				CString strColor;
				strColor.Format("%d",ct);
				CBrush br (ct);
				if (strColor != _T("-1") && strColor != _T("-2") ) //不为缺省值时候不需要显示颜色 -1 为bylayer -2 为全部显示
				{
				pDC->FillRect (rectColor, &br);
				pDC->Draw3dRect (rectColor, 0, 0);
				}

				if (bFtrsColorByLayer)
				{
					strColor = _T("ByLayer");
					pDC->FillRect (rectColor, &br);
					pDC->Draw3dRect (rectColor, 0, 0);
				}
/*				
				CString strColor;
				strColor.Format("%x%x%x",GetRValue(ct),GetGValue(ct),GetBValue(ct));*/
				 if(strColor == _T("-2") ||strColor == _T("") )//右键全部 显示 ?
				{
					ShowBitmap(pDC,rectValue,col.BitmapOffset+4);//不显示
					break;
				}
				else if (strColor == _T("-1"))//颜色为Bylayer
				{
					strColor = _T("ByLayer");
					pDC->DrawText(strColor,rectValue,DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);	
					break;
				}
				pDC->DrawText(strColor,rectText,DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
			}
			break;
		case CLVLPropColumn::COMBO:
			{
				CString lEditValue = VarToString(*m_PropValues[i]);
				if (lEditValue == _T("-2")) // -2为空值状态 全选
				{
					lEditValue ="";
					*m_PropValues[i] = (_variant_t)(LPCTSTR)lEditValue;
				}
				pDC->DrawText((lEditValue),rectValue,DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
			}
			break;
/*		case CLVLPropColumn::STYLE:
			{
				CPen pen(PS_DOT, 1 , RGB(135,135,135));
				CPen* OldPen = (CPen *) pDC->SelectObject(&pen);
				CRect rectStyle = m_PropColumn[i].Rect;
				pDC->MoveTo(rectStyle.left+4,rectStyle.top+rectStyle.Height()/2);
				pDC->LineTo(rectStyle.right - rectStyle.Height(),rectStyle.top+rectStyle.Height()/2);
				pDC->SelectObject(OldPen);
				
//				rect.left = rect.right - rect.Height();
			}
			break;
		case CLVLPropColumn::WEIGHT:
			{
				CPen pen(PS_SOLID, 3 , RGB(135,135,135));
				CPen* OldPen = (CPen *) pDC->SelectObject(&pen);
				CRect rectStyle = m_PropColumn[i].Rect;
				pDC->MoveTo(rectStyle.left+4,rectStyle.top+rectStyle.Height()/2);
				pDC->LineTo(rectStyle.right - rectStyle.Height(),rectStyle.top+rectStyle.Height()/2);
				pDC->SelectObject(OldPen);
				
//				rect.left = rect.right - rect.Height();
			}
			break;
*/		case CLVLPropColumn::CHECK:
			{
				//bool bShow1 = (bool)*m_PropValues[i];
				if (m_pWndList->GetFilterItem()==this)
				{
					//long  bShow = long(*m_PropValues[i]);
					CString bShow = VarToString(*m_PropValues[i]);
					if(bShow == "1")
					{
						ShowBitmap(pDC,rectValue,col.BitmapOffset);//显示
					}
					else if(bShow == "0"  )
					{
						ShowBitmap(pDC,rectValue,col.BitmapOffset+2);//不显示
					}
					else if (bShow =="-2"||bShow == "")
					{
						ShowBitmap(pDC,rectValue,col.BitmapOffset+4);//全部显示
					}
					
					
					
					

				}
				else
				{
					bool bShow = (bool)*m_PropValues[i];
				
					if(bShow)
					{
						ShowBitmap(pDC,rectValue,col.BitmapOffset);
					}
					else if(!bShow)
					{
						ShowBitmap(pDC,rectValue,col.BitmapOffset+2);
					}
				


				}
			

			}
			break;
		}

		pDC->SelectClipRgn (NULL);
	}
}

#include "DlgMessageBox.h"
//******************************************************************************************
BOOL CLVLPropItem::OnUpdateValue (CLVLPropColumn * Pc)
{
 	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);
	static int  nClickCount=0;

	if (Pc->bReadOnly&&m_pWndList->GetFilterItem()!=this) return FALSE;

	int icol = m_pWndList->GetColumnIndexByName(Pc->ColumnName);

	//字段为只读 不允许修改
	if (IsColReadOnly(icol)) return FALSE;
// 	if (strcmp(Pc->FieldName,FIELDNAME_LAYUSED)==0&&m_pWndList->GetFilterItem()!=this)
// 	{
// 		return FALSE;
// 	}
// 	
// 	if (Pc->bReadOnly&&strcmp(Pc->FieldName,FIELDNAME_LAYUSED)!=0) return FALSE;
// 	if (strcmp(Pc->FieldName,FIELDNAME_LAYUSED)==0&&m_pWndList->GetFilterItem()!=this)
// 	{
// 		return FALSE;
// 	}
	BOOL bValueBeChanged = false;
	_variant_t Value = *m_PropValues[icol];
	_variant_t OldValue = Value;
	_variant_t tValue = (long)0;


	CString strtmp;
	switch(Pc->Type)
	{
	case CLVLPropColumn::EDIT:
		{
			CString strEdit;
			m_pWndInPlace->GetWindowText(strEdit);
// 			if (strEdit == VarToString(Value))
// 			{
// 			}
// 			else
			{
				//这儿加东西
				bValueBeChanged = true;
			}
			BOOL bAllow = FALSE;
			if (strcmp(Pc->FieldName,FIELDNAME_FTRCODE/*FIELDNAME_LAYMAPNAME*/)==0)
			{
				bAllow = TRUE;
			}
			if( !StringToVar(strEdit,Pc->ValueType,Value,bAllow) )
				Value.Clear();
		}
		break;
	case CLVLPropColumn::COLOR:
		{
			COLORREF ct = m_WndInColor.GetColor();
			if (m_pWndList->GetSelectedCount()==1 && (long)Value == (long)ct)
			{
			}
			else
				bValueBeChanged = true;

			Value   = (long)ct;
		}
		break;
	case CLVLPropColumn::COMBO:
		{
			CString str;
			int index = m_WndComboBox.GetCurSel();
			if (index == -1) break;
			m_WndComboBox.GetLBText(index,str);
			if (m_pWndList->GetSelectedCount()==1 && str.CompareNoCase((const char*)(_bstr_t)(Value))==0 )
			{
			}
			else
				bValueBeChanged = true;
			
			Value = (const char*)(_bstr_t)(str);
		}
		break;
	case CLVLPropColumn::CHECK:
		//当前改变项为过滤器项 修改值
		if (m_pWndList->GetFilterItem()==this)
		{
//***************************************方案二*************************//
			
				if (!m_pWndList->m_bMulSel) //是否为多选
				{
					bool bShow = (bool)Value;
					strtmp = VarToString(Value);
					if (strtmp == "0")  //当前为不显示 变更后为全部显示
					{	
						Value.vt = VT_INT;
						Value  = (long)SELNULL;
						//	Value = (bool) !bShow;
						
						strtmp = VarToString(Value);
						
					}
					else if (strtmp == "1")//当前为显示 变更后为不显示
					{
						Value.vt = VT_BOOL;
						Value = (_variant_t)bool(false);
						strtmp = VarToString(Value);
						
						
					}
					else if (strtmp == "-2" ||strtmp == "")
					{
						Value.vt = VT_BOOL;
						Value = (_variant_t)bool(true);
						strtmp = VarToString(Value);
						
					}
							
					
			}





		}
		else	//为非过滤器项
		{
			Value.vt = VT_BOOL;
			bool bShow = (bool)Value;
			if (!m_pWndList->m_bMulSel) //当前的选中不为多选
			{

				Value = (bool) !bShow;  //修改为其反向值  修改
		
			}
			else						//为多选
			{
				bShow = (bool)m_pWndList->m_vValueBeforeCheck;
				Value = (bool)!bShow;				
			}	
			
			
		}

		
		
		bValueBeChanged = true;
		break;
	}

	// 当不为删除列的时候
	if (stricmp(Pc->ColumnName,StrFromResID(IDS_DELETED)) != 0)
	{
		//
		if( Value.vt==VT_EMPTY || this==m_pWndList->GetFilterItem() )
		{
			*m_PropValues[icol] = Value;

		}
		else if( m_PropValues[icol]->vt==VT_EMPTY || m_PropValues[icol]->vt==Value.vt )
			*m_PropValues[icol] = Value;
		
	//	Redraw();
	}	

	//向PropList发出数据需要改变的消息
	if( bValueBeChanged )
		m_pWndList->UpdatePropItem((CLVLPropItem*)this,Pc,OldValue);

	if (stricmp(Pc->ColumnName,StrFromResID(IDS_DELETED)) != 0)
		Redraw();

	return TRUE;
}


//******************************************************************************************
BOOL CLVLPropItem::OnEdit (CLVLPropColumn* Pc)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);

	if(m_bReadOnlyRefCol&&Pc->bReadOnlyRefItem) return FALSE;
	if (Pc->bReadOnly&&strcmp(Pc->FieldName,FIELDNAME_LAYUSED)!=0&&m_pWndList->GetFilterItem()!=this)
		return FALSE;

	int icol = m_pWndList->GetColumnIndexByName(Pc->ColumnName);
	
	if (IsColReadOnly(icol)) return FALSE;

	m_pWndInPlace = CreateInPlaceEdit (Pc->Rect&m_Rect);

	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetWindowText (VarToString(GetValue(Pc->FieldName)));
		m_pWndInPlace->SetFocus ();

		m_bInPlaceEdit = TRUE;
		return TRUE;
	}

	return FALSE;
}

BOOL CLVLPropItem::OnCombo(CLVLPropColumn* Pc)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);
	
	if(m_bReadOnlyRefCol&&Pc->bReadOnlyRefItem) return FALSE;
	if (Pc->bReadOnly&&strcmp(Pc->FieldName,FIELDNAME_LAYUSED)!=0&&m_pWndList->GetFilterItem()!=this)
		return FALSE;
	
	int icol = m_pWndList->GetColumnIndexByName(Pc->ColumnName);
	
	if (IsColReadOnly(icol)) return FALSE;
	
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_VSCROLL|CBS_DROPDOWNLIST;
	
	CRect rect = Pc->Rect&m_Rect;
	rect.InflateRect(0,0,rect.Width()/4,rect.Height()*20);

	if(::IsWindow(m_WndComboBox.GetSafeHwnd()))
	{
		m_WndComboBox.ResetContent();
		for (int i=0; i<Pc->arrComboValues.GetSize(); i++)
		{
			m_WndComboBox.AddString(Pc->arrComboValues[i]);
		}
		m_WndComboBox.MoveWindow(rect);
		m_WndComboBox.ShowWindow(SW_SHOW);
	}
	else
	{
		if(!m_WndComboBox.Create(dwStyle,rect,m_pWndList,ID_COMBOBOX_POPUP)) 
			return FALSE;

		CWnd *pParentWnd = m_pWndList->GetParent();
		if (pParentWnd)
		{
			CFont *pFont = pParentWnd->GetFont();	
			m_WndComboBox.SetFont(pFont);
		}

		for (int i=0; i<Pc->arrComboValues.GetSize(); i++)
		{
			m_WndComboBox.AddString(Pc->arrComboValues[i]);
		}

	}
	
	m_WndComboBox.SelectString(-1,VarToString(GetValue(Pc->FieldName)));
	m_WndComboBox.SetFocus();
	m_WndComboBox.ShowWindow(SW_SHOW);

	m_WndComboBox.ShowDropDown();
	
	m_bInComboBox = TRUE;
	return TRUE;
}

//******************************************************************************************
BOOL CLVLPropItem::OnColor(CLVLPropColumn* Pc)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);

	DWORD dwStyle = WS_CHILD|WS_VISIBLE|WS_TABSTOP;
	
	if(::IsWindow(m_WndInColor.GetSafeHwnd()))
		m_WndInColor.MoveWindow(Pc->Rect&m_Rect);
	else
		if(!m_WndInColor.Create (NULL,dwStyle,Pc->Rect&m_Rect,m_pWndList,ID_COLOR_POPUP)) 
			return FALSE;
	m_WndInColor.ShowWindow(SW_HIDE);
	// Create color picker:
	if (((CManageBaseDlg*)m_pWndList->GetParent())->GetManageType() == ManageFtr)
	{
		COLORREF col = -1;
		if (this != m_pWndList->m_pFilterItem)
		{
			col = ((FeatureItem*)m_dwData)->pFtrLayer->GetColor();			
		}

		m_WndInColor.EnableAutomaticButton (_T("ByLayer"), col);
		
	}
	else
	{
		m_WndInColor.EnableAutomaticButton (_T(/*"Automatic"*/"ByLayer"), RGB (255, 255, 255));
	}
	m_WndInColor.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
	m_WndInColor.SetColumnsNumber (5);

	CManageBaseDlg *pDlg = (CManageBaseDlg*)m_pWndList->GetParent();
	if (pDlg->GetManageType() == ManageFtr && ((CFtrsMgrDlg*)pDlg)->IsFtrscolorByLayer(m_dwData))
	{
		m_WndInColor.SetColor(-1);
	}
	else
		m_WndInColor.SetColor(COLORREF((long)GetValue(Pc->FieldName)));
	m_WndInColor.ShowColorPopup();

	m_bInColorSel = TRUE;	
	return TRUE;
}
//******************************************************************************************
CWnd* CLVLPropItem::CreateInPlaceEdit (CRect rectEdit)
{
	if (m_pWndList->GetFilterItem()==this)
	{
		int i=0;
	}

	CEdit* pWndEdit = NULL;
	
	pWndEdit = new CEdit;
	
	DWORD dwStyle = WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL |WS_BORDER | ES_CENTER;
	
	pWndEdit->Create (dwStyle, rectEdit, m_pWndList, UIFROPLIST_ID_INPLACE);
	
	return pWndEdit;
}

//******************************************************************************************
CLVLColorButton * CLVLPropItem::CreateInColorEdit (CRect rectColor)
{
	CLVLColorButton* pWndColorButton = NULL;
	
	pWndColorButton = new CLVLColorButton;
	


	return pWndColorButton;
}

//*****************************************************************************
BOOL CLVLPropItem::OnEndEdit ()
{

	ASSERT_VALID (this);

	m_bInPlaceEdit = FALSE;
	m_bInColorSel  = FALSE;
	m_bInComboBox  = FALSE;
	OnDestroyWindow ();
	return TRUE;
}

//****************************************************************************************
BOOL CLVLPropItem::OnDblClick (CPoint /*point*/)
{
	if (m_pWndList->GetFilterItem()==this)
	{
		int i=0;
	}
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);

	if (m_pWndInPlace == NULL)
	{
		return FALSE;
	}

	ASSERT (::IsWindow (m_pWndInPlace->GetSafeHwnd ()));

	return FALSE;
}

//****************************************************************************************
BOOL CLVLPropItem::OnSetCursor () const
{
	return FALSE;
}

//*******************************************************************************************
HBRUSH CLVLPropItem::OnCtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);

	return NULL;
}
//*******************************************************************************************
BOOL CLVLPropItem::PushChar (UINT nChar)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);
	ASSERT (m_pWndList->IsInSelection(this));

	OnEndEdit ();
	ReleaseCapture ();
	return FALSE;
}

//显示勾选图标
void CLVLPropItem::ShowBitmap(CDC * pDC,CRect rect, int offset)
{
	if (m_pWndList->GetFilterItem()==this)
	{
		int i=0;
	}
	CDC dcMemory;
	dcMemory.CreateCompatibleDC(pDC);
	dcMemory.SelectObject(&(m_pWndList->m_bmProp));
	pDC->BitBlt(rect.left+rect.Width()/2-8,rect.top,m_pWndList->m_bmPropInfo.bmWidth/6,m_pWndList->m_bmPropInfo.bmHeight,&dcMemory,BITMAPWIDTH*offset,0,SRCAND);
	dcMemory.DeleteDC();
}

BOOL CLVLPropItem::OnRBClick (CPoint point , BOOL bSelChanged)
{
	
	
	
	
	
	
return TRUE;
}


//单击鼠标左键消息
BOOL CLVLPropItem::OnLBClick (CPoint point)
{

	if (m_pWndList->GetFilterItem()==this)
	{
		int i=0;
	}
	if (!m_pWndList || m_pWndList->GetSafeHwnd() == NULL)
		return FALSE;
	
	//返回当前选择项的列索引
	int iCol = m_pWndList->GetSelColumnIndex();
	//通过索引获取当前列的值
	CLVLPropColumn col = m_pWndList->GetColumn(iCol);

	if (m_pWndList->GetFilterItem()==this&&strcmp(col.FieldName,FIELDNAME_LAYUSED)==0)
	{
		col.Type = CLVLPropColumn::CHECK;
	}

	if ((!col.bReadOnly && IsColReadOnly(iCol)) || (col.bReadOnlyRefItem&&m_bReadOnlyRefCol)||(col.bReadOnly&&m_pWndList->GetFilterItem()!=this))
	{
		return TRUE;
	}	
	switch( col.Type ) 
	{
	case CLVLPropColumn::EDIT:
		if (m_pWndList->EditItem (this,&col) && m_pWndInPlace != NULL)
		{
			m_pWndInPlace->SetFocus();
			m_pWndInPlace->SendMessage (WM_LBUTTONDOWN);
			m_pWndInPlace->SendMessage (WM_LBUTTONUP);
			((CEdit*)m_pWndInPlace)->SetSel(0,-1);
		}
		break;
	case CLVLPropColumn::COLOR:
		{
			m_pWndList->EditItem (this,&col);
		}
		break;
	case CLVLPropColumn::CHECK:
		OnUpdateValue(&col);
		break;
	
	case CLVLPropColumn::COMBO:
		{
			m_pWndList->EditItem (this,&col);
		}
		break;
	default:;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CLVLPropList

IMPLEMENT_DYNAMIC(CLVLPropList, CWnd)

/////////////////////////////////////////////////////////////////////////////
// CLVLPropList notification messages:

//UINT UIFM_PROPERTY_CHANGED = ::RegisterWindowMessage (_T("UIFM_PROPERTYCHANGED"));
//UINT UIFM_PROPERTY_SELECTED= ::RegisterWindowMessage (_T("UIFM_PROPERTYSELECTED"));

CLVLPropList::CLVLPropList()
{
	m_rectList.SetRectEmpty ();
	m_rectTrackHeader.SetRectEmpty ();
	m_nRowHeight        = 0;
	m_nHeaderHeight     = 0;
	m_nVertScrollOffset = 0;
	m_nVertScrollTotal  = 0;
	m_nVertScrollPage   = 0;

	m_bFocused          = FALSE;
	m_bTracking         = FALSE;

	m_bmProp.LoadBitmap(IDB_PROPERTY);
	m_bmProp.GetObject(sizeof(BITMAP),&m_bmPropInfo);

	m_pInEditItem		= NULL;
	m_nSelColumn		= -1;
	m_nSelHeadColumn    = -1;
	m_pFilterItem		= NULL;
	m_bMulSel			= FALSE;
	m_ptLastLBD = CPoint(-1,-1);
	m_bIgnoreColSel = FALSE;
//	m_nStart = -1;
}

CLVLPropList::~CLVLPropList()
{
	RemoveAll();
	m_bmProp.DeleteObject();	
}

BEGIN_MESSAGE_MAP(CLVLPropList, CWnd)
	//{{AFX_MSG_MAP(CLVLPropList)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETTINGCHANGE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_CANCELMODE()
	ON_WM_KILLFOCUS()
	ON_WM_GETDLGCODE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETCURSOR()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_SETFOCUS()
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_CTLCOLOR()
	ON_COMMAND(ID_SELALL, OnSelAll)
	ON_COMMAND(ID_UNSELALL, OnUnSelAll)
	//}}AFX_MSG_MAP
	ON_NOTIFY(HDN_ITEMCHANGED, ID_HEADER, OnHeaderItemChanged)
	ON_NOTIFY(HDN_TRACK, ID_HEADER, OnHeaderTrack)
	ON_NOTIFY(HDN_ENDTRACK, ID_HEADER, OnHeaderEndTrack)
	ON_NOTIFY(HDN_ITEMCLICK, ID_HEADER, OnHeaderClick)
	ON_COMMAND(ID_COLOR_POPUP,OnColorChange)
	ON_CBN_SELCHANGE(ID_COMBOBOX_POPUP, OnComboChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLVLPropList message handlers


void CLVLPropList::OnColorChange()
{
	if (!m_pInEditItem) return;

	ASSERT(m_pInEditItem != NULL);
	COLORREF color = m_pInEditItem->m_WndInColor.GetColor();
	CLVLPropItem *pSel = NULL;
	if (GetSelectedCount()>=1)
	{
		pSel = GetSelectedItem(0);
	}
	else if (m_pFilterItem != NULL)
	{
		pSel = m_pFilterItem;
	}
	
	if (pSel)
	{
		CLVLPropColumn col = GetColumn(m_nSelColumn);
		if( col.Type == CLVLPropColumn::COLOR)
		{
			pSel->m_bInColorSel = TRUE;
		}	
		OnSetFocus(NULL);
	}	
		
}

void CLVLPropList::OnComboChange()
{
	CLVLPropItem *pSel = NULL;
	if (GetSelectedCount()>=1)
	{
		pSel = GetSelectedItem(0);
	}
	else if (m_pFilterItem != NULL)
	{
		pSel = m_pFilterItem;
	}
	
	if (pSel)
	{
		CLVLPropColumn col = GetColumn(m_nSelColumn);
		if( col.Type == CLVLPropColumn::COMBO)
		{
			pSel->m_bInComboBox = TRUE;
		}	
		OnSetFocus(NULL);
	}	
	
}

void CLVLPropList::PreSubclassWindow() 
{
	CWnd::PreSubclassWindow();

	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState ();
	if (pThreadState->m_pWndInit == NULL)
	{
		Init ();
	}
}
//******************************************************************************************
int CLVLPropList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	Init ();
	return 0;
}
//******************************************************************************************
void CLVLPropList::Init ()
{
	CRect rectDummy;

	rectDummy.SetRectEmpty ();

	m_wndHeader.Create (WS_CHILD | WS_VISIBLE | HDS_HORZ | CCS_BOTTOM, rectDummy, this, ID_HEADER);
	m_wndScrollVert.Create(WS_CHILD | WS_VISIBLE | SBS_VERT, rectDummy, this, ID_SCROLL_VERT);
	m_wndScrollHorz.Create(WS_CHILD | WS_VISIBLE | SBS_HORZ, rectDummy, this, ID_SCROLL_HORZ);

	AdjustLayout ();
}
//*****************************************************************************************
void CLVLPropList::AdjustLayout ()
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}

	CClientDC dc (this);

	//设置头控件位置
	TEXTMETRIC tm;
	dc.GetTextMetrics (&tm);
	m_nRowHeight    = tm.tmHeight;
	m_nHeaderHeight = m_nRowHeight;

	CRect rectClient;
	GetClientRect (rectClient);
	
	GetHeaderCtrl ().SetWindowPos (NULL, rectClient.left-m_nColWidth*m_nHorzScrollOffset, rectClient.top,rectClient.Width()+m_nColWidth*m_nHorzScrollOffset, m_nHeaderHeight,
		SWP_NOZORDER | SWP_NOACTIVATE);

	
	m_rectList = rectClient;
	m_rectList.top += m_nHeaderHeight;

	int	iWidth	= 0;
	int	iCount	= m_wndHeader.GetItemCount(); 
	
	for (int i = 0; i < iCount; i++)
	{	
		HDITEM	hd;
		hd.mask		= HDI_WIDTH;
		m_wndHeader.GetItem(i,&hd);
		
		iWidth += hd.cxy;
	}
	
	if (iCount == 0)
	{
		m_nColWidth = 0;
	}
	else
		m_nColWidth = iWidth/iCount;

	SetScrollSizes ();

	//设置纵向滚动条位置
	int cxScroll = ::GetSystemMetrics (SM_CXHSCROLL);

	if (m_nVertScrollTotal > 0)
	{
		m_rectList.right	-= cxScroll;
		m_wndScrollVert.SetWindowPos (NULL, m_rectList.right, m_rectList.top,
			cxScroll, m_rectList.Height (), SWP_NOZORDER | SWP_NOACTIVATE);
	}
	else
	{
		m_wndScrollVert.SetWindowPos (NULL, 0, 0,
			0, 0, SWP_NOZORDER | SWP_NOACTIVATE);
	}

	//设置水平滚动条位置
	int cyScroll = ::GetSystemMetrics (SM_CYVSCROLL);
	if (m_nHorzScrollTotal > 0)
	{
		m_rectList.bottom	-= cyScroll;
		m_wndScrollHorz.SetWindowPos (NULL, m_rectList.left, m_rectList.bottom,
			m_rectList.Width(), cyScroll, SWP_NOZORDER | SWP_NOACTIVATE);
	}
	else
	{
		m_wndScrollHorz.SetWindowPos (NULL, 0, 0,
			0, 0, SWP_NOZORDER | SWP_NOACTIVATE);
	}

	ReposProperties ();
}
//******************************************************************************************
void CLVLPropList::ReposProperties ()
{
	ASSERT_VALID (this);

	int x = m_nColWidth * m_nHorzScrollOffset;

	int y = m_rectList.top - m_nRowHeight * m_nVertScrollOffset - 1;
	int y0 = y;

	if( m_pFilterItem )
		m_pFilterItem->Repos(x,y);
	
	for (POSITION pos = m_lstProps.GetHeadPosition (); pos != NULL;)
	{
		CLVLPropItem* pProp = m_lstProps.GetNext (pos);
		ASSERT_VALID (pProp);
		
		pProp->Repos(x,y);
	}
	
	int nCol = GetColumnCount();
	
	for(int i = 0 ; i < nCol; i++)
	{
		CLVLPropColumn col = GetColumn(i);
		
		GetHeaderCtrl().GetItemRect(i,col.Rect);
		col.Rect.top = y0;
		col.Rect.bottom = y;

		col.Rect.left -= x;
		col.Rect.right -= x;
		
		SetColumn(i,col);
	}
	
	return;
}

void CLVLPropList::InitHeaderCtrl()
{
	int nCount = GetColumnCount();
	
	HDITEM hdItem;
	hdItem.fmt = HDF_CENTER;
	
	for(int i = 0; i < nCount ; i++)
	{ 
		CLVLPropColumn col = GetColumn(i);

		hdItem.mask = HDI_TEXT | HDI_FORMAT;
		hdItem.pszText = col.ColumnName;
		hdItem.cchTextMax = sizeof(col.ColumnName);
		m_wndHeader.InsertItem (i, &hdItem);
		
		hdItem.mask = HDI_WIDTH ;
		hdItem.cxy	= 60;

		m_wndHeader.SetItem (i, &hdItem);
	}
	
	AdjustLayout();
}

//******************************************************************************************
void CLVLPropList::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	EndEditItem (FALSE);

 	AdjustLayout ();
}
//******************************************************************************************
void CLVLPropList::OnSettingChange(UINT uFlags, LPCTSTR lpszSection) 
{
	CWnd::OnSettingChange(uFlags, lpszSection);	
	AdjustLayout ();
}
//******************************************************************************************
void CLVLPropList::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rectClip;
	dc.GetClipBox (rectClip);

	CRect rectClient;
	GetClientRect (rectClient);

	CDC*		pDC = &dc;
	BOOL		bMemDC = FALSE;
	CDC			dcMem;
	CBitmap		bmp;
	CBitmap*	pOldBmp = NULL;

	if (dcMem.CreateCompatibleDC (&dc) &&
		bmp.CreateCompatibleBitmap (&dc, rectClient.Width (),
								  rectClient.Height ()))
	{
		//-------------------------------------------------------------
		// Off-screen DC successfully created. Better paint to it then!
		//-------------------------------------------------------------
		bMemDC = TRUE;
		pOldBmp = dcMem.SelectObject (&bmp);
		pDC = &dcMem;
	}

	pDC->FillRect(rectClient, &GetGlobalData()->brWindow);

	HFONT hfontOld = (HFONT)::SelectObject (pDC->GetSafeHdc (),GetStockObject (DEFAULT_GUI_FONT));

	pDC->SetTextColor(GetGlobalData()->clrWindowText);
	pDC->SetBkMode (TRANSPARENT);


	//画一个表单
	OnDrawList (pDC);
	::SelectObject (pDC->GetSafeHdc (), hfontOld);

	if (bMemDC)
	{
		//--------------------------------------
		// Copy the results to the on-screen DC:
		//-------------------------------------- 
		dc.BitBlt (rectClip.left, rectClip.top, rectClip.Width(), rectClip.Height(),
					   &dcMem, rectClip.left, rectClip.top, SRCCOPY);

		dcMem.SelectObject(pOldBmp);
	}
}
//******************************************************************************************
//画一个列表框
void CLVLPropList::OnDrawList (CDC* pDC)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);

	CPen penLine (PS_SOLID, 1,1);
	CPen* pOldPen = pDC->SelectObject (&penLine);

	//画过滤器列表
	if( m_pFilterItem )
		OnDrawProperty( pDC, m_pFilterItem );

	const CList<CLVLPropItem*, CLVLPropItem*>& lst = m_lstProps;

	for (POSITION pos = lst.GetHeadPosition (); pos != NULL;)
	{
		CLVLPropItem* pProp = lst.GetNext (pos);
		ASSERT_VALID (pProp);

		if (!OnDrawProperty (pDC, pProp))
		{
			break;
		}
	}

	pDC->SelectObject (pOldPen);
}

#include "dlglayervisible.h"
//在PropList的编辑完成后,触发修改函数
BOOL CLVLPropList::UpdatePropItem(CLVLPropItem *pItem, CLVLPropColumn* Pc, _variant_t exData)
{
	//当前选项为过滤器
	if( pItem==m_pFilterItem )
	{
		((CManageBaseDlg*)GetParent())->SaveFilter(NULL);
		FilterPropItems();	//过滤后显示过滤后的项
	}
	else
	{
		((CManageBaseDlg*)GetParent())->ModifyAttri(pItem,Pc,exData);
		/*if (IsParentLayerVisible())
		{
			((CDlgLayerVisible*)GetParent())->ModifyLayerAttri(pItem,Pc,exData);
		}
		else
		{
			((CLayersMgrDlg*)GetParent())->ModifyLayerAttri(pItem,Pc,exData);
		}*/

	}
	return TRUE;
}

BOOL CLVLPropList::IsParentLayerVisible()
{
	return  ( ((CManageBaseDlg*)GetParent())->GetManageType()==ManageLayerVisible );
	/*CString strCaption;
	GetParent()->GetWindowText(strCaption);
	
	if (strCaption.CompareNoCase(StrFromResID(IDS_TIPS_LAYERVISIBLE)) == 0)
		return TRUE;
	else
		return FALSE;*/
}

void CLVLPropList::SetFilterItem(CLVLPropItem *pItem)
{
	if( m_pFilterItem )
		delete m_pFilterItem;
	m_pFilterItem = pItem;

	if( pItem )
	{
		pItem->SetOwnerList(this);
		pItem->SetColumnCount(GetColumnCount());
	}
}

//过滤后显示过滤后的项
void CLVLPropList::FilterPropItems(BOOL bRedraw)
{
	CLVLPropItem *pItem = m_pFilterItem;

	DeselectAll(FALSE);
	m_lstProps.RemoveAll();

	int ncol = GetColumnCount();//返回有多少列
	CString strTmp,strToTal;
	for( POSITION pos = m_lstAllProps.GetHeadPosition(); pos!=NULL; )
	{
		CLVLPropItem *pp = m_lstAllProps.GetNext(pos);

		if( pItem==NULL )
		{
			m_lstProps.AddTail(pp);
		}
		else
		{
			//匹配每一列
			for( int i=0; i<ncol; i++)
			{
				
				_variant_t var1 = pItem->GetValue(i); //过滤器的值
				_variant_t var2 = pp->GetValue(i);   //所有行数据的值
				if (i==ncol-1) //地物数列是最后一列
				{
					if (var1.vt==VT_EMPTY)
					{
						continue;
					}
					int check  = (long)var1;
					if (check)
					{
						if ((long)var2>0)
						{
							continue;
						}
					}
					else
					{
						if ((long)var2>0)
						{
							break;
						}
					}

				}

			
				CString str1 = VarToString(var1);
				CString	str2 = VarToString(var2);
				
				if( str1.IsEmpty() )continue;

				if( str2.IsEmpty() )break;

				str1.MakeLower();//大写改成小写
				str2.MakeLower();
				//This code is added by cjc
				if (str1=="2"||str1=="" ||str1 == "-2") //2为全选
				{
					continue;
				}
			
				if( str2.Find(str1)<0 )break;//找不到 跳出循环
			}

			/*
			for( int i=0; i<ncol; i++)
			{
				_variant_t var1 = pItem->GetValue(i);
				_variant_t var2 = pp->GetValue(i);

				BOOL bMatch = FALSE;
				
				switch( var1.vt ) 
				{
				case VT_I2:
					bMatch = ( (short)var1==(short)var2 );
					break;
				case VT_I4:
					bMatch = ( (long)var1==(long)var2 );
					break;
				case VT_R4:
					bMatch = ( (float)var1==(float)var2 );
					break;
				case VT_R8:
					bMatch = ( (double)var1==(double)var2 );
					break;
				case VT_BSTR:
					bMatch = (strcmp((LPCTSTR)(_bstr_t)var1,(LPCTSTR)(_bstr_t)var2)==0);
					break;
				case VT_BOOL:
					bMatch = ( (bool)var1==(bool)var2 );
					break;
				default: continue;
				}

				if( !bMatch )break;
			}*/

			if( i>=ncol )m_lstProps.AddTail(pp);
		}
		
	}
	//	AfxMessageBox(strToTal,MB_OKCANCEL);
	m_ptLastLBD = CPoint(-1,-1);
//	m_nStart = -1;
	if( bRedraw )
	{
		AdjustLayout();
		RedrawWindow();
	}
}


//******************************************************************************************
BOOL CLVLPropList::OnDrawProperty (CDC* pDC, CLVLPropItem* pProp) const
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);
	ASSERT_VALID (pProp);

	if (!pProp->m_Rect.IsRectEmpty ())
	{
		if (pProp->m_Rect.top >= m_rectList.bottom)
		{
			return FALSE;
		}

		if (pProp->m_Rect.bottom >= m_rectList.top)
		{
			COLORREF clrTextOld = (COLORREF)-1;
			
			//调用CLVLPropItem的画值函数
			pProp->OnDrawValue (pDC);

			if (clrTextOld != (COLORREF)-1)
			{
				pDC->SetTextColor (clrTextOld);
			}
		}
	}

	return TRUE;
}
//****************************************************************************************
void CLVLPropList::OnItemChanged (CLVLPropItem* pProp) const
{
	ASSERT_VALID (this);
	ASSERT_VALID (pProp);
}
//*******************************************************************************************
BOOL CLVLPropList::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}

//******************************************************************************************
void CLVLPropList::OnHeaderItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{	
	*pResult = 0;
}
//*****************************************************************************************
void CLVLPropList::OnHeaderTrack(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMHEADER* pHeader = (NMHEADER*) pNMHDR;

	pHeader->pitem->cxy = min (pHeader->pitem->cxy, m_rectList.Width ());

	TrackHeader (pHeader->pitem->cxy,pHeader);
	*pResult = 0;
}
//******************************************************************************************
void CLVLPropList::OnHeaderEndTrack(NMHDR* pNMHDR, LRESULT* pResult)
{
	int nOffset = 0;
	NMHEADER* pHeader = (NMHEADER*) pNMHDR;
	pHeader->pitem->cxy = min (pHeader->pitem->cxy, m_rectList.Width ());
    nOffset = pHeader->pitem->cxy;
	if(nOffset < m_nRowHeight)
	{
		nOffset = m_nRowHeight;
	}

	HDITEM hdItem;
	hdItem.cxy = nOffset;
	hdItem.mask = HDI_WIDTH ;
	GetHeaderCtrl ().SetItem (pHeader->iItem, &hdItem);
	ReposProperties ();
	InvalidateRect(m_rectList);
	UpdateWindow();

	if (pHeader->iItem > 0)
	{	
	}
	else
	{
	}
	

	*pResult = 0;
}

void CLVLPropList::OnHeaderClick (NMHDR* pNMHDR , LRESULT* pResult)
{
	LPNMHEADER phdr = (LPNMHEADER)pNMHDR;

	EndEditItem (TRUE);
	SetFocus();

	if (phdr->iButton == 1)				//右键
	{
		CMenu obMenu;

		obMenu.LoadMenu(IDR_LIST_POPUP);

		m_pPopupMenu = obMenu.GetSubMenu(0);

		ASSERT(m_pPopupMenu); 

		// Get the cursor position
		CPoint obCursorPoint = (0, 0);
		
		GetCursorPos(&obCursorPoint);


		m_pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, obCursorPoint.x, 
										obCursorPoint.y, this);
	}
	else if (phdr->iButton == 0)		//左键
	{
		HDITEM	hdItem;
		char	szName[255];
		memset(szName,0,sizeof(szName));
		hdItem.mask			= HDI_TEXT;
		hdItem.pszText		= szName;
		hdItem.cchTextMax	= sizeof(szName);
		m_wndHeader.GetItem(phdr->iItem,&hdItem);		

		if (strstr(szName,StrFromResID(IDS_DELETED)) != NULL)
		{
			{
				SelectAll(FALSE);
				CWnd *pWnd = GetParent();
				if(pWnd)
				{
					((CManageBaseDlg*)pWnd)->SendMessage(WM_COMMAND,ID_DELLAYEROBJ);	
				}
			}

			*pResult = 0 ;

			return;
		}

		int nsort = m_wndHeader.GetSortColumn();
		BOOL bAscend = (nsort==phdr->iItem?!m_wndHeader.IsAscending():TRUE);
		m_wndHeader.SetSortColumn(phdr->iItem,bAscend);

		SortItems(phdr->iItem,bAscend);
	}
	
	*pResult = 0 ;
}

void CLVLPropList::ReSortItems(int ncol)
{
	BOOL bAscend = m_wndHeader.IsAscending();
	SortItems(ncol,bAscend);
}

void CLVLPropList::SortItems(int ncol, BOOL bAscend)
{
	int nItem = m_lstProps.GetCount(), i=0, j, k;

	CArray<CLVLPropItem*,CLVLPropItem*> arrItems;
	arrItems.SetSize(nItem);

	for( POSITION pos = m_lstProps.GetHeadPosition(); pos!=NULL; )
	{
		CLVLPropItem* pProp = m_lstProps.GetNext(pos);
		arrItems.SetAt(i++,pProp);
	}

	CLVLPropItem **buf = arrItems.GetData(), *p1, *p2;
	for( i=0; i<nItem; i++)
	{
		p1 = buf[i];
		k = -1;
		for( j=i+1; j<nItem; j++)
		{
			p2 = buf[j];

			_variant_t var1 = p1->GetValue(ncol);
			_variant_t var2 = p2->GetValue(ncol);

			ASSERT(var1.vt==var2.vt);
			int ret = -2;

			switch( var1.vt ) 
			{
			case VT_I2:
				{
					short v1 = (short)var1, v2 = (short)var2;
					if( v1==v2 )ret = 0;
					else if( v1<v2 )ret = -1;
					else ret = 1;
				}
				break;
			case VT_I4:
				{
					long v1 = (long)var1, v2 = (long)var2;
					if( v1==v2 )ret = 0;
					else if( v1<v2 )ret = -1;
					else ret = 1;
				}
				break;
			case VT_R4:
				{
					float v1 = (float)var1, v2 = (float)var2;
					if( v1==v2 )ret = 0;
					else if( v1<v2 )ret = -1;
					else ret = 1;
				}
				break;
			case VT_R8:
				{
					double v1 = (double)var1, v2 = (double)var2;
					if( v1==v2 )ret = 0;
					else if( v1<v2 )ret = -1;
					else ret = 1;
				}
				break;
			case VT_BSTR:
				{
					ret = strcmp((LPCTSTR)(_bstr_t)var1,(LPCTSTR)(_bstr_t)var2);
					if( ret<0 )ret = -1;
					else if( ret>0 )ret = 1;
				}
				break;
			case VT_BOOL:
				{
					short v1 = ((bool)var1)?1:0, v2 = ((bool)var2)?1:0;
					if( v1==v2 )ret = 0;
					else if( v1<v2 )ret = -1;
					else ret = 1;
				}
				break;
			default: return;
			}

			if( bAscend )
			{
				if( ret==1 )
				{
					p1 = p2;
					k = j;
				}
			}
			else
			{
				if( ret==-1 )
				{
					p1 = p2;
					k = j;
				}
			}
		}

		if( k!=-1 )
		{
			buf[k] = buf[i]; 
			buf[i] = p1;
		}
	}

	m_lstProps.RemoveAll();

	for( i=0; i<nItem; i++ )
	{
		m_lstProps.AddTail(buf[i]);
	}

	AdjustLayout();
	RedrawWindow();
}

//*****************************************************************************************
void CLVLPropList::TrackHeader (int nOffset,NMHEADER* pHeader)
{
	EndEditItem (TRUE);
	SetFocus();
}
//*****************************************************************************************
void CLVLPropList::RemoveAll ()
{
	ASSERT_VALID (this);

	//删除数据
	while (!m_lstAllProps.IsEmpty ())
	{
		delete m_lstAllProps.RemoveHead();
	}

	m_lstAllProps.RemoveAll();
	m_lstProps.RemoveAll();
	m_lstPSels.RemoveAll();

	m_pInEditItem = NULL;

	m_nSelColumn = -1;

	if( m_pFilterItem )
		delete m_pFilterItem;

	m_pFilterItem = NULL;

	//删除列表
	m_arrColumns.RemoveAll();

	//界面更新
	if( m_wndHeader.GetSafeHwnd()!=NULL )
	{
		int nCount = m_wndHeader.GetItemCount();
		for(int i = 0; i< nCount; i++)
			m_wndHeader.DeleteItem(0);
	}
	
	m_bTracking = FALSE;
}


void CLVLPropList::RemovePropItem(CLVLPropItem *pItem)
{
	POSITION pos = m_lstAllProps.Find( pItem );
	if( !pos )return;

	m_lstAllProps.RemoveAt( pos );

	pos = m_lstProps.Find( pItem );
	if( pos )m_lstProps.RemoveAt( pos );

	if( IsInSelection(pItem) )
		m_lstPSels.RemoveAt(m_lstPSels.Find(pItem));
	if (pItem==m_pCurLayItem)
	{
		m_pCurLayItem = NULL;
	}

	delete pItem;
}

//*******************************************************************************************
CLVLPropItem* CLVLPropList::HitTest (CPoint pt) const
{
	ASSERT_VALID (this);

	const CList<CLVLPropItem*, CLVLPropItem*>& lst = m_lstProps;

	for (POSITION pos = lst.GetHeadPosition (); pos != NULL;)
	{
		CLVLPropItem* pProp = lst.GetNext (pos);
		ASSERT_VALID (pProp);

		if( pProp->HitTest(pt) )
		{
			return pProp;
		}
	}

	return NULL;
}

int CLVLPropList::HitColumn(CPoint pt) const
{
	CLVLPropColumn col;
	int nCount = GetColumnCount();
	for(int i = 0 ; i<nCount; i++)
	{
		col = GetColumn(i);
		if( col.Rect.PtInRect(pt))
		{
			break;
		}
	}
	if( i<nCount )return i;
	return -1;
}
//*******************************************************************************************
void CLVLPropList::SelectItem (const CLVLPropItem* pProp, BOOL bOnlyOne, BOOL bRedraw)
{
	ASSERT_VALID (this);

	if( bOnlyOne )
	{
		if( GetSelectedCount()==1 && IsInSelection(pProp) )
			return;
		DeselectAll(bRedraw);
	}

	if( IsInSelection(pProp) )
		return;

	if( m_pInEditItem!=NULL && (m_pInEditItem->m_bInPlaceEdit || m_pInEditItem->m_bInColorSel) )
	{
		EndEditItem (TRUE);
	}

	m_lstPSels.AddTail((CLVLPropItem*)pProp);

	if (pProp != NULL)
	{
		if (bRedraw) InvalidateRect (pProp->m_Rect);
	}

	if (bRedraw)
	{
		UpdateWindow ();
	}

	CWnd *pWnd = GetParent();
	if(pWnd && bRedraw)
	{
		((CManageBaseDlg*)pWnd)->UpdateStatusString();
	}
}

void CLVLPropList::DeselectItem(const CLVLPropItem* pProp, BOOL bRedraw)
{
	ASSERT_VALID (this);
	
	if( !IsInSelection(pProp) )
		return;
	
	if( m_pInEditItem==pProp && (m_pInEditItem->m_bInPlaceEdit || m_pInEditItem->m_bInColorSel) )
	{
		EndEditItem (TRUE);
	}
	
	m_lstPSels.RemoveAt(m_lstPSels.Find((CLVLPropItem*)pProp));
	
	if (pProp != NULL)
	{
		if (bRedraw) InvalidateRect (pProp->m_Rect);
	}
	
	if (bRedraw)
	{
		UpdateWindow ();
	}

	CWnd *pWnd = GetParent();
	if(pWnd && bRedraw)
	{
		((CManageBaseDlg*)pWnd)->UpdateStatusString();
	}
}

void CLVLPropList::SelectAll(BOOL bRedraw )
{
	ASSERT_VALID (this);
	
	if( m_pInEditItem!=NULL && (m_pInEditItem->m_bInPlaceEdit || m_pInEditItem->m_bInColorSel) )
	{
		EndEditItem (TRUE);
	}
	CLVLPropItem* pProp = NULL;
	for (POSITION pos = m_lstProps.GetHeadPosition();pos!=NULL;)
	{
		pProp = m_lstProps.GetNext(pos);
		if (pProp&&!IsInSelection(pProp))
		{
			m_lstPSels.AddTail(pProp);
			if (bRedraw) InvalidateRect (pProp->m_Rect);
		}
	}
	if(bRedraw)
		UpdateWindow();

	CWnd *pWnd = GetParent();
	if(pWnd && bRedraw)
	{
		((CManageBaseDlg*)pWnd)->UpdateStatusString();
	}
}

void CLVLPropList::DeselectAll(BOOL bRedraw)
{
	ASSERT_VALID (this);

	if( m_pInEditItem!=NULL && (m_pInEditItem->m_bInPlaceEdit || m_pInEditItem->m_bInColorSel) )
	{
		EndEditItem (TRUE);
	}

	if( bRedraw && m_lstPSels.GetCount()>0 )
	{
		for( POSITION pos = m_lstPSels.GetHeadPosition(); pos!=NULL; )
		{
			CLVLPropItem* pProp = m_lstPSels.GetNext(pos);
			if( pProp )
			{
				InvalidateRect (pProp->m_Rect);
			}
		}

		m_lstPSels.RemoveAll();
		UpdateWindow();
	}
	else
		m_lstPSels.RemoveAll();

	CWnd *pWnd = GetParent();
	if(pWnd && bRedraw)
	{
		((CManageBaseDlg*)pWnd)->UpdateStatusString();
	}
}

void CLVLPropList::SetSelColumn( int idx)
{
	if( m_nSelColumn==idx )
		return;

	EndEditItem();

	if( m_nSelColumn>=0 )
	{
		HDITEM item;
		TCHAR text[256]={0};
		item.pszText = text;
		item.mask = HDI_TEXT;
		item.cchTextMax = 256;
		GetHeaderCtrl().GetItem(m_nSelColumn,&item);	
		strcpy(text,GetColumn(m_nSelColumn).ColumnName);
		item.pszText = text;	
		GetHeaderCtrl().SetItem(m_nSelColumn,&item);
	}	

	m_nSelColumn = idx;

	if( m_nSelColumn>=0 )
	{
		HDITEM item;
		TCHAR text0[256]={0};
		item.pszText = text0;
		item.mask = HDI_TEXT;
		item.cchTextMax = 256;

		GetHeaderCtrl().GetItem(m_nSelColumn,&item);
		
		char text[256] = "*";
		strcat(text,GetColumn(m_nSelColumn).ColumnName);
		item.pszText = text;
		
		GetHeaderCtrl().SetItem(m_nSelColumn,&item);
	}

	GetHeaderCtrl().RedrawWindow();
}


//响应鼠标左键消息
//******************************************************************************************
void CLVLPropList::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonDown(nFlags, point);

/*	if(GetSelectedCount()<=0)*/
		

	SetFocus();

	CLVLPropItem* pHit = HitTest (point);

	int icol = HitColumn(point);

	if (IsParentLayerVisible())
	{
		m_lstPSels.RemoveAll();
		RedrawWindow();

		if (pHit == NULL) return;

		m_ptLastLBD = point;
		SetSelColumn(1);		
		pHit->OnUpdateValue(&GetColumn(1));

		InvalidateRect(pHit->m_Rect);

		m_lstPSels.AddTail(pHit);
		return;
	}

	// 删除列处理
	BOOL bIsDelCol = FALSE;
	DWORD nCurSelItem = 0;
	CWnd *pWnd = GetParent();
	if(pWnd)
		nCurSelItem = ((CManageBaseDlg*)pWnd)->GetCurSelItem();
	
  	//第四列为删除列 cjc 2012-10-31 添加 nCurSelItem == LM_TREEID_FILTERITEM
	if ( (icol == 4 && (nCurSelItem == LM_TREEID_ALLLEVELS||nCurSelItem == LM_TREEID_FILTERITEM)) || (icol == 2 && nCurSelItem == LM_TREEID_ALLFEATURES))
	{
		bIsDelCol = TRUE;
	}
	
	//单击过滤器后
	if( pHit==NULL && m_pFilterItem!=NULL && m_pFilterItem->HitTest(point) &&
		icol>=0 && nFlags!=MK_CONTROL&&  nFlags!=MK_SHIFT )
	{
		
		m_bMulSel = FALSE;

		DeselectAll(TRUE);
		SetSelColumn(icol);

		// 删除列处理
		DWORD nCurSelItem = 0;
		CWnd *pWnd = GetParent();
		if(pWnd)
			nCurSelItem = ((CManageBaseDlg*)pWnd)->GetCurSelItem();

		if (bIsDelCol) //为删除列 不操作 返回
		{
			return;						
		}
	
		//对应到ITEM的鼠标左键响应
		m_pFilterItem->OnLBClick(point);
		m_ptLastLBD = CPoint(-1,-1);
//		m_nStart = -1;
		return;
	}

	//单击了空位置
	if (pHit == NULL || pHit->m_pWndList->GetSafeHwnd() == NULL &&nFlags!=MK_CONTROL&&  nFlags!=MK_SHIFT  )
	{
		EndEditItem();
		DeselectAll(TRUE);
		m_ptLastLBD = CPoint(-1,-1);
//		m_nStart = -1;
		return;
	}

	BOOL bOldSel = IsInSelection(pHit);


	// 多选的状态下点击 check 类型的列
	if( icol>=0 && GetColumn(icol).Type==CLVLPropColumn::CHECK && GetSelectedCount()>1 &&bOldSel )
	{
		m_ptLastLBD = point;
		SetSelColumn(icol);
		if( GetSelectedCount()>1 && GetColumn(icol).Type==CLVLPropColumn::CHECK )
		{
			// 删除列处理
			DWORD nCurSelItem = 0;
			CWnd *pWnd = GetParent();
			if(pWnd)
				nCurSelItem = ((CManageBaseDlg*)pWnd)->GetCurSelItem();
			
			if (bIsDelCol)
			{
				POSITION pos = m_lstPSels.GetHeadPosition();
				if (pos != NULL)
				{
					m_lstPSels.GetNext(pos)->OnLBClick(point);
				}
				
				return;			
			}

			m_bMulSel = TRUE;
			m_vValueBeforeCheck = pHit->GetValue(icol);

			CList<CLVLPropItem*, CLVLPropItem*> lstSave;
			for( POSITION pos0 = m_lstPSels.GetHeadPosition(); pos0!=NULL; )
			{
				lstSave.AddTail(m_lstPSels.GetNext(pos0));
			}
			
			for (int i=0; i<m_lstPSels.GetCount(); i++)
			{
				POSITION pos = m_lstPSels.FindIndex(i);
				if (pos != NULL)
				{
					CLVLPropItem *pProp = m_lstPSels.GetAt(pos);

					// 保留一项
					m_lstPSels.RemoveAll();
					m_lstPSels.AddTail(pProp);
					
					pProp->OnLBClick(point);
					//恢复
					m_lstPSels.RemoveAll();
					for( POSITION pos1 = lstSave.GetHeadPosition(); pos1!=NULL; )
					{
						m_lstPSels.AddTail(lstSave.GetNext(pos1));
					}
				}
				
			}
	
		}
	}
	else if (icol>=0 && GetColumn(icol).Type==CLVLPropColumn::COLOR && GetSelectedCount()>1 &&bOldSel)
	{
		SetSelColumn(icol);
		if( GetSelectedCount()>1 && GetColumn(icol).Type==CLVLPropColumn::COLOR )
		{
			EnsureVisible (pHit);			
			pHit->OnLBClick (point);
		}
	}
	else if (icol>=0 && GetColumn(icol).Type==CLVLPropColumn::COMBO && GetSelectedCount()>1 &&bOldSel)
	{
		SetSelColumn(icol);
		if( GetSelectedCount()>1 && GetColumn(icol).Type==CLVLPropColumn::COMBO )
		{
			EnsureVisible (pHit);			
			pHit->OnLBClick (point);
		}
	}
	else if (icol>=0 && GetColumn(icol).Type==CLVLPropColumn::EDIT && GetSelectedCount()>1 &&bOldSel)
	{
		m_ptLastLBD = point;
		SetSelColumn(icol);
		if( GetSelectedCount()>1 && GetColumn(icol).Type==CLVLPropColumn::EDIT )
		{
			EnsureVisible (pHit);
			// 层名列不弹出编辑框
			if (icol != 0)
				pHit->OnLBClick (point);

			// 当点击只读行，只读列或第一列时为单选
			if (GetColumn(icol).bReadOnly || (pHit->IsReadOnly() && GetColumn(icol).bReadOnlyRefItem) || pHit->IsColReadOnly(icol))
			{
				if (!(nFlags&MK_CONTROL)&&  !(nFlags&MK_SHIFT))
					SelectItem(pHit);
				else if (nFlags&MK_CONTROL)
				{
					DeselectItem(pHit);
				}
			}
		}		
		
	}
	else
	{
		if (!(nFlags&MK_CONTROL)&&  !(nFlags&MK_SHIFT))
		{
			m_bMulSel = FALSE;
			m_ptLastLBD = point;
// 			CLVLPropItem* pHit = HitTest (ptTest);
// 			m_nStart = -1;
			SelectItem(pHit);
			
			EnsureVisible (pHit);
			if( icol>=0 )SetSelColumn(icol);
			else 
				return;
			
			if( bOldSel || bIsDelCol || IsParentLayerVisible())
			{
				pHit->OnLBClick (point);
			}


		}
		else if ((nFlags&MK_CONTROL)&&!(nFlags&MK_SHIFT))
		{
			m_ptLastLBD = point;
			SelectItem(pHit,FALSE);
			
			EnsureVisible (pHit);
			if( icol>=0 )SetSelColumn(icol);
			else 
				return;
			if( bOldSel )
			{
				pHit->OnLBClick (point);
			}
		}
		else if (!(nFlags&MK_CONTROL)&&(nFlags&MK_SHIFT))
		{
			if (GetSelectedCount()>0)
			{
				CLVLPropItem* pItem = GetSelectedItem(0);
				CRect rect = pItem->GetRect();

				CPoint ptTest = rect.CenterPoint();
				CPoint ptTest0 = ptTest;
				CRect rc(ptTest0,point);
				rc.NormalizeRect();				
			
				int dy = point.y>ptTest0.y?m_nRowHeight:-m_nRowHeight;
				// get the new selection
				CList<CLVLPropItem*,CLVLPropItem*> lstNewSels;
				
				while( 1 )
				{
					CLVLPropItem* pHit = HitTest (ptTest);
					if( pHit!=NULL )
					{
						lstNewSels.AddTail( pHit );
					}			
					
					ptTest.y += dy;
					if( ptTest.y<min(point.y,ptTest0.y) || ptTest.y>max(point.y,ptTest0.y) )
					{
						if( abs(ptTest.y-point.y)<abs(dy) )
						{
							ptTest.y = point.y;
							continue;
						}
						break;
					}
				}
				
				// deselect items not among the new selection 
				for( POSITION pos = m_lstPSels.GetHeadPosition(); pos!=NULL; )
				{
					CLVLPropItem* pProp = m_lstPSels.GetNext(pos);
					if( lstNewSels.Find(pProp)==NULL )
					{
						DeselectItem(pProp,FALSE);
						pos = m_lstPSels.GetHeadPosition();
					}
				}

				CLVLPropItem *pPropLast = NULL;
				
				// select items not among the old selection 
				for( pos = lstNewSels.GetHeadPosition(); pos!=NULL; )
				{
					CLVLPropItem* pProp = lstNewSels.GetNext(pos);
					if( m_lstPSels.Find(pProp)==NULL )
					{
						SelectItem(pProp,FALSE,FALSE);
						pPropLast = pProp;
					}
				}

				if( pPropLast )
				{
					EnsureVisible(pPropLast);
				}
				RedrawWindow();
			}
			else
			{
				SelectItem(pHit);
				
				EnsureVisible (pHit);
				if( icol>=0 )SetSelColumn(icol);
				else 
					return;
				if( bOldSel )
				{
					pHit->OnLBClick (point);
				}
			}
			
		}		
	}

	if(pWnd)
	{
		((CManageBaseDlg*)pWnd)->UpdateStatusString();
	}

}
//************************************************************************************
//响应鼠标右键 全部选中
void CLVLPropList::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CWnd::OnRButtonDown(nFlags, point);

	SetFocus ();
	

	CLVLPropItem* pHit = HitTest (point);

	//选中的为过滤器时	
	if( pHit==NULL && m_pFilterItem!=NULL && m_pFilterItem->HitTest(point) )
	{
		int icol = HitColumn(point);
		if( icol<0 )return;
		
		_variant_t var;
		var.vt = VT_EMPTY;
		if (icol == 3) //颜色选项
		{
			var.vt = VT_INT;
			var = (long)SELNULL;// -2 控制状态
		}
		m_pFilterItem->SetValue(var,icol);
		FilterPropItems();
		//m_pFilterItem
	//	AfxMessageBox("鼠标右键,全部选中!",MB_OK);
		return;

	}
	else //选中的为普通项时
	{
		if(pHit)
		{
			if(!IsInSelection(pHit))
			{
				SelectItem (pHit);
				//
				EnsureVisible (pHit);
			}
		
		}	
	}
	CWnd *pWnd = GetParent();
	if(pWnd)
	{
		((CManageBaseDlg*)pWnd)->RButtonDown(nFlags, point);	
	}
}
//******************************************************************************************
BOOL CLVLPropList::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CWnd::Create(NULL, _T(""), dwStyle, rect, pParentWnd, nID, NULL);
}
//******************************************************************************************
BOOL CLVLPropList::EditItem (CLVLPropItem* pProp,CLVLPropColumn* Pc)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pProp);

	if (!EndEditItem ())
	{
		return FALSE;
	}

	switch(Pc->Type) 
	{
	case CLVLPropColumn::EDIT:
		if (pProp->OnEdit (Pc))
		{
			RedrawWindow (pProp->m_Rect);
			m_pInEditItem = pProp;
			SetCapture ();
		}
		break;
	case CLVLPropColumn::COLOR:
		if (pProp->OnColor(Pc))
		{
			m_pInEditItem = pProp;			
		}
		break;
	case CLVLPropColumn::COMBO:
		if (pProp->OnCombo(Pc))
		{
			m_pInEditItem = pProp;	
		}
		break;
	default: m_pInEditItem = NULL;
	}
	return TRUE;
}
//******************************************************************************************
BOOL CLVLPropList::EndEditItem (BOOL bUpdateData/* = TRUE*/)
{
	ASSERT_VALID (this);

	BOOL bIsNeedUpdate	= false;

	CLVLPropItem *pEditItem = m_pInEditItem;
	m_pInEditItem = NULL;

	if (pEditItem == NULL)
	{
		return TRUE;
	}

	ASSERT_VALID (pEditItem);

	if (!pEditItem->m_bInPlaceEdit && !pEditItem->m_bInColorSel && !pEditItem->m_bInComboBox)
	{
		return TRUE;
	}
	_variant_t newValue;
	if(pEditItem->m_bInColorSel)
	{
		newValue = (long)pEditItem->m_WndInColor.GetColor();
	}
	else if (pEditItem->m_bInPlaceEdit)
	{
		CString str;
		pEditItem->m_pWndInPlace->GetWindowText(str);
		newValue = (_bstr_t)(LPCTSTR)str;
	}
	else if (pEditItem->m_bInComboBox)
	{
		CString str;
		int index = pEditItem->m_WndComboBox.GetCurSel();
		if (index == -1) return FALSE;
		pEditItem->m_WndComboBox.GetLBText(index,str);
		newValue = (_bstr_t)(LPCTSTR)str;
	}

	//过滤栏不作重名检查
	if ( pEditItem==m_pFilterItem || ((CManageBaseDlg*)GetParent())->IsValidNewValue(GetColumn(m_nSelColumn).FieldName,newValue) )
	{
		if (bUpdateData)
		{
			CLVLPropColumn col = GetColumn(m_nSelColumn);
			if (!pEditItem->OnUpdateValue (&col))
			{
				return FALSE;
			}
			bIsNeedUpdate	= true;
		}
	}
	else
	{
		AfxMessageBox(IDS_MODIFY_LAYERINVALID);
	}	

	if (pEditItem)
	{
		if (!pEditItem->OnEndEdit ())
		{
			return FALSE;
		}

		CLVLPropColumn col = GetColumn(m_nSelColumn);
		if (GetSelectedCount()>1 && !col.bReadOnly && m_nSelColumn!=0)
		{
			POSITION pos;					
			
		//	int idx = GetColumnIndexByField(FIELDNAME_LAYCOLOR);
			BOOL bIsNeedUpdate	= false;
			CRect rect;
			rect.SetRectEmpty();
			for(  pos = m_lstPSels.GetHeadPosition(); pos!=NULL; )
			{
				CLVLPropItem *pEditItem = m_lstPSels.GetNext(pos);	
				
				if (pEditItem == NULL || (pEditItem->IsReadOnly() && col.bReadOnlyRefItem) || pEditItem->IsColReadOnly(m_nSelColumn))
				{
					continue;
				}
				
				ASSERT_VALID (pEditItem);
				
				pEditItem->SetValue(newValue,m_nSelColumn);			
				
				bIsNeedUpdate	= true;			
				
				rect+=pEditItem->m_Rect;				
			}
			ReleaseCapture ();			
			if (bIsNeedUpdate)
			RedrawWindow (rect);		
		}
	}

	ReleaseCapture ();
	
	if (m_pInEditItem)
		RedrawWindow (pEditItem->m_Rect);

	m_pInEditItem = NULL;

	return TRUE;
}

//选择属性字段
BOOL CLVLPropList::OnSelFeaField()
{
	return TRUE;
}


//*******************************************************************************************
BOOL CLVLPropList::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_RBUTTONDOWN)
	{
		/*
		if (pMsg->hwnd == m_wndHeader.m_hWnd)
		{
			int	i = 0;
			TRACE("Message:WM_RBUTTONDOWN \n");
			LRESULT	lRet = 0;
			NMHEADER	head;
			memset(&head,0,sizeof(NMHEADER));
			head.iButton = 1;
			
			for (i = 0; i < m_wndHeader.GetItemCount(); i++)
			{
				RECT		rt;
				m_wndHeader.GetItemRect(i,&rt);
				POINT		pt;
				pt.x	= pMsg->pt.x;
				pt.y	= pMsg->pt.y;
				m_wndHeader.ScreenToClient(&pt);
				if (::PtInRect(&rt,pt))
				{
					head.iItem = i;
					break;
				}
			}
			
			OnHeaderClick((NMHDR*)&head,&lRet);
		}*/
	}
	
   	switch (pMsg->message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_NCLBUTTONDOWN:
	case WM_NCRBUTTONDOWN:
	case WM_NCMBUTTONDOWN:
	case WM_NCLBUTTONUP:
	case WM_NCRBUTTONUP:
	case WM_NCMBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_COMMAND:
		{
			if(pMsg->wParam == ID_SELECT_FEAFIELD)
			{
				return OnSelFeaField();
			}
			break;
		}
	}

	if (m_pInEditItem != NULL && m_pInEditItem->m_bInPlaceEdit != NULL)
	{
		ASSERT_VALID (m_pInEditItem);

		if (pMsg->message == WM_KEYDOWN)
		{
			switch (pMsg->wParam)
			{
			case VK_RETURN:
				if (!EndEditItem ())
				{
					MessageBeep ((UINT)-1);
				}

				SetFocus ();
				break;

			case VK_ESCAPE:
				EndEditItem (FALSE);
				SetFocus ();
				break;

			case VK_DOWN:
			default:

				return FALSE;
			}

			return TRUE;
		}
		else if (pMsg->message >= WM_MOUSEFIRST &&
				 pMsg->message <= WM_MOUSELAST)
		{
			CRect rectEdit;

			CPoint ptCursor;
			::GetCursorPos (&ptCursor);
			ScreenToClient (&ptCursor);

			if (rectEdit.PtInRect (ptCursor) &&
				pMsg->message == WM_LBUTTONDBLCLK)
			{
				if (m_pInEditItem->OnDblClick (ptCursor))
				{
					return TRUE;
				}
			}
			
		}
	}
	
	return CWnd::PreTranslateMessage(pMsg);
}
//******************************************************************************************
void CLVLPropList::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);
	
	if (pOldWnd != NULL && pOldWnd->IsKindOf(RUNTIME_CLASS(CComboBox)))
	{
		pOldWnd->ShowWindow(SW_HIDE);
	}

	m_bFocused = TRUE;

	if(pOldWnd==NULL&& m_pInEditItem!=NULL && m_nSelColumn>=0 )
	{
		CLVLPropColumn col = GetColumn(m_nSelColumn);
		if(col.Type == CLVLPropColumn::COLOR || col.Type == CLVLPropColumn::COMBO)
			EndEditItem(TRUE);
	}
}
//******************************************************************************************
void CLVLPropList::OnKillFocus(CWnd* pNewWnd) 
{
	if (!IsChild (pNewWnd))
	{
		if (m_pInEditItem == NULL/* || m_pInEditItem->OnKillFocus (pNewWnd)*/)
		{
			EndEditItem (FALSE);
//			m_bFocused = FALSE;

			if (m_pInEditItem != NULL)
			{
				RedrawWindow (m_pInEditItem->m_Rect);
			}
		}
	}
	else if (pNewWnd->IsKindOf(RUNTIME_CLASS(CMFCColorButton)) || pNewWnd->IsKindOf(RUNTIME_CLASS(CComboBox)))
	{
		EndEditItem (TRUE);
		if (pNewWnd->IsKindOf(RUNTIME_CLASS(CMFCColorButton)))
		{
			m_bIgnoreColSel = TRUE;
		}
	}

	CWnd::OnKillFocus(pNewWnd);
}
//******************************************************************************************
//void CLVLPropList::OnStyleChanged (int nStyleType, LPSTYLESTRUCT lpStyleStruct)
//{
//	CWnd::OnStyleChanged (nStyleType, lpStyleStruct);
//	AdjustLayout ();
//}
//******************************************************************************************
UINT CLVLPropList::OnGetDlgCode() 
{
	return DLGC_WANTARROWS | DLGC_WANTCHARS;
}
//******************************************************************************************
void CLVLPropList::SetScrollSizes ()
{
	ASSERT_VALID (this);

	if (m_wndScrollHorz.GetSafeHwnd () != NULL)
	{
		if (m_nColWidth == 0)
		{
			m_nHorzScrollPage = 0;
			m_nHorzScrollTotal = 0;
			m_nHorzScrollOffset = 0;
		}
		else
		{
			m_nHorzScrollPage = m_rectList.Width()/m_nColWidth;
			m_nHorzScrollTotal = m_arrColumns.GetSize();
			
			if (m_nHorzScrollTotal <= m_nHorzScrollPage)
			{
				m_nHorzScrollPage = 0;
				m_nHorzScrollTotal = 0;
			}
			
			m_nHorzScrollOffset = min (m_nHorzScrollOffset, m_nHorzScrollTotal);
		}
		
		SCROLLINFO si;
		
		ZeroMemory (&si, sizeof (SCROLLINFO));
		si.cbSize = sizeof (SCROLLINFO);
		
		si.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
		si.nMin = 0;
		si.nMax = m_nHorzScrollTotal;
		si.nPage = m_nHorzScrollPage;
		si.nPos = m_nHorzScrollOffset;
		
		//m_wndScrollHorz.SetScrollInfo(&si,TRUE);
		SetScrollInfo (SB_HORZ, &si, TRUE);
		m_wndScrollHorz.EnableScrollBar (m_nHorzScrollTotal > 0 ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH);
		m_wndScrollHorz.EnableWindow ();
	}

	if (m_wndScrollVert.GetSafeHwnd () != NULL)
	{
		if (m_nRowHeight == 0)
		{
			m_nVertScrollPage = 0;
			m_nVertScrollTotal = 0;
			m_nVertScrollOffset = 0;
		}
		else
		{

			m_nVertScrollPage = m_rectList.Height () / m_nRowHeight - 1;
			m_nVertScrollTotal = GetTotalItems ();

			if (m_nVertScrollTotal <= m_nVertScrollPage)
			{
				m_nVertScrollPage = 0;
				m_nVertScrollTotal = 0;
			}

			m_nVertScrollOffset = min (m_nVertScrollOffset, m_nVertScrollTotal);
		}

		SCROLLINFO si;

		ZeroMemory (&si, sizeof (SCROLLINFO));
		si.cbSize = sizeof (SCROLLINFO);

		si.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
		si.nMin = 0;
		si.nMax = m_nVertScrollTotal;
		si.nPage = m_nVertScrollPage;
		si.nPos = m_nVertScrollOffset;

		//m_wndScrollVert.SetScrollInfo(&si,TRUE);
		SetScrollInfo (SB_VERT, &si, TRUE);
		m_wndScrollVert.EnableScrollBar (m_nVertScrollTotal > 0 ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH);
		m_wndScrollVert.EnableWindow ();
	}

	
}
//******************************************************************************************
int CLVLPropList::GetTotalItems () const
{
	ASSERT_VALID (this);

	int nCount = 0;

	for (POSITION pos = m_lstProps.GetHeadPosition (); pos != NULL;)
	{
		CLVLPropItem* pProp = m_lstProps.GetNext (pos);
		ASSERT_VALID (pProp);

 		nCount++;
	}

	return nCount;
}

int CLVLPropList::GetTotalColWid()const
{
	int nCount = m_arrColumns.GetSize();
	int nWid = 0;

	for( int i=0; i<nCount; i++)
	{
		CLVLPropColumn col = m_arrColumns.GetAt(i);
		nWid += col.Rect.Width();
	}

	return nWid;
}

//******************************************************************************************
void CLVLPropList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/) 
{
	EndEditItem (FALSE);

	int nPrevOffset = m_nVertScrollOffset;

	switch (nSBCode)
	{
	case SB_LINEUP:
		m_nVertScrollOffset--;
		break;

	case SB_LINEDOWN:
		m_nVertScrollOffset++;
		break;

	case SB_TOP:
		m_nVertScrollOffset = 0;
		break;

	case SB_BOTTOM:
		m_nVertScrollOffset = m_nVertScrollTotal;
		break;

	case SB_PAGEUP:
		m_nVertScrollOffset -= m_nVertScrollPage;
		break;

	case SB_PAGEDOWN:
		m_nVertScrollOffset += m_nVertScrollPage;
		break;

	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_nVertScrollOffset = nPos;
		break;

	default:
		return;
	}

	m_nVertScrollOffset = min (max (0, m_nVertScrollOffset), 
		m_nVertScrollTotal - m_nVertScrollPage + 1);

	if (m_nVertScrollOffset == nPrevOffset)
	{
		return;
	}

	SetScrollPos (SB_VERT, m_nVertScrollOffset);

	ReposProperties ();

	int dy = m_nRowHeight * (nPrevOffset - m_nVertScrollOffset);
	ScrollWindow (0, dy, m_rectList, m_rectList);


	if (m_pInEditItem != NULL)
	{
		ASSERT_VALID (m_pInEditItem);
	}

}

void CLVLPropList::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/) 
{
	EndEditItem (FALSE);
	
	int nPrevOffset = m_nHorzScrollOffset;
	
	switch (nSBCode)
	{
	case SB_LINELEFT:
		m_nHorzScrollOffset--;
		break;
		
	case SB_LINERIGHT:
		m_nHorzScrollOffset++;
		break;
		
	case SB_LEFT:
		m_nHorzScrollOffset = 0;
		break;
		
	case SB_RIGHT:
		m_nHorzScrollOffset = m_nHorzScrollTotal;
		break;
		
	case SB_PAGELEFT:
		m_nHorzScrollOffset -= m_nHorzScrollPage;
		break;
		
	case SB_PAGERIGHT:
		m_nHorzScrollOffset += m_nHorzScrollPage;
		break;
		
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_nHorzScrollOffset = nPos;
		break;
		
	default:
		return;
	}
	
	m_nHorzScrollOffset = min (max (0, m_nHorzScrollOffset), 
		m_nHorzScrollTotal - m_nHorzScrollPage + 1);
	
	if (m_nHorzScrollOffset == nPrevOffset)
	{
		return;
	}
	
	SetScrollPos (SB_HORZ, m_nHorzScrollOffset);
	
	ReposProperties ();
	
	int dx = m_nColWidth * (nPrevOffset - m_nHorzScrollOffset);

// 	 CRect rect = m_rectList;
// 	 rect.top -= m_nRowHeight;
// 	 CRect rect1 = rect;
// 	 rect1.right += m_nColWidth*m_nHorzScrollOffset;
	ScrollWindow (dx, 0);
	
	AdjustLayout();

	if (m_pInEditItem != NULL)
	{
		ASSERT_VALID (m_pInEditItem);
	}

// 	CRect rect = m_rectList;
// 	rect.top -= m_nRowHeight;
// 	rect.right += m_nColWidth*m_nHorzScrollOffset;
// 	InvalidateRect(rect);
	Invalidate();
	
}

//*******************************************************************************************
CScrollBar* CLVLPropList::GetScrollBarCtrl(int nBar) const
{
	if (nBar == SB_VERT && m_wndScrollVert.GetSafeHwnd () != NULL)
	{
		return (CScrollBar*)&m_wndScrollVert;
	}
	
	if (nBar == SB_HORZ && m_wndScrollHorz.GetSafeHwnd () != NULL)
	{
		return (CScrollBar*)&m_wndScrollHorz;
	}

	return NULL;
}
//******************************************************************************************
BOOL CLVLPropList::OnMouseWheel(UINT /*nFlags*/, short zDelta, CPoint /*pt*/) 
{
	int nSteps = abs(zDelta) / WHEEL_DELTA;

	if (m_nVertScrollTotal <= 0)
		return true;

	for (int i = 0; i < nSteps; i++)
	{
		OnVScroll (zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0, NULL);
	}

	return TRUE;
}
//*******************************************************************************************
void CLVLPropList::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnLButtonDown(nFlags,point);
	OnLButtonUp(nFlags,point);
/*	CWnd::OnLButtonDblClk(nFlags, point);

	if (m_pSel == NULL)
	{
		return;
	}

	ASSERT_VALID (m_pSel);

	SetFocus ();
	m_pSel->OnDblClick (point);*/
}
//*******************************************************************************************
BOOL CLVLPropList::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (nHitTest == HTCLIENT)
	{
		CPoint point;

		::GetCursorPos (&point);
		ScreenToClient (&point);

		if (point.y <= m_rectList.bottom)
		{
			CLVLPropItem* pHit = HitTest (point);

			if (pHit != NULL && pHit == m_pInEditItem &&
				pHit->OnSetCursor ())
			{
				return TRUE;
			}
		}
	}
	
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}
//******************************************************************************************
void CLVLPropList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (m_lstProps.IsEmpty ())
	{
		CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}

	switch (nChar)
	{
	case VK_UP:
		{
			int selNum = m_lstPSels.GetCount();

			if (selNum == 0)
			{
				SelectItem ( m_lstProps.GetHead ());
				OnVScroll (SB_TOP, 0, NULL);
			}
			else
			{
				CLVLPropItem *pLastSelItem = m_lstPSels.GetTail();

				// 检测最后选中的项在所有选中项的位置：最前，最后，中间

				CLVLPropItem *pUpItem = NULL, *pDownItem = NULL;
				POSITION pos = m_lstPSels.GetHeadPosition();
				while (pos != NULL)
				{
					CLVLPropItem *pItem = m_lstPSels.GetNext(pos);
					if (pUpItem == NULL || pUpItem->m_Rect.top > pItem->m_Rect.top)
					{
						pUpItem = pItem;
					}

					if (pDownItem == NULL || pDownItem->m_Rect.top < pItem->m_Rect.top)
					{
						pDownItem = pItem;
					}
				}

				if (pLastSelItem == pUpItem)
				{
					// Select prev. item:
					CPoint point (pUpItem->m_Rect.right - 1, pUpItem->m_Rect.top - 2);
					
					CLVLPropItem* pHit = HitTest (point);
					
					if (pHit != NULL)
					{
						SelectItem (pHit,!(GetKeyState(VK_SHIFT)<0));
						EnsureVisible (pHit);
					}
				}
				else if (pLastSelItem == pDownItem)
				{
					if (IsInSelection(pDownItem))
					{
						DeselectItem(pDownItem);
					}
				}
				else
				{
					DeselectAll();
					SelectItem (pLastSelItem);

					CPoint point (pLastSelItem->m_Rect.right - 1, pLastSelItem->m_Rect.top - 2);
					
					CLVLPropItem* pHit = HitTest (point);
					
					if (pHit != NULL)
					{
						SelectItem (pHit,!(GetKeyState(VK_SHIFT)<0));
						EnsureVisible (pHit);
					}
				}
			}

			/*if (m_pInEditItem == NULL)
			{
				SelectItem ( m_lstProps.GetHead ());
				OnVScroll (SB_TOP, 0, NULL);
				return;
			}

			// Select prev. item:
			CPoint point (m_pInEditItem->m_Rect.right - 1, m_pInEditItem->m_Rect.top - 2);

			CLVLPropItem* pHit = HitTest (point);;

			if (pHit != NULL)
			{
				SelectItem (pHit);
				EnsureVisible (pHit);
			}*/
		}
		return;
	case VK_LEFT:
		{
			if (m_pInEditItem == NULL)
			{
				SelectItem ( m_lstProps.GetHead ());
				OnVScroll (SB_LINELEFT, 0, NULL);
			}
			return;
		}
	case VK_RIGHT:
		{
			if (m_pInEditItem == NULL)
			{
				SelectItem ( m_lstProps.GetHead ());
				OnVScroll (SB_LINERIGHT, 0, NULL);
			}
			return;
		}

	case VK_DOWN:
		{
			int selNum = m_lstPSels.GetCount();
			
			if (selNum == 0)
			{
				SelectItem ( m_lstProps.GetHead ());
				OnVScroll (SB_TOP, 0, NULL);
			}
			else
			{
				CLVLPropItem *pLastSelItem = m_lstPSels.GetTail();
				
				// 检测最后选中的项在所有选中项的位置：最前，最后，中间
				
				CLVLPropItem *pUpItem = NULL, *pDownItem = NULL;
				POSITION pos = m_lstPSels.GetHeadPosition();
				while (pos != NULL)
				{
					CLVLPropItem *pItem = m_lstPSels.GetNext(pos);
					if (pUpItem == NULL || pUpItem->m_Rect.top > pItem->m_Rect.top)
					{
						pUpItem = pItem;
					}
					
					if (pDownItem == NULL || pDownItem->m_Rect.top < pItem->m_Rect.top)
					{
						pDownItem = pItem;
					}
				}
				
				if (pLastSelItem == pDownItem)
				{
					// Select next item:
					CPoint point (pDownItem->m_Rect.right - 1, pDownItem->m_Rect.bottom + 2);
					
					CLVLPropItem* pHit = HitTest (point);
					
					if (pHit != NULL)
					{
						SelectItem (pHit,!(GetKeyState(VK_SHIFT)<0));
						EnsureVisible (pHit);
					}
				}
				else if (pLastSelItem == pUpItem)
				{
					if (IsInSelection(pUpItem))
					{
						DeselectItem(pUpItem);
					}
				}
				else
				{
					DeselectAll();
					SelectItem (pLastSelItem);
					
					CPoint point (pLastSelItem->m_Rect.right - 1, pLastSelItem->m_Rect.bottom + 2);
					
					CLVLPropItem* pHit = HitTest (point);
					
					if (pHit != NULL)
					{
						SelectItem (pHit,!(GetKeyState(VK_SHIFT)<0));
						EnsureVisible (pHit);
					}
				}
			}

			/*if (m_pInEditItem == NULL)
			{
				SelectItem ( m_lstProps.GetHead ());
				OnVScroll (SB_TOP, 0, NULL);
				return;
			}

			// Select next item:
			CPoint point (m_pInEditItem->m_Rect.right - 1, m_pInEditItem->m_Rect.bottom + 2);

			CLVLPropItem* pHit = HitTest (point);

			if (pHit != NULL)
			{
				SelectItem (pHit);
				EnsureVisible (pHit);
			}*/
		}
		return;

	case VK_NEXT:
		{
			if (m_pInEditItem == NULL)
			{
				SelectItem (m_lstProps.GetHead ());
				OnVScroll (SB_TOP, 0, NULL);
				return;
			}

			if (m_nVertScrollPage != 0)
			{
				EnsureVisible (m_pInEditItem);

				CPoint point (m_pInEditItem->m_Rect.right - 1, 
					m_pInEditItem->m_Rect.top + m_nVertScrollPage * m_nRowHeight);

				CLVLPropItem* pHit = HitTest (point);
				if (pHit != NULL)
				{
					SelectItem (pHit);
					OnVScroll (SB_PAGEDOWN, 0, NULL);
					return;
				}
			}
		}

	case VK_END:
		{
			CLVLPropItem* pLastProp = NULL;
			
			pLastProp = m_lstProps.GetTail ();
			ASSERT_VALID (pLastProp);

			SelectItem (pLastProp);
			OnVScroll (SB_BOTTOM, 0, NULL);
		}
		return;

	case VK_PRIOR:
		{
			if (m_pInEditItem != NULL && m_nVertScrollPage != 0)
			{
				EnsureVisible (m_pInEditItem);

				CPoint point (m_pInEditItem->m_Rect.right - 1, 
					m_pInEditItem->m_Rect.top - m_nVertScrollPage * m_nRowHeight);

				CLVLPropItem* pHit = HitTest (point);
				if (pHit != NULL)
				{
					SelectItem (pHit);
					OnVScroll (SB_PAGEUP, 0, NULL);
					return;
				}
			}
		}

	case VK_HOME:
		SelectItem ( m_lstProps.GetHead ());
		OnVScroll (SB_TOP, 0, NULL);
		return;
	}
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
//******************************************************************************************
void CLVLPropList::EnsureVisible (CLVLPropItem* pProp)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pProp);

	CRect rect = pProp->m_Rect;

	if (m_nRowHeight == 0)
	{
		goto HScroll;
	}

	if (rect.top >= m_rectList.top - 1 && rect.bottom <= m_rectList.bottom)
	{
		goto HScroll;
	}

	if (rect.top < m_rectList.top - 1 && rect.bottom >= m_rectList.top - 1)
	{
		OnVScroll (SB_LINEUP, 0, NULL);
	}
	else if (rect.bottom > m_rectList.bottom && rect.top <= m_rectList.bottom)
	{
		OnVScroll (SB_LINEDOWN, 0, NULL);
	}
	else
	{
		OnVScroll (SB_THUMBPOSITION, rect.top / m_nRowHeight - m_nVertScrollOffset, NULL);
	}

HScroll:

	if (m_nColWidth == 0)
	{
		return;
	}
	
	if (rect.left <= m_rectList.left && rect.right >= m_rectList.right)
	{
		return;
	}
	
	if (rect.left <= m_rectList.left-1 && rect.right >= m_rectList.left)
	{
		OnHScroll (SB_LINERIGHT, 0, NULL);
	}
	else if (rect.right >= m_rectList.right-1 && rect.left <= m_rectList.right)
	{
		OnHScroll (SB_LINELEFT, 0, NULL);
	}
	else
	{
		OnHScroll (SB_THUMBPOSITION, rect.right / m_nColWidth - m_nHorzScrollOffset, NULL);
	}
}
//******************************************************************************************
void CLVLPropList::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CWnd::OnChar(nChar, nRepCnt, nFlags);

	if (m_pInEditItem == NULL )
	{
		return;
	}

	ASSERT_VALID (m_pInEditItem);
}

//****************************************************************************************
BOOL CLVLPropList::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	BOOL bRes = CWnd::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT (pNMHDR != NULL);

	// wParam is one for Header ctrl
	if( wParam == ID_HEADER )
	{
		switch (pNMHDR->code)
		{
		case NM_RCLICK:
			{
				if ( !((CManageBaseDlg*)GetParent())->CanModifyCheckState() && !IsParentLayerVisible() )
				{
					return bRes;
				}
				// Right button was clicked on header
				CPoint pt(GetMessagePos());
				m_wndHeader.ScreenToClient(&pt);
				
				// Determine the column index
				int icol = -1;
				CRect rcCol;
				for( int i=0; Header_GetItemRect(m_wndHeader.m_hWnd, i, &rcCol); i++ )
				{
					if( rcCol.PtInRect( pt ) )
					{
						icol = i;
						break;
					}
				}
				
				if (icol == -1)
				{
					return bRes;
				}
				
				m_nSelHeadColumn = icol;
				
				CLVLPropColumn col = GetColumn(icol);
				
				if( icol>=0 && col.Type==CLVLPropColumn::CHECK && !col.bReadOnly && stricmp(col.FieldName,FIELDNAME_LAYERDEL)!=0)
				{
					CMenu obMenu;
					obMenu.LoadMenu(IDR_LAYERHEADMENU); 
					
					CMenu* pPopupMenu = obMenu.GetSubMenu(0);
					ASSERT(pPopupMenu);
					CPoint obCursorPoint = (0, 0);
					
					GetCursorPos(&obCursorPoint);
					
					// Track the popup menu
					pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, obCursorPoint.x, 
						obCursorPoint.y, this);
				}
				break;
			}
		case HDN_ENDTRACKW:
        case HDN_ENDTRACKA:
			{
				AdjustLayout();
				break;
			}
		}
	}

	return bRes;

}
//****************************************************************************************
void CLVLPropList::OnDestroy() 
{
	RemoveAll();

	CWnd::OnDestroy();
}
//****************************************************************************************
void CLVLPropList::OnMouseMove(UINT nFlags, CPoint point) 
{
	CWnd::OnMouseMove(nFlags, point);

	if (m_bTracking)
	{
		TrackHeader (point.x);
	}

	if (m_bIgnoreColSel || (m_pInEditItem && (m_pInEditItem->m_bInPlaceEdit || m_pInEditItem->m_bInComboBox || m_pInEditItem->m_bInColorSel)))
	{
		m_bIgnoreColSel = FALSE;
		return;
	}

	if (IsParentLayerVisible() && nFlags&MK_LBUTTON)
	{
		CPoint ptTest = m_ptLastLBD;
		CRect rc(m_ptLastLBD,point);
		rc.NormalizeRect();
		
		int dy = point.y>m_ptLastLBD.y?m_nRowHeight:-m_nRowHeight;
		
		// get the new selection
		CList<CLVLPropItem*,CLVLPropItem*> lstNewSels;
		
		while( 1 )
		{
			CLVLPropItem* pHit = HitTest (ptTest);
			if( pHit!=NULL && !pHit->GetRect().PtInRect(m_ptLastLBD) && m_lstPSels.Find(pHit)==NULL)
			{
				SetSelColumn(1);		
				pHit->OnUpdateValue(&GetColumn(1));
				m_lstPSels.AddTail(pHit);

				InvalidateRect(pHit->m_Rect);
			}			
			
			ptTest.y += dy;
			if( ptTest.y<min(point.y,m_ptLastLBD.y) || ptTest.y>max(point.y,m_ptLastLBD.y) )
			{
				if( abs(ptTest.y-point.y)<abs(dy) )
				{
					ptTest.y = point.y;
					continue;
				}
				break;
			}
		}

		return;
	}

	if( nFlags&MK_LBUTTON )
	{
		CPoint ptTest = m_ptLastLBD;
		CRect rc(m_ptLastLBD,point);
		rc.NormalizeRect();

		int dy = point.y>m_ptLastLBD.y?m_nRowHeight:-m_nRowHeight;
		
		// get the new selection
		CList<CLVLPropItem*,CLVLPropItem*> lstNewSels;

		while( 1 )
		{
			CLVLPropItem* pHit = HitTest (ptTest);
			if( pHit!=NULL )
			{
				lstNewSels.AddTail( pHit );
			}			

			ptTest.y += dy;
			if( ptTest.y<min(point.y,m_ptLastLBD.y) || ptTest.y>max(point.y,m_ptLastLBD.y) )
			{
				if( abs(ptTest.y-point.y)<abs(dy) )
				{
					ptTest.y = point.y;
					continue;
				}
				break;
			}
		}

		// deselect items not among the new selection 
		for( POSITION pos = m_lstPSels.GetHeadPosition(); pos!=NULL; )
		{
			CLVLPropItem* pProp = m_lstPSels.GetNext(pos);
			if( lstNewSels.Find(pProp)==NULL )
			{
				DeselectItem(pProp);
				pos = m_lstPSels.GetHeadPosition();
			}
		}

		// select items not among the old selection 
		for( pos = lstNewSels.GetHeadPosition(); pos!=NULL; )
		{
			CLVLPropItem* pProp = lstNewSels.GetNext(pos);
			if( m_lstPSels.Find(pProp)==NULL )
			{
				SelectItem(pProp,FALSE);
				EnsureVisible(pProp);
			}
		}
	}
}
//*****************************************************************************************
void CLVLPropList::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonUp(nFlags, point);

	if (m_bTracking)
	{
		TrackHeader (-1);
		m_bTracking = FALSE;
		ReleaseCapture ();

		CRect rectClient;
		GetClientRect (rectClient);

// 		m_Property[0].nWidth = min (max (m_nRowHeight, point.x), rectClient.Width () - ::GetSystemMetrics (SM_CXHSCROLL) - 5);

		HDITEM hdItem;
		hdItem.mask = HDI_WIDTH ;
	}
}
//****************************************************************************************
void CLVLPropList::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);

	if (GetStyle() & WS_BORDER) 
	{
		lpncsp->rgrc[0].left++; 
		lpncsp->rgrc[0].top++ ;
		lpncsp->rgrc[0].right--;
		lpncsp->rgrc[0].bottom--;
	}

}

//****************************************************************************************
void CLVLPropList::OnNcPaint() 
{
	if (GetStyle () & WS_BORDER)
	{
		CMFCVisualManager::GetInstance()->OnDrawControlBorder(this);
	}

}


HBRUSH CLVLPropList::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);

	return hbr;
}



//插入一个用于输入过滤条件的行
BOOL CLVLPropList::InsertInputRow()
{
	return true;
}


//插入一个ITEM
BOOL CLVLPropList::AddItem(CLVLPropItem* pItem)
{
	if (!pItem)
		return FALSE;

	pItem->SetOwnerList(this);
	pItem->SetColumnCount(GetColumnCount());
	
	m_lstAllProps.AddTail(pItem);
	return TRUE;
}


//得到头控件实际需要的宽度
int  CLVLPropList::GetHeaderCtrlActualWidth()
{
	int	iWidth	= 0;
	int	iCount	= m_wndHeader.GetItemCount(); 

	for (int i = 0; i < iCount; i++)
	{	
		HDITEM	hd;
		hd.mask		= HDI_WIDTH;
		m_wndHeader.GetItem(i,&hd);
	
		iWidth += hd.cxy;
	}

	return iWidth;
}

void CLVLPropList::OnSelAll()
{
	m_lstPSels.RemoveAll();
	
	m_ptLastLBD = CPoint(-1,-1);

	int nItem = m_lstProps.GetCount(), i=0;

	CLVLPropColumn col = GetColumn(m_nSelHeadColumn);

	_variant_t var = (bool)TRUE;
	
	CArray<CLVLPropItem*,CLVLPropItem*> arrItems;
	arrItems.SetSize(nItem);
	
	for( POSITION pos = m_lstProps.GetHeadPosition(); pos!=NULL; )
	{
		CLVLPropItem* pProp = m_lstProps.GetNext(pos);
		arrItems.SetAt(i++,pProp);
	}

	((CManageBaseDlg*)GetParent())->ModifyCheckState(arrItems.GetData(),nItem,&col,m_nSelHeadColumn,var);

	/*if (IsParentLayerVisible())
	{
		((CDlgLayerVisible*)GetParent())->ModifyCheckState(arrItems.GetData(),nItem,&col,m_nSelHeadColumn,var);
	}
	else
	{
		((CLayersMgrDlg*)GetParent())->ModifyCheckState(arrItems.GetData(),nItem,&col,m_nSelHeadColumn,var);
	}*/

	RedrawWindow();
}

void CLVLPropList::OnUnSelAll()
{
	m_lstPSels.RemoveAll();
	
	m_ptLastLBD = CPoint(-1,-1);

	int nItem = m_lstProps.GetCount(), i=0;
	
	CLVLPropColumn col = GetColumn(m_nSelHeadColumn);
	
	_variant_t var = (bool)FALSE;
	
	CArray<CLVLPropItem*,CLVLPropItem*> arrItems;
	arrItems.SetSize(nItem);
	
	for( POSITION pos = m_lstProps.GetHeadPosition(); pos!=NULL; )
	{
		CLVLPropItem* pProp = m_lstProps.GetNext(pos);
		arrItems.SetAt(i++,pProp);
	}

	((CManageBaseDlg*)GetParent())->ModifyCheckState(arrItems.GetData(),nItem,&col,m_nSelHeadColumn,var);

	/*if (IsParentLayerVisible())
	{
		((CDlgLayerVisible*)GetParent())->ModifyCheckState(arrItems.GetData(),nItem,&col,m_nSelHeadColumn,var);
	}
	else
	{
		((CLayersMgrDlg*)GetParent())->ModifyCheckState(arrItems.GetData(),nItem,&col,m_nSelHeadColumn,var);
	}*/
	
	RedrawWindow();
}
