// ObjectXmlIO.h: interface
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTXMLIO_H__D0ED5A31_88AD_4E8B_A721_A5A7616A0722__INCLUDED_)
#define AFX_OBJECTXMLIO_H__D0ED5A31_88AD_4E8B_A721_A5A7616A0722__INCLUDED_

// #ifndef EXPORT_EDITBASE
// #define EXPORT_EDITBASE
// #endif
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Markup.h"
#include "GrBuffer.h"
#include "ValueTable.h"


#define XMLTAG_VERTEXES				_T("Vertexes")
#define XMLTAG_GR_VERTEXES			_T("GrVertexes")

#define XMLTAG_GRBUFFER				_T("GrBuffer")
#define XMLTAG_GRPOINT				_T("GrPoint")
#define XMLTAG_GRLINESTRING			_T("GrLineString")
#define XMLTAG_GRPOLYGON			_T("GrPolygon")
#define XMLTAG_GRTEXT				_T("GrText")
#define XMLTAG_GRLINEEXTSTYLE		_T("GrLineExtStyle")
#define XMLTAG_GRTEXTURE			_T("GrTexture")

#define XMLTAG_POINT3D				_T("Point3d")
#define XMLTAG_POINT2D				_T("Point2d")
#define XMLTAG_SIZE					_T("Size")
#define XMLTAG_ISGRDSIZE			_T("IsGrdSize")
#define XMLTAG_WIDTH				_T("Width")
#define XMLTAG_ISGRDWID				_T("IsGrdWid")
#define XMLTAG_STYLE				_T("Style")
#define XMLTAG_SEGS					_T("Segs")
#define XMLTAG_TEXT					_T("Text")
#define XMLTAG_FONT					_T("Font")
#define XMLTAG_CHARSET				_T("CharSet")
#define XMLTAG_COLOR				_T("Color")
#define XMLTAG_ENVELOPE				_T("Envelope")
MyNameSpaceBegin

BOOL EXPORT_EDITBASE Xml_ReadTextFile(CMarkup& xml, LPCTSTR path, BOOL bUnicode=TRUE);
BOOL EXPORT_EDITBASE Xml_WriteTextFile(CMarkup& xml, LPCTSTR path, BOOL bUnicode=TRUE);

BOOL EXPORT_EDITBASE Xml_ReadValueTable(CMarkup& xml, CValueTable& tab);
BOOL EXPORT_EDITBASE Xml_WriteValueTable(CMarkup& xml, CValueTable& tab);

MyNameSpaceEnd
#endif // !defined(AFX_OBJECTXMLIO_H__D0ED5A31_88AD_4E8B_A721_A5A7616A0722__INCLUDED_)
