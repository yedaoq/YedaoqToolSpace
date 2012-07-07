/* ___________________________________________________________________________
@ 
@ file - XmlSchemaEnum.h
@ 
@ auth - yedaoq@gmail.com          http://blog.yedaoq.info
@ date - 2012-3-5
@ info -
@     
/* ___________________________________________________________________________*/
#pragma once 
#include "CPP\Cloneable.h"
#include <map>
#include "XmlSchemaEnum.h"
#include <tchar.h>

#define INVALIDXMLSCHEMATYPESN	-1
#define DEFAULT_MAXOCCURS_VAL	1
#define DEFAULT_MINOCCURS_VAL	1

namespace nsYedaoqXmlSchema
{
	typedef int XmlSchemaTypeSN_t;
	
	/* 
	  a interface for all entity define for contain the data of xml schema component 
	*/
	struct IXmlSchemaObject : ICloneable
	{
		virtual EnumXmlSchemaObject ObjectCategory() const = 0;
	};

	/*
	  this struct used to keep the attribute info, which undefined in xs namespace, in the declaration of xml schema component element.
	   example : if a xs:element label has a attribute named "title", which has no mean for schema definition, it should be placed in MetaAnyAttribute
	*/
	struct MetaAnyAttribute
	{
		MetaAnyAttribute(const tstring& name, const tstring& val)
			: Name(name), Value(val)
		{}

		tstring Name;
		tstring Value;
	};

	struct MetaAnyAttributeCollection : public std::map<tstring, tstring>
	{
		tstring const* find(const tstring& name) const;
		tstring* find(const tstring& name);
		bool insert(const tstring& name, const tstring& val);
	};

	struct CXsString
	{
		CXsString(tchar const* str)
			: Str(str)
		{
			XsPrefixOffset = _tcsncmp(Str, TEXT("xs:"), 3) ? 3 : 0;
		}

		bool operator==(tchar const* str) const
		{
			return operator==(CXsString(str));
		}

		bool operator==(const CXsString& other) const
		{
			return !_tcscmp(Str + XsPrefixOffset, other.Str + other.XsPrefixOffset);
		}

		bool operator!=(const CXsString& other) const
		{
			return _tcscmp(Str + XsPrefixOffset, other.Str + other.XsPrefixOffset);
		}

		tchar const*	Str;
		int				XsPrefixOffset;

		// xs element
		static const CXsString all;
		static const CXsString annotation;
		static const CXsString any;
		static const CXsString anyAttribute;
		static const CXsString appInfo;
		static const CXsString attribute;
		static const CXsString attributeGroup;
		static const CXsString choice;
		static const CXsString complexContent;
		static const CXsString complexType;
		static const CXsString documentation;
		static const CXsString element;
		static const CXsString extension;
		static const CXsString field;
		static const CXsString group;
		static const CXsString import;
		static const CXsString include;
		static const CXsString key;
		static const CXsString keyref;
		static const CXsString list;
		static const CXsString notation;
		static const CXsString redefine;
		static const CXsString restriction;
		static const CXsString schema;
		static const CXsString selector;
		static const CXsString sequence;
		static const CXsString simpleContent;
		static const CXsString simpleType;
		static const CXsString union_;
		static const CXsString unique;

		// xs simple type defination element
		static const CXsString enumeration;
		static const CXsString fractionDigits;
		static const CXsString length;
		static const CXsString maxExclusive;
		static const CXsString maxInclusive;
		static const CXsString maxLength;
		static const CXsString minExclusive;
		static const CXsString minInclusive;
		static const CXsString minLength;
		static const CXsString pattern;
		static const CXsString totalDigits;
		static const CXsString whiteSpace;

		// xs buildin simple type list
		static const CXsString AnyType;
		static const CXsString AnySimpleType;
		static const CXsString AnyAtomicType;
		static const CXsString AnyURI;
		static const CXsString Base64Binary;
		static const CXsString Boolean;
		static const CXsString Date;
		static const CXsString DateTime;
		static const CXsString DateTimeStamp;
		static const CXsString Decimal;
		static const CXsString Double;
		static const CXsString Duration;
		static const CXsString DayTimeDuration;
		static const CXsString YearMonthDuration;
		static const CXsString Float;
		static const CXsString GDay;
		static const CXsString GMonth;
		static const CXsString GMonthDay;
		static const CXsString GYear;
		static const CXsString GYearMonth;
		static const CXsString HexBinary;
		static const CXsString NOTATION;
		static const CXsString QName;
		static const CXsString String;
		static const CXsString Time;
		static const CXsString ENTITIES;
		static const CXsString IDREFS;
		static const CXsString NMTOKENS;
		static const CXsString Integer;
		static const CXsString Long;
		static const CXsString Int;
		static const CXsString Short;
		static const CXsString Byte;
		static const CXsString NonNegativeInteger;
		static const CXsString PositiveInteger;
		static const CXsString UnsignedLong;
		static const CXsString UnsignedInt;
		static const CXsString UnsignedShort;
		static const CXsString UnsignedByte;
		static const CXsString NonPostiveInteger;
		static const CXsString NegativeInteger;
		static const CXsString NormalizedString;
		static const CXsString Token;
		static const CXsString Language;
		static const CXsString Name;
		static const CXsString NCName;
		static const CXsString ENTITY;
		static const CXsString ID;
		static const CXsString IDREF;
		static const CXsString NMTOKEN;
	};

}