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
        [ConfigurationProperty("PSAEnabled", DefaultValue = true),Description("�Ƿ����ù����������")]
        public bool PSAEnabled
        {
            get { return (bool)base["PSAEnabled"]; }
            set { base["PSAEnabled"] = value; }
        }

        [ConfigurationProperty("Name", DefaultValue = "Ye Daoquan"),Description("����")]
        public string Name
        {
            get { return this["Name"] as string; }
            set { this["Name"] = value; }
        }
    }
}
