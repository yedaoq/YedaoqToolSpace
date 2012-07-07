using System;
using System.Collections.Generic;
using System.Text;

namespace YedaoqConfiguration
{
    public interface IAppConfiguration:IDisposable
    {
        /// <summary>
        /// 所有配置单元信息
        /// </summary>
        IEnumerable<IConfigurationUnitInfo> ConfigUnitInfos
        {
            get;
        }

        /// <summary>
        /// 客户信息
        /// </summary>
        IClientInfo ClientInfo
        {
            get;
        }

        /// <summary>
        /// 包含的配置单元数
        /// </summary>
        int ConfigUnitInfoCount
        {
            get;
        }
    }
}
