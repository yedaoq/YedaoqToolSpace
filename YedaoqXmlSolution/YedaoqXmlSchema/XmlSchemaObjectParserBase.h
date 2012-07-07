#pragma once
#include "XmlSchemaObjectParser.h"
#include "boost\dynamic_bitset.hpp"
#include <map>

namespace nsYedaoqXmlSchema{ namespace nsSerialize{	

	class CXmlSchemaObjectParserBase : public IXmlSchemaObjectParser
	{
	protected:
		typedef void (__thiscall CXmlSchemaObjectParserBase::*AttributeHandler)(const tstring& name, tchar const* val);

		struct AttributeMapItem
		{
			AttributeHandler	Handler;
			int					Index;

			AttributeMapItem(AttributeHandler handler, int idx)
				: Handler(handler), Index(idx)
			{}

			AttributeMapItem()
				: Handler(0), Index(-1)
			{}
		};

		typedef std::map<tstring, AttributeMapItem>	AttributeMap;

	protected:
		virtual void DispatchAttribute(xnode_t* node);

		virtual void OnMetaAttribute(const tstring& name, tchar const* val);
		virtual void OnMetaAnyAttribute(const tstring& name, tchar const* val) {};

		void OnIngoredMetaAttr(const tstring& name, tchar const* val);

		virtual AttributeMap const* GetAttributeMap() = 0;

		CXmlSchemaObjectParserBase(int attr_count, CSerializeContext *ctx)
			: AttributeMask(attr_count), Context(ctx)
		{}

	protected:
		boost::dynamic_bitset<>		AttributeMask;
		CSerializeContext*			Context;
	};


#define DECLARE_XSCOMPONENTPARSER_ATTRMAP	\
	static AttributeMap AttrMap;			\
	virtual AttributeMap const* GetAttributeMap() { return &AttrMap; }

#define MAKE_ATTR_MAP(Enum, fun) AttrMap[TEXT(#Enum)] = AttributeMapItem(static_cast<AttributeHandler>(&fun), Enum);
	#define MAKE_ATTR_MAP_EX(Enum, Str, fun) AttrMap[TEXT(#Str)] = AttributeMapItem(static_cast<AttributeHandler>(&fun), Enum);

}}