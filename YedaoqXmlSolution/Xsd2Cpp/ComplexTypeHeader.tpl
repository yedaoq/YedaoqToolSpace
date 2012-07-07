/* ___________________________________________________________________________
@ 
@ file - {{project.name}}.h
@ 
@ auth - yedaoq@gmail.com          http://blog.yedaoq.info
@ date - 
@ info - auto generated by yedaoq xmlschema tool
@     
/* ___________________________________________________________________________*/
#ifndef _{{project.name.upper}}_XSD_
#define _{{project.name.upper}}_XSD_

#include "CPP\XmlMapping\HeaderMicro.h"

{{#nsblock}}namespace {{project.namespace}} { {{/nsblock}}

{{#xsdcomplex}}

XML_COMPLEXTYPE_BEGIN({{cppname}})

{{#xsdele}}	XML_ELE({{xmlname}}, {{cppname}}, {{cpptype}}) {{/xsdele}}
{{#xsdeles}}	XML_ELES({{xmlname}}, {{cppname}}, {{cpptype}}) {{/xsdeles}}
{{#xsdattr}}	XML_ATTR({{xmlname}}, {{cppname}}, {{cpptype}}) {{/xsdattr}}
{{#xsdenumattr}}	XML_ENUMATTR({{xmlname}}, {{cppname}}, {{cpptype}}) {{/xsdenumattr}}

XML_COMPLEXTYPE_END

{{/xsdcomplex}}

{{#nsblock}} } {{/nsblock}}

#endif //_{{project.name.upper}}_XSD_