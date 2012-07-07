#pragma once
#include "XmlSchema.h"
#include "XmlSchemaObjectParser.h"

namespace nsYedaoqXmlSchema{ namespace nsSerialize 
{
	class CXmlSchemaSerializer : public IXmlSchemaComponentDispatcher
	{
	public:
		CXmlSchemaSerializer() {};

		int					Load(nsYedaoqXmlSchema::CXmlSchema& schema, tchar* text);
		virtual void		Dispatch(xnode_t* node);
		virtual void		DispatchChilds(xnode_t* node);

	protected:
		CSerializeContext Context;
	};
}}