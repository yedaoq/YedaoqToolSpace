using System;
using System.Collections.Generic;
using System.Text;

namespace YedaoqConfiguration
{
    public interface IAppConfigurationProvider
    {
        /// <summary>
        /// 所有客户
        /// </summary>
        IEnumerable<IClientInfo> Clients
        {
            get;
        }

        /// <summary>
        /// 客户数量
        /// </summary>
        int ClientCount
        {
            get;
        }

        /// <summary>
        /// 当前客户
        /// </summary>
        IClientInfo CurrentClient
        {
            get;
            set;
        }

        /// <summary>
        /// 当前客户配置
        /// </summary>
        IAppConfiguration CurrentClientConfiguration
        {
            get;
        }

        /// <summary>
        /// 初始化
        /// </summary>
        int Initialize();
    }
}
