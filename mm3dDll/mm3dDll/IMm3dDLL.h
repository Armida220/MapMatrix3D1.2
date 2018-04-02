#pragma once
#include <memory>

#ifdef MM3DIBDLL  
#define MM3DDLLAPI _declspec(dllexport)  
#else  
#define MM3DDLLAPI  _declspec(dllimport)  
#endif  


class MM3DDLLAPI IMm3dDLL
{
public:
	virtual void showDlg() = 0;

};

class MM3DDLLAPI IMm3dDLLFactory
{
public:
	static std::shared_ptr<IMm3dDLL> create();

};