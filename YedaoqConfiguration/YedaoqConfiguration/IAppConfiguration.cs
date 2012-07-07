using System;
using System.Collections.Generic;
using System.Text;

namespace YedaoqConfiguration
{
    public interface IAppConfiguration:IDisposable
    {
        /// <summary>
        /// �������õ�Ԫ��Ϣ
        /// </summary>
        IEnumerable<IConfigurationUnitInfo> ConfigUnitInfos
        {
            get;
        }

        /// <summary>
        /// �ͻ���Ϣ
        /// </summary>
        IClientInfo ClientInfo
        {
            get;
        }

        /// <summary>
        /// ���������õ�Ԫ��
        /// </summary>
        int ConfigUnitInfoCount
        {
            get;
        }
    }
}
