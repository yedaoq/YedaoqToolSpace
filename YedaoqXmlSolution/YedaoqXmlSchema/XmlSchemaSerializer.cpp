#include "stdafx.h"
#include <vector>
#include "XmlSchemaSerializer.h"
#include "SchemaParser.h"
#include "ElementParser.h"
#include "ComplexTypeParser.h"
#include "SimpleTypeParser.h"
#include "GroupParser.h"
#include "AnnotationParser.h"
#include "AnyParser.h"
#include <boost\smart_ptr\scoped_ptr.hpp>
#include <boost\property_tree\detail\rapidxml.hpp>
#include "AttributeParser.h"

using namespace nsYedaoqXmlSchema;
using namespace nsYedaoqXmlSchema::nsSerialize;
using namespace boost::property_tree::detail;

struct CRAII_ParserStackTopper
{
	CRAII_ParserStackTopper(CSerializeContext* ctx, IXmlSchemaObjectParser* parser)
		: Context(ctx), Parser(parser)
	{
		Context->PaserStack.push(parser);
	}

	~CRAII_ParserStackTopper()
	{
		BOOST_ASSERT(Context->PaserStack.top() == Parser);
		Context->PaserStack.pop();
	}

protected:
	CSerializeContext*		Context;
	IXmlSchemaObjectParser*	Parser;
};

int CXmlSchemaSerializer::Load( CXmlSchema& schema, tchar* text )
{
	xdoc_t doc;
	doc.parse<rapidxml::parse_trim_whitespace>(text);
	
	// Xml Schema Document should have and only have a root element : xs:schema
	if(!doc.first_node() || doc.first_node()->next_sibling() || CXsString(doc.first_node()->name()) != CXsString::schema)
	{
		throw std::exception("invalid xml schema doc!");
	}

	// initialize context
	Context.Schema = &schema;
	Context.Dispatcher = this;

	// parse the root xs:schema
	CSchemaParser parser(&Context);

	Context.PaserStack.push(&parser);
	bool bSuccessed = parser.Parse(doc.first_node());
	BOOST_ASSERT(Context.PaserStack.top() == &parser);
	Context.PaserStack.pop();
	
	return bSuccessed;
}

void nsYedaoqXmlSchema::nsSerialize::CXmlSchemaSerializer::Dispatch( xnode_t* node )
{
	if(rapidxml::node_type::node_element != node->type())
	{
		throw std::exception("unexpected xml node!");
	}

	boost::scoped_ptr<IXmlSchemaObjectParser> parser;
	CXsString nodeName(node->name());

	if(nodeName == CXsString::simpleType)
	{
		parser.reset(new CSimpleTypeParser(&Context));
	}
	else if(nodeName == CXsString::complexType)
	{
		parser.reset(new CComplexTypeParser(&Context));
	}
	else if(nodeName == CXsString::element)
	{
		parser.reset(new CElementParser(&Context));
	}
	else if(nodeName == CXsString::sequence || nodeName == CXsString::choice || nodeName == CXsString::all)
	{
		parser.reset(new CGroupParser(&Context));
	}
	else if(nodeName == CXsString::any)
	{
		parser.reset(new CAnyParser(&Context));
	}
	else if(nodeName == CXsString::attribute)
	{
		parser.reset(new CAttributeParser(&Context));
	}
	else if(nodeName == CXsString::annotation)
	{
		parser.reset(new CAnnotationParser(&Context));
	}
	else
	{
		// error
		BOOST_ASSERT(false);
	}

	if(parser.get())
	{
		Context.PaserStack.push(parser.get());
		bool bSuccessed = parser->Parse(node);
		BOOST_ASSERT(Context.PaserStack.top() == parser.get());
		Context.PaserStack.pop();
		
		if(bSuccessed)
		{
			Context.PaserStack.top()->OnInnerObject(parser->Object());
		}
	}
}

void nsYedaoqXmlSchema::nsSerialize::CXmlSchemaSerializer::DispatchChilds( xnode_t* node )
{
	for(xnode_t* child = node->first_node(); child; child = child->next_sibling())
	{
		Dispatch(child);
	}
}

