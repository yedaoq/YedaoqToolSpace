#include "stdafx.h"
#include "XmlSchemaCommon.h"

using namespace nsYedaoqXmlSchema;

tstring const* nsYedaoqXmlSchema::MetaAnyAttributeCollection::find( const tstring& name ) const
{
	const_iterator iter = std::map<tstring, tstring>::find(name);
	if(iter != end())
	{
		return &(iter->second);
	}
	return 0;
}

tstring* nsYedaoqXmlSchema::MetaAnyAttributeCollection::find( const tstring& name )
{
	return const_cast<tstring*>(const_cast<MetaAnyAttributeCollection const*>(this)->find(name));
}

bool nsYedaoqXmlSchema::MetaAnyAttributeCollection::insert( const tstring& name, const tstring& val )
{
	if(find(name))
	{
		return false;
	}
	std::map<tstring, tstring>::insert(std::make_pair(name, val));
	return true;
}

const CXsString CXsString::anyAttribute(TEXT("xs:anyAttribute"));
const CXsString CXsString::any(TEXT("xs:any"));
const CXsString CXsString::annotation(TEXT("xs:annotation"));
const CXsString CXsString::all(TEXT("xs:all"));
const CXsString CXsString::appInfo(TEXT("xs:appInfo"));
const CXsString CXsString::attribute(TEXT("xs:attribute"));
const CXsString CXsString::attributeGroup(TEXT("xs:attributeGroup"));
const CXsString CXsString::choice(TEXT("xs:choice"));
const CXsString CXsString::complexContent(TEXT("xs:complexContent"));
const CXsString CXsString::complexType(TEXT("xs:complexType"));
const CXsString CXsString::documentation(TEXT("xs:documentation"));
const CXsString CXsString::element(TEXT("xs:element"));
const CXsString CXsString::extension(TEXT("xs:extension"));
const CXsString CXsString::field(TEXT("xs:field"));
const CXsString CXsString::group(TEXT("xs:group"));
const CXsString CXsString::import(TEXT("xs:import"));
const CXsString CXsString::include(TEXT("xs:include"));
const CXsString CXsString::key(TEXT("xs:key"));
const CXsString CXsString::keyref(TEXT("xs:keyref"));
const CXsString CXsString::list(TEXT("xs:list"));
const CXsString CXsString::notation(TEXT("xs:notation"));
const CXsString CXsString::redefine(TEXT("xs:redefine"));
const CXsString CXsString::restriction(TEXT("xs:restriction"));
const CXsString CXsString::schema(TEXT("xs:schema"));
const CXsString CXsString::selector(TEXT("xs:selector"));
const CXsString CXsString::sequence(TEXT("xs:sequence"));
const CXsString CXsString::simpleContent(TEXT("xs:simpleContent"));
const CXsString CXsString::simpleType(TEXT("xs:simpleType"));
const CXsString CXsString::union_(TEXT("xs:union"));
const CXsString CXsString::unique(TEXT("xs:unique"));

const CXsString CXsString::enumeration(TEXT("xs:enumeration"));
const CXsString CXsString::fractionDigits(TEXT("xs:fractionDigits"));
const CXsString CXsString::length(TEXT("xs:length"));
const CXsString CXsString::maxExclusive(TEXT("xs:maxExclusive"));
const CXsString CXsString::maxInclusive(TEXT("xs:maxInclusive"));
const CXsString CXsString::maxLength(TEXT("xs:maxLength"));
const CXsString CXsString::minExclusive(TEXT("xs:minExclusive"));
const CXsString CXsString::minInclusive(TEXT("xs:minInclusive"));
const CXsString CXsString::minLength(TEXT("xs:minLength"));
const CXsString CXsString::pattern(TEXT("xs:pattern"));
const CXsString CXsString::totalDigits(TEXT("xs:totalDigits"));
const CXsString CXsString::whiteSpace(TEXT("xs:whiteSpace"));

