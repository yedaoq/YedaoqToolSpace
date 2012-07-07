#pragma once
#include "CPP\microtype.h"
#include "CPP\tstring.h"

namespace nsYedaoqXmlSchema{ namespace nsXsd2Cpp
{
	struct INameMapper
	{
		enum EnumIDCategory
		{
			ElementName,
			AttributeName,
		};

		virtual tstring MapId(const tstring& xmlName, EnumIDCategory nameCategory) = 0;
		virtual tstring	MapType(nsYedaoqXmlSchema::CXmlSchema* schema, XmlSchemaTypeSN_t sn) = 0;

		virtual ~INameMapper() = 0 {}
	};

	struct CNameMapperBase : public INameMapper
	{
		virtual tstring MapId(const tstring& xmlName, EnumIDCategory nameCategory)
		{
			tstring strRet = xmlName;
			if(FirstCharUpperCase)
			{
				strRet[0] = ::toupper(strRet[0]);
			}

			switch(nameCategory)
			{
			case ComplexType:
				strRet.insert(0, ComplexTypeNamePrefix);
				break;
			}

			return strRet;
		}

		virtual tstring	MapType(nsYedaoqXmlSchema::CXmlSchema* schema, XmlSchemaTypeSN_t sn)
		{

		}

		CNameMapperBase()
			: ComplexTypeNamePrefix(TEXT("C")), FirstCharUpperCase(true)
		{}

		tstring		ComplexTypeNamePrefix;
		bool		FirstCharUpperCase;
	};
}}