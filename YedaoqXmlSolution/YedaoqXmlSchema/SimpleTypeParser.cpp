#include "StdAfx.h"
#include "XmlSchema.h"
#include "SimpleTypeParser.h"
#include "FacetGroup.h"
#include "XmlSchemaTypeDefinition.h"
#include <boost\tokenizer.hpp>
#include <boost\typeof\typeof.hpp>
#include "CPP\task.h"

using namespace nsYedaoqXmlSchema;
using namespace nsYedaoqXmlSchema::nsSerialize;
using namespace boost::property_tree::detail::rapidxml;

bool CSimpleTypeParser::Parse( xnode_t* node )
{
	DispatchAttribute(node);
	
	xnode_t* child = 0;
	if( 0 != (child = node->first_node(TEXT("xs:restriction"))) || 0 != (child = node->first_node(TEXT("restriction"))))
	{
		ParseRestriction(child);
	}
	else if( 0 != (child = node->first_node(TEXT("xs:list"))) || 0 != (child = node->first_node(TEXT("list"))))
	{
		ParseList(child);
	}
	else if( 0 != (child = node->first_node(TEXT("xs:union"))) || 0 != (child = node->first_node(TEXT("union"))))
	{
		ParseUnion(child);
	}

	TypeInfo.SerialNO = Context->Schema->AddType(TypeInfo);

	return true;
}

void CSimpleTypeParser::OnInnerObject( IXmlSchemaObject* obj )
{
	if(obj->ObjectCategory() == EnumXmlSchemaObject::SimpleType)
	{
		XmlSchemaSimpleType* typeTmp = static_cast<XmlSchemaSimpleType*>(obj);
		if(TypeInfo.SimpleTypeCategory() == EnumSimpleTypeCategory::List)
		{
			SimpleTypeListData* data = TypeInfo.GetDataGroup<SimpleTypeListData>();			
			data->ItemType = typeTmp->SerialNO;
		}
		else if(TypeInfo.SimpleTypeCategory() == EnumSimpleTypeCategory::Union)
		{
			SimpleTypeUnionData* data = TypeInfo.GetDataGroup<SimpleTypeUnionData>();
			data->push_back(typeTmp->SerialNO);
		}
	}
	else if(obj->ObjectCategory() == EnumXmlSchemaObject::Annotation)
	{

	}
	else
	{
		BOOST_ASSERT(false);
	}
}

void CSimpleTypeParser::OnAttrID( const tstring& name, tchar const* val )
{

}

void CSimpleTypeParser::OnAttrName( const tstring& name, tchar const* val )
{
	TypeInfo.Name = val;
}

void CSimpleTypeParser::ParseRestriction( xnode_t* node )
{
	xattr_t* attr = node->first_attribute(TEXT("base"));

	if(!attr)
	{
		throw std::exception("xs:restriction need a base attribute");
	}

	// Get the named type SN, when nonexist, create a virtual type
	TypeInfo.BaseType(Context->Schema->GetType(attr->value(), true)->SerialNO);

	for(xnode_t* child = node->first_node(); child; child = child->next_sibling())
	{
		DispatchFacet(child);
	}		
}

void CSimpleTypeParser::ParseList( xnode_t* node )
{
	SimpleTypeListData data;
	xattr_t* attr = node->first_attribute(TEXT("itemType"));

	if(attr)
	{
		data.ItemType = Context->Schema->GetType(attr->value(),true)->SerialNO;
	}
	
	TypeInfo.SetDataGroup(data);

	Context->Dispatcher->DispatchChilds(node);
}

void CSimpleTypeParser::ParseUnion( xnode_t* node )
{
	SimpleTypeUnionData data;
	xattr_t* attr = node->first_attribute(TEXT("memberTypes"));

	if(attr)
	{
		boost::tokenizer<boost::char_separator<tchar>, tstring::const_iterator, tstring> tok(tstring(attr->value()));
		for(BOOST_AUTO(pos, tok.begin()); pos != tok.end(); ++pos)
		{
			XmlSchemaTypeHeader* typeHeader = Context->Schema->GetType(*pos, true);
			data.push_back(Context->Schema->GetType(*pos, true)->SerialNO);
		}
	}

	TypeInfo.SetDataGroup(data);

	Context->Dispatcher->DispatchChilds(node);
}

