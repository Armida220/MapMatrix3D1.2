#include "stdafx.h"
#include "ConvertTool.h"
#include "tinyxml.h"
#include"tinystr.h"

int writeXmlFile(const char* filePath,double x = 0,double y = 0,double z = 0)  
{  
	TiXmlDocument *writeDoc = new TiXmlDocument; //xml文档指针  

	//文档格式声明  
	TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "UTF-8","yes");  
	writeDoc->LinkEndChild(decl); //写入文档  

	int n = 3;  //父节点个数  

	TiXmlElement *RootElement = new TiXmlElement("ModelMetadata");//根元素  
	RootElement->SetAttribute("version", 1); //属性  
	writeDoc->LinkEndChild(RootElement);  

	TiXmlElement *nameElement = new TiXmlElement("SRS");  
	RootElement->LinkEndChild(nameElement);  

	TiXmlText *nameContent = new TiXmlText("ENU:32.51128,111.08104");  
	nameElement->LinkEndChild(nameContent); 


	TiXmlElement *nameElement1 = new TiXmlElement("SRSOrigin");  
	RootElement->LinkEndChild(nameElement1);  
	char msg[99];
	sprintf(msg,"%f,%f,%f",x,y,z);
	TiXmlText *nameContent1 = new TiXmlText(msg);  
	nameElement1->LinkEndChild(nameContent1); 
	writeDoc->SaveFile(filePath);  
	delete writeDoc;  
	return 1;  
}  

int  _logop(char* ptext,int level = 0)
{
	char ca[256]="";
	struct timeb tmm;
	ftime(&tmm);
	struct tm*t;
	struct tm tt;
	t = &tt;
	localtime_s(t,&tmm.time);

	FILE* handle = 0;

	//1 open or create the file;
	errno_t ret = fopen_s(&handle,"runop.log","a+");
	if( ret != 0 )
	{
		//printf("Error! When open file of run log!");
		//g_cs1.Leave();
		return -2;
	}

	//create log text;
	sprintf_s(ca,255,"\n[%d-%d-%d %d:%d:%d.%03d] ",t->tm_year+1900,t->tm_mon+1,t->tm_mday,
		t->tm_hour,t->tm_min,t->tm_sec,tmm.millitm);
	//2 write log;
	fwrite(ca,strlen(ca),1,handle);	
	fwrite(ptext,strlen(ptext),1,handle);

	fclose(handle);

	//3 if size>500K,rename and delete;
	struct stat sst;
	if(!stat("runop.log",&sst))
	{
		if(sst.st_size/1024>1800)//1M大小
		{
			if (_access("runop.log.bak", 0) != -1)
			{
				remove("runop.log.bak");
			}

			rename("runop.log","runop.log.bak");
		}
	}

	return 0;
}

CPageLodSetter::CPageLodSetter(std::string FileName) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
{
	fileName = FileName;
}


CPageLodSetter::~CPageLodSetter(void)
{
}

void CPageLodSetter::SetLodFileName(osg::ref_ptr<osg::Node> node, std::string fileName)
{
	CPageLodSetter ive(fileName);
	node->accept(ive);
}

void CPageLodSetter::apply(osg::PagedLOD& pagedLod)
{
	pagedLod.setDatabasePath("");
	traverse(pagedLod);
}

void CTransNodeFinder::apply(osg::MatrixTransform& node)
{
	if (node.getName() != "repairMatrix")
	{
		osg::Matrix matrix = node.getMatrix();
		vecTrans.push_back(&node);
	}

	traverse(node);
}

void CTransNodeFinder::FindTransNode(osg::ref_ptr<osg::Node> node, std::vector<osg::MatrixTransform*> &vecTrans)
{
	CTransNodeFinder ive;
	node->accept(ive);
	vecTrans = ive.vecTrans;
}

void CChildPagedLodTraveller::apply(osg::PagedLOD& pagedLod)
{
	osg::ref_ptr<osg::Node> node = readChildPagedLodNode(pagedLod);

	if (!node)
	{
		return;
	}
	osg::BoundingSphere bs = node->computeBound();
	osg::Vec3d p = bs.center();
	char msg[99];
	sprintf_s(msg,"----%f %f %f",p.x(),p.y(),p.z());
	_logop(msg);

	CPageLodCenterReCalculator ive;
	node->accept(ive);

	bs = node->computeBound();
	p = bs.center();
	sprintf_s(msg,"----%f %f %f",p.x(),p.y(),p.z());
	_logop(msg);
	bool isSuccess = outputChildPagedLodNode(pagedLod, node);

	if (isSuccess)
	{
		travelChildNode(pagedLod, node);
		traverse(pagedLod);	
	}

}

