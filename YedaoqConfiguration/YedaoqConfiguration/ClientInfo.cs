using System;
using System.Collections.Generic;
using System.Text;

namespace YedaoqConfiguration
{
    public class ClientInfo : IClientInfo
    {
        public ClientInfo(int id, string name, string uDirectory, string sDirectory)
        {
            this.ID = id;
            this.Name = name;
            this.UserConfigurationDirectory = uDirectory;
            this.SystemConfigurationDirectory = sDirectory;
        }

        #region Fields

        /// <summary>
        /// �ͻ�ID
        /// </summary>
        private int _ID;
        /// <summary>
        /// �ͻ�����
        /// </summary>
        private string _Name;
        /// <summary>
        /// �ͻ��û�����Ŀ¼
        /// </summary>
        private string _UserConfigurationDirectory;
        /// <summary>
        /// �ͻ�ϵͳ����Ŀ¼
        /// </summary>
        private string _SystemConfigurationDirectory;

        #endregion

        #region Properties

        public int ID
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

        public string UserConfigurationDirectory
        {
            get
            {
                return _UserConfigurationDirectory;
            }
            set
            {
                _UserConfigurationDirectory = value;
            }
        }

        public string SystemConfigurationDirectory
        {
            get
            {
                return _SystemConfigurationDirectory;
            }
            set
            {
                _SystemConfigurationDirectory = value;
            }
        }

        #endregion

        public override string ToString()
        {
            return this.Name;
        }
    }
}
