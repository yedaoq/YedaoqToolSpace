/* ___________________________________________________________________________
@ 
@ file - FacetGroup.h
@ 
@ auth - yedaoq@gmail.com          http://blog.yedaoq.info
@ date - 2012-3-13
@ info -
@     
/* ___________________________________________________________________________*/

#pragma once

#include "CPP\Cloneable.h"
#include "CPP\tstring.h"
#include "CPP\Singlton.h"
#include <vector>
#include "XmlSchemaEnum.h"
#include "XmlSchemaCommon.h"

namespace nsYedaoqXmlSchema
{
	interface ISimpleTypeDataGroup : public ICloneable
	{
		virtual EnumSimpleTypeCategory	SimpleTypeCategory() const = 0;
		virtual ~ISimpleTypeDataGroup() = 0 {}
	};

	struct SimpleTypeFacetLengthData : public CCloneable<SimpleTypeFacetLengthData, ISimpleTypeDataGroup>	// length countable, such as string, for xs:length, xs:minLength, xs:maxLength
	{
		int								Length;
		int								MaxLength;
		int								MinLength;

		virtual EnumSimpleTypeCategory	SimpleTypeCategory() const { return EnumSimpleTypeCategory::RestrictLength; }
	};

	struct SimpleTypeFacetRangeData : public CCloneable<SimpleTypeFacetRangeData, ISimpleTypeDataGroup>	// number type or comparable type, such as int,short,time,date , for xs:totalDigits, xs:fractionDigits, xs:minInclusive ...
	{
		int								TotalDigits;
		int								FractionDigits;
		tstring							UpperBound;
		tstring							LowerBound;
		bool							UpperInclusive;
		bool							LowerInclusive;

		virtual EnumSimpleTypeCategory	SimpleTypeCategory() const { return EnumSimpleTypeCategory::RestrictRange; }
	};

	struct SimpleTypeFacetPatternData : public CCloneable<SimpleTypeFacetPatternData, ISimpleTypeDataGroup>, public std::vector<tstring> 
	{
		virtual EnumSimpleTypeCategory	SimpleTypeCategory() const { return EnumSimpleTypeCategory::RestrictPattern; }
	};

	struct SimpleTypeFacetEnumData : public CCloneable<SimpleTypeFacetEnumData, ISimpleTypeDataGroup>, public std::vector<tstring> 
	{
		virtual EnumSimpleTypeCategory	SimpleTypeCategory() const { return EnumSimpleTypeCategory::RestrictEnum; }
	};

	struct SimpleTypeUnionData : public CCloneable<SimpleTypeUnionData, ISimpleTypeDataGroup>, public std::vector<XmlSchemaTypeSN_t> 
	{
		virtual EnumSimpleTypeCategory	SimpleTypeCategory() const { return EnumSimpleTypeCategory::Union; }
	};

	struct SimpleTypeListData : public CCloneable<SimpleTypeListData, ISimpleTypeDataGroup>
	{
		XmlSchemaTypeSN_t				ItemType;

		virtual EnumSimpleTypeCategory	SimpleTypeCategory() const { return EnumSimpleTypeCategory::List; }
	};

	struct SimpleTypeFacetNoneData : public ISimpleTypeDataGroup, public CSingleton<SimpleTypeFacetNoneData>
	{
		virtual EnumSimpleTypeCategory	SimpleTypeCategory() const { return EnumSimpleTypeCategory::RestrictNone; }
		virtual ICloneable*				Clone() const { return const_cast<SimpleTypeFacetNoneData*>(this); }
		virtual	void					Delete() { }

	protected:
		//SimpleTypeFacetNoneData() {};
	};

	template<typename T> inline EnumSimpleTypeCategory SimpleTypeCategory_Traits()					{ throw std::exception(); }
	template<> inline EnumSimpleTypeCategory SimpleTypeCategory_Traits<SimpleTypeFacetEnumData>()	{ return EnumSimpleTypeCategory::RestrictEnum; }
	template<> inline EnumSimpleTypeCategory SimpleTypeCategory_Traits<SimpleTypeFacetRangeData>()	{ return EnumSimpleTypeCategory::RestrictRange; }
	template<> inline EnumSimpleTypeCategory SimpleTypeCategory_Traits<SimpleTypeFacetPatternData>(){ return EnumSimpleTypeCategory::RestrictPattern; }
	template<> inline EnumSimpleTypeCategory SimpleTypeCategory_Traits<SimpleTypeFacetLengthData>()	{ return EnumSimpleTypeCategory::RestrictLength; }
	template<> inline EnumSimpleTypeCategory SimpleTypeCategory_Traits<SimpleTypeFacetNoneData>()	{ return EnumSimpleTypeCategory::RestrictNone; }
	template<> inline EnumSimpleTypeCategory SimpleTypeCategory_Traits<SimpleTypeUnionData>()		{ return EnumSimpleTypeCategory::Union; }
	template<> inline EnumSimpleTypeCategory SimpleTypeCategory_Traits<SimpleTypeListData>()		{ return EnumSimpleTypeCategory::List; }
}
