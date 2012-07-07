#include "StdAfx.h"
#include "XmlSchemaObjectParserBase.h"

using namespace nsYedaoqXmlSchema;
using namespace nsYedaoqXmlSchema::nsSerialize;

void CXmlSchemaObjectParserBase::DispatchAttribute( xnode_t* node )
{
	xattr_t* attr = node->first_attribute();

	while(attr)
	{
		OnMetaAttribute(attr->name(), attr->value());
		attr = attr->next_attribute();
	}
}

void CXmlSchemaObjectParserBase::OnMetaAttribute( const tstring& name, tchar const* val )
{
	AttributeMap const* amap = GetAttributeMap();
	AttributeMap::const_iterator iter = amap->find(name);

	if(amap->end() == iter)
	{
		OnMetaAnyAttribute(name, val);
	}
	else
	{
		if(AttributeMask[iter->second.Index])
		{
			throw std::exception("attribute can not repeat !");
		}
		else
		{
			AttributeMask[iter->second.Index] = true;
			(this->*(iter->second.Handler))(name, val);
		}
	}
}

void CXmlSchemaObjectParserBase::OnIngoredMetaAttr( const tstring& name, tchar const* val )
{

}

