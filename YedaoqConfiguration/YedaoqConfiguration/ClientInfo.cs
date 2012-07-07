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
        /// 客户ID
        /// </summary>
        private int _ID;
        /// <summary>
        /// 客户名称
        /// </summary>
        private string _Name;
        /// <summary>
        /// 客户用户配置目录
        /// </summary>
        private string _UserConfigurationDirectory;
        /// <summary>
        /// 客户系统配置目录
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
