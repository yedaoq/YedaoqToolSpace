/* ___________________________________________________________________________
@ 
@ file - XmlSchemaAnnotation.h
@ 
@ auth - yedaoq@gmail.com          http://blog.yedaoq.info
@ date - 2012-4-5
@ info -
@     
/* ___________________________________________________________________________*/
#pragma once
#include "XmlSchemaCommon.h"
#include "XmlSchemaEnum.h"

namespace nsYedaoqXmlSchema
{
	struct XmlSchemaAnnotation : CCloneable<XmlSchemaAnnotation, IXmlSchemaObject>
	{
		virtual EnumXmlSchemaObject		ObjectCategory() const { return EnumXmlSchemaObject::Annotation; }
	};
}
