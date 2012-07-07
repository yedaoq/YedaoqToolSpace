/* ___________________________________________________________________________
@ 
@ file - XmlSchema.h
@ 
@ auth - yedaoq@gmail.com          http://blog.yedaoq.info
@ date - 2012-2-28
@ info -
@     
/* ___________________________________________________________________________*/

#pragma once

#include "CPP\tstring.h"
#include "vector"
#include "list"
#include "CPP\Enumerator.h"
#include "CPP\microtype.h"
#include "XmlSchemaEnum.h"
#include "FacetGroup.h"
#include "ElementGroup.h"

namespace nsYedaoqXmlSchema
{
	class XmlSchemaAttribute : public CCloneable<XmlSchemaAttribute, IXmlSchemaObject>
	{
	public:
		tstring							Name;
		XmlSchemaTypeSN_t				Type;
		tstring							Default;
		EnumAttributeUse				FlagUse;
		MetaAnyAttributeCollection		MetaAnyAttributes;

		virtual EnumXmlSchemaObject	ObjectCategory() const { return EnumXmlSchemaObject::Attribute; }
	};

	class XmlSchemaTypeHeader : public IXmlSchemaObject
	{
	public:
		XmlSchemaTypeSN_t				SerialNO;
		tstring							Name;
		tstring							Annotation;

		virtual EnumDataTypeCategory	TypeCategory() const = 0;

	protected:
		XmlSchemaTypeHeader( const tstring& name, XmlSchemaTypeSN_t sn = INVALIDXMLSCHEMATYPESN)
			: SerialNO(sn), Name(name)
		{}

		XmlSchemaTypeHeader(const tchar* name = TEXT(""), XmlSchemaTypeSN_t sn = INVALIDXMLSCHEMATYPESN)
			: SerialNO(sn), Name(name)
		{}

		virtual ~XmlSchemaTypeHeader() = 0 {}
	};

	class XmlSchemaVirtualType : public CCloneable<XmlSchemaVirtualType, XmlSchemaTypeHeader>
	{
	public:
		virtual EnumXmlSchemaObject		ObjectCategory() const { return EnumXmlSchemaObject::Virtual; }
		virtual EnumDataTypeCategory	TypeCategory() const { return EnumDataTypeCategory::Virtual; }
	};

	class XmlSchemaSimpleType : public CCloneable<XmlSchemaSimpleType, XmlSchemaTypeHeader>
	{
	public:			// visit function
		typedef cloned_ptr<ISimpleTypeDataGroup> ISimpleTypeDataGroupPtr;

		XmlSchemaTypeSN_t				BaseType() const;
		void							BaseType(XmlSchemaTypeSN_t sn) { BaseType_ = sn; }

		template<typename T> const T*	GetDataGroup() const { Assert(SimpleTypeCategory_Traits<T>()); return static_cast<T*>(DataGroup_.get()); }			// where T is a ISimpleTypeGroupData				
		template<typename T> T*			GetDataGroup() { Assert(SimpleTypeCategory_Traits<T>()); return static_cast<T*>(DataGroup_.get()); }			// where T is a ISimpleTypeGroupData				
		void							SetDataGroup(const ISimpleTypeDataGroup& data);

	public:
		XmlSchemaSimpleType();

		virtual EnumXmlSchemaObject		ObjectCategory() const { return EnumXmlSchemaObject::SimpleType; }
		virtual EnumDataTypeCategory	TypeCategory(void) const { return EnumDataTypeCategory::Simple; }
		EnumSimpleTypeCategory			SimpleTypeCategory() const { return DataGroup_->SimpleTypeCategory(); }
		void							SimpleTypeCategory(EnumSimpleTypeCategory cat);

	protected:
		bool							Assert(EnumSimpleTypeCategory t) const;

	protected:		// indirect member
		XmlSchemaTypeSN_t				BaseType_;
		ISimpleTypeDataGroupPtr			DataGroup_;

	public:		// direct member

		EnumWhiteSpaceRule				FacetWhiteSpace;
		EnumFinalFlag					FlagFinal;			// final
		EnumSimpleTypeFlag				Flag;
	};

	class XmlSchemaComplexType : public CCloneable<XmlSchemaComplexType, XmlSchemaTypeHeader>
	{
	public:
		typedef std::vector<XmlSchemaAttribute> AttributeCollection;
		enum EnumContentModel{Simple, Element, Mixed };

	public:
		XmlSchemaComplexType()
			: BaseType(INVALIDXMLSCHEMATYPESN), InheritPolicy(EnumComplexTypeInheritPolicy::extension), ContentModel(Element), Flag(0)
		{}

		virtual EnumDataTypeCategory	TypeCategory(void) const { return EnumDataTypeCategory::Complex; }
		virtual EnumXmlSchemaObject		ObjectCategory() const { return EnumXmlSchemaObject::SimpleType; }

	public:
		AttributeCollection						Attributes;
		XmlSchemaSubitemGroup					Subitems;

		XmlSchemaTypeSN_t						BaseType;
		EnumComplexTypeInheritPolicy			InheritPolicy;	
		EnumContentModel						ContentModel;
		EnumFinalFlag							FinalFlag;
		EnumBlockFlag							BlockFlag;
		EnumComplexTypeFlag						Flag;
	};
	
};
