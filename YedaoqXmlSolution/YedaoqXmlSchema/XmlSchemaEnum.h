/* ___________________________________________________________________________
@ 
@ file - XmlSchemaEnum.h
@ 
@ auth - yedaoq@gmail.com          http://blog.yedaoq.info
@ date - 2012-3-5
@ info -
@     
/* ___________________________________________________________________________*/

#ifndef _XMLSCHEMA_ENUM_
#define _XMLSCHEMA_ENUM_

#include "CPP\EnumDeclaration3x\EnumHeaderMicro.h"

namespace nsYedaoqXmlSchema
{

//////////////////////////////////////////////////////////////////////////
// EnumFacetsetCategory ——
//    在所有的facet中，可分为三部分：  见http://www.w3.org/TR/xmlschema11-2/#defn-rep-constr
//    一类是所有原生类型均可应用的enumeration,pattern,whiteSpace,assertions,
//    一类是长度可量度的类型可应用的length,minLength,maxLength,
//    一类是值可比较的类型可应用的maxInclusive,maxInclusive,maxExclusive,minExclusive,
//    List类型可应用长度量度相关的Facet；
//    Union只能应用enumeration,pattern,assertions；
BEGINENUM(FacetsetCategory, unsigned char, false)
	ENUMITEM0(Comparable)		// 值可比较，可应用Facet : maxInclusive,maxInclusive,maxExclusive,minExclusive
	ENUMITEM0(Measurable)		// 长度可度量，可应用Facet : length,minLength,maxLength
	ENUMITEM0(List)				// List SimpleType可应用的Facet : assertions, length, minLength, maxLength, pattern, enumeration, and whiteSpace.
	ENUMITEM0(Union)			// Union SimpleType可应用的Facet : 无
ENDENUM

BEGINENUM(DataTypeCategory, unsigned char, false)
	ENUMITEM0(Virtual)
	ENUMITEM0(Simple)
	ENUMITEM0(Complex)
ENDENUM

BEGINENUM(SimpleTypeCategory, unsigned char, false)
	ENUMITEM0(RestrictNone)
	ENUMITEM0(RestrictEnum)
	ENUMITEM0(RestrictRange)
	ENUMITEM0(RestrictPattern)
	ENUMITEM0(RestrictLength)	
	ENUMITEM0(Union)
	ENUMITEM0(List)
ENDENUM

BEGINENUM(WhiteSpaceRule, unsigned char, false)
	ENUMITEM0(Unappinted)
	ENUMITEM0(Preserve)
	ENUMITEM0(Replace)
	ENUMITEM0(Collapse)
ENDENUM

BEGINENUM(SimpleTypeVariety, unsigned char, false)
	ENUMITEM0(Absent)
	ENUMITEM0(Atomic)
	ENUMITEM0(List)
	ENUMITEM0(Union)
ENDENUM

BEGINENUM(SimpleTypeFlag, unsigned char, true)
	ENUMITEM2(None, 0)
	ENUMITEM2(Buildin, 1)
	ENUMITEM2(Special, 2)
	ENUMITEM2(Primitive, 4)
	ENUMITEM2(Atomic, 16)
	ENUMITEM2(List, 32)
	ENUMITEM2(Union, 64)
ENDENUM

BEGINENUM(FinalFlag, unsigned char, true)
	ENUMITEM2(None, 0)
	ENUMITEM2(Restriction, 1)
	ENUMITEM2(Extension, 2)
	ENUMITEM2(List, 4)
	ENUMITEM2(Union, 8)
	ENUMITEM2(All, 15)
ENDENUM

BEGINENUM(BlockFlag, unsigned char, true)	// used for xs:element and xs:complexType
ENUMITEM2(None, 0)
ENUMITEM2(Restriction, 1)
ENUMITEM2(Extension, 2)
ENUMITEM2(Substitution, 4)		// only for xs:element
ENUMITEM2(All, 7)
ENDENUM

BEGINENUM(BuildInType, int, false)
	ENUMITEM2(AnyType, 1)
	ENUMITEM0(AnySimpleType)
	ENUMITEM0(ENTITIES)
	ENUMITEM0(IDREFS)
	ENUMITEM0(NMTOKENS)
	ENUMITEM0(AnyAtomicType)
	ENUMITEM0(AnyURL)
	ENUMITEM0(Base64Binary)
	ENUMITEM0(Boolean)
	ENUMITEM0(Date)
	ENUMITEM0(DateTime)
	ENUMITEM0(DateTimeStamp)
	ENUMITEM0(Decimal)
	ENUMITEM0(Double)
	ENUMITEM0(Duration)
	ENUMITEM0(DayTimeDuration)
	ENUMITEM0(YearMonthDuration)
	ENUMITEM0(Float)
	ENUMITEM0(GDay)
	ENUMITEM0(GMonth)
	ENUMITEM0(GMonthDay)
	ENUMITEM0(GYear)
	ENUMITEM0(GYearMonth)
	ENUMITEM0(HexBirary)
	ENUMITEM0(NOTATION)
	ENUMITEM0(QName)
	ENUMITEM0(String)
	ENUMITEM0(NormalizedString)
	ENUMITEM0(Token)
	ENUMITEM0(Language)
	ENUMITEM0(Name)
	ENUMITEM0(NCName)
	ENUMITEM0(ENTITY)
	ENUMITEM0(ID)
	ENUMITEM0(IDREF)
	ENUMITEM0(NMTOKEN)
	ENUMITEM0(Time)
ENDENUM

BEGINENUM(AttributeUse, unsigned char, false)
	ENUMITEM0(optional)
	ENUMITEM0(required)
	ENUMITEM0(prohibited)
ENDENUM

BEGINENUM(SubitemCategory, unsigned char, false)
	ENUMITEM0(element)
	ENUMITEM0(sequence)
	ENUMITEM0(choice)
	ENUMITEM0(all)
	ENUMITEM0(any)
ENDENUM

BEGINENUM(ComplexTypeInheritPolicy, unsigned char, false)
	ENUMITEM0(extension)
	ENUMITEM0(restrict)
ENDENUM

BEGINENUM(ComplexTypeFlag, unsigned char, true)
	ENUMITEM2(Abstract, 1)
	ENUMITEM2(AnyAttribute, 2)
	ENUMITEM2(AnyElement, 4)
ENDENUM

BEGINENUM(XmlSchemaObject, unsigned char, false)
	ENUMITEM0(Schema)		// xs:schema
	ENUMITEM0(Virtual)		// inexistence object
	ENUMITEM0(Element)		// xs:element
	ENUMITEM0(SimpleType)	// xs:simpleType
	ENUMITEM0(ComplexType)	// xs:complexType
	ENUMITEM0(Attribute)	// xs:attribute
	ENUMITEM0(Group)		// xs:sequence, xs:choice, xs:all
	ENUMITEM0(Any)			// xs:any
	ENUMITEM0(NamedGroup)	// xs:group
	ENUMITEM0(Annotation)	// xs:annotation
ENDENUM 

BEGINENUM(XmlSchemaAnyProcessPolicy, unsigned char, false)
	ENUMITEM0(strict)
	ENUMITEM0(lax)
	ENUMITEM0(skip)
ENDENUM

};

