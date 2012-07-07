#include "StdAfx.h"
#include "GroupParser.h"
#include "CPP\task.h"

using namespace nsYedaoqXmlSchema;
using namespace nsYedaoqXmlSchema::nsSerialize;
using namespace boost::property_tree::detail::rapidxml;

bool nsYedaoqXmlSchema::nsSerialize::CGroupParser::Parse( xnode_t* node )
{
	DispatchAttribute(node);
	Context->Dispatcher->DispatchChilds(node);
	return true;
}

void nsYedaoqXmlSchema::nsSerialize::CGroupParser::OnInnerObject( IXmlSchemaObject* obj )
{
	if(obj->ObjectCategory() == EnumXmlSchemaObject::Group || obj->ObjectCategory() == EnumXmlSchemaObject::Element || obj->ObjectCategory() == EnumXmlSchemaObject::Any)
	{
		GroupInfo.Subitems.push_back(static_cast<XmlSchemaSubitem&>(*obj));
	}
}

void nsYedaoqXmlSchema::nsSerialize::CGroupParser::Initialize()
{
	MAKE_ATTR_MAP(id, CGroupParser::OnAttrID);
	MAKE_ATTR_MAP(maxOccurs, CGroupParser::OnAttrMaxOccurs);
	MAKE_ATTR_MAP(minOccurs, CGroupParser::OnAttrMinOccurs);
}

void nsYedaoqXmlSchema::nsSerialize::CGroupParser::OnAttrID( const tstring& name, tchar const* val )
{

}

void nsYedaoqXmlSchema::nsSerialize::CGroupParser::OnAttrMaxOccurs( const tstring& name, tchar const* val )
{
	GroupInfo.MaxOccurs = boost::lexical_cast<int>(val);
}

void nsYedaoqXmlSchema::nsSerialize::CGroupParser::OnAttrMinOccurs( const tstring& name, tchar const* val )
{
	GroupInfo.MinOccurs = boost::lexical_cast<int>(val);
}

CXmlSchemaObjectParserBase::AttributeMap CGroupParser::AttrMap;
CRAII_PreposeTaskRunner xsGroupParserAttrMapInitializer(NewRunnableFunction(&CGroupParser::Initialize));