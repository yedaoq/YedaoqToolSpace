using System;
using System.Collections.Generic;
using System.Text;
using System.Configuration;

namespace YedaoqConfiguration
{
    public interface IConfigurationUnitInfo
    {
        /// <summary>
        /// 当不存在是否自动初始化
        /// </summary>
        bool AutoInitWhenNonExist
        {
            get;
            set;
        }

        /// <summary>
        /// 配置域
        /// </summary>
        EnumConfigDomain Domain
        {
            get;
            set;
        }

        /// <summary>
        /// 所属配置文件名
        /// </summary>
        string FileName
        {
            get;
            set;
        }

        /// <summary>
        /// 配置名
        /// </summary>
        string ConfigName
        {
            get;
            set;
        }

        /// <summary>
        /// 配置单元类的类型
        /// </summary>
        Type UnitType
        {
            get;
            set;
        }

        /// <summary>
        /// 配置信息是否已读取的标记
        /// </summary>
        bool InitFlag
        {
            get;
            set;
        }

        /// <summary>
        /// 配置信息
        /// </summary>
        object Value
        {
            get;
            set;
        }
    }
}
