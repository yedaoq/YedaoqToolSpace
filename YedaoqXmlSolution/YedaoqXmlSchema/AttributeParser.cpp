#include "StdAfx.h"
#include "AttributeParser.h"
#include "CPP\task.h"
#include <boost\lexical_cast.hpp>
#include "XmlSchemaObjectParser.h"
#include "xsNamespace.h"

using namespace nsYedaoqXmlSchema;
using namespace nsYedaoqXmlSchema::nsSerialize;
using namespace boost::property_tree::detail::rapidxml;

bool nsYedaoqXmlSchema::nsSerialize::CAttributeParser::Parse( xnode_t* node )
{
	DispatchAttribute(node);
	Context->Dispatcher->DispatchChilds(node);

	return true;
}

void nsYedaoqXmlSchema::nsSerialize::CAttributeParser::OnInnerObject( IXmlSchemaObject* obj )
{
	if(obj->ObjectCategory() == EnumXmlSchemaObject::SimpleType)
	{
		if(AttributeMask[type])
		{
			throw std::exception("xs:attribute with a type attribute and a xs:simpleType sub element simultaneously is not allowed!");
		}

		//XmlSchemaTypeHeader* typeInfo = static_cast<XmlSchemaTypeHeader&>(*obj);
		AttrInfo.Type = Context->Schema->AddType(static_cast<XmlSchemaTypeHeader&>(*obj));
	}
	else if(obj->ObjectCategory() == EnumXmlSchemaObject::Annotation)
	{

	}
	else
	{
		throw std::exception("unsupported xs:attribute sub element");
	}
}

void nsYedaoqXmlSchema::nsSerialize::CAttributeParser::OnAttrID( const tstring& name, tchar const* val )
{

}

void nsYedaoqXmlSchema::nsSerialize::CAttributeParser::OnAttrDefault( const tstring& name, tchar const* val )
{

}

void nsYedaoqXmlSchema::nsSerialize::CAttributeParser::OnAttrFixed( const tstring& name, tchar const* val )
{

}

void nsYedaoqXmlSchema::nsSerialize::CAttributeParser::OnAttrForm( const tstring& name, tchar const* val )
{
	
}

void nsYedaoqXmlSchema::nsSerialize::CAttributeParser::OnAttrName( const tstring& name, tchar const* val )
{
	AttrInfo.Name = val;
}

void nsYedaoqXmlSchema::nsSerialize::CAttributeParser::OnAttrRef( const tstring& name, tchar const* val )
{
	throw std::exception("do not support ref attribute");
}

void nsYedaoqXmlSchema::nsSerialize::CAttributeParser::OnAttrType( const tstring& name, tchar const* val )
{
	AttrInfo.Type = Context->Schema->GetType(val, true)->SerialNO;
}

void nsYedaoqXmlSchema::nsSerialize::CAttributeParser::OnAttrUse( const tstring& name, tchar const* val )
{

}

void nsYedaoqXmlSchema::nsSerialize::CAttributeParser::OnMetaAnyAttribute( const tstring& name, tchar const* val )
{
	AttrInfo.MetaAnyAttributes.insert(name, val);
}

void nsYedaoqXmlSchema::nsSerialize::CAttributeParser::Initialize()
{	
	MAKE_ATTR_MAP(id, CAttributeParser::OnAttrID);	
	MAKE_ATTR_MAP(default, CAttributeParser::OnAttrDefault);
	MAKE_ATTR_MAP(fixed, CAttributeParser::OnAttrFixed);
	MAKE_ATTR_MAP(form, CAttributeParser::OnAttrForm);
	MAKE_ATTR_MAP(name, CAttributeParser::OnAttrName);
	MAKE_ATTR_MAP(ref, CAttributeParser::OnAttrRef);
	MAKE_ATTR_MAP(type, CAttributeParser::OnAttrType);
	MAKE_ATTR_MAP(use, CAttributeParser::OnAttrUse);
}


CXmlSchemaObjectParserBase::AttributeMap CAttributeParser::AttrMap;
CRAII_PreposeTaskRunner xsAttributeParserAttrMapInitializer(NewRunnableFunction(&CAttributeParser::Initialize));