using System;
using System.Collections.Generic;
using System.Text;

namespace YedaoqConfiguration
{
    public interface IClientInfo
    {
        /// <summary>
        /// 客户ID
        /// </summary>
        int ID
        {
            get;
            set;
        }

        /// <summary>
        /// 客户名称
        /// </summary>
        string Name
        {
            get;
            set;
        }

        /// <summary>
        /// 客户的用户配置目录
        /// </summary>
        string UserConfigurationDirectory
        {
            get;
            set;
        }

        /// <summary>
        /// 客户的系统配置目录
        /// </summary>
        string SystemConfigurationDirectory
        {
            get;
            set;
        }
    }
}
