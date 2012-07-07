#pragma once
#include "TransParams.h"
#include <vector>
#include <boost\dynamic_bitset\dynamic_bitset.hpp>

namespace nsYedaoqXmlSchema{
	class CXmlSchema;
	class XmlSchemaComplexType;
	class XmlSchemaElement;
	class XmlSchemaSimpleType;
	class XmlSchemaAttribute;
	class XmlSchemaSubitemGroup;
};

namespace nsYedaoqXmlSchema{ namespace nsXsd2Cpp
{
	struct Xsd2CppContext
	{
		typedef std::vector<nsYedaoqXmlSchema::XmlSchemaComplexType*>	ComplexTypeCollection;
		typedef std::vector<nsYedaoqXmlSchema::XmlSchemaSimpleType*>	SimpleTypeCollection;

		CXmlSchema*				Schema;
		CTransParams*			Params;
		tstring					ProjectName;	
		tstring					ProjectNamespace;
		tstring					OutDir;

		ComplexTypeCollection	SortedComplexTypes;
		SimpleTypeCollection	EnumTypes;
		SimpleTypeCollection	FlagEnumTypes;

		boost::dynamic_bitset<>	TypeMask;

		Xsd2CppContext(CXmlSchema* schema, CTransParams* params, const tstring& project)
			: Schema(schema), Params(params), ProjectName(project), TypeMask(0)
		{}
	};
}}