const CXsString CXsString::AnyType(TEXT("xs:anyType"));
const CXsString CXsString::AnySimpleType(TEXT("xs:anySimpleType"));
const CXsString CXsString::AnyAtomicType(TEXT("xs:anyAtomicType"));
const CXsString CXsString::AnyURI(TEXT("xs:anyURI"));
const CXsString CXsString::Base64Binary(TEXT("xs:base64Binary"));
const CXsString CXsString::Boolean(TEXT("xs:boolean"));
const CXsString CXsString::Date(TEXT("xs:date"));
const CXsString CXsString::DateTime(TEXT("xs:dateTime"));
const CXsString CXsString::DateTimeStamp(TEXT("xs:dateTimeStamp"));
const CXsString CXsString::Decimal(TEXT("xs:decimal"));
const CXsString CXsString::Double(TEXT("xs:double"));
const CXsString CXsString::Duration(TEXT("xs:duration"));
const CXsString CXsString::DayTimeDuration(TEXT("xs:dayTimeDuration"));
const CXsString CXsString::YearMonthDuration(TEXT("xs:yearMonthDuration"));
const CXsString CXsString::Float(TEXT("xs:float"));
const CXsString CXsString::GDay(TEXT("xs:gDay"));
const CXsString CXsString::GMonth(TEXT("xs:gMonth"));
const CXsString CXsString::GMonthDay(TEXT("xs:gMonthDay"));
const CXsString CXsString::GYear(TEXT("xs:gYear"));
const CXsString CXsString::GYearMonth(TEXT("xs:gYearMonth"));
const CXsString CXsString::HexBinary(TEXT("xs:hexBinary"));
const CXsString CXsString::NOTATION(TEXT("xs:NOTATION"));
const CXsString CXsString::QName(TEXT("xs:QName"));
const CXsString CXsString::String(TEXT("xs:string"));
const CXsString CXsString::Time(TEXT("xs:time"));
const CXsString CXsString::ENTITIES(TEXT("xs:ENTITIES"));
const CXsString CXsString::IDREFS(TEXT("xs:IDREFS"));
const CXsString CXsString::NMTOKENS(TEXT("xs:NMTOKENS"));
const CXsString CXsString::Integer(TEXT("xs:integer"));
const CXsString CXsString::Long(TEXT("xs:long"));
const CXsString CXsString::Int(TEXT("xs:int"));
const CXsString CXsString::Short(TEXT("xs:short"));
const CXsString CXsString::Byte(TEXT("xs:byte"));
const CXsString CXsString::NonNegativeInteger(TEXT("xs:nonNegativeInteger"));
const CXsString CXsString::PositiveInteger(TEXT("xs:positiveInteger"));
const CXsString CXsString::UnsignedLong(TEXT("xs:unsignedLong"));
const CXsString CXsString::UnsignedInt(TEXT("xs:unsignedInt"));
const CXsString CXsString::UnsignedShort(TEXT("xs:unsignedShort"));
const CXsString CXsString::UnsignedByte(TEXT("xs:unsignedByte"));
const CXsString CXsString::NonPostiveInteger(TEXT("xs:nonPostiveInteger"));
const CXsString CXsString::NegativeInteger(TEXT("xs:negativeInteger"));
const CXsString CXsString::NormalizedString(TEXT("xs:normalizedString"));
const CXsString CXsString::Token(TEXT("xs:token"));
const CXsString CXsString::Language(TEXT("xs:language"));
const CXsString CXsString::Name(TEXT("xs:Name"));
const CXsString CXsString::NCName(TEXT("xs:NCName"));
const CXsString CXsString::ENTITY(TEXT("xs:ENTITY"));
const CXsString CXsString::ID(TEXT("xs:ID"));
const CXsString CXsString::IDREF(TEXT("xs:IDREF"));
const CXsString CXsString::NMTOKEN(TEXT("xs:NMTOKEN"));

