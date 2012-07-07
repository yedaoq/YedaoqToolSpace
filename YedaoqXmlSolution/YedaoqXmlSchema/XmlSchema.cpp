#include "stdafx.h"
#include "XmlSchema.h"

#define PREFILLBUILDINTYPE_ITEM(name, baseType, flag)		\
	info.BaseType(GetType(baseType.Str)->SerialNO);			\
	info.Name = name.Str;									\
	info.Flag = flag | EnumSimpleTypeFlag::Buildin;			\
	AddType(info);

#define BUILDINTYPECOUNT 50

using namespace nsYedaoqXmlSchema;

struct DataTypeCategoryFilter
{
	DataTypeCategoryFilter(EnumDataTypeCategory cat)
		: Category(cat)
	{}

	bool operator()(const cloned_ptr<XmlSchemaTypeHeader>& ptr) const
	{
		return ptr->TypeCategory() == Category;
	}

protected:
	EnumDataTypeCategory Category;
};

template<typename T>
struct XmlSchemaTypeHeaderPtr2Type
{
	T& operator()(const cloned_ptr<XmlSchemaTypeHeader>& ptr) const
	{
		return static_cast<T&>(*ptr);
	}
};

nsYedaoqXmlSchema::CXmlSchema::CXmlSchema()
{
	PreFillBuildinTypes();
}

IEnumerator<XmlSchemaSimpleType>* nsYedaoqXmlSchema::CXmlSchema::EnumSimpeType() const
{
	return new_convert_enumerator<XmlSchemaSimpleType>(
		make_filter_enumerator(
			make_iterator_enumerator(Types_.begin(), Types_.end()),
			DataTypeCategoryFilter(EnumDataTypeCategory::Simple)),
		XmlSchemaTypeHeaderPtr2Type<XmlSchemaSimpleType>()
		);
}

IEnumerator<XmlSchemaTypeHeader>* nsYedaoqXmlSchema::CXmlSchema::EnumUserType() const
{
	return new_convert_enumerator<XmlSchemaTypeHeader>(
		make_iterator_enumerator(Types_.begin() + BUILDINTYPECOUNT, Types_.end()),
		XmlSchemaTypeHeaderPtr2Type<XmlSchemaTypeHeader>()
		);
}

IEnumerator<XmlSchemaTypeHeader>* nsYedaoqXmlSchema::CXmlSchema::EnumType() const
{
	return new_convert_enumerator<XmlSchemaTypeHeader>(
		make_iterator_enumerator(Types_.begin(), Types_.end()),
		XmlSchemaTypeHeaderPtr2Type<XmlSchemaTypeHeader>()
	);	
}


IEnumerator<XmlSchemaElement>* nsYedaoqXmlSchema::CXmlSchema::EnumGlobalElement() const
{
	return new_iterator_enumerator(Elements_.begin(), Elements_.end());
}


nsYedaoqXmlSchema::XmlSchemaTypeSN_t nsYedaoqXmlSchema::CXmlSchema::AddVirtualType( const tstring& name )
{
	BOOST_ASSERT(name.size() > 0);
	XmlSchemaVirtualType typeInfo;
	typeInfo.Name = name;
	return AddType(typeInfo);
}

nsYedaoqXmlSchema::XmlSchemaTypeSN_t nsYedaoqXmlSchema::CXmlSchema::AddType( const XmlSchemaTypeHeader& header )
{
	if(header.Name.size() > 0)
	{
		XmlSchemaTypeHeader* typeInfo = GetType(header.Name);
		if(typeInfo)
		{
			if(typeInfo->TypeCategory() == EnumDataTypeCategory::Virtual)
			{
				XmlSchemaTypeSN_t sn = typeInfo->SerialNO;
				Types_[sn] = header;
				Types_[sn]->SerialNO = sn;
			}
			else
			{
				throw std::exception("type name conflict!");
			}
		}
	}

	int count = Types_.size();
	Types_.push_back(header);		
	Types_[count]->SerialNO = count;
	return count;
}

nsYedaoqXmlSchema::XmlSchemaTypeSN_t nsYedaoqXmlSchema::CXmlSchema::AddType(XmlSchemaTypeHeader* header )
{
	if(header->Name.size() > 0)
	{
		XmlSchemaTypeHeader* typeInfo = GetType(header->Name);
		if(typeInfo)
		{
			if(typeInfo->TypeCategory() == EnumDataTypeCategory::Virtual)
			{
				header->SerialNO = typeInfo->SerialNO;
				Types_[typeInfo->SerialNO] = header;
			}
			else
			{
				throw std::exception("type name conflict!");
			}
		}
	}

	int count = Types_.size();
	Types_.push_back(header);		
	Types_[count]->SerialNO = count;
	return count;
}

