#include "stdafx.h"
#include "xsNamespace.h"
#include "boost\assert.hpp"

const xsNamespace::Element xsNamespace::anyAttribute(TEXT("xs:anyAttribute"));
const xsNamespace::Element xsNamespace::any(TEXT("xs:any"));
const xsNamespace::Element xsNamespace::annotation(TEXT("xs:annotation"));
const xsNamespace::Element xsNamespace::all(TEXT("xs:all"));
const xsNamespace::Element xsNamespace::appInfo(TEXT("xs:appInfo"));
const xsNamespace::Element xsNamespace::attribute(TEXT("xs:attribute"));
const xsNamespace::Element xsNamespace::attributeGroup(TEXT("xs:attributeGroup"));
const xsNamespace::Element xsNamespace::choice(TEXT("xs:choice"));
const xsNamespace::Element xsNamespace::complexContent(TEXT("xs:complexContent"));
const xsNamespace::Element xsNamespace::complexType(TEXT("xs:complexType"));
const xsNamespace::Element xsNamespace::documentation(TEXT("xs:documentation"));
const xsNamespace::Element xsNamespace::element(TEXT("xs:element"));
const xsNamespace::Element xsNamespace::extension(TEXT("xs:extension"));
const xsNamespace::Element xsNamespace::field(TEXT("xs:field"));
const xsNamespace::Element xsNamespace::group(TEXT("xs:group"));
const xsNamespace::Element xsNamespace::import(TEXT("xs:import"));
const xsNamespace::Element xsNamespace::include(TEXT("xs:include"));
const xsNamespace::Element xsNamespace::key(TEXT("xs:key"));
const xsNamespace::Element xsNamespace::keyref(TEXT("xs:keyref"));
const xsNamespace::Element xsNamespace::list(TEXT("xs:list"));
const xsNamespace::Element xsNamespace::notation(TEXT("xs:notation"));
const xsNamespace::Element xsNamespace::redefine(TEXT("xs:redefine"));
const xsNamespace::Element xsNamespace::restriction(TEXT("xs:restriction"));
const xsNamespace::Element xsNamespace::schema(TEXT("xs:schema"));
const xsNamespace::Element xsNamespace::selector(TEXT("xs:selector"));
const xsNamespace::Element xsNamespace::sequence(TEXT("xs:sequence"));
const xsNamespace::Element xsNamespace::simpleContent(TEXT("xs:simpleContent"));
const xsNamespace::Element xsNamespace::simpleType(TEXT("xs:simpleType"));
const xsNamespace::Element xsNamespace::union_(TEXT("xs:union"));
const xsNamespace::Element xsNamespace::unique(TEXT("xs:unique"));

xsNamespace::Element::Element( tchar const* v )
{
	BOOST_ASSERT(v && v[0] == 'x' && v[1] == 's' && v[2] == ':');
	str_ = new tchar(_tcslen(v) + 1);
	_tcscpy(str_, v);
}

// xsNamespace::Element::Element( tchar (&v)[] )
// {
// 	str_ = new tchar(count_of(v) + 1);
// 	_tcscpy(str_, v);
// }

xsNamespace::Element::~Element()
{
	delete(str_);
	str_ = 0;
}
