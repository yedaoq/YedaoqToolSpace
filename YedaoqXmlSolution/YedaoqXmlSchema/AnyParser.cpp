#include "StdAfx.h"
#include "AnyParser.h"
#include "CPP\task.h"

using namespace nsYedaoqXmlSchema;
using namespace nsYedaoqXmlSchema::nsSerialize;
using namespace boost::property_tree::detail::rapidxml;


bool nsYedaoqXmlSchema::nsSerialize::CAnyParser::Parse( xnode_t* node )
{
	DispatchAttribute(node);
	Context->Dispatcher->Dispatch(node);
	return true;
}

void nsYedaoqXmlSchema::nsSerialize::CAnyParser::OnInnerObject( IXmlSchemaObject* obj )
{
	if(obj->ObjectCategory() == EnumXmlSchemaObject::Annotation)
	{

	}
	else
	{
		throw std::exception("unexpected sub element of xs:any");
	}
}

void nsYedaoqXmlSchema::nsSerialize::CAnyParser::Initialize()
{
	MAKE_ATTR_MAP(id, CAnyParser::OnAttrID);	
	MAKE_ATTR_MAP(maxOccurs, CAnyParser::OnAttrMaxOccurs);
	MAKE_ATTR_MAP(minOccurs, CAnyParser::OnAttrMinOccurs);
	MAKE_ATTR_MAP(processContents, CAnyParser::OnAttrProcessContents);
	MAKE_ATTR_MAP_EX(Namespace, namespace, CAnyParser::OnAttrNamespace);
}

void nsYedaoqXmlSchema::nsSerialize::CAnyParser::OnAttrID( const tstring& name, tchar const* val )
{

}

void nsYedaoqXmlSchema::nsSerialize::CAnyParser::OnAttrMaxOccurs( const tstring& name, tchar const* val )
{
	AnyInfo.MaxOccurs = boost::lexical_cast<int>(val);
}

void nsYedaoqXmlSchema::nsSerialize::CAnyParser::OnAttrMinOccurs( const tstring& name, tchar const* val )
{
	AnyInfo.MinOccurs = boost::lexical_cast<int>(val);
}

void nsYedaoqXmlSchema::nsSerialize::CAnyParser::OnAttrNamespace( const tstring& name, tchar const* val )
{
	AnyInfo.NameSpace = val;
}

void nsYedaoqXmlSchema::nsSerialize::CAnyParser::OnAttrProcessContents( const tstring& name, tchar const* val )
{
	if(val)
	{
		if(EnumXmlSchemaAnyProcessPolicy(EnumXmlSchemaAnyProcessPolicy::lax).str() == val)
		{
			AnyInfo.ProcessPolicy = EnumXmlSchemaAnyProcessPolicy::lax;
		}
		else if(EnumXmlSchemaAnyProcessPolicy(EnumXmlSchemaAnyProcessPolicy::strict).str() == val)
		{
			AnyInfo.ProcessPolicy = EnumXmlSchemaAnyProcessPolicy::strict;
		}
		else if(EnumXmlSchemaAnyProcessPolicy(EnumXmlSchemaAnyProcessPolicy::skip).str() == val)
		{
			AnyInfo.ProcessPolicy = EnumXmlSchemaAnyProcessPolicy::skip;
		}
		else
		{
			throw std::exception("unrecognized xs:any process policy!");
		}
	}
}

CXmlSchemaObjectParserBase::AttributeMap CAnyParser::AttrMap;
CRAII_PreposeTaskRunner xsAnyParserAttrMapInitializer(NewRunnableFunction(&CAnyParser::Initialize));
