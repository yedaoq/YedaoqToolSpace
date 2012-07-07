#include "TypeCollecter.h"
#include "XmlSchema.h"
#include <exception>

using namespace nsYedaoqXmlSchema;
using namespace nsYedaoqXmlSchema::nsXsd2Cpp;

#define RETatFAIL(expr) if(!(expr)) return;
#define RETatFAILr(expr, ret) if(!(expr)) return ret;

void CTypeCollector::Collect( CXmlSchema* schema, Xsd2CppContext& ctx)
{
	IEnumerator<XmlSchemaElement>* pEnumEle = schema->EnumGlobalElement();
	if(pEnumEle)
	{
		while(pEnumEle->MoveNext())
		{
			Collect(schema->GetElement(pEnumEle->Current().Name), ctx);
		}
	}
}

void CTypeCollector::Collect( XmlSchemaElement* ele, Xsd2CppContext& ctx)
{
	XmlSchemaTypeHeader* tHeader = ctx.Schema->GetType(ele->Type);
	if(tHeader && tHeader->TypeCategory() == EnumDataTypeCategory::Complex)
	{
		if(tHeader->Name.size() <= 0)
		{
			const tstring* name = ele->MetaAnyAttributes.find(TEXT("cppName"));
			if(name)
			{
				tHeader->Name = *name;
			}
			else
			{
				tHeader->Name = ele->Name;
			}			
		}

		Collect(static_cast<XmlSchemaComplexType*>(tHeader), ctx);
	}
	else
	{
		throw std::exception("when as a schema for config, element need a complex type!");
	}
}

void CTypeCollector::Collect( XmlSchemaAttribute* attr, Xsd2CppContext& ctx )
{
	XmlSchemaTypeHeader* tHeader = ctx.Schema->GetType(attr->Type);
	if(tHeader->TypeCategory() != EnumDataTypeCategory::Simple)
	{
		throw std::exception("attribute need a simple type");
	}

	XmlSchemaSimpleType* simpleType = static_cast<XmlSchemaSimpleType*>(tHeader);

	if(IsEnumDataType(simpleType, ctx.Schema) || IsFlagEnumDataType(simpleType, ctx.Schema))
	{
		if(simpleType->Name.size() <= 0)
		{
			simpleType->Name = attr->Name;
		}
		if(IsEnumDataType(simpleType, ctx.Schema))
		{
			ctx.EnumTypes.push_back(simpleType);
		}
		else
		{
			ctx.FlagEnumTypes.push_back(simpleType);
		}		
	}
}

void CTypeCollector::Collect( XmlSchemaComplexType* complexType, Xsd2CppContext& ctx)
{
	if(!ctx.TypeMask[complexType->SerialNO])
	{
		ctx.TypeMask[complexType->SerialNO] = true;

		if(complexType->BaseType != INVALIDXMLSCHEMATYPESN)
		{
			throw std::exception("when as a schema for config, type definition don't support inherit!");
		}

		Collect(&complexType->Subitems, ctx);	
		ctx.SortedComplexTypes.push_back(complexType);

		IEnumerator<XmlSchemaAttribute>* pEnumAttr = new_iterator_enumerator(complexType->Attributes.begin(), complexType->Attributes.end());
		if(pEnumAttr)
		{
			while(pEnumAttr->MoveNext())
			{
				Collect(const_cast<XmlSchemaAttribute*>(&pEnumAttr->Current()), ctx);				
			}
		}
	}
}

void CTypeCollector::Collect( XmlSchemaSubitemGroup* group, Xsd2CppContext& ctx)
{
	if(group->Category() != EnumSubitemCategory::sequence)
	{
		throw std::exception("when as a schema for config, only sequence support!");
	}

	IEnumerator<XmlSchemaSubitem*>* pEnumItem = group->Subitems.Enum();
	if(pEnumItem)
	{
		while(pEnumItem->MoveNext())
		{
			if(pEnumItem->Current()->Category() == EnumSubitemCategory::element)
			{
				Collect(static_cast<XmlSchemaElement*>(pEnumItem->Current()), ctx);
			}
			else if(pEnumItem->Current()->Category() == EnumSubitemCategory::sequence)
			{
				Collect(static_cast<XmlSchemaSubitemGroup*>(pEnumItem->Current()), ctx);
			}
			else
			{
				throw std::exception("when as a schema for config, only sequence and element support!");
			}
		}
	}
}

bool CTypeCollector::Collect( Xsd2CppContext& ctx )
{
	Collect(ctx.Schema, ctx);
	return true;
}

bool CTypeCollector::IsEnumDataType(XmlSchemaSimpleType* simpleType, CXmlSchema* schema)
{
	return (simpleType->SimpleTypeCategory() == EnumSimpleTypeCategory::RestrictEnum);
}

bool CTypeCollector::IsFlagEnumDataType(XmlSchemaSimpleType* simpleType, CXmlSchema* schema)
{
	if(simpleType->SimpleTypeCategory() == EnumSimpleTypeCategory::List)
	{
		XmlSchemaTypeSN_t sn = simpleType->GetDataGroup<SimpleTypeListData>()->ItemType;
		XmlSchemaSimpleType* itemType = static_cast<XmlSchemaSimpleType*>(schema->GetType(sn));
		if(itemType && itemType->SimpleTypeCategory() == EnumSimpleTypeCategory::RestrictEnum)
		{
			return true;
		}
	}

	return false;
}
