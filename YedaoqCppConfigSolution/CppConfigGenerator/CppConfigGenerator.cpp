#include "stdafx.h"
#include "XmlSchema.h"
#include "XmlSchemaSerializer.h"
#include <fstream>
#include "BuildinType2CppType.h"

using namespace nsYedaoqXmlSchema;
using namespace nsYedaoqXmlSchema::nsSerialize;

CCppConfigGenerator::TypeCollectContext::TypeCollectContext( XmlSchema* schema )
: Schema(schema), TypeMask(schema->TypeCount())
{}


bool IsEnumDataType(XmlSchemaSimpleType* simpleType, XmlSchema* schema)
{
	return (simpleType->SimpleTypeCategory() == EnumSimpleTypeCategory::RestrictEnum);
}

bool IsFlagEnumDataType(XmlSchemaSimpleType* simpleType, XmlSchema* schema)
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

bool CCppConfigGenerator::GenerateCppCodeFromSchema( CXmlSchema* schema, const tstring& path, const tstring name)
{
	if(!schema || schema->ElementCount() <= 0)
	{
		return false;
	}

	TypeCollectContext ctx(schema);
	ctx.ProjectName = name;
	TypeCollect(schema, ctx);

	GenerateEnumFile(ctx);


	IEnumerator<XmlSchemaElement>* pEnumEle = schema->EnumGlobalElement();
	if(pEnumEle)
	{
		while(pEnumEle->MoveNext())
		{

		}
	}
}

void CCppConfigGenerator::TypeCollect( XmlSchema* schema, TypeCollectContext& ctx)
{
	IEnumerator<XmlSchemaElement>* pEnumEle = schema->EnumGlobalElement();
	if(pEnumEle)
	{
		while(pEnumEle->MoveNext())
		{
			TypeCollect(pEnumEle->Current(), types);
		}
	}
}

void CCppConfigGenerator::TypeCollect( XmlSchemaElement* ele, TypeCollectContext& ctx)
{
	XmlSchemaTypeHeader* tHeader = ctx.Schema->GetType(ele->Type);
	if(tHeader && tHeader->TypeCategory() == EnumDataTypeCategory::Complex)
	{
		if(tHeader->Name.size() <= 0)
		{
			tstring* name = ele->GetMetaAnyAttribute(TEXT("cppName"));
			if(name)
			{
				tHeader->Name = *name;
			}
			else
			{
				tHeader->Name = ele->Name;
			}			
		}

		 TypeCollect(static_cast<XmlSchemaComplexType*>(tHeader), ctx);
	}
	else
	{
		throw std::exception("when as a schema for config, element need a complex type!");
	}
}

void CCppConfigGenerator::TypeCollect( XmlSchemaAttribute* attr, TypeCollectContext& ctx )
{
	XmlSchemaTypeHeader* tHeader = ctx.Schema->GetType(attr->Type);
	if(tHeader->TypeCategory() != EnumDataTypeCategory::Simple)
	{
		throw std::exception("attribute need a simple type");
	}

	XmlSchemaSimpleType* simpleType = static_cast<XmlSchemaSimpleType*>(tHeader);

	if(IsEnumDataType(simpleType) || IsFlagEnumDataType(simpleType))
	{
		if(simpleType->Name.size() <= 0)
		{
			simpleType->Name = attr->Name;
		}
		if(IsEnumDataType(simpleType))
		{
			ctx.EnumTypes.push_back(simpleType);
		}
		else
		{
			ctx.FlagEnumTypes.push_back(simpleType);
		}		
	}
}

void CCppConfigGenerator::TypeCollect( XmlSchemaComplexType* complexType, TypeCollectContext& ctx)
{
	if(!ctx.TypeMask[complexType->SerialNO])
	{
		ctx.TypeMask[complexType->SerialNO] = true;

		if(complexType->BaseType != INVALIDXMLSCHEMATYPESN)
		{
			throw std::exception("when as a schema for config, type definition don't support inherit!")
		}

		TypeCollect(&complexType->Subitems, ctx);	
		ctx.SortedTypes.push_front(complexType);

		IEnumerator<XmlSchemaAttribute>* pEnumAttr = new_iterator_enumerator(complexType->Attributes.begin(), complexType->Attributes.end());
		if(pEnumAttr)
		{
			while(pEnumAttr->MoveNext())
			{
				XmlSchemaSimpleType* simpleType = static_cast<XmlSchemaSimpleType*>(ctx.Schema->GetType(pEnumAttr->Current().Type));
				TypeCollect(simpleType);				
			}
		}
	}
}

void CCppConfigGenerator::TypeCollect( XmlSchemaSubitemGroup* group, TypeCollectContext& ctx)
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
				TypeCollect(static_cast<XmlSchemaElement*>(pEnumItem->Current()), ctx);
			}
			else if(pEnumItem->Current()->Category() == EnumSubitemCategory::sequence)
			{
				TypeCollect(static_cast<XmlSchemaSubitemGroup*>(pEnumItem->Current()), ctx);
			}
			else
			{
				throw std::exception("when as a schema for config, only sequence and element support!");
			}
		}
	}
}

