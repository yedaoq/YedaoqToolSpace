#pragma once
#include "Xsd2CppContext.h"

namespace nsYedaoqXmlSchema{ namespace nsXsd2Cpp
{
	class CTypeCollector
	{
	public:
		bool Collect(Xsd2CppContext& ctx);

		void Collect(CXmlSchema* schema, Xsd2CppContext& ctx);
		void Collect(XmlSchemaElement* ele, Xsd2CppContext& ctx);
		void Collect(XmlSchemaComplexType* complexType, Xsd2CppContext& ctx);
		void Collect(XmlSchemaAttribute* attr, Xsd2CppContext& ctx);
		void Collect(XmlSchemaSubitemGroup* group, Xsd2CppContext& ctx);

		bool IsEnumDataType(XmlSchemaSimpleType* simpleType, CXmlSchema* schema);
		bool IsFlagEnumDataType(XmlSchemaSimpleType* simpleType, CXmlSchema* schema);
	};

}}