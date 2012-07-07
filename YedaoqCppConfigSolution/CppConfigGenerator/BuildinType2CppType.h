#pragma once

#include <map>
#include "Singleton.h"

class CBuildinType2CppType : public std::map<tstring, tstring>, public CSingleton<CBuildinType2CppType>
{
public:
	CBuildinType2CppType(void);

	const tstring& operator[](const tstring& xsType) const;
};
