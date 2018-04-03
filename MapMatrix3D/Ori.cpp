#include "stdafx.h"
#include "Ori.h"

using namespace std;

COri::COri()
{
}


COri::~COri()
{
}

bool COri::operator == (const COri& rhs)
{
	string lName = getFileName(this->imgName);
	string rName = getFileName(rhs.imgName);

	if (lName == rName)
	{
		return true;
	}
	else
	{
		return false;
	}
}

std::string COri::getFileName(std::string imgName)
{
	int pos = imgName.rfind("\\") + 1;
	int length = imgName.length() - 1;

	return imgName.substr(pos, length - pos + 1);
}