void nsYedaoqXmlSchema::nsSerialize::CSimpleTypeParser::DispatchFacet( xnode_t* node )
{
	CXsString nodeName(node->name());
	tchar* facetValue = 0;
	xattr_t* attr = node->first_attribute(TEXT("value"));
	if(attr)
	{
		facetValue = attr->value();
	}

	if(nodeName == CXsString::enumeration)
	{
		OnEnumeration(facetValue);
	}
	else if(nodeName == CXsString::fractionDigits)
	{
		OnFractionDigits(facetValue);	
	}
	else if(nodeName == CXsString::length)
	{
		OnLength(facetValue);
	}
	else if(nodeName == CXsString::maxExclusive)
	{
		OnMaxExclusive(facetValue);
	}
	else if(nodeName == CXsString::maxInclusive)
	{
		OnMaxInclusive(facetValue);
	}
	else if(nodeName == CXsString::maxLength)
	{
		OnMaxLength(facetValue);
	}
	else if(nodeName == CXsString::minExclusive)
	{
		OnMinExclusive(facetValue);
	}
	else if(nodeName == CXsString::minInclusive)
	{
		OnMinInclusive(facetValue);
	}
	else if(nodeName == CXsString::minLength)
	{
		OnMinLength(facetValue);
	}
	else if(nodeName == CXsString::pattern)
	{
		OnPattern(facetValue);
	}
	else if(nodeName == CXsString::totalDigits)
	{
		OnTotalDigits(facetValue);
	}
	else if(nodeName == CXsString::whiteSpace)
	{
		OnWhiteSpace(facetValue);
	}
	else
	{
		//error
	}
}

void nsYedaoqXmlSchema::nsSerialize::CSimpleTypeParser::OnFractionDigits( tchar* facetValue )
{
	OnFacetDataCategory(EnumSimpleTypeCategory::RestrictRange);
	SimpleTypeFacetRangeData* data = TypeInfo.GetDataGroup<SimpleTypeFacetRangeData>();
	data->FractionDigits = boost::lexical_cast<int>(facetValue);
}

void nsYedaoqXmlSchema::nsSerialize::CSimpleTypeParser::OnEnumeration( tchar* facetValue )
{
	OnFacetDataCategory(EnumSimpleTypeCategory::RestrictEnum);
	SimpleTypeFacetEnumData* data = TypeInfo.GetDataGroup<SimpleTypeFacetEnumData>();
	data->push_back(facetValue);
}

void nsYedaoqXmlSchema::nsSerialize::CSimpleTypeParser::OnLength( tchar* facetValue )
{
	OnFacetDataCategory(EnumSimpleTypeCategory::RestrictLength);
	SimpleTypeFacetLengthData* data = TypeInfo.GetDataGroup<SimpleTypeFacetLengthData>();
	data->Length = boost::lexical_cast<int>(facetValue);
}

void nsYedaoqXmlSchema::nsSerialize::CSimpleTypeParser::OnMaxExclusive( tchar* facetValue )
{
	OnFacetDataCategory(EnumSimpleTypeCategory::RestrictRange);
	SimpleTypeFacetRangeData* data = TypeInfo.GetDataGroup<SimpleTypeFacetRangeData>();
	data->UpperBound = facetValue;
}

void nsYedaoqXmlSchema::nsSerialize::CSimpleTypeParser::OnMaxInclusive( tchar* facetValue )
{
	OnFacetDataCategory(EnumSimpleTypeCategory::RestrictRange);
	SimpleTypeFacetRangeData* data = TypeInfo.GetDataGroup<SimpleTypeFacetRangeData>();
	data->UpperBound = facetValue;
	data->UpperInclusive = true;
}

