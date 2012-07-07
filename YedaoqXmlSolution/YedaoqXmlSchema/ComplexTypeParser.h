#pragma once
/*#include "ElementGroup.h"*/
#include "XmlSchemaTypeDefinition.h"
#include "XmlSchemaObjectParserBase.h"

namespace nsYedaoqXmlSchema{ namespace nsSerialize
{
	class CComplexTypeParser : public CXmlSchemaObjectParserBase
	{
	protected:
		enum EnumAttribute
		{
			id,	name, mixed, block, abstract, final, __upperbound,
		};

		DECLARE_XSCOMPONENTPARSER_ATTRMAP

	public:
		CComplexTypeParser(CSerializeContext* ctx)
			: CXmlSchemaObjectParserBase(EnumAttribute::__upperbound, ctx)
		{}

		static void Initialize();

		virtual bool Parse(xnode_t* node);
		virtual void OnInnerObject(IXmlSchemaObject* obj);
		virtual IXmlSchemaObject* Object() { return &TypeInfo; }

		virtual void CheckAttribute();

		virtual void ParseComplexContent( xnode_t* child );
		virtual void ParseSimpleContent( xnode_t* child );
		virtual void ParseContentDefination( xnode_t* node );
		virtual void ParseRestriction( xnode_t* node);
		virtual void ParseExtension( xnode_t* node);

		virtual void OnAttrID(const tstring& name, tchar const* val);
		virtual void OnAttrMixed(const tstring& name, tchar const* val);
		virtual void OnAttrName(const tstring& name, tchar const* val);
		virtual void OnAttrAbstract(const tstring& name, tchar const* val);
		virtual void OnAttrBlock(const tstring& name, tchar const* val);
		virtual void OnAttrFinal(const tstring& name, tchar const* val);

	public:
		XmlSchemaComplexType TypeInfo;
	};
}}

