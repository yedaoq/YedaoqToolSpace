#pragma once
#include "XmlSchema.h"
#include "XmlSchemaObjectParserBase.h"

namespace nsYedaoqXmlSchema{namespace nsSerialize
{
	class CAttributeParser : public CXmlSchemaObjectParserBase
	{
	protected:
		enum EnumAttribute
		{
			id, default, fixed, form, name, ref, type, use, __upperbound,
		};

		DECLARE_XSCOMPONENTPARSER_ATTRMAP

	public:
		CAttributeParser(CSerializeContext* ctx)
			: CXmlSchemaObjectParserBase(EnumAttribute::__upperbound, ctx)
		{}

		virtual bool Parse(xnode_t* node);
		virtual void OnInnerObject(IXmlSchemaObject* obj);
		virtual IXmlSchemaObject* Object() { return &AttrInfo; }

		static void Initialize();

		virtual void OnAttrID(const tstring& name, tchar const* val);
		virtual void OnAttrDefault(const tstring& name, tchar const* val);
		virtual void OnAttrFixed(const tstring& name, tchar const* val);
		virtual void OnAttrForm(const tstring& name, tchar const* val);
		virtual void OnAttrName(const tstring& name, tchar const* val);
		virtual void OnAttrRef(const tstring& name, tchar const* val);
		virtual void OnAttrType(const tstring& name, tchar const* val);
		virtual void OnAttrUse(const tstring& name, tchar const* val);

		virtual void OnMetaAnyAttribute(const tstring& name, tchar const* val);

	protected:
		XmlSchemaAttribute	AttrInfo;
	};
}}
