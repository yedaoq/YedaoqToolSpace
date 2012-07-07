#include "StdAfx.h"
#include "SchemaParser.h"
#include "CPP\task.h"
#include <boost\lexical_cast.hpp>
#include "XmlSchema.h"

using namespace nsYedaoqXmlSchema;
using namespace nsYedaoqXmlSchema::nsSerialize;
using namespace boost::property_tree::detail::rapidxml;

bool nsYedaoqXmlSchema::nsSerialize::CSchemaParser::Parse( xnode_t* node )
{
	DispatchAttribute(node);
	Context->Dispatcher->DispatchChilds(node);
	return true;
}

void nsYedaoqXmlSchema::nsSerialize::CSchemaParser::OnInnerObject( IXmlSchemaObject* obj )
{
	if(obj->ObjectCategory() == EnumXmlSchemaObject::Element)
	{
		Context->Schema->AddElement(static_cast<XmlSchemaElement&>(*obj));
	}
	else if(obj->ObjectCategory() == EnumXmlSchemaObject::SimpleType || obj->ObjectCategory() == EnumXmlSchemaObject::ComplexType)
	{
		//Context->Schema->AddType(static_cast<XmlSchemaTypeHeader&>(*obj));
	}
	else if(obj->ObjectCategory() == EnumXmlSchemaObject::Annotation)
	{

	}
	else
	{
		throw std::exception("do not support!");
	}
}

void nsYedaoqXmlSchema::nsSerialize::CSchemaParser::Initialize()
{
	AttrMap[TEXT("id")]					= AttributeMapItem(static_cast<AttributeHandler>(&CSchemaParser::OnAttrID), Id);
	AttrMap[TEXT("attributeFormDefault")]= AttributeMapItem(static_cast<AttributeHandler>(&CSchemaParser::OnAttrAttributeFormDefault), AttributeFormDefault);
	AttrMap[TEXT("elementFormDefault")]	= AttributeMapItem(static_cast<AttributeHandler>(&CSchemaParser::OnAttrElementFormDefault), ElementFormDefault);
	AttrMap[TEXT("blockDefault")]		= AttributeMapItem(static_cast<AttributeHandler>(&CSchemaParser::OnAttrBlockDefault), BlockDefault);
	AttrMap[TEXT("finalDefault")]		= AttributeMapItem(static_cast<AttributeHandler>(&CSchemaParser::OnAttrFinalDefault), FinalDefault);
	AttrMap[TEXT("targetNamespace")]	= AttributeMapItem(static_cast<AttributeHandler>(&CSchemaParser::OnAttrTargetNamespace), TargetNamespace);
	AttrMap[TEXT("version")]			= AttributeMapItem(static_cast<AttributeHandler>(&CSchemaParser::OnAttrVersion), Version);
	AttrMap[TEXT("xmlns")]				= AttributeMapItem(static_cast<AttributeHandler>(&CSchemaParser::OnAttrXmlns), Xmlns);
}

void nsYedaoqXmlSchema::nsSerialize::CSchemaParser::OnAttrID( const tstring& name, tchar const* val )
{

}

void nsYedaoqXmlSchema::nsSerialize::CSchemaParser::OnAttrAttributeFormDefault( const tstring& name, tchar const* val )
{

}

void nsYedaoqXmlSchema::nsSerialize::CSchemaParser::OnAttrElementFormDefault( const tstring& name, tchar const* val )
{

}

void nsYedaoqXmlSchema::nsSerialize::CSchemaParser::OnAttrBlockDefault( const tstring& name, tchar const* val )
{

}


void nsYedaoqXmlSchema::nsSerialize::CSchemaParser::OnAttrFinalDefault( const tstring& name, tchar const* val )
{

}

void nsYedaoqXmlSchema::nsSerialize::CSchemaParser::OnAttrTargetNamespace( const tstring& name, tchar const* val )
{

}

void nsYedaoqXmlSchema::nsSerialize::CSchemaParser::OnAttrVersion( const tstring& name, tchar const* val )
{

}

void nsYedaoqXmlSchema::nsSerialize::CSchemaParser::OnAttrXmlns( const tstring& name, tchar const* val )
{

}

CXmlSchemaObjectParserBase::AttributeMap CSchemaParser::AttrMap;
CRAII_PreposeTaskRunner xsSchemaParserAttrMapInitializer(NewRunnableFunction(&CSchemaParser::Initialize));
