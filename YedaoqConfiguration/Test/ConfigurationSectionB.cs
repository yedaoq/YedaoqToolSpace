using System;
using System.Collections.Generic;
using System.Text;
using System.Configuration;
using YedaoqConfiguration;

namespace Test
{
    public class ConfigurationSectionB : ExConfigurationSection
    {
        [ConfigurationProperty("ConnectionStr", DefaultValue = "DataSource = \"PERFECT\"")]
        public string ConnectionStr
        {
            get { return this["ConnectionStr"] as string; }
            set { this["ConnectionStr"] = value; }
        }

        [ConfigurationProperty("DBProvider", DefaultValue = "Sql Server")]
        public string DBProvider
        {
            get { return this["DBProvider"] as string; }
            set { this["DBProvider"] = value; }
        }
    }
}
