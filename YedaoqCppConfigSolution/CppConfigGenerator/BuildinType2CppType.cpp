#include "StdAfx.h"
#include "BuildinType2CppType.h"

typedef std::pair<tstring, tstring> tpair;

#define INSERT(xsType, cppType) this->insert(tpair(xsType.Str, TEXT(#cppType)));

CBuildinType2CppType::CBuildinType2CppType(void)
{
	INSERT(CXsString::Boolean, bool);
	INSERT(CXsString::String, tstring);
	INSERT(CXsString::Byte, unsigned char);
	INSERT(CXsString::Short, short);
	INSERT(CXsString::Float, float);
	INSERT(CXsString::Decimal, double);
	INSERT(CXsString::Double, double);
	INSERT(CXsString::Long, long);
	INSERT(CXsString::Integer, int);
}

const tstring& CBuildinType2CppType::operator[](const tstring& xsType) const
{
	const_iterator iter = find(xsType);
	if(iter != end())
	{
		return iter->second;
	}
	throw std::exception("unsupported buildin type in config schema definition!")
}