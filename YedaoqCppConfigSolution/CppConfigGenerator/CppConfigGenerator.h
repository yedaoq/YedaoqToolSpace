#pragma once

#include "ConfigCommon.h"

nsYedaoqXmlSchema{
	class XmlSchema;
	class XmlSchemaComplexType;
	class XmlSchemaElement;
	class XmlSchemaSimpleType;
};

class CCppConfigGenerator
{
	struct TypeCollectContext
	{
		typedef std::list<XmlSchemaSimpleType*>	EnumTypeCollection;
		typedef std::stack<XmlSchemaComplexType*> SortedTypeCollection;

		tstring								ProjectName;
		XmlSchema*							Schema;
		SortedTypeCollection				SortedTypes;
		EnumTypeCollection					EnumTypes;
		EnumTypeCollection					FlagEnumTypes;
		boost::dynamic_bitset<>				TypeMask;

		TypeCollectContext(XmlSchema* schema);
	};

public:
	CCppConfigGenerator();

	bool GenerateCppCodeFromSchema(XmlSchema* doc, const tstring& path, const tstring name);

	void GenerateEnumFile(TypeCollectContext& ctx);
	void GenerateEnumDefinition(TypeCollectContext& ctx, std::tofstream& fstrm);

	void GenerateConfigFile(TypeCollectContext& ctx);
	void GenerateConfigDefinition(TypeCollectContext& ctx, std::tofstream& fstrm);


	void TypeCollect(XmlSchema* schema, TypeCollectContext& ctx);
	void TypeCollect(XmlSchemaElement* ele, TypeCollectContext& ctx);
	void TypeCollect(XmlSchemaComplexType* complexType, TypeCollectContext& ctx);
	void TypeCollect(XmlSchemaAttribute* attr, TypeCollectContext& ctx);
	void TypeCollect(XmlSchemaSubitemGroup* group, TypeCollectContext& ctx);

	const tstring& GetSimpleTypeCppNameStr(XmlSchema* schema, XmlSchemaTypeSN_t sn);
	const tstring& GetSimpleTypeCppNameStr(XmlSchema* schema, XmlSchemaTypeSN_t sn);

protected:
	static std::map<tstring, tstring> map_buildintype_cpptype;
};
