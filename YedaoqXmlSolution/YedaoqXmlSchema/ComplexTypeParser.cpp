#include "stdafx.h"
#include "ComplexTypeParser.h"
#include "XmlSchema.h"
#include "CPP\task.h"

using namespace nsYedaoqXmlSchema;
using namespace nsYedaoqXmlSchema::nsSerialize;

bool CComplexTypeParser::Parse( xnode_t* node )
{
	DispatchAttribute(node);
	CheckAttribute();
	
	xnode_t* child = 0;
	if( 0 != (child = node->first_node(TEXT("xs:complexContent"))) || 0 != (child = node->first_node(TEXT("complexContent"))))
	{
		ParseComplexContent(child);
	}
	else if( 0 != (child = node->first_node(TEXT("xs:simpleContent"))) || 0 != (child = node->first_node(TEXT("simpleContent"))))
	{
		ParseSimpleContent(child);
	}
	else
	{
		Context->Dispatcher->DispatchChilds(node);
	}

	TypeInfo.SerialNO = Context->Schema->AddType(TypeInfo);
	return true;
}

void CComplexTypeParser::OnInnerObject( IXmlSchemaObject* obj )
{
	if(obj->ObjectCategory() == EnumXmlSchemaObject::Group)
	{
		TypeInfo.Subitems = static_cast<XmlSchemaSubitemGroup&>(*obj);
	}
	else if(obj->ObjectCategory() == EnumXmlSchemaObject::Attribute)
	{
		TypeInfo.Attributes.push_back(static_cast<XmlSchemaAttribute&>(*obj));
	}
	else if(obj->ObjectCategory() == EnumXmlSchemaObject::Annotation)
	{
		
	}
	else
	{
		throw std::exception("unsupported complextype defination");
	}
}

void CComplexTypeParser::CheckAttribute()
{
	if(Context->PaserStack.size() <= 2)
	{
		if(!AttributeMask[EnumAttribute::name])
		{
			throw std::exception("global type declaration need a name");
		}
	}
}

void CComplexTypeParser::ParseRestriction( xnode_t* node) 
{
	throw std::exception("do not support complexType restriction.");
}

void CComplexTypeParser::ParseExtension( xnode_t* node) 
{
	xattr_t* attr = node->first_attribute(TEXT("base"));
	
	if(!attr)
	{
		throw std::exception("xs:extension need a base attribute");
	}

	// Get the named type SN, when nonexist, create a virtual type
	TypeInfo.BaseType = Context->Schema->GetType(attr->value(), true)->SerialNO;

	Context->Dispatcher->DispatchChilds(node);
}

void CComplexTypeParser::ParseContentDefination( xnode_t* node ) 
{
	xnode_t* child = 0;
	if( 0 != (child = node->first_node(TEXT("xs:restriction"))) || 0 != (child = node->first_node(TEXT("restriction"))))
	{
		ParseRestriction(child);
	}
	else if( 0 != (child = node->first_node(TEXT("xs:extension"))) || 0 != (child = node->first_node(TEXT("extension"))))
	{
		ParseExtension(child);
	}
	else
	{
		throw std::exception("xs:simpleContent or xs:complexContent need a xs:extension or xs:restriction defination");
	}
}

void CComplexTypeParser::ParseComplexContent( xnode_t* node ) 
{
	xattr_t* attr = node->first_attribute(TEXT("mixed"));	// mixed property of xs:complexContent overlap mixed property of xs:complexType
	if(attr)
	{
		OnAttrMixed(TEXT("mixed"), attr->value());
	}

	ParseContentDefination(node);
}

void CComplexTypeParser::ParseSimpleContent( xnode_t* node ) 
{
	if(TypeInfo.ContentModel == XmlSchemaComplexType::EnumContentModel::Mixed)
	{
		throw std::exception("xs:complexType with mixed content can't define with xs:simpleContent");
	}

	TypeInfo.ContentModel = XmlSchemaComplexType::EnumContentModel::Simple;

	ParseContentDefination(node);
}

void CComplexTypeParser::OnAttrID( const tstring& name, tchar const* val )
{

}

void CComplexTypeParser::OnAttrMixed( const tstring& name, tchar const* val )
{
	if(val && 0 == _tcscmp(val, TEXT("true")))
	{
		TypeInfo.ContentModel = XmlSchemaComplexType::EnumContentModel::Mixed;
	}
	else
	{
		TypeInfo.ContentModel = XmlSchemaComplexType::EnumContentModel::Element;
	}
}

void CComplexTypeParser::OnAttrName( const tstring& name, tchar const* val )
{
	TypeInfo.Name = val;
}

void CComplexTypeParser::OnAttrAbstract( const tstring& name, tchar const* val )
{

}

void CComplexTypeParser::OnAttrBlock( const tstring& name, tchar const* val )
{

}

void CComplexTypeParser::OnAttrFinal( const tstring& name, tchar const* val )
{

}

void CComplexTypeParser::Initialize()
{
	MAKE_ATTR_MAP(id, CComplexTypeParser::OnAttrID);	
	MAKE_ATTR_MAP(name, CComplexTypeParser::OnAttrName);
	MAKE_ATTR_MAP(mixed, CComplexTypeParser::OnAttrMixed);
	MAKE_ATTR_MAP(abstract, CComplexTypeParser::OnAttrAbstract);
	MAKE_ATTR_MAP(block, CComplexTypeParser::OnAttrBlock);
	MAKE_ATTR_MAP(final, CComplexTypeParser::OnAttrFinal);
}

CXmlSchemaObjectParserBase::AttributeMap CComplexTypeParser::AttrMap;
CRAII_PreposeTaskRunner xsComplexTypeParserAttrMapInitializer(NewRunnableFunction(&CComplexTypeParser::Initialize));