namespace nsYedaoqXmlSchema { namespace nsSerialize 
{ 
BEGINENUM(ParseError, unsigned short, false)
	ENUMITEM1(NameMissedofRootElement, TEXT("root element definition need a 'name' attribute, see <%s>"))
ENDENUM

BEGINENUM(Component, unsigned short, false)
	ENUMITEM1(All, xs:all)
	ENUMITEM1(Annotation, xs:annotation)
	ENUMITEM1(Any, xs:any)
	ENUMITEM1(AnyAttribute, xs:anyTttribute)
	ENUMITEM1(AppInfo, xs:appInfo)
	ENUMITEM1(Attribute, xs:attribute)
	ENUMITEM1(AttributeGroup, xs:attributeGroup)
	ENUMITEM1(Choice, xs:choice)
	ENUMITEM1(ComplexContent, xs:complexContent)
	ENUMITEM1(ComplexType, xs:complexType)
	ENUMITEM1(Documentation, xs:documentation)
	ENUMITEM1(Element, xs:element)
	ENUMITEM1(Extension, xs:extension)
	ENUMITEM1(Field, xs:field)
	ENUMITEM1(Group, xs:group)
	ENUMITEM1(Import, xs:import)
	ENUMITEM1(Include, xs:include)
	ENUMITEM1(Key, xs:key)
	ENUMITEM1(KeyRef, xs:keyref)
	ENUMITEM1(List, xs:list)
	ENUMITEM1(Notation, xs:notation)
	ENUMITEM1(Redefine, xs:redefine)
	ENUMITEM1(Restriction, xs:restriction)
	ENUMITEM1(Schema, xs:schema)
	ENUMITEM1(Selector, xs:selector)
	ENUMITEM1(Sequence, xs:sequence)
	ENUMITEM1(SimpleContent, xs:simpleContent)
	ENUMITEM1(SimpleType, xs:simpleType)
	ENUMITEM1(Union, xs:union)
	ENUMITEM1(Unique, xs:unique)
ENDENUM
}}

#endif