void nsYedaoqXmlSchema::CXmlSchema::SetType(XmlSchemaTypeSN_t sn, const XmlSchemaTypeHeader& header )
{
	BOOST_ASSERT(sn >= 0 && sn < Types_.size());
	if(!NameConflict(header.Name, sn))
	{
		Types_[sn] = header;
		Types_[sn]->SerialNO = sn;
	}	
}

void nsYedaoqXmlSchema::CXmlSchema::SetType(XmlSchemaTypeSN_t sn, XmlSchemaTypeHeader* header )
{
	BOOST_ASSERT(sn >= 0 && sn < Types_.size());
	header->SerialNO = sn;
	if(!NameConflict(header->Name, sn))
	{
		Types_[sn] = header;
	}	
}

XmlSchemaTypeHeader const* nsYedaoqXmlSchema::CXmlSchema::GetType( XmlSchemaTypeSN_t sn ) const
{
	BOOST_ASSERT(sn < Types_.size() && sn >= 0);
	return Types_[sn].get();
}

XmlSchemaTypeHeader* nsYedaoqXmlSchema::CXmlSchema::GetType( XmlSchemaTypeSN_t sn )
{
	return const_cast<XmlSchemaTypeHeader*>(const_cast<const CXmlSchema*>(this)->GetType(sn));
}

XmlSchemaTypeHeader const* nsYedaoqXmlSchema::CXmlSchema::GetType( const tstring& name ) const
{
	for (TypeCollection::const_iterator iter = Types_.begin(); iter != Types_.end(); ++iter)
	{
		if((*iter)->Name == name)
		{
			return iter->get();
		}
	}

	return 0;
}

XmlSchemaTypeHeader* nsYedaoqXmlSchema::CXmlSchema::GetType( const tstring& name, bool autoCreate /*= false*/ )
{
	XmlSchemaTypeHeader* typeInfo = const_cast<XmlSchemaTypeHeader*>(const_cast<const CXmlSchema*>(this)->GetType(name));
	if(!typeInfo && autoCreate)
	{
		typeInfo = GetType(AddVirtualType(name));
	}
	return typeInfo;
}

void nsYedaoqXmlSchema::CXmlSchema::AddElement( const XmlSchemaElement& ele )
{
	if(ele.Name.size() <= 0)
	{
		throw std::exception("global element need a name!");
		return;
	}

	XmlSchemaElement* eleInfo = GetElement(ele.Name);
	if(eleInfo)
	{
		throw std::exception("element name conflict!");
		return;
	}
	Elements_.push_back(ele);
}

void nsYedaoqXmlSchema::CXmlSchema::SetElement(const tstring& name, const XmlSchemaElement* ele )
{
	XmlSchemaElement* eleInfo = GetElement(name);
	if(eleInfo)
	{
		*eleInfo = *ele;

	}
	else
	{
		throw std::exception("named element not found!");
	}
}

XmlSchemaElement const* nsYedaoqXmlSchema::CXmlSchema::GetElement( const tstring& name ) const
{
	for (ElementCollection::const_iterator iter = Elements_.begin(); iter != Elements_.end(); ++iter)
	{
		if(iter->Name == name)
		{
			return &(*iter);
		}
	}

	return 0;
}

XmlSchemaElement* nsYedaoqXmlSchema::CXmlSchema::GetElement( const tstring& name)
{
	return const_cast<XmlSchemaElement*>(const_cast<const CXmlSchema*>(this)->GetElement(name));
}

void nsYedaoqXmlSchema::CXmlSchema::Clear()
{
	Types_.clear();
	Elements_.clear();
}

bool nsYedaoqXmlSchema::CXmlSchema::NameConflict( const tstring& name, XmlSchemaTypeSN_t sn ) const
{
	if(name.size() > tstring::size_type(0))	// anonymous type should not examine
	{
		XmlSchemaTypeHeader const* typeInfo = GetType(name);
		if(typeInfo && typeInfo->SerialNO != sn)
		{
			throw std::exception("name conflict!");
			return true;
		}
	}
	return false;
}

