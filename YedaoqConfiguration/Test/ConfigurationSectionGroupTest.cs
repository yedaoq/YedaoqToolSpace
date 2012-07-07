using System;
using System.Collections.Generic;
using System.Text;
using YedaoqConfiguration;
using System.Configuration;
using System.Reflection;
using System.ComponentModel;

namespace Test
{
    public class ConfigurationSectionGroupTest : ConfigurationSectionGroup
    {
        [ConfigurationProperty("SectionA"), Description("ConfigurationSectionGroupTest的第一个子配置段")]
        public ConfigurationSectionA SectionA
        {
            get
            {
                //return this.Sections["SectionA"] as ConfigurationSectionA;
                return ConfigurationHelper.GetConfigurationSection<ConfigurationSectionA>(this, "SectionA", true, true);
            }
        }

        [ConfigurationProperty("SectionB"), Description("ConfigurationSectionGroupTest的第二个子配置段")]
        public ConfigurationSectionB SectionB
        {
            get
            {
                //return this.Sections["SectionB"] as ConfigurationSectionB;
                return ConfigurationHelper.GetConfigurationSection<ConfigurationSectionB>(this, "SectionB", true, true);
            }
        }
    }
}