void CChildPagedLodTraveller::apply(osg::Geode& geode)
{
	int num = geode.getNumDrawables();

	for(int j = num  - 1; j >= 0; --j) 
	{
		osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode.getDrawable(j));
		if( !geom ) 
			continue; 
		osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
		for (int k = 0;k<verts->size();++k)
		{
			double x1 = verts->at(k).x();
			double y1 = verts->at(k).y();
			double z1 = verts->at(k).z();
			osg::Vec3d pt1(x1, y1, z1);
			pt1 = pt1 * matrix; 
			verts->at(k).x() = pt1.x();
			verts->at(k).y() = pt1.y();
			verts->at(k).z() = pt1.z();
		}	
	}
}

std::string CChildPagedLodTraveller::ObtainTileFileName(std::string fileName)
{
	int pos = fileName.find("Tile");
	int length = fileName.length();
	std::string tileFileName = ".\\" + fileName.substr(pos, length - pos);
	return tileFileName;
}

void CChildPagedLodTraveller::CheckClipFolder(std::string filePath, std::string fileTitle, std::string &outFileName)
{
	//判断是否有clip
	int pos = filePath.find("Clip");

	if (pos != -1)
	{
		outFileName = filePath + ObtainTileFileName(fileTitle);
	}
	else
	{
		outFileName = filePath + fileTitle;														// 合并
	}
}

osg::ref_ptr<osg::Node> CChildPagedLodTraveller::readChildPagedLodNode(osg::PagedLOD& pagedLod)
{
	//读取照片
	std::string childFileTitle = pagedLod.getFileName(1);
	std::string childFileName = inFilePath + childFileTitle;
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(childFileName);
	return node;
}

bool CChildPagedLodTraveller::outputChildPagedLodNode(osg::PagedLOD& pagedLod, osg::ref_ptr<osg::Node> node)
{
	//查找三角形个数
	if (!node)
	{
		return false;
	}

	//先删除
	std::string childFileTitle = pagedLod.getFileName(1);

	//检查clip文件夹
	std::string outChildFileName;
	CheckClipFolder(outFilePath, childFileTitle, outChildFileName);

	bool isExsists = PathFileExists(CString(outChildFileName.c_str()));
	if (isExsists)
	{
		DeleteFile(CString(outChildFileName.c_str()));
	}


	//输出照片
	osg::ref_ptr<osgDB::Options> spOptions = new osgDB::Options;
	spOptions->setPluginStringData("WriteImageHint", "IncludeFile");
	//设置相对路径
	CPageLodSetter ive("");
	node->accept(ive);
	
	osgDB::writeNodeFile(*node, outChildFileName, spOptions.get());

	return true;
}

void CChildPagedLodTraveller::travelChildNode(osg::PagedLOD& pagedLod, osg::ref_ptr<osg::Node> node)
{
	//继续访问
	std::string childFileTitle = pagedLod.getFileName(1);
	std::string childFileName = inFilePath + childFileTitle;

	int pos = childFileName.rfind('\\') + 1;
	std::string childFilePath = childFileName.substr(0, pos);

	CChildPagedLodTraveller ive(childFilePath, outFilePath,matrix);
	node->accept(ive);
}

void CConvertTool::doConvert(void* ptr)
{
	USES_CONVERSION;
	CConvertTool* cvt = (CConvertTool*)ptr;
	cvt->convert();
}

void CConvertTool::startProcess()
{
	_beginthread(&doConvert, 0, this);
}

void CConvertTool::startConvert()
{
	startProcess();
}

