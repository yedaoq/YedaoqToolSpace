using System;
using System.Collections.Generic;
using System.Text;

namespace YedaoqConfiguration
{
    /// <summary>
    /// ���ڱ��һ������Ӧ�ó��������ṩ�ߵ�����
    /// </summary>
    [AttributeUsage(AttributeTargets.Class,Inherited = true,AllowMultiple = false)]
    public class AppConfigurationProviderAttribute : Attribute
    {
        public AppConfigurationProviderAttribute(string id, string name)
        {
            this.ID = id;
            this.Name = name;
        }

        private string _ID;
        private string _Name;

        /// <summary>
        /// ID
        /// </summary>
        public string ID
        {
            get
            {
                return _ID;
            }
            set
            {
                _ID = value;
            }
        }

        /// <summary>
        /// ����
        /// </summary>
        public string Name
        {
            get
            {
                return _Name;
            }
            set
            {
                _Name = value;
            }
        }
    }
}
