#pragma once
//#include "XmlSchema.h"
#include "XmlSchemaObjectParserBase.h"
#include "ElementGroup.h"

namespace nsYedaoqXmlSchema{namespace nsSerialize
{
	class CAnyParser : public CXmlSchemaObjectParserBase
	{
	protected:
		enum EnumAttribute
		{
			id, maxOccurs, minOccurs, Namespace, processContents, __upperbound,
		};

		DECLARE_XSCOMPONENTPARSER_ATTRMAP

	public:
		CAnyParser(CSerializeContext* ctx)
			: CXmlSchemaObjectParserBase(EnumAttribute::__upperbound, ctx)
		{}

		virtual bool Parse(xnode_t* node);
		virtual void OnInnerObject(IXmlSchemaObject* obj);
		virtual IXmlSchemaObject* Object() { return &AnyInfo; }

		static void Initialize();

		virtual void OnAttrID(const tstring& name, tchar const* val);
		virtual void OnAttrMaxOccurs(const tstring& name, tchar const* val);
		virtual void OnAttrMinOccurs(const tstring& name, tchar const* val);
		virtual void OnAttrNamespace(const tstring& name, tchar const* val);
		virtual void OnAttrProcessContents(const tstring& name, tchar const* val);

	public:
		XmlSchemaAny AnyInfo;
	};
}}