void CCppConfigGenerator::GenerateEnumFile( TypeCollectContext& ctx )
{
	if(ctx.EnumTypes.size() <= 0 || ctx.FlagEnumTypes.size() <= 0)
	{
		return;
	}

	std::tofstream f(ctx.ProjectName + TEXT("Enum.h"), std::ios_base::out);
	tstring tmp = ctx.ProjectName + TEXT("_ENUM_\n");
	f << TEXT("#ifndef _") << tmp;
	f << TEXT("#define _") << tmp;

	f << TEXT("#include \"EnumDeclaration3x/EnumHeaderMicro.h\"\n");
	
	GenerateEnumDefinition(ctx, f);

	f<< TEXT("#endif _") << tmp;

	f.flush();
	f.close();
}

void CCppConfigGenerator::GenerateEnumDefinition( TypeCollectContext& ctx, std::tofstream& fstrm )
{
	boost::scoped_ptr<IEnumerator<XmlSchemaSimpleType*>> pEnumType = new_iterator_enumerator(ctx.EnumTypes);
	XmlSchemaSimpleType* t = 0;

	if(pEnumType)
	{		
		while(pEnumType->MoveNext(t))
		{
			f << TEXT("\nBEGINENUM(") << t->Name << TEXT(", int, false)\n");
			SimpleTypeFacetEnumData* data = t->GetDataGroup<SimpleTypeFacetEnumData>();
			for (SimpleTypeFacetEnumData::iterator iter = data->begin(); iter != data->end(); ++iter)
			{
				f << TEXT("ENUMITEM0(") << *iter << TEXT(")\n");
			}
			f << TEXT("ENDENUM\n\n");
		}
	}

	pEnumType = new_iterator_enumerator(ctx.FlagEnumTypes);
	if(pEnumType)
	{
		while(pEnumType->MoveNext(t))
		{
			f << TEXT("\nBEGINENUM(") << t->Name << TEXT(", unsigned int, true)\n");
			SimpleTypeFacetEnumData* data = t->GetDataGroup<SimpleTypeFacetEnumData>();
			
			if(data->size() > 32)
			{
				throw std::exception("to many enum flag!");
			}
			int pos = 0;
			for (SimpleTypeFacetEnumData::iterator iter = data->begin(); iter != data->end(); ++iter)
			{
				f << TEXT("ENUMITEM2(") << *iter << TEXT(", ") << (1 << pos++) << TEXT(")\n");
			}
			f << TEXT("ENDENUM\n\n");
		}
	}
}

void CCppConfigGenerator::GenerateConfigFile( TypeCollectContext& ctx )
{
	std::tofstream f(ctx.ProjectName + TEXT("Config.h"), std::ios_base::out);
	tstring tmp = ctx.ProjectName + TEXT("_CONFIG_\n");
	f << TEXT("#ifndef _") << tmp;
	f << TEXT("#define _") << tmp;

	f << TEXT("#include \"ConfigHeaderMicro.h\"\n");

	GenerateConfigDefinition(ctx, f);

	f<< TEXT("#endif _") << tmp;

	f.flush();
	f.close();
}

void CCppConfigGenerator::GenerateConfigDefinition( TypeCollectContext& ctx, std::tofstream& fstrm )
{
	boost::scoped_ptr<IEnumerator<XmlSchemaComplexType*>> pEnumType = new_iterator_enumerator(ctx.SortedTypes);
	XmlSchemaComplexType* t = 0;

	if(pEnumType)
	{		
		while(pEnumType->MoveNext(t))
		{
			f << TEXT("\nBEGINDEFINE_CONFIG_SECTION(") << t->Name << TEXT(")\n");
			
			boost::scoped_ptr<IEnumerator<XmlSchemaAttribute>> pEnumAttr = new_iterator_enumerator(t->Attributes.begin(), t->Attributes.end());

			if(pEnumAttr)
			{
				while(pEnumAttr->MoveNext())
				{
					f << TEXT("CONFIG_PROP(") << GetSimpleTypeCppNameStr(ctx.Schema, pEnumAttr->Current().Type) << TEXT(", ") << pEnumAttr->Current().Name << TEXT(")\n");
				}
			}
			
			if(t->Subitems.Category() != EnumSubitemCategory::sequence)
			{
				throw std::exception("unsupported group!")
			}

			boost::scoped_ptr<IEnumerator<XmlSchemaSubitem*>> pEnumSub = t->Subitems.Enum();

			if(pEnumSub)
			{
				while(pEnumSub->MoveNext())
				{
					if(pEnumSub->Current()->Category() != EnumSubitemCategory::element)
					{
						throw std::exception("don't support group nesting!");
					}

					XmlSchemaElement* ele = static_cast<XmlSchemaElement*>(pEnumSub->Current());

					if(ele->MaxOccurs > 1)
					{
						f << TEXT("CONFIG_SUBSECTION(");
					}
					else
					{
						f << TEXT("CONFIG_SUBSECTIONARRAY(");
					}

					f << ctx.Schema->GetType(ele->Type)->Name << TEXT(", ") << ele->Name << TEXT(")\n");
				}
			}

			f << TEXT("ENDDEFINE_CONFIG_SECTION\n\n");
		}
	}
}

const tstring& CCppConfigGenerator::GetSimpleTypeCppNameStr( XmlSchema* schema, XmlSchemaTypeSN_t sn )
{
	XmlSchemaSimpleType* typeInfo = static_cast<XmlSchemaSimpleType*>(schema->GetType(sn));
	if(IsEnumDataType(typeInfo, schema) || IsFlagEnumDataType(typeInfo, schema))
	{
		return typeInfo->Name;
	}

	if(typeInfo->Flag & EnumSimpleTypeFlag::Buildin)
	{
		return CBuildinType2CppType::GetInstance()[typeInfo->Name];
	}
}


