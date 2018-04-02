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

	//�ӿ�
	virtual std::string absOrient(osg::ref_ptr<osg::Node> oriNode, std::string oriInfo);										//���Զ���

	virtual std::string predict(std::string oriInfo);

protected:
	void modelOrientation(CSevenParameter &sevenParamCalulator, osg::ref_ptr<osg::Node> oriNode);

};

