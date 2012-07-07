using System;
using System.Collections.Generic;
using System.Text;
using YedaoqConfiguration;
using System.Configuration;
using System.ComponentModel;

namespace Test
{
    public class TestAppConfiguration : YedaoqConfiguration.AppConfiguration
    {
        public TestAppConfiguration(IClientInfo client)
            : base(client)
        {

        }

        private IConfigurationUnitInfo _TestGroup = 
            new ConfigurationSectionGroupInfo<ConfigurationSectionGroupTest>("TestGroup", EnumConfigDomain.Application, "TestConfig", true);
        
        private IConfigurationUnitInfo _TestSection = 
            new ConfigurationSectionInfo<ConfigurationSectionA>("TestSection", EnumConfigDomain.User, "TestConfig", true);

        [Description("Ӧ�ó���ĵ�һ�������õ�Ԫ")]
        public ConfigurationSectionGroupTest TestGroup
        {
            get
            {
                object obj = this[_TestGroup];
                return object.ReferenceEquals(null, obj) ? null : obj as ConfigurationSectionGroupTest;
            }
        }

        [Description("Ӧ�ó���ĵڶ��������õ�Ԫ")]
        public ConfigurationSectionA SectionA
        {
            get
            {
                object obj = this[_TestSection];
                return object.ReferenceEquals(null, obj) ? null : obj as ConfigurationSectionA;
            }
        }
    }
}