void nsYedaoqXmlSchema::CXmlSchema::PreFillBuildinTypes()
{
	XmlSchemaSimpleType info;
	
	info.BaseType(INVALIDXMLSCHEMATYPESN);
	info.Name = CXsString::AnyType.Str;
	info.Flag = EnumSimpleTypeFlag::Special;
	AddType(info);

	PREFILLBUILDINTYPE_ITEM(CXsString::AnySimpleType,	CXsString::AnyType,					EnumSimpleTypeFlag::Special );
	PREFILLBUILDINTYPE_ITEM(CXsString::AnyAtomicType,	CXsString::AnySimpleType,			EnumSimpleTypeFlag::Special);
	PREFILLBUILDINTYPE_ITEM(CXsString::AnyURI,			CXsString::AnyAtomicType,			EnumSimpleTypeFlag::Atomic | EnumSimpleTypeFlag::Primitive);
	PREFILLBUILDINTYPE_ITEM(CXsString::Base64Binary,	CXsString::AnyAtomicType,			EnumSimpleTypeFlag::Atomic | EnumSimpleTypeFlag::Primitive);
	PREFILLBUILDINTYPE_ITEM(CXsString::Boolean,			CXsString::AnyAtomicType,			EnumSimpleTypeFlag::Atomic | EnumSimpleTypeFlag::Primitive);
	PREFILLBUILDINTYPE_ITEM(CXsString::Date,			CXsString::AnyAtomicType,			EnumSimpleTypeFlag::Atomic | EnumSimpleTypeFlag::Primitive);
	PREFILLBUILDINTYPE_ITEM(CXsString::DateTime,		CXsString::AnyAtomicType,			EnumSimpleTypeFlag::Atomic | EnumSimpleTypeFlag::Primitive);
	PREFILLBUILDINTYPE_ITEM(CXsString::DateTimeStamp,	CXsString::DateTime,				EnumSimpleTypeFlag::Atomic);
	PREFILLBUILDINTYPE_ITEM(CXsString::Decimal,			CXsString::AnyAtomicType,			EnumSimpleTypeFlag::Atomic | EnumSimpleTypeFlag::Primitive);
	PREFILLBUILDINTYPE_ITEM(CXsString::Double,			CXsString::AnyAtomicType,			EnumSimpleTypeFlag::Atomic | EnumSimpleTypeFlag::Primitive);
	PREFILLBUILDINTYPE_ITEM(CXsString::Duration,		CXsString::AnyAtomicType,			EnumSimpleTypeFlag::Atomic | EnumSimpleTypeFlag::Primitive);
	PREFILLBUILDINTYPE_ITEM(CXsString::DayTimeDuration, CXsString::Duration,				EnumSimpleTypeFlag::Atomic);
	PREFILLBUILDINTYPE_ITEM(CXsString::YearMonthDuration, CXsString::Duration,				EnumSimpleTypeFlag::Atomic);

	PREFILLBUILDINTYPE_ITEM(CXsString::Float,			CXsString::AnyAtomicType,			EnumSimpleTypeFlag::Atomic | EnumSimpleTypeFlag::Primitive);
	PREFILLBUILDINTYPE_ITEM(CXsString::GDay,			CXsString::AnyAtomicType,			EnumSimpleTypeFlag::Atomic | EnumSimpleTypeFlag::Primitive);
	PREFILLBUILDINTYPE_ITEM(CXsString::GMonth,			CXsString::AnyAtomicType,			EnumSimpleTypeFlag::Atomic | EnumSimpleTypeFlag::Primitive);
	PREFILLBUILDINTYPE_ITEM(CXsString::GMonthDay,		CXsString::AnyAtomicType,			EnumSimpleTypeFlag::Atomic | EnumSimpleTypeFlag::Primitive);
	PREFILLBUILDINTYPE_ITEM(CXsString::GYear,			CXsString::AnyAtomicType,			EnumSimpleTypeFlag::Atomic | EnumSimpleTypeFlag::Primitive);
	PREFILLBUILDINTYPE_ITEM(CXsString::GYearMonth,		CXsString::AnyAtomicType,			EnumSimpleTypeFlag::Atomic | EnumSimpleTypeFlag::Primitive);
	PREFILLBUILDINTYPE_ITEM(CXsString::HexBinary,		CXsString::AnyAtomicType,			EnumSimpleTypeFlag::Atomic | EnumSimpleTypeFlag::Primitive);
	PREFILLBUILDINTYPE_ITEM(CXsString::NOTATION,		CXsString::AnyAtomicType,			EnumSimpleTypeFlag::Atomic | EnumSimpleTypeFlag::Primitive);
	PREFILLBUILDINTYPE_ITEM(CXsString::QName,			CXsString::AnyAtomicType,			EnumSimpleTypeFlag::Atomic | EnumSimpleTypeFlag::Primitive);
	PREFILLBUILDINTYPE_ITEM(CXsString::String,			CXsString::AnyAtomicType,			EnumSimpleTypeFlag::Atomic | EnumSimpleTypeFlag::Primitive);
	PREFILLBUILDINTYPE_ITEM(CXsString::Time,			CXsString::AnyAtomicType,			EnumSimpleTypeFlag::Atomic | EnumSimpleTypeFlag::Primitive);

	PREFILLBUILDINTYPE_ITEM(CXsString::ENTITIES,		CXsString::AnySimpleType,			EnumSimpleTypeFlag::Atomic | EnumSimpleTypeFlag::Primitive);
	PREFILLBUILDINTYPE_ITEM(CXsString::IDREFS,			CXsString::AnySimpleType,			EnumSimpleTypeFlag::Atomic | EnumSimpleTypeFlag::Primitive);
	PREFILLBUILDINTYPE_ITEM(CXsString::NMTOKENS,		CXsString::AnySimpleType,			EnumSimpleTypeFlag::Atomic | EnumSimpleTypeFlag::Primitive);

	PREFILLBUILDINTYPE_ITEM(CXsString::Integer,			CXsString::Decimal,					EnumSimpleTypeFlag::Atomic);
	PREFILLBUILDINTYPE_ITEM(CXsString::Long,			CXsString::Integer,					EnumSimpleTypeFlag::Atomic);
	PREFILLBUILDINTYPE_ITEM(CXsString::Int,				CXsString::Long,					EnumSimpleTypeFlag::Atomic);
	PREFILLBUILDINTYPE_ITEM(CXsString::Short,			CXsString::Int,						EnumSimpleTypeFlag::Atomic);
	PREFILLBUILDINTYPE_ITEM(CXsString::Byte,			CXsString::Short,					EnumSimpleTypeFlag::Atomic);
	PREFILLBUILDINTYPE_ITEM(CXsString::NonNegativeInteger, CXsString::Integer,				EnumSimpleTypeFlag::Atomic);
	PREFILLBUILDINTYPE_ITEM(CXsString::PositiveInteger, CXsString::NonNegativeInteger,		EnumSimpleTypeFlag::Atomic);
	PREFILLBUILDINTYPE_ITEM(CXsString::UnsignedLong,	CXsString::NonNegativeInteger,		EnumSimpleTypeFlag::Atomic);
	PREFILLBUILDINTYPE_ITEM(CXsString::UnsignedInt,		CXsString::UnsignedLong,			EnumSimpleTypeFlag::Atomic);
	PREFILLBUILDINTYPE_ITEM(CXsString::UnsignedShort,	CXsString::UnsignedInt,				EnumSimpleTypeFlag::Atomic);
	PREFILLBUILDINTYPE_ITEM(CXsString::UnsignedByte,	CXsString::UnsignedShort,			EnumSimpleTypeFlag::Atomic);
	PREFILLBUILDINTYPE_ITEM(CXsString::NonPostiveInteger,CXsString::Integer,				EnumSimpleTypeFlag::Atomic);
	PREFILLBUILDINTYPE_ITEM(CXsString::NegativeInteger, CXsString::NonPostiveInteger,		EnumSimpleTypeFlag::Atomic);

	PREFILLBUILDINTYPE_ITEM(CXsString::NormalizedString,CXsString::String,					EnumSimpleTypeFlag::Atomic);
	PREFILLBUILDINTYPE_ITEM(CXsString::Token,			CXsString::NormalizedString,		EnumSimpleTypeFlag::Atomic);
	PREFILLBUILDINTYPE_ITEM(CXsString::Language,		CXsString::Token,					EnumSimpleTypeFlag::Atomic);
	PREFILLBUILDINTYPE_ITEM(CXsString::Name,			CXsString::Token,					EnumSimpleTypeFlag::Atomic);
	PREFILLBUILDINTYPE_ITEM(CXsString::NCName,			CXsString::Name,					EnumSimpleTypeFlag::Atomic);
	PREFILLBUILDINTYPE_ITEM(CXsString::ENTITY,			CXsString::NCName,					EnumSimpleTypeFlag::Atomic);
	PREFILLBUILDINTYPE_ITEM(CXsString::ID,				CXsString::NCName,					EnumSimpleTypeFlag::Atomic);
	PREFILLBUILDINTYPE_ITEM(CXsString::IDREF,			CXsString::NCName,					EnumSimpleTypeFlag::Atomic);
	PREFILLBUILDINTYPE_ITEM(CXsString::NMTOKEN,			CXsString::Token,					EnumSimpleTypeFlag::Atomic);
}

