#include "stdafx.h"
#include "XmlSchema.h"
#include "boost\assert.hpp"

using namespace nsYedaoqXmlSchema;

#define SAFEDELETE(ptr) if(ptr){delete ptr; ptr = 0;}

bool XmlSchemaSimpleType::Assert( EnumSimpleTypeCategory t ) const
{
	BOOST_ASSERT(SimpleTypeCategory() == t);
	return SimpleTypeCategory() == t;
}

nsYedaoqXmlSchema::XmlSchemaTypeSN_t XmlSchemaSimpleType::BaseType() const
{
	switch (SimpleTypeCategory())
	{
	case EnumSimpleTypeCategory::List:
	case EnumSimpleTypeCategory::Union:
		return EnumBuildInType::AnySimpleType;
		break;
	default:
		return BaseType_;
		break;
	}
}

void XmlSchemaSimpleType::SetDataGroup( const ISimpleTypeDataGroup& data )
{
	DataGroup_ = data;
}

XmlSchemaSimpleType::XmlSchemaSimpleType()
	: DataGroup_(&SimpleTypeFacetNoneData::GetInstance()), BaseType_(INVALIDXMLSCHEMATYPESN), FacetWhiteSpace(EnumWhiteSpaceRule::Unappinted)
{}


void nsYedaoqXmlSchema::XmlSchemaSimpleType::SimpleTypeCategory( EnumSimpleTypeCategory cat )
{
	if(cat == EnumSimpleTypeCategory::List)
	{
		SetDataGroup(SimpleTypeListData());
	}
	else if(cat == EnumSimpleTypeCategory::Union)
	{
		SetDataGroup(SimpleTypeUnionData());
	}
	else if(cat == EnumSimpleTypeCategory::RestrictEnum)
	{
		SetDataGroup(SimpleTypeFacetEnumData());
	}
	else if(cat == EnumSimpleTypeCategory::RestrictLength)
	{
		SetDataGroup(SimpleTypeFacetLengthData());
	}
	else if(cat == EnumSimpleTypeCategory::RestrictRange)
	{
		SetDataGroup(SimpleTypeFacetRangeData());
	}
	else if(cat == EnumSimpleTypeCategory::RestrictPattern)
	{
		SetDataGroup(SimpleTypeFacetPatternData());
	}
	else if(cat == EnumSimpleTypeCategory::RestrictNone)
	{
		SetDataGroup(SimpleTypeFacetNoneData::GetInstance());
	}
}