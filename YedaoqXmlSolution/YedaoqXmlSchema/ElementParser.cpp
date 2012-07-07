#include "stdafx.h"
#include "ElementParser.h"
#include "XmlSchema.h"
#include "CPP\task.h"

using namespace nsYedaoqXmlSchema::nsSerialize;
using namespace boost::property_tree::detail::rapidxml;

CElementParser::CElementParser(CSerializeContext* ctx)
	: CXmlSchemaObjectParserBase(EnumAttribute::__upperbound, ctx)
{}

bool nsYedaoqXmlSchema::nsSerialize::CElementParser::Parse( xnode_t* node)
{
	DispatchAttribute(node);
	CheckAttribute();	

	Context->Dispatcher->DispatchChilds(node);

	return true;
}

void nsYedaoqXmlSchema::nsSerialize::CElementParser::OnInnerObject( IXmlSchemaObject* obj )
{
	if(obj->ObjectCategory() == EnumXmlSchemaObject::SimpleType
		|| obj->ObjectCategory() == EnumXmlSchemaObject::ComplexType)
	{
		ElementInfo.Type = Context->Schema->AddType(static_cast<XmlSchemaTypeHeader&>(*obj));
	}
	else
	{
		throw std::exception("unsupported xs:element defination!");
	}
}

void nsYedaoqXmlSchema::nsSerialize::CElementParser::CheckAttribute()
{
	// global xs:element need a name
	if(Context->PaserStack.size() <= 2)
	{
		if(!AttributeMask[name])
		{
			throw std::exception("global xs:element declaration need a 'name'");
		}

		if(AttributeMask[ref])
		{
			throw std::exception("global xs:element declaration do not support 'ref'");
		}

		if(AttributeMask[substitutionGroup])
		{
			throw std::exception("global xs:element declaration do not support 'substitutionGroup'");
		}

		if(AttributeMask[maxOccurs])
		{
			throw std::exception("global xs:element declaration do not support 'maxOccurs'");
		}

		if(AttributeMask[minOccurs])
		{
			throw std::exception("global xs:element declaration do not support 'minOccurs'");
		}
	}
}

void nsYedaoqXmlSchema::nsSerialize::CElementParser::OnMetaAnyAttribute( const tstring& name, tchar const* val )
{
	ElementInfo.MetaAnyAttributes.insert(name, val);
}

void nsYedaoqXmlSchema::nsSerialize::CElementParser::OnComplexType( xnode_t* child )
{
	throw std::exception("The method or operation is not implemented.");
}

void nsYedaoqXmlSchema::nsSerialize::CElementParser::OnSimpleType( xnode_t* child )
{
	throw std::exception("The method or operation is not implemented.");
}

void CElementParser::OnAttrID(const tstring& name,  tchar const* val )
{

}

void CElementParser::OnAttrRef(const tstring& name,  tchar const* val )
{

}

void CElementParser::OnAttrName(const tstring& name,  tchar const* val )
{
	if(!val || internal::measure(val) == 0 || _tcschr(val, ':'))
	{
		throw std::exception("invalid value");
	}
	ElementInfo.Name = val;
}

void CElementParser::OnAttrType(const tstring& name,  tchar const* val )
{
	ElementInfo.Type = Context->Schema->GetType(val, true)->SerialNO;
}

void CElementParser::OnAttrSubstitutionGroup(const tstring& name,  tchar const* val )
{
	
}

void CElementParser::OnAttrDefault(const tstring& name,  tchar const* val )
{
	ElementInfo.Default = val;
}

void CElementParser::OnAttrFixed(const tstring& name,  tchar const* val )
{

}

void CElementParser::OnAttrForm(const tstring& name,  tchar const* val )
{

}

void CElementParser::OnAttrMaxOccurs(const tstring& name,  tchar const* val )
{
	ElementInfo.MaxOccurs = boost::lexical_cast<int>(val);
}

void CElementParser::OnAttrMinOccurs(const tstring& name,  tchar const* val )
{
	ElementInfo.MinOccurs = boost::lexical_cast<int>(val);
}

void CElementParser::OnAttrNillable(const tstring& name,  tchar const* val )
{
	
}

void CElementParser::OnAttrAbstract(const tstring& name,  tchar const* val )
{

}

void CElementParser::OnAttrBlock(const tstring& name,  tchar const* val )
{

}

void CElementParser::OnAttrFinal(const tstring& name,  tchar const* val )
{
	
}

void CElementParser::Initialize()
{
	MAKE_ATTR_MAP(id, CElementParser::OnAttrID);	
	MAKE_ATTR_MAP(name, CElementParser::OnAttrName);
	MAKE_ATTR_MAP(ref, CElementParser::OnAttrRef);
	MAKE_ATTR_MAP(type, CElementParser::OnAttrType);
	MAKE_ATTR_MAP(default, CElementParser::OnAttrDefault);
	MAKE_ATTR_MAP(fixed, CElementParser::OnAttrFixed);
	MAKE_ATTR_MAP(form, CElementParser::OnAttrForm);
	MAKE_ATTR_MAP(substitutionGroup, CElementParser::OnAttrSubstitutionGroup);
	MAKE_ATTR_MAP(maxOccurs, CElementParser::OnAttrMaxOccurs);	
	MAKE_ATTR_MAP(minOccurs, CElementParser::OnAttrMinOccurs);
	MAKE_ATTR_MAP(nillable, CElementParser::OnAttrNillable);
	MAKE_ATTR_MAP(abstract, CElementParser::OnAttrAbstract);
	MAKE_ATTR_MAP(block, CElementParser::OnAttrBlock);
	MAKE_ATTR_MAP(final, CElementParser::OnAttrFinal);
}

CXmlSchemaObjectParserBase::AttributeMap CElementParser::AttrMap;
CRAII_PreposeTaskRunner xsElementParserAttrMapInitializer(NewRunnableFunction(&CElementParser::Initialize));