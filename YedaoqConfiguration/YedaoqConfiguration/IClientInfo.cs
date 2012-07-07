using System;
using System.Collections.Generic;
using System.Text;

namespace YedaoqConfiguration
{
    public interface IClientInfo
    {
        /// <summary>
        /// �ͻ�ID
        /// </summary>
        int ID
        {
            get;
            set;
        }

        /// <summary>
        /// �ͻ�����
        /// </summary>
        string Name
        {
            get;
            set;
        }

        /// <summary>
        /// �ͻ����û�����Ŀ¼
        /// </summary>
        string UserConfigurationDirectory
        {
            get;
            set;
        }

        /// <summary>
        /// �ͻ���ϵͳ����Ŀ¼
        /// </summary>
        string SystemConfigurationDirectory
        {
            get;
            set;
        }
    }
}
