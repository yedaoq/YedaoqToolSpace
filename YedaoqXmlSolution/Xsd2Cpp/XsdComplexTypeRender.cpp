#include "XsdComplexTypeRender.h"

CXsdComplexTypeRender::CXsdComplexTypeRender(void)
{
}

CXsdComplexTypeRender::~CXsdComplexTypeRender(void)
{
}

void CXsdComplexTypeRender::Render( nsYedaoqXmlSchema::nsXsd2Cpp::Xsd2CppContext& ctx )
{
	ctemplate::TemplateDictionary dict("xsd");

	tstring tmp = ctx.ProjectName;
	boost::algorithm::to_upper(tmp);

	dict.SetValue("project.name.upper", tmp);
	dict.SetValue("project.name", ctx.ProjectName);

	if(ctx.ProjectNamespace.length() >= 0)
	{
		dict.SetValueAndShowSection("project.namespace", ctx.ProjectNamespace, "nsblock");
	}

	boost::scoped_ptr<IEnumerator<XmlSchemaComplexType*> > pEnumType(new_iterator_enumerator(ctx.SortedComplexTypes.begin(), ctx.SortedComplexTypes.end()));
	if(pEnumType.get())
	{
		XmlSchemaComplexType* complexType;
		while(pEnumType->MoveNext(complexType))
		{
			ctemplate::TemplateDictionary* dictEnum = dict.AddSectionDictionary("xsdcomplex");
			dictEnum->SetValue("cppname", ctx.Params->NameMapper->MapId(complexType->Name, INameMapper::ComplexType);

			for(XmlSchemaComplexType::AttributeCollection::const_iterator iter = complexType->Attributes.begin();
				iter != complexType->Attributes.end(); ++iter)
			{
				ctemplate::TemplateDictionary* dictAttr = dictEnum->AddSectionDictionary("XML_ATTR");
				dictAttr->SetValue("xmlname", iter->Name);
				dictAttr->SetValue("cppname", ctx.Params->NameMapper->MapId(iter->Name, INameMapper::AttributeName));
				dictAttr->SetValue("cpptype", ctx.Params->NameMapper->MapId(iter->Type, INameMapper::SimpleType));
			}

			for(XmlSchemaSubitemGroup::SubitemCollection::const_iterator iter = complexType->Subitems.Subitems.begin();
				iter != complexType->Subitems.Subitems.end(); ++iter)
			{
				if((*iter)->Category() != EnumSubitemCategory::element) break;

				ctemplate::TemplateDictionary* dictEle = dictEnum->AddSectionDictionary(((*iter)->MaxOccurs > 1) ? "XML_ELES" : "XML_ELE");
				dictEle->SetValue("xmlname", (*iter)->Name);
				dictEle->SetValue("cppname", ctx.Params->NameMapper->MapId((*iter)->Name, INameMapper::AttributeName));
				dictEle->SetValue("cpptype", ctx.Params->NameMapper->MapId((*iter)->Type, INameMapper::SimpleType));
			}
		}
	}

	std::string content;
	std::string filepath;

	// generate enum header file
	ctemplate::ExpandTemplate("EnumHeader.tpl", ctemplate::DO_NOT_STRIP, &dict, &content);

	filepath = ctx.ProjectName + "Enum.h";
	std::tofstream f(filepath.c_str(), std::ios_base::out);
	f<<content;
	f.flush();
	f.close();

	// generate enum entity file
	ctemplate::ExpandTemplate("EnumEntity.tpl", ctemplate::DO_NOT_STRIP, &dict, &content);

	filepath = ctx.ProjectName + "Enum.cpp";
	f.open(filepath.c_str(), std::ios_base::out);
	f<<content;
	f.flush();
	f.close();
}
