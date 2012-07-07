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
        [ConfigurationProperty("SectionA"), Description("ConfigurationSectionGroupTest�ĵ�һ�������ö�")]
        public ConfigurationSectionA SectionA
        {
            get
            {
                //return this.Sections["SectionA"] as ConfigurationSectionA;
                return ConfigurationHelper.GetConfigurationSection<ConfigurationSectionA>(this, "SectionA", true, true);
            }
        }

        [ConfigurationProperty("SectionB"), Description("ConfigurationSectionGroupTest�ĵڶ��������ö�")]
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
