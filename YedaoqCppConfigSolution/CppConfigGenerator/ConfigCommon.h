#pragma once

#include "\rapidxml-1.13\rapidxml.hpp"
#include "\rapidxml-1.13\rapidxml_print.hpp"
#include "tstring.h"
#include <fstream>

typedef rapidxml::xml_node<tchar>		xnode_t;
typedef rapidxml::xml_attribute<tchar>	xattr_t;
typedef rapidxml::xml_document<tchar>	xdoc_t;

// xattr_t* new_xmlattr(tchar const* name, tchar const* val)
// {
// 	xattr_t* attr = new xattr_t();
// 	attr->name(name);
// 	attr->value(val);
// }
// 
// xnode_t* new_xmlnode(xdoc_t* doc, tchar const* name)
// {
// 	xnode_t* node = doc->allocate_node(rapidxml::node_type::node_element, );
// 	node->name(name);
// }

template<typename T>
struct CConfigValueSerializer
{
	tstring Serialize(const T& val) const
	{
		return boost::lexical_cast<tstring>(val);
	}

	T Parse(tstring& val) const
	{
		return boost::lexical_cast<T>(val);
	}
};

template<>
struct CConfigValueSerializer<tstring>
{
	tstring Serialize(const tstring& val) const
	{
		return val;
	}

	tstring Parse(tstring& val) const
	{
		return val;
	}
};

template<>
struct CConfigValueSerializer<tchar*>
{
	tstring Serialize(tchar const* val) const
	{
		return val;
	}

	tchar const* Parse(tstring& val) const
	{
		return val.c_str();
	}
};

template<>
struct CConfigValueSerializer<tchar*>
{
	tstring Serialize(tchar const* val) const
	{
		return val;
	}

	tchar const* Parse(tstring& val) const
	{
		return val.c_str();
	}
};

template<typename T>
struct CGlobalConfig
{
	static T Instance;

	static bool Load(tchar const* file);
	static bool Save(tchar const* file);
};

template<typename T>
bool CGlobalConfig<T>::Save( tchar const* file )
{
	xdoc_t doc;
	doc.append_node(Instance.Serialize(&doc));

	tstring content;
	rapidxml::print(std::back_inserter(content), doc, 0);

	std::locale prev_loc = std::locale::global(std::locale("chs"));
	std::tofstream f(file, std::ios_base::out);

	f<<content;
	f.flush();
	f.close();

	return true;
}

template<typename T>
bool CGlobalConfig<T>::Load( tchar const* file )
{
	std::locale prev_loc = std::locale::global(std::locale("chs"));
	std::tifstream f(file, std::ios_base::in);

	tstring content;
	std::getline(f, content, EOF);

	tchar* bufXml = new tchar[content.size() + 1];
	content.copy(bufXml, content.size());
	bufXml[content.size()] = 0;

	xdoc_t doc;
	doc.parse<0>(bufXml);

	Instance.Parse(doc.first_node());

	return true;
}

template<typename T> T CGlobalConfig<T>::Instance;