bool CConvertTool::convert()
{
	USES_CONVERSION;
	std::string infilePath = W2A(convertDlg->m_path);
	std::string outfilePath = convertDlg->outputFilePath;
	std::string s1,s2;
	int pos1 = infilePath.rfind("\\");
	if (pos1 != -1)
	{
		s1 = infilePath.substr(0,pos1+1);
	}
	int pos2 = outfilePath.rfind("\\");
	if (pos2 != -1)
	{
		s2 = outfilePath.substr(0,pos2+1);
	}
	if (s1 == s2)
	{

		AfxMessageBox(LoadString(IDS_ERROR_IOPATH));
		//AfxMessageBox(_T("输入和输出不能为同一目录下！"));
		convertDlg->m_progress.ShowWindow(SW_HIDE);
		return false;
	}
	osg::ref_ptr<osg::Node> scenceNode = osgDB::readNodeFile(infilePath);
	std::vector<osg::MatrixTransform*> vecTrans;
	FindTransNode(scenceNode,vecTrans);

	if (vecTrans.size() == 0)
	{
		AfxMessageBox(LoadString(IDS_ERROR_NEEDSET));
		//AfxMessageBox(_T("场景未置平，请先置平"));
		convertDlg->m_progress.ShowWindow(SW_HIDE);
		return false;
	}

	
	for (int i = 0; i < vecTrans.size(); i++)
	{
		osg::ref_ptr<osg::MatrixTransform> mTrans = vecTrans[i];
		osg::Matrix matrix = mTrans->getMatrix();
		osg::Vec3d p = matrix.getTrans();
		osg::Quat q = matrix.getRotate();
		std::vector<std::string> vecHalfInPolygonFileName, vecTotalInPolygonFileName;
		ClipSceneByScope(mTrans, s1, s2,  vecTotalInPolygonFileName);

		int pos = outfilePath.find_last_of(".");
		outfilePath = outfilePath.substr(0,pos);
		outfilePath += ".xml";
		writeXmlFile(outfilePath.c_str(),p.x(),p.y(),p.z());
	}
	/*osg::ref_ptr<osg::Node> copyNode = dynamic_cast<osg::Node*> (scenceNode->clone(osg::CopyOp::DEEP_COPY_ALL));

	ClipOsgHeader(copyNode);
	CPageLodSetter ive("");
	copyNode->accept(ive);

	//osgDB::writeNodeFile(*copyNode, outfilePath);*/
	convertDlg->m_progress.EndModalState();
	convertDlg->m_progress.ShowWindow(SW_HIDE);
	AfxMessageBox(LoadString(IDS_SUCCESS_CONVERT));
	//AfxMessageBox(_T("转换完成！"));
	return true;
}

void CConvertTool::FindTransNode(osg::ref_ptr<osg::Node> node, std::vector<osg::MatrixTransform*> &vecTrans)
{
	CTransNodeFinder ive;
	node->accept(ive);
	vecTrans = ive.vecTrans;
}

bool CConvertTool::ClipOsgHeader(osg::ref_ptr<osg::Node> sceneNode)
{
	std::vector<osg::MatrixTransform*> vecMatTrans; 
	FindTransNode(sceneNode, vecMatTrans);

	if (vecMatTrans.size() > 0)
	{
		osg::Matrix matrix = vecMatTrans[0]->getMatrix();

		//database路径设为相对,对头结点裁切
		CMultiScopeHeadGeodeClipper ive( matrix);
		sceneNode->accept(ive);
	}

	return true;
}



bool CConvertTool::ClipSceneByScope(osg::ref_ptr<osg::MatrixTransform> mTrans, std::string inFilePath, std::string outputFilePath, std::vector<std::string> &vecTotalInPolygonFileName)
{
	osg::ref_ptr<osg::Group> mGroup = mTrans->getChild(0)->asGroup();

	if (!mGroup)
	{
		return false;
	}
	osg::Matrix worldMatrix = mTrans->getMatrix();
	ObtainSelTileName( mGroup, vecTotalInPolygonFileName);//获得tile名
	int total = vecTotalInPolygonFileName.size() + 0.1 * vecTotalInPolygonFileName.size();
	convertDlg->m_progress.SetRange(0,total);
	convertDlg->m_progress.SetPos(total - vecTotalInPolygonFileName.size());
	convertDlg->m_progress.ShowWindow(SW_SHOW);
	//合并组
	osg::ref_ptr<osg::Group> combineTotalInPolygonGroup = CombinePagedLod(vecTotalInPolygonFileName, inFilePath);

	CMultiScopePagedLodClipper iveClipper(convertDlg,inFilePath, outputFilePath, worldMatrix);
	combineTotalInPolygonGroup->accept(iveClipper);
	
	return true;
}

