#pragma once
#include <string>
#include <vector>
#include "IOsgOrient.h"
#include "SevenParameter.h"

class CAbsoluteOrientationor : public IOsgOrient
{
public:
	CAbsoluteOrientationor(void);
	~CAbsoluteOrientationor(void);																				

	//接口
	virtual std::string absOrient(osg::ref_ptr<osg::Node> oriNode, std::string oriInfo);										//绝对定向

	virtual std::string predict(std::string oriInfo);

protected:
	void modelOrientation(CSevenParameter &sevenParamCalulator, osg::ref_ptr<osg::Node> oriNode);

};

