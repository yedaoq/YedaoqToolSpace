#pragma once
#include "XmlSchemaObjectParserBase.h"
#include "ElementGroup.h"

namespace nsYedaoqXmlSchema{ namespace nsSerialize
{
	class CGroupParser : public CXmlSchemaObjectParserBase
	{
	protected:
		enum EnumAttribute
		{
			id, maxOccurs, minOccurs, __upperbound,
		};

		DECLARE_XSCOMPONENTPARSER_ATTRMAP

	public:
		CGroupParser(CSerializeContext* ctx)
			: CXmlSchemaObjectParserBase(EnumAttribute::__upperbound, ctx)
		{}

		virtual bool Parse(xnode_t* node);
		virtual void OnInnerObject(IXmlSchemaObject* obj);
		virtual IXmlSchemaObject* Object() { return &GroupInfo; }

		static void Initialize();

		virtual void OnAttrID(const tstring& name, tchar const* val);
		virtual void OnAttrMaxOccurs(const tstring& name, tchar const* val);
		virtual void OnAttrMinOccurs(const tstring& name, tchar const* val);

	public:
		XmlSchemaSubitemGroup GroupInfo;
	};
}}

