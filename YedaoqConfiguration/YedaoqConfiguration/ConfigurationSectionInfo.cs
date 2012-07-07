using System;
using System.Collections.Generic;
using System.Text;
using System.Configuration;

namespace YedaoqConfiguration
{
    public class ConfigurationSectionInfo<T> : ConfigurationUnitBase where T : ConfigurationSection, new()
    {
        public ConfigurationSectionInfo(string configName, EnumConfigDomain domain, string fileName, bool autoInitFlag)
            : base(configName, typeof(T), domain, fileName, autoInitFlag)
        {
        }
    }
}
