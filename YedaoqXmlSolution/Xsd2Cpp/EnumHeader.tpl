/* ___________________________________________________________________________
@ 
@ file - {{project.name}}Enum.h
@ 
@ auth - yedaoq@gmail.com          http://blog.yedaoq.info
@ date - 
@ info - auto generated by yedaoq xmlschema tool
@     
/* ___________________________________________________________________________*/
#ifndef _{{project.name.upper}}_ENUM_
#define _{{project.name.upper}}_ENUM_

#include "CPP\EnumDeclaration3x\EnumHeaderMicro.h"

{{#nsblock}}namespace {{project.namespace}} { {{/nsblock}}

{{#xsdenum}}

BEGINENUM({{name}},{{base}},{{flag}})

{{#xsdenumitem}}	ENUMITEM2({{name}}, {{val}}) {{/xsdenumitem}}

ENDENUM

{{/xsdenum}}

{{#nsblock}} } {{/nsblock}}

#endif //_{{project.name.upper}}_ENUM_