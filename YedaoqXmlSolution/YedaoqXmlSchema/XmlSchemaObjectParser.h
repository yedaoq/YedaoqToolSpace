/* ___________________________________________________________________________
@ 
@ file - xsComponentHandlerFactory.h
@ 
@ auth - yedaoq@gmail.com          http://blog.yedaoq.info
@ date - 2012-3-21
@ info -
@     
/* ___________________________________________________________________________*/
#pragma once

#include "CPP\microtype.h"
#include "CPP\tstring.h"
#include "boost\property_tree\detail\rapidxml.hpp"
#include "XmlSchemaCommon.h"
#include <stack>

typedef boost::property_tree::detail::rapidxml::xml_document<tchar>		xdoc_t;
typedef boost::property_tree::detail::rapidxml::xml_node<tchar>			xnode_t;
typedef boost::property_tree::detail::rapidxml::xml_attribute<tchar>	xattr_t;

namespace nsYedaoqXmlSchema
{
	class CXmlSchema;
}

namespace nsYedaoqXmlSchema{ namespace nsSerialize{	

	interface IXmlSchemaComponentDispatcher
	{
		virtual void Dispatch(xnode_t* node) = 0;
		virtual void DispatchChilds(xnode_t* node) = 0;
	};

	interface IXmlSchemaObjectParser
	{
		virtual bool				Parse(xnode_t* node) = 0;
		virtual IXmlSchemaObject*	Object() = 0;
		virtual void				OnInnerObject(IXmlSchemaObject* obj) = 0;
	};

	struct CSerializeContext
	{
		typedef std::stack<IXmlSchemaObjectParser*> ParserStack_t;

		CXmlSchema*						Schema;
		IXmlSchemaComponentDispatcher*	Dispatcher;
		ParserStack_t					PaserStack;
	};
}}