void nsYedaoqXmlSchema::nsSerialize::CSimpleTypeParser::OnMaxLength( tchar* facetValue )
{
	OnFacetDataCategory(EnumSimpleTypeCategory::RestrictLength);
	SimpleTypeFacetLengthData* data = TypeInfo.GetDataGroup<SimpleTypeFacetLengthData>();
	data->MaxLength = boost::lexical_cast<int>(facetValue);
}

void nsYedaoqXmlSchema::nsSerialize::CSimpleTypeParser::OnMinExclusive( tchar* facetValue )
{
	OnFacetDataCategory(EnumSimpleTypeCategory::RestrictRange);
	SimpleTypeFacetRangeData* data = TypeInfo.GetDataGroup<SimpleTypeFacetRangeData>();
	data->LowerBound = facetValue;
}

void nsYedaoqXmlSchema::nsSerialize::CSimpleTypeParser::OnMinInclusive( tchar* facetValue )
{
	OnFacetDataCategory(EnumSimpleTypeCategory::RestrictRange);
	SimpleTypeFacetRangeData* data = TypeInfo.GetDataGroup<SimpleTypeFacetRangeData>();
	data->LowerBound = facetValue;
	data->LowerInclusive = true;
}

void nsYedaoqXmlSchema::nsSerialize::CSimpleTypeParser::OnMinLength( tchar* facetValue )
{
	OnFacetDataCategory(EnumSimpleTypeCategory::RestrictLength);
	SimpleTypeFacetLengthData* data = TypeInfo.GetDataGroup<SimpleTypeFacetLengthData>();
	data->MinLength = boost::lexical_cast<int>(facetValue);
}

void nsYedaoqXmlSchema::nsSerialize::CSimpleTypeParser::OnPattern( tchar* facetValue )
{
	OnFacetDataCategory(EnumSimpleTypeCategory::RestrictPattern);
	SimpleTypeFacetPatternData* data = TypeInfo.GetDataGroup<SimpleTypeFacetPatternData>();
	data->push_back(facetValue);
}

void nsYedaoqXmlSchema::nsSerialize::CSimpleTypeParser::OnTotalDigits( tchar* facetValue )
{
	OnFacetDataCategory(EnumSimpleTypeCategory::RestrictRange);
	SimpleTypeFacetRangeData* data = TypeInfo.GetDataGroup<SimpleTypeFacetRangeData>();
	data->TotalDigits = boost::lexical_cast<int>(facetValue);
}

void nsYedaoqXmlSchema::nsSerialize::CSimpleTypeParser::OnWhiteSpace( tchar* facetValue )
{
	const EnumWhiteSpaceRule::item* e = EnumWhiteSpaceRule::get(facetValue);
	if(e)
	{
		TypeInfo.FacetWhiteSpace = e->Val;
	}
}

void nsYedaoqXmlSchema::nsSerialize::CSimpleTypeParser::OnFacetDataCategory( EnumSimpleTypeCategory cat )
{
	if(TypeInfo.SimpleTypeCategory() == EnumSimpleTypeCategory::RestrictNone)
	{
		TypeInfo.SimpleTypeCategory(cat);
	}
	else if(TypeInfo.SimpleTypeCategory() != cat)
	{
		throw std::exception("unsupported mixed facet!");
	}
}

void CSimpleTypeParser::CheckAttribute()
{
	if(Context->PaserStack.size() <= 2)
	{
		if(!AttributeMask[EnumAttribute::name])
		{
			throw std::exception("global type declaration need a name");
		}
	}
}

void CSimpleTypeParser::Initialize()
{
	MAKE_ATTR_MAP(id, OnAttrID);
	MAKE_ATTR_MAP(name, OnAttrName);
}

CXmlSchemaObjectParserBase::AttributeMap CSimpleTypeParser::AttrMap;
CRAII_PreposeTaskRunner xsSimpleTypeAttrMapInitializer(NewRunnableFunction(&CSimpleTypeParser::Initialize));
