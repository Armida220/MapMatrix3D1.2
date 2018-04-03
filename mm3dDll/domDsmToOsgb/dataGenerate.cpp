//#include "stdafx.h"

#define  VPB_LIBRARY_STATIC

#include <vpb/Commandline>
#include <vpb/DataSet>
#include <vpb/DatabaseBuilder>
#include <vpb/System>
#include <vpb/Version>
#include <vpb/FileUtils>

#include "dataGenerate.h"

#include <osgDB/WriteFile>

#include <sstream>
#include <regex>
#include <algorithm>
#include <gdal_priv.h>
using namespace std;
using namespace osg;
using namespace osgDB;

template <class T>
std::string TToS(const T & t)
{
	ostringstream oss;
	oss << t;
	return oss.str();
}


void CDataConvertor::startProduce()
{
	startProcess();
}

void CDataConvertor::startProcess()
{
	_beginthread(&produce, 0, this);
}

void CDataConvertor::produce(void* ptr)
{
	CDataConvertor* convertor = (CDataConvertor*)ptr;
	convertor->produceDsmDomOsgb();
}


int CDataConvertor::dsmDemToOSGB(std::string strDomPath, std::string strDsmPath, std::string strOutPath, int lod)
{
	osg::Timer_t startTick = osg::Timer::instance()->tick();
	std::string _argv[] = {
		"--GenerateTiles",
		"True",
		"--PagedLOD",
		"--image-ext",
		".jpg",
		"-d",
		strDsmPath,
		"-t",
		strDomPath,
		"-l",
		TToS<int>(lod),
		"-o",
		strOutPath
	};
	bool report = true;
	int sz = 13;
	int argc = sz;
	char **argv = new char *[sz];
	for (int i = 0; i < sz; i++)
	{
		argv[i] = new char[_argv[i].size()+2];
		strcpy(argv[i], _argv[i].c_str());
	}
	osg::ArgumentParser arguments(&argc, argv);
	vpb::Commandline commandline;
	commandline.getUsage(*arguments.getApplicationUsage());
	vpb::System::instance()->readArguments(arguments);

	double duration = 0.0;
	osg::ref_ptr<osgTerrain::TerrainTile> terrain = 0;
	if (!terrain) terrain = new osgTerrain::TerrainTile;
	int result = commandline.read(std::cout, arguments, terrain.get());
	if (terrain.valid())
	{
		try
		{

			vpb::DatabaseBuilder* db = dynamic_cast<vpb::DatabaseBuilder*>(terrain->getTerrainTechnique());
			vpb::BuildOptions* bo = db ? db->getBuildOptions() : 0;

			if (bo)
			{
				osg::setNotifyLevel(osg::NotifySeverity(bo->getNotifyLevel()));

			}
			osg::ref_ptr<vpb::DataSet> dataset = new vpb::DataSet;

			if (bo && !(bo->getLogFileName().empty()))
			{
				dataset->setBuildLog(new vpb::BuildLog(bo->getLogFileName()));
			}

			dataset->addTerrain(terrain.get());

			// make sure the OS writes changes to disk
			vpb::sync();

			// check to make sure that the build itself is ready to run and configured correctly.
			std::string buildProblems = dataset->checkBuildValidity();
			if (buildProblems.empty())
			{
				result = dataset->run();

				if (dataset->getBuildLog() && report)
				{
					dataset->getBuildLog()->report(std::cout);
				}

				duration = osg::Timer::instance()->delta_s(startTick, osg::Timer::instance()->tick());

				dataset->log(osg::NOTICE, "Elapsed time = %f", duration);

				
			}
			else
			{
				dataset->log(osg::NOTICE, "Build configuration invalid : %s", buildProblems.c_str());
				
			}

		}
		catch (std::string str)
		{
			printf("Caught exception : %s\n", str.c_str());

			result = 1;

		}
		catch (...)
		{
			printf("Caught exception.\n");

			result = 1;
		}

	}

	vpb::sync();
	for (int i = 0; i < sz; i++)
	{
		delete argv[i];
	}
	delete argv;
	return 0;
}

int CDataConvertor::produceDsmDomOsgb(/*string inputFilePath, string strDomTitle, string strDsmTitle, int xPart, int yPart*/)
{
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	std::string strOsgPath = produceDlg->m_osgPath;
	std::string strDomPath = produceDlg->m_domPath;
	std::string strDsmPath = produceDlg->m_dsmPath;
	dsmDemToOSGB(strDomPath, strDsmPath, strOsgPath, produceDlg->xPart);
	return 1;
}
