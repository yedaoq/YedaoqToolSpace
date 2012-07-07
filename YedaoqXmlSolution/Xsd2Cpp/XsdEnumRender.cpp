#include "XsdEnumRender.h"
#include "ctemplate\template.h"
#include "CPP\Enumerator.h"
//#include <memory>
#include <boost\smart_ptr\scoped_ptr.hpp>
#include <boost\algorithm\string\case_conv.hpp>
#include "XmlSchemaTypeDefinition.h"
#include <fstream>

using namespace nsYedaoqXmlSchema::nsXsd2Cpp;
using namespace nsYedaoqXmlSchema;

CXsdEnumRender::CXsdEnumRender(void)
{
}

CXsdEnumRender::~CXsdEnumRender(void)
{
}

void CXsdEnumRender::Render( Xsd2CppContext& ctx )
{
	ctemplate::TemplateDictionary dict("enum");

	tstring tmp = ctx.ProjectName;
	boost::algorithm::to_upper(tmp);

	dict.SetValue("project.name.upper", tmp);
	dict.SetValue("project.name", ctx.ProjectName);

	if(ctx.ProjectNamespace.length() >= 0)
	{
		dict.SetValueAndShowSection("project.namespace", ctx.ProjectNamespace, "nsblock");
	}

	boost::scoped_ptr<IEnumerator<XmlSchemaSimpleType*> > pEnumType(new_iterator_enumerator(ctx.EnumTypes.begin(), ctx.EnumTypes.end()));
	if(pEnumType.get())
	{
		while(pEnumType->MoveNext())
		{
			ctemplate::TemplateDictionary* dictEnum = dict.AddSectionDictionary("xsdenum");
			dictEnum->SetValue("name", pEnumType->Current()->Name);
			dictEnum->SetValue("base", "int");
			dictEnum->SetValue("flag", "false");

			SimpleTypeFacetEnumData* data = pEnumType->Current()->GetDataGroup<SimpleTypeFacetEnumData>();
			int idx = 0;
			for(SimpleTypeFacetEnumData::iterator iter = data->begin(); iter != data->end(); ++iter)
			{
				ctemplate::TemplateDictionary* dictItem = dictEnum->AddSectionDictionary("xsdenumitem");
				dictItem->SetValue("name", iter->c_str());
				dictItem->SetIntValue("val", idx++);
			}
		}
	}

	pEnumType.reset(new_iterator_enumerator(ctx.FlagEnumTypes.begin(), ctx.FlagEnumTypes.end()));
	if(pEnumType)
	{
		while(pEnumType->MoveNext())
		{
			ctemplate::TemplateDictionary* dictEnum = dict.AddSectionDictionary("xsdenum");
			dictEnum->SetValue("name", pEnumType->Current()->Name);
			dictEnum->SetValue("base", "int");
			dictEnum->SetValue("flag", "true");

			SimpleTypeFacetEnumData* data = pEnumType->Current()->GetDataGroup<SimpleTypeFacetEnumData>();
			int idx = 1;
			for(SimpleTypeFacetEnumData::iterator iter = data->begin(); iter != data->end(); ++iter)
			{
				ctemplate::TemplateDictionary* dictItem = dictEnum->AddSectionDictionary("xsdenumitem");
				dictItem->SetValue("name", *iter);
				dictItem->SetIntValue("val", 1 << idx++);
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
