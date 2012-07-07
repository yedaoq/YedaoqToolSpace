using System;
using System.Collections.Generic;
using System.Text;

namespace YedaoqConfiguration
{
    public interface IAppConfigurationProvider
    {
        /// <summary>
        /// ���пͻ�
        /// </summary>
        IEnumerable<IClientInfo> Clients
        {
            get;
        }

        /// <summary>
        /// �ͻ�����
        /// </summary>
        int ClientCount
        {
            get;
        }

        /// <summary>
        /// ��ǰ�ͻ�
        /// </summary>
        IClientInfo CurrentClient
        {
            get;
            set;
        }

        /// <summary>
        /// ��ǰ�ͻ�����
        /// </summary>
        IAppConfiguration CurrentClientConfiguration
        {
            get;
        }

        /// <summary>
        /// ��ʼ��
        /// </summary>
        int Initialize();
    }
}
