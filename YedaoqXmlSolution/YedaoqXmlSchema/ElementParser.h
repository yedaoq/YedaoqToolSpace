#pragma once
#include "ElementGroup.h"
#include "XmlSchemaObjectParserBase.h"

namespace nsYedaoqXmlSchema{ namespace nsSerialize
{
	class CElementParser : public CXmlSchemaObjectParserBase
	{
	protected:
		enum EnumAttribute
		{
			id,	name, ref, type, substitutionGroup,  default, fixed, form, maxOccurs, minOccurs, nillable, abstract, block, final, __upperbound,
		};

		DECLARE_XSCOMPONENTPARSER_ATTRMAP

	public:
		CElementParser(CSerializeContext* ctx);

		virtual bool Parse(xnode_t* node);
		virtual void OnInnerObject(IXmlSchemaObject* obj);
		virtual IXmlSchemaObject* Object() { return &ElementInfo; }

		virtual void CheckAttribute();

		static void Initialize();

		virtual void OnAttrID(const tstring& name, tchar const* val);
		virtual void OnAttrRef(const tstring& name, tchar const* val);
		virtual void OnAttrName(const tstring& name, tchar const* val);
		virtual void OnAttrType(const tstring& name, tchar const* val);
		virtual void OnAttrSubstitutionGroup(const tstring& name, tchar const* val);
		virtual void OnAttrDefault(const tstring& name, tchar const* val);
		virtual void OnAttrFixed(const tstring& name, tchar const* val);
		virtual void OnAttrForm(const tstring& name, tchar const* val);
		virtual void OnAttrMaxOccurs(const tstring& name, tchar const* val);
		virtual void OnAttrMinOccurs(const tstring& name, tchar const* val);
		virtual void OnAttrNillable(const tstring& name, tchar const* val);
		virtual void OnAttrAbstract(const tstring& name, tchar const* val);
		virtual void OnAttrBlock(const tstring& name, tchar const* val);
		virtual void OnAttrFinal(const tstring& name, tchar const* val);

		virtual void OnMetaAnyAttribute(const tstring& name, tchar const* val);
		void OnComplexType( xnode_t* child );
		void OnSimpleType( xnode_t* child );
	public:
		XmlSchemaElement ElementInfo;
	};
}}

