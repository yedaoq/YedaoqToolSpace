#pragma once
#include "XmlSchemaObjectParser.h"
#include "XmlSchemaAnnotation.h"

namespace nsYedaoqXmlSchema{namespace nsSerialize
{
	class CAnnotationParser : public IXmlSchemaObjectParser
	{

	public:
		CAnnotationParser(CSerializeContext* ctx)
			: Context(ctx)
		{}

		virtual bool Parse(xnode_t* node);
		virtual void OnInnerObject(IXmlSchemaObject* obj) {}
		virtual IXmlSchemaObject* Object() { return &AnnotationInfo; }

	public:
		XmlSchemaAnnotation AnnotationInfo;
		CSerializeContext*	Context;
	};
}}

