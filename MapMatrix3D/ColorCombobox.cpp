// ColorCombobox.cpp: implementation of the CColorCombobox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ColorCombobox.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CColorComboBox::CColorComboBox()
{
    current_zone_index = 0;
}

CColorComboBox::~CColorComboBox()
{
    for(int t=0; t<color_zones.size();++t)
	{
		DeleteObject((HBITMAP)color_zones[t].pzonebmp);
	}
    color_zones.clear();
}

BEGIN_MESSAGE_MAP(CColorComboBox, CComboBox)
//{{AFX_MSG_MAP(CColorComboBox)
ON_WM_DRAWITEM()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorComboBox message handlers

int CColorComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if( CComboBox::OnCreate( lpCreateStruct ) == -1 )
		return( -1 );
	//
	
	return( 0 );
}

void CColorComboBox::GetSelectColorZone(std::vector<COLORREF>& color_zone, int& type, CString& id)
{
    int index = current_zone_index;
	color_zone.insert(color_zone.begin(),color_zones[index].colors.begin(),color_zones[index].colors.end());
	type = color_zones[index].zone_type;
	id = color_zones[index].id;
}


void CColorComboBox::PreSubclassWindow()
{
	if(color_zones.size()==0)
      InitColor();
	//
	for(int i=0; i<color_zones.size();++i)
	{
		AddString("");
	}
	SetCurSel( current_zone_index );
	CComboBox::PreSubclassWindow();
}

void CColorComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
// TODO: Add your code to draw the specified item
	UINT id = lpDrawItemStruct->itemID;
	if(id!=0)
	{
		int a = 0;
	}
	if(id<0 || id>=color_zones.size())
	  return;
	//
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	CDC memdc;
	CBitmap membitmap;
	memdc.CreateCompatibleDC(NULL);
	CRect rec(&(lpDrawItemStruct->rcItem));
	rec.InflateRect(-1, -1);
	membitmap.CreateCompatibleBitmap(&dc, rec.Width(), rec.Height());
	memdc.SelectObject(&membitmap);
    
	BITMAP membitmap_info;
	membitmap.GetBitmap(&membitmap_info);

	CBitmap zone_bmp;
	zone_bmp.Detach();
	zone_bmp.Attach((HBITMAP)color_zones[id].pzonebmp);
	BITMAP zone_bmp_info;
	zone_bmp.GetBitmap(&zone_bmp_info);
	char* pzonebmp_data = (char*)zone_bmp_info.bmBits;


	double height_rate = (double)zone_bmp_info.bmHeight/(double)membitmap_info.bmHeight;
	double width_rate = (double)zone_bmp_info.bmWidth/(double)membitmap_info.bmWidth;
	for(int i=0; i<membitmap_info.bmHeight; ++i)
	{
		int m = height_rate*i;
		for(int j=0;j<membitmap_info.bmWidth;++j)
		{
            int n = width_rate*j;
			//
			char b = pzonebmp_data[m*zone_bmp_info.bmWidthBytes+n*3];
			char g = pzonebmp_data[m*zone_bmp_info.bmWidthBytes+n*3+1];
			char r = pzonebmp_data[m*zone_bmp_info.bmWidthBytes+n*3+2];
			memdc.SetPixel(j,i,RGB(r,g,b));
		}
	}

	zone_bmp.Detach();

	dc.BitBlt(rec.left, rec.top, rec.Width(), rec.Height(), &memdc, 0, 0, SRCCOPY);
	
	DeleteObject(membitmap.m_hObject);
    memdc.DeleteDC();
	//
	if (lpDrawItemStruct->itemState & ODS_SELECTED)
	{
			dc.DrawFocusRect(rec);
			current_zone_index = id;
	}
	//
	dc.Detach();
}

void CColorComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	ASSERT(lpMeasureItemStruct->CtlType==ODT_COMBOBOX);
	UINT id = lpMeasureItemStruct->itemData;
	if(id<0 || id>=color_zones.size())
	return;
	CRect rc;
	GetClientRect(&rc);
	lpMeasureItemStruct->itemWidth = rc.Width();
	lpMeasureItemStruct->itemHeight = 20;
}

void CColorComboBox::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CComboBox::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CColorComboBox::InitColor()
{
	FindColorZoneFile();
	GetColorZoneFromFile();
}


