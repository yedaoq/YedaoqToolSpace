#pragma once

#ifdef BEGINDEFINE_CONFIG_SECTION
	#undef BEGINDEFINE_CONFIG_SECTION
#endif
#define BEGINDEFINE_CONFIG_SECTION(name) void Cfg##name##::Parse(xnode_t& node){ xattr_t* attr; xnode_t* child;


#ifdef ENDDEFINE_CONFIG_SECTION
	#undef ENDDEFINE_CONFIG_SECTION
#endif
#define ENDDEFINE_CONFIG_SECTION }


#ifdef CONFIG_SUBSECTION
	#undef CONFIG_SUBSECTION
#endif
#define CONFIG_SUBSECTION(t, name) child = node.first_node(TEXT(#name)); if(child){ name.Parse(*child); }


#ifdef CONFIG_SUBSECTIONARRAY
	#undef CONFIG_SUBSECTIONARRAY
#endif
#define CONFIG_SUBSECTIONARRAY(t, name)										\
	for(child = node.first_node(TEXT(#name)); child; child = child->next_sibling())	\
	{																				\
		CCfg##name sub;																\
		sub.Parse(*child);															\
		name##s.push_back(sub);														\
	}


#ifdef CONFIG_PROP
	#undef CONFIG_PROP
#endif
#define CONFIG_PROP(t, name) attr = node.first_attribute(TEXT(#name)); if(attr){ name = CConfigValueSerializer<t>().Parse(attr->value());}

#ifdef CONFIG_ENUMPROP
	#undef CONFIG_ENUMPROP
#endif
#define CONFIG_ENUMPROP(t, name) attr = node.first_attribute(TEXT(#name)); if(attr){ name = t::parse(attr->value(), TEXT(" "));}
