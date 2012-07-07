#pragma once
#include "XmlSchema.h"
#include "XmlSchemaObjectParserBase.h"

namespace nsYedaoqXmlSchema{namespace nsSerialize
{
	class CSchemaParser : public CXmlSchemaObjectParserBase
	{
	protected:
		enum EnumAttribute
		{
			Id, AttributeFormDefault, ElementFormDefault, BlockDefault, FinalDefault, TargetNamespace, Version, Xmlns, __upperbound,
		};

		DECLARE_XSCOMPONENTPARSER_ATTRMAP

	public:
		CSchemaParser(CSerializeContext* ctx)
			: CXmlSchemaObjectParserBase(EnumAttribute::__upperbound, ctx)
		{}

		virtual bool Parse(xnode_t* node);
		virtual void OnInnerObject(IXmlSchemaObject* obj);
		virtual IXmlSchemaObject* Object() { return Context->Schema; }

		static void Initialize();

		virtual void OnAttrID(const tstring& name, tchar const* val);
		virtual void OnAttrAttributeFormDefault(const tstring& name, tchar const* val);
		virtual void OnAttrElementFormDefault(const tstring& name, tchar const* val);
		virtual void OnAttrBlockDefault(const tstring& name, tchar const* val);
		virtual void OnAttrFinalDefault(const tstring& name, tchar const* val);
		virtual void OnAttrTargetNamespace(const tstring& name, tchar const* val);
		virtual void OnAttrVersion(const tstring& name, tchar const* val);
		virtual void OnAttrXmlns(const tstring& name, tchar const* val);
	};
}}
