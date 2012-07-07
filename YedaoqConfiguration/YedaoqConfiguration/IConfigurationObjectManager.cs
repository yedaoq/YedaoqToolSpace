using System;
using System.Collections.Generic;
using System.Text;
using System.Configuration;

namespace YedaoqConfiguration
{
    public interface IConfigurationObjectManager:IDisposable
    {
        /// <summary>
        /// 获取指定的Cofiguration对象
        /// </summary>
        /// <param name="File">配置文件名</param>
        Configuration GetConfiguration(string file, bool createWhenNonExist);

        /// <summary>
        /// 将Configuration对象保存到配置文件
        /// </summary>
        int Save();

        /// <summary>
        /// 将Configuration对象保存到配置文件
        /// </summary>
        /// <param name="file">文件名</param>
        int Save(string file);
    }
}
