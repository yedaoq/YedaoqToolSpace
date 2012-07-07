using System;
using System.Collections.Generic;
using System.Text;
using System.Configuration;
using YedaoqConfiguration;
using System.ComponentModel;

namespace Test
{
    public class ConfigurationSectionA : ExConfigurationSection
    {
        [ConfigurationProperty("PSAEnabled", DefaultValue = true),Description("是否启用供电分析功能")]
        public bool PSAEnabled
        {
            get { return (bool)base["PSAEnabled"]; }
            set { base["PSAEnabled"] = value; }
        }

        [ConfigurationProperty("Name", DefaultValue = "Ye Daoquan"),Description("名称")]
        public string Name
        {
            get { return this["Name"] as string; }
            set { this["Name"] = value; }
        }
    }
}
