#pragma once

#include <string>
#include <osg/Vec3d>
#include <osg/Matrix>
#include <memory>
#include <vector>

#ifdef CLIPLIBDLL  
#define CLIPAPI _declspec(dllexport)  
#else  
#define CLIPAPI  _declspec(dllimport)  
#endif  

class CLIPAPI IOsgClip
{
public:
	virtual void accurateClip(std::string inFileName, std::string outputFileName, std::vector<osg::Vec3d> area) = 0;																										//°´·¶Î§É¾³ý

};


class CLIPAPI IOsgClipFactory
{
public:
	static std::shared_ptr<IOsgClip> create();
};

