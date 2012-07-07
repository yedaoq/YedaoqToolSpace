using System;
using System.Collections.Generic;
using System.Text;
using System.Configuration;

namespace YedaoqConfiguration
{
    public class ConfigurationSectionGroupInfo<T> : ConfigurationUnitBase where T : ConfigurationSectionGroup, new()
    {
        public ConfigurationSectionGroupInfo(string configName, EnumConfigDomain domain, string fileName, bool autoInitFlag)
            : base(configName, typeof(T), domain, fileName, autoInitFlag)
        {
        }
    }
}
