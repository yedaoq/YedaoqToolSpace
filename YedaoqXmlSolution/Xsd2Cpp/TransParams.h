#pragma once

#include "NameMapper.h"
#include <memory>
#include <map>

namespace nsYedaoqXmlSchema{ namespace nsXsd2Cpp
{
	struct CTransParams
	{
		std::map<tstring, tstring>	TypeMappingEx;
		bool						DisableDefaultTypeMapping;
		std::auto_ptr<INameMapper>	NameMapper;		

		CTransParams()
			: DisableDefaultTypeMapping(false), NameMapper(0)
		{}
	};
}}