CString GetColorZonePath()  
{   
	CString    sPath;   
	GetModuleFileName(NULL,sPath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);   
	sPath.ReleaseBuffer    ();   
	int    nPos;   
	nPos=sPath.ReverseFind('\\');   
	sPath=sPath.Left(nPos);   
	nPos=sPath.ReverseFind('\\');   
	sPath=sPath.Left(nPos); 
	sPath+="\\Config\\color_zone";
	
	return    sPath;   
}
void CColorComboBox::FindColorZoneFile()
{
	color_file_paths.clear();
	//
	CString m_cstrFileList="";   
    CFileFind tempFind;   
	CString filter = GetColorZonePath() +"/*.bmp";
    BOOL bFound=tempFind.FindFile(filter);   //修改" "内内容给限定查找文件类型  
    CString strTmp;   //如果找到的是文件夹 存放文件夹路径  
    while(bFound)      //遍历所有文件  
    {   
        bFound=tempFind.FindNextFile(); //第一次执行FindNextFile是选择到第一个文件，以后执行为选择到下一个文件  
        if(!tempFind.IsDots() && tempFind.IsDirectory())   
            continue;   
        else   
        {   
            strTmp=tempFind.GetFilePath(); //保存文件名，包括后缀名  
            color_file_paths.push_back(strTmp);
        }   
    }   
    tempFind.Close();   
}

void CColorComboBox::GetColorZoneFromFile()
{
	for(int t=0; t<color_zones.size();++t)
	{
		DeleteObject((HBITMAP)color_zones[t].pzonebmp);
	}
    color_zones.clear();
	//
	for(int i=0; i<color_file_paths.size(); ++i)
	{
		CBitmap bmp;
		HBITMAP  bitmap;  
		bmp.Detach();  
		//从文件路径加载图片  
		bitmap =(HBITMAP)::LoadImage(NULL,color_file_paths[i], IMAGE_BITMAP, 0, 0,  
            LR_CREATEDIBSECTION|LR_LOADFROMFILE|LR_DEFAULTSIZE);  
		
		if(!bmp.Attach(bitmap))  
		{  
			continue;  
		}   
		//
		BITMAP bmp_info;
		bmp.GetBitmap(&bmp_info);
		int the_heignt = bmp_info.bmHeight/2;
		char pre_r, pre_g, pre_b;
		char last_r, last_g, last_b;
		char* pdata = (char*)bmp_info.bmBits;
		ColorZone temp_zone;
		for(int t=0;t<bmp_info.bmWidth;t+=2)
		{
			if(t==0)
			{
				pre_b = pdata[the_heignt*bmp_info.bmWidthBytes+t*3];
				pre_g = pdata[the_heignt*bmp_info.bmWidthBytes+t*3+1];
		        pre_r = pdata[the_heignt*bmp_info.bmWidthBytes+t*3+2];
				//
				temp_zone.colors.push_back(RGB(pre_r,pre_g,pre_b));
				//
				continue;
			}
            
			last_b = pdata[the_heignt*bmp_info.bmWidthBytes+t*3];
			last_g = pdata[the_heignt*bmp_info.bmWidthBytes+t*3+1];
		    last_r = pdata[the_heignt*bmp_info.bmWidthBytes+t*3+2];

			if(abs(last_r-pre_r)+abs(last_g-pre_g)+abs(last_b-pre_b)>=9)
			{
				temp_zone.colors.push_back(RGB(last_r,last_g,last_b));
				pre_r = last_r;
				pre_g = last_g;
				pre_b = last_b;
			}
		}
		//
		pre_b = pdata[the_heignt*bmp_info.bmWidthBytes+1*3];
		pre_g = pdata[the_heignt*bmp_info.bmWidthBytes+1*3+1];
		pre_r = pdata[the_heignt*bmp_info.bmWidthBytes+1*3+2];

		last_b = pdata[the_heignt*bmp_info.bmWidthBytes+5*3];
		last_g = pdata[the_heignt*bmp_info.bmWidthBytes+5*3+1];
		last_r = pdata[the_heignt*bmp_info.bmWidthBytes+5*3+2];

		if(pre_b==last_b && pre_g==last_g && pre_r==last_r)
		{
			temp_zone.zone_type = BLOCK_ZONE;
		}
		else
			temp_zone.zone_type = GRADUAL_ZONE;
		//
		bmp.Detach();
		temp_zone.pzonebmp = bitmap;
		temp_zone.id = color_file_paths[i];
        color_zones.push_back(temp_zone);
	}
}


void CColorComboBox::SetCurSelByZoneid(CString zone_id)
{
    for(int i=0; i<color_zones.size(); ++i)
	{
		if(color_zones[i].id == zone_id)
		{
			current_zone_index = i;
			SetCurSel(i);
			return;
		}
	}
}
