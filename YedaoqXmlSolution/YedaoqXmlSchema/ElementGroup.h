/* ___________________________________________________________________________
@ 
@ file - ElementGroup.h
@ 
@ auth - yedaoq@gmail.com          http://blog.yedaoq.info
@ date - 2012-3-15
@ info -
@     
/* ___________________________________________________________________________*/
#pragma once

#include <map>
#include <vector>
#include "CPP\Enumerator.h"
#include "CPP\functor_cast.h"
#include "XmlSchemaEnum.h"

#include "XmlSchemaCommon.h"

namespace nsYedaoqXmlSchema
{
	/*
	  this interface used as a abstract node in a element hierarchy tree, it can be a xs:element, or xs:sequence, or xs:choice, or xs:all
	*/
	interface XmlSchemaSubitem : public IXmlSchemaObject
	{
		virtual EnumSubitemCategory			Category() const = 0;
		virtual	~XmlSchemaSubitem() = 0 {};

		XmlSchemaSubitem()
			: MinOccurs(DEFAULT_MAXOCCURS_VAL), MaxOccurs(DEFAULT_MINOCCURS_VAL)
		{}

		int									MinOccurs;
		int									MaxOccurs;
	};

	/*
	  this struct used to keep the xs:element definition info
	*/
	class XmlSchemaElement : public CCloneable<XmlSchemaElement, XmlSchemaSubitem>
	{
	public:
		tstring								Name;
		XmlSchemaTypeSN_t					Type;
		tstring								Default;
		MetaAnyAttributeCollection			MetaAnyAttributes;

		XmlSchemaElement()
			: Type(INVALIDXMLSCHEMATYPESN)
		{}

		virtual EnumXmlSchemaObject			ObjectCategory() const { return EnumXmlSchemaObject::Element; }
		virtual EnumSubitemCategory			Category() const { return EnumSubitemCategory::element; }
	};

	class XmlSchemaAny : public CCloneable<XmlSchemaAny, XmlSchemaSubitem>
	{
	public:
		tstring								NameSpace;
		EnumXmlSchemaAnyProcessPolicy		ProcessPolicy;

		XmlSchemaAny()
			: ProcessPolicy(EnumXmlSchemaAnyProcessPolicy::strict)
		{}

		virtual EnumXmlSchemaObject			ObjectCategory() const { return EnumXmlSchemaObject::Element; }
		virtual EnumSubitemCategory			Category() const { return EnumSubitemCategory::any; }
	};

	class XmlSchemaSubitemGroup : public CCloneable<XmlSchemaSubitemGroup, XmlSchemaSubitem>
	{
	public:
		typedef cloned_container<XmlSchemaSubitem, EnumStdContainer::Vector>	SubitemCollection;

	public:
		XmlSchemaSubitemGroup(EnumSubitemCategory cat = EnumSubitemCategory::sequence)
		{
			Category(cat);
		}

		virtual EnumXmlSchemaObject			ObjectCategory() const { return EnumXmlSchemaObject::Group; }
		IEnumerator<XmlSchemaSubitem*>*		Enum() const { return Subitems.Enum(); }

		EnumSubitemCategory					Category() const { return Category_; }
		void Category(EnumSubitemCategory cat) { Assert(cat); Category_ = cat; }

		void Assert(EnumSubitemCategory cat) const { if(cat == EnumSubitemCategory::element || cat == EnumSubitemCategory::any) throw std::exception(); }

	public:
		SubitemCollection					Subitems;

	protected:
		EnumSubitemCategory					Category_;
	};
}