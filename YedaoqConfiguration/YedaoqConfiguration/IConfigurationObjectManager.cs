using System;
using System.Collections.Generic;
using System.Text;
using System.Configuration;

namespace YedaoqConfiguration
{
    public interface IConfigurationObjectManager:IDisposable
    {
        /// <summary>
        /// ��ȡָ����Cofiguration����
        /// </summary>
        /// <param name="File">�����ļ���</param>
        Configuration GetConfiguration(string file, bool createWhenNonExist);

        /// <summary>
        /// ��Configuration���󱣴浽�����ļ�
        /// </summary>
        int Save();

        /// <summary>
        /// ��Configuration���󱣴浽�����ļ�
        /// </summary>
        /// <param name="file">�ļ���</param>
        int Save(string file);
    }
}
