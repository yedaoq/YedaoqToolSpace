/* ___________________________________________________________________________
@ 
@ file - ConfigSchema.h
@ 
@ auth - yedaoq@gmail.com          http://blog.yedaoq.info
@ date - 2012-3-5
@ info -
@     
/* ___________________________________________________________________________*/

#pragma once

#include "tstring.h"

struct ConfigGroupInfo
{
	tstring						Name;
	tstring						Description;
	tstring						Format;
	tstring						Collection;

	std::list<ConfigItemInfo>	Items;
	std::list<ConfigGroupInfo>	SubGroups;
};

struct ConfigItemInfo
{
	tstring						Name;
	tstring						Description;
	tstring						Format;

	tstring						XmlType;
	tstring						CppType;
	tstring						Default;
};