void CConvertTool::ObtainSelTileName(osg::ref_ptr<osg::Group> mGroup, std::vector<std::string> &vecTotalInPagedLodFileName)
{
	std::vector<std::string> vecTotalInFileName;
	ObtainSelFileNameInPolygon(mGroup, vecTotalInFileName);
	for (int j = 0; j < vecTotalInFileName.size(); j++)
	{
		if (std::find(vecTotalInPagedLodFileName.begin(), vecTotalInPagedLodFileName.end(), vecTotalInFileName[j]) == vecTotalInPagedLodFileName.end())
		{
			vecTotalInPagedLodFileName.push_back(vecTotalInFileName[j]);
		}
	}
}

void CConvertTool::ObtainSelFileNameInPolygon(osg::ref_ptr<osg::Group> group,std::vector<std::string> &vecTotalInFileName)
{
	int groupNum = group->getNumChildren();

	//根据多边形范围筛选

	for (int i = groupNum - 1; i >= 0; i--)
	{
		osg::ref_ptr<osg::PagedLOD> pagedLod = dynamic_cast<osg::PagedLOD*>(group->getChild(i));

		if (!pagedLod)
		{
			continue;
		}
		std::string fileName = pagedLod->getFileName(1);
		vecTotalInFileName.push_back(fileName);
	}
}

void CConvertTool::CheckClipFolder(std::string filePath, std::string fileTitle, std::string &outFileName)
{
	//判断是否有clip
	int pos = filePath.find("Clip");

	if (pos != -1)
	{
		outFileName = filePath + ObtainTileFileName(fileTitle);
	}
	else
	{
		outFileName = filePath + fileTitle;														// 合并
	}
}

std::string CConvertTool::ObtainTileFileName(std::string fileName)
{
	int pos = fileName.find("Tile");
	int length = fileName.length();
	std::string tileFileName = ".\\" + fileName.substr(pos, length - pos);
	return tileFileName;
}

bool CConvertTool::CreateFolder(std::string createFilePath)
{
	if (!PathFileExists(CString(createFilePath.c_str())))
	{
		char strMsg[MAX_PATH];
		sprintf_s(strMsg, "%s", createFilePath.c_str());
		CString str(strMsg);
		bool isSuccess = MakeDirectory(str);
		return isSuccess;
	}

	return false;
}

osg::ref_ptr<osg::Group> CConvertTool::CombinePagedLod(std::vector<std::string> vecPagedLodFileName, std::string inFilePath)
{
	//把所有的pagedlod加入group
	osg::ref_ptr<osg::Group> group = new osg::Group;

	for (int i = 0; i < vecPagedLodFileName.size(); i++)
	{
		std::string fileFolder = vecPagedLodFileName[i];
		std::string fileName = inFilePath + fileFolder;
		osg::ref_ptr<osg::PagedLOD> lod = dynamic_cast<osg::PagedLOD*>(osgDB::readNodeFile((fileName)));

		if (!lod)
		{
			continue;
		}

		char strMsg[99];
		sprintf_s(strMsg, "%d\n", i);
		lod->setName(strMsg);
		lod->setFileName(1, fileFolder); 
		lod->setDatabasePath("");
		group->addChild(lod);
	}

	return group;
}

