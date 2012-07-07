#pragma once

#ifdef BEGINDEFINE_CONFIG_SECTION
	#undef BEGINDEFINE_CONFIG_SECTION
#endif
#define BEGINDEFINE_CONFIG_SECTION(name) xnode_t* Cfg##name##::Serialize(xdoc_t* doc){ xnode_t* node = doc->allocate_node(rapidxml::node_type::node_element, TEXT(#name));


#ifdef ENDDEFINE_CONFIG_SECTION
	#undef ENDDEFINE_CONFIG_SECTION
#endif
#define ENDDEFINE_CONFIG_SECTION return node; }


#ifdef CONFIG_SUBSECTION
	#undef CONFIG_SUBSECTION
#endif
#define CONFIG_SUBSECTION(t, name) node.append_node(name##.Serialize());


#ifdef CONFIG_SUBSECTIONARRAY
	#undef CONFIG_SUBSECTIONARRAY
#endif
#define CONFIG_SUBSECTIONARRAY(t, name)												\
	for(std::vector<CCfg##t>::iterator iter = name##s.begin(); iter != name##s.end(); ++iter)	\
	{																						\
		node.append_node(iter->Serialize());												\
	}																						


#ifdef CONFIG_PROP
	#undef CONFIG_PROP
#endif
#define CONFIG_PROP(t, name) node.append_attribute(doc->allocate_attribute(TEXT(#name), CConfigValueSerializer<t>().Serialize(name)));

#ifdef CONFIG_ENUMPROP
	#undef CONFIG_ENUMPROP
#endif
#define CONFIG_ENUMPROP(t, name) node.append_attribute(doc->allocate_attribute(TEXT(#name), name.str()));
