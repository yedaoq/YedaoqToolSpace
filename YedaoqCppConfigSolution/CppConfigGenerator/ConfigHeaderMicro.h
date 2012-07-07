#pragma once

#ifndef BEGINDEFINE_CONFIG_SECTION
	#define BEGINDEFINE_CONFIG_SECTION(name) struct Cfg##name{ xnode_t* Serialize(); void Parse(xnode_t& node);
#endif

#ifndef ENDDEFINE_CONFIG_SECTION
	#define ENDDEFINE_CONFIG_SECTION };
#endif

#ifndef CONFIG_SUBSECTION
	#define CONFIG_SUBSECTION(t, name) Cfg##t name;
#endif

#ifndef CONFIG_SUBSECTIONARRAY
	#define CONFIG_SUBSECTIONARRAY(t, name) std::vector<Cfg##t> name##s;
#endif

#ifndef CONFIG_PROP
	#define CONFIG_PROP(t, name) t name;
#endif

#ifndef CONFIG_ENUMPROP
	#define CONFIG_ENUMPROP(t, name) Enum##t name;
#endif