BOOL CConvertTool::MakeDirectory(CString dd)
{
	HANDLE fFile; // File Handle
	WIN32_FIND_DATA fileinfo; // File Information Structure
	CStringArray m_arr; // CString Array to hold Directory Structures
	BOOL tt; // BOOL used to test if Create Directory was successful
	int x1 = 0; // Counter
	CString tem = _T(""); // Temporary CString Object

	fFile = FindFirstFile(dd,&fileinfo);

	// if the file exists and it is a directory
	if(fileinfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
	{
		//  Directory Exists close file and return
		FindClose(fFile);
		return FALSE;
	}
	m_arr.RemoveAll();

	for(x1 = 0; x1 < dd.GetLength(); x1++ ) // Parse the supplied CString Directory String
	{ 
		if(dd.GetAt(x1) != '\\') // if the Charachter is not a \ 
			tem += dd.GetAt(x1); // add the character to the Temp String
		else
		{
			m_arr.Add(tem); // if the Character is a \ 
			tem += "\\"; // Now add the \ to the temp string
		}
		if(x1 == dd.GetLength()-1) // If we reached the end of the String
			m_arr.Add(tem);
	}

	// Close the file
	FindClose(fFile);

	// Now lets cycle through the String Array and create each directory in turn
	for(x1 = 1; x1 < m_arr.GetSize(); x1++)
	{
		tem = m_arr.GetAt(x1);
		tt = CreateDirectory(tem,NULL);

		// If the Directory exists it will return a false
		if(tt)
			SetFileAttributes(tem,FILE_ATTRIBUTE_NORMAL);
		// If we were successful we set the attributes to normal
	}
	//  Now lets see if the directory was successfully created
	fFile = FindFirstFile(dd,&fileinfo);

	m_arr.RemoveAll();
	if(fileinfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
	{
		//  Directory Exists close file and return
		FindClose(fFile);
		return TRUE;
	}
	else
	{
		// For Some reason the Function Failed  Return FALSE
		FindClose(fFile);
		return FALSE;
	}
}

void CConvertTool::FileCopyTo(std::string source, std::string destination, std::string searchStr, BOOL cover)
{
	char sourceStr[MAX_PATH];
	sprintf_s(sourceStr, "%s", source.c_str());
	CString strSourcePath(sourceStr);

	char destStr[MAX_PATH];
	sprintf_s(destStr, "%s", destination.c_str());
	CString strDesPath(destStr);

	char StrSearch[MAX_PATH];
	sprintf_s(StrSearch, "%s", searchStr.c_str());
	CString strFileName(StrSearch);
	CFileFind filefinder;
	CString strSearchPath = strSourcePath + "\\" + strFileName;
	CString filename;
	BOOL bfind = filefinder.FindFile(strSearchPath);
	CString SourcePath, DisPath;

	while (bfind)
	{
		bfind = filefinder.FindNextFile();
		filename = filefinder.GetFileName();
		SourcePath = strSourcePath + "\\" + filename;
		DisPath = strDesPath + "\\" + filename;
		CopyFile(SourcePath, DisPath, cover);
	}

	filefinder.Close();
}


void CMultiScopePagedLodClipper::travelChildNode(osg::PagedLOD& pagedLod)
{
	CConvertTool tool;
	//访问下一级
	std::string fileName = pagedLod.getFileName(1);
	int posStart = fileName.find_first_of('\\');
	int posEnd = fileName.find_last_of('\\');
	std::string outFolder = fileName.substr(posStart, posEnd - posStart + 1);	//输出目录

	std::string outFilePath1;
	tool.CheckClipFolder(outFilePath, outFolder, outFilePath1);						//检查clipFolder

	std::string inChildFileName = inFilePath + fileName;

	int pos = inChildFileName.rfind('\\') + 1;
	std::string inChildFilePath = inChildFileName.substr(0, pos);

	CMultiScopeCHildPagedLodClipper ive( inChildFilePath, outFilePath1, matrix);
	osg::ref_ptr<osg::Node> childNode = osgDB::readNodeFile(inChildFileName);
	childNode->accept(ive);
}

bool CMultiScopePagedLodClipper::outputPagedLodNode(osg::PagedLOD& pagedLod, osg::ref_ptr<osg::Node> node)
{
	CConvertTool tool;
	////查找三角形个数
	

	std::string fileName = pagedLod.getFileName(1);
	int posEnd = fileName.find_last_of('\\');  
	int posStart = fileName.find_first_of('\\');
	std::string outFolder = fileName.substr(posStart, posEnd - posStart + 1);				//输出目录

	////判断是否有clip
	std::string outFilePath1;
	tool.CheckClipFolder(outFilePath, outFolder, outFilePath1);


	tool.CreateFolder(outFilePath1);											//创建目录

	////判断是否有clip
	std::string outFileNameInPiece;
	tool.CheckClipFolder(outFilePath, fileName, outFileNameInPiece);

	bool isExsists = PathFileExists(CString(outFileNameInPiece.c_str()));
	if (isExsists)
	{
		DeleteFile(CString(outFileNameInPiece.c_str()));
	}

	//输出照片
	osg::ref_ptr<osgDB::Options> spOptions = new osgDB::Options;
	spOptions->setPluginStringData("WriteImageHint", "IncludeFile");
	osgDB::writeNodeFile(*node, outFileNameInPiece, spOptions.get());						//输出

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void CMultiScopePagedLodClipper::apply(osg::PagedLOD& pagedLod)
{
	if (pagedLod.getNumParents() == 0)
	{
		return;
	}

	osg::ref_ptr<osg::Node> node = readPagedLodNode(pagedLod);

	if (!node)
	{
		return;
	}

	osg::BoundingSphere bs = pagedLod.computeBound();
	osg::Vec3d p = bs.center();
	
	std::string nodeName = node->getName();

	CMultiChildScopeGeoClipper iveClipper(matrix);
	node->accept(iveClipper);

	CPageLodCenterReCalculator ive;
	node->accept(ive);

	bs = node->computeBound();
	p = bs.center();
	
	bool isSuccess = outputPagedLodNode(pagedLod, node);

	if (isSuccess)
	{
		int pos = ctoolDlg->m_progress.GetPos();
		ctoolDlg->m_progress.SetPos(pos + 1);
		travelChildNode(pagedLod);
	}
}

void CMultiScopeHeadGeodeClipper::apply(osg::Geode& geode)
{
	osg::MatrixList matrixList = geode.getWorldMatrices();

	if (matrixList.size() != 0)
	{
		osg::Matrix newMatrix;

		for (int i = 0; i < matrixList.size(); i++)
		{
			newMatrix.postMult(matrixList[i]);
		}

		matrix = newMatrix;
	}

	int num = geode.getNumDrawables();

	for(int j = num  - 1; j >= 0; --j) 
	{
		bool isEmpty = true;

		osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode.getDrawable(j));

		if( !geom ) 
			continue; //得到可绘制结点的顶点序列 

		osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray()); 

		for (int k = 0; k < geom->getNumPrimitiveSets(); k++)
		{
			osg::PrimitiveSet* priSet = geom->getPrimitiveSet(k);
			osg::ref_ptr<osg::DrawElements> drawElementTriangle = priSet->getDrawElements();
			osg::ref_ptr<osg::DrawElementsUInt> newDrawElementsTriangle = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES);
			int cnt = drawElementTriangle->getNumIndices();
			int triCnt = cnt / 3;

			for (int w = 0; w < triCnt; w++)
			{
				int index1 = drawElementTriangle->index(w * 3);
				int index2 = drawElementTriangle->index(w * 3 + 1);
				int index3 = drawElementTriangle->index(w * 3 + 2);

				double x1 = verts->at(index1).x();
				double y1 = verts->at(index1).y();
				double z1 = verts->at(index1).z();
				osg::Vec3d pt1(x1, y1, z1);
				pt1 = pt1 * matrix; 

				double x2 = verts->at(index2).x();
				double y2 = verts->at(index2).y();
				double z2 = verts->at(index2).z();
				osg::Vec3d pt2(x2, y2, z2);
				pt2 = pt2 * matrix; 

				double x3 = verts->at(index3).x();
				double y3 = verts->at(index3).y();
				double z3 = verts->at(index3).z();
				osg::Vec3d pt3(x3, y3, z3);
				pt3 = pt3 * matrix;
		
				newDrawElementsTriangle->push_back(index1);
				newDrawElementsTriangle->push_back(index2);
				newDrawElementsTriangle->push_back(index3);
		
			}

			geom->setPrimitiveSet(k, newDrawElementsTriangle);
		}
	}
}

void CMultiScopeCHildPagedLodClipper::apply(osg::PagedLOD& pagedLod)
	{
		std::string fileName = pagedLod.getFileName(1);

		osg::ref_ptr<osg::Node> node = readChildPagedLodNode(pagedLod);

		if (!node)
		{
			return;
		}

		osg::BoundingSphere bs = pagedLod.computeBound();
		osg::Vec3d p = bs.center();
		char msg[99];
		sprintf_s(msg,"----%f %f %f",p.x(),p.y(),p.z());
		_logop(msg);

		CMultiChildScopeGeoClipper iveClipper( matrix);
		node->accept(iveClipper);

		CPageLodCenterReCalculator ive;
		node->accept(ive);

		bs = node->computeBound();
		p = bs.center();
		sprintf_s(msg,"----%f %f %f\n\n",p.x(),p.y(),p.z());
		_logop(msg);
		bool isSuccess = outputChildPagedLodNode(pagedLod, node);

		if (isSuccess)
		{
			travelChildNode(pagedLod, node);
		}
	}