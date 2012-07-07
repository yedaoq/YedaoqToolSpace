#pragma once
#include "XmlSchemaObjectParserBase.h"
#include "XmlSchemaTypeDefinition.h"

namespace nsYedaoqXmlSchema{ namespace nsSerialize
{
	class CSimpleTypeParser : public CXmlSchemaObjectParserBase
	{
	protected:
		enum EnumAttribute
		{
			id,	name, __upperbound,
		};

		DECLARE_XSCOMPONENTPARSER_ATTRMAP

	public:
		CSimpleTypeParser(CSerializeContext* ctx)
			: CXmlSchemaObjectParserBase(EnumAttribute::__upperbound, ctx)
		{}

		static void Initialize();

		virtual bool Parse(xnode_t* node);
		virtual void OnInnerObject(IXmlSchemaObject* obj);
		virtual IXmlSchemaObject* Object() { return &TypeInfo; }

		virtual void CheckAttribute();
		virtual void DispatchFacet(xnode_t* node);

		void ParseRestriction( xnode_t* child );
		void ParseList( xnode_t* child );
		void ParseUnion( xnode_t* child );

		void OnFacetDataCategory(EnumSimpleTypeCategory cat);

		virtual void OnAttrID(const tstring& name, tchar const* val);
		virtual void OnAttrName(const tstring& name, tchar const* val);
		void OnFractionDigits( tchar* facetValue );
		void OnEnumeration( tchar* facetValue );
		void OnLength( tchar* facetValue );
		void OnMaxExclusive( tchar* facetValue );
		void OnMaxInclusive( tchar* facetValue );
		void OnMaxLength( tchar* facetValue );
		void OnMinExclusive( tchar* facetValue );
		void OnMinInclusive( tchar* facetValue );
		void OnMinLength( tchar* facetValue );
		void OnPattern( tchar* facetValue );
		void OnTotalDigits( tchar* facetValue );
		void OnWhiteSpace( tchar* facetValue );
	public:
		XmlSchemaSimpleType TypeInfo;
	};
}}
