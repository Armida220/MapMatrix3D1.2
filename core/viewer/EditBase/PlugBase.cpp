// PlugBase.cpp: implementation of the CPlugBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PlugBase.h"

MyNameSpaceBegin
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlugBase::CPlugBase()
{
	m_nType = PLUG_TYPE_INVALID;
}

CPlugBase::~CPlugBase()
{
	
}

CPlugCmds::CPlugCmds()
{
	m_nType = PLUG_TYPE_CMD;
}

CPlugCmds::~CPlugCmds()
{
	
}

CPlugDocs::CPlugDocs()
{
	m_nType = PLUG_TYPE_DOC;
	
}

CPlugDocs::~CPlugDocs()
{
}

CPlugFiles::CPlugFiles()
{
	m_nType = PLUG_TYPE_FILE;
}

CPlugFiles::~CPlugFiles()
{

}

MyNameSpaceEnd