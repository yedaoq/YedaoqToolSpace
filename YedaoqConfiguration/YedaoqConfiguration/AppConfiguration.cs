using System;
using System.Collections.Generic;
using System.Text;
using System.Reflection;
using System.Reflection.Emit;
using System.Configuration;
using CommonLibrary;

namespace YedaoqConfiguration
{
    public abstract class AppConfiguration : IAppConfiguration,IDisposable
    {
        public AppConfiguration(IClientInfo client)
        {
            if(object.ReferenceEquals(null,client))
            {
                throw new Exception("Client info nedded");
            }
            _ClientInfo = client;
            ConfigurationObjectManager = new ConfigurationObjectManager();
        }

        #region Fields

        /// <summary>
        /// 客户信息
        /// </summary>
        private IClientInfo _ClientInfo;

        /// <summary>
        /// 配置单元信息集合
        /// </summary>
        private IConfigurationUnitInfo[] _ConfigUnitInfos;

        /// <summary>
        /// Configuration对象管理器
        /// </summary>
        private IConfigurationObjectManager _ConfigurationObjectManager;

        #endregion               

        #region Properties

        /// <summary>
        /// 获取指定配置单元的配置信息
        /// </summary>
        /// <param name="ConfigInfo"></param>
        /// <returns></returns>
        protected object this[IConfigurationUnitInfo ConfigInfo]
        {
            get
            {
                if (ConfigInfo.InitFlag) return ConfigInfo.Value;
                GetConfigUnit(ConfigInfo);
                return ConfigInfo.Value;
            }
        }

        /// <summary>
        /// 配置单元信息集合
        /// </summary>
        public IEnumerable<IConfigurationUnitInfo> ConfigUnitInfos
        {
            get
            {
                if (object.ReferenceEquals(null, _ConfigUnitInfos)) InitConfigUnitInfos();
                return _ConfigUnitInfos;
            }
        }

        /// <summary>
        /// 客户信息
        /// </summary>
        public IClientInfo ClientInfo
        {
            get
            {
                return _ClientInfo;
            }
        }

        /// <summary>
        /// 配置单元数量
        /// </summary>
        public int ConfigUnitInfoCount
        {
            get
            {
                if (object.ReferenceEquals(null, _ConfigUnitInfos)) InitConfigUnitInfos();
                return _ConfigUnitInfos.Length;
            }
        }

        /// <summary>
        /// Configuration对象管理器
        /// </summary>
        protected IConfigurationObjectManager ConfigurationObjectManager
        {
            get
            {
                return _ConfigurationObjectManager;
            }
            set
            {
                _ConfigurationObjectManager = value;
            }
        }

        #endregion

        #region Methods

        /// <summary>
        /// 获取指定的配置单元
        /// </summary>
        /// <param name="ConfigInfo">配置单元信息</param>
        protected void GetConfigUnit(IConfigurationUnitInfo unitInfo)
        {
            string path = GetConfigUnitFilePath(this.ClientInfo, unitInfo);

            if (object.ReferenceEquals(null, path) || path.Equals(string.Empty)) return;

            Configuration configuration = ConfigurationObjectManager.GetConfiguration(path, true);
            if (object.ReferenceEquals(null, configuration)) return;

            unitInfo.Value = GetConfigUnit(configuration, unitInfo);

            unitInfo.InitFlag = true;
        }

        /// <summary>
        /// 获取指定配置单元
        /// </summary>
        /// <param name="configuration">配置对象</param>
        /// <param name="unitInfo">配置单元信息</param>
        /// <returns>相关的配置信息</returns>
        protected object GetConfigUnit(Configuration configuration, IConfigurationUnitInfo unitInfo)
        {
            if(unitInfo.UnitType.IsSubclassOf(typeof(ConfigurationSectionGroup)))
            {
                return ConfigurationHelper.GetConfigurationSectionGroup(configuration.RootSectionGroup, unitInfo.ConfigName, unitInfo.UnitType, unitInfo.AutoInitWhenNonExist, true);
            }
            else if (unitInfo.UnitType.IsSubclassOf(typeof(ConfigurationSection)))
            {
                return ConfigurationHelper.GetConfigurationSection(configuration.RootSectionGroup, unitInfo.ConfigName, unitInfo.UnitType, unitInfo.AutoInitWhenNonExist, true);
            }
            else
            {
                throw new Exception("Unknown Configuration Unit");
            }

            return null;
        }

        /// <summary>
        /// 获取指定配置单元的文件路径
        /// </summary>
        /// <param name="client">客户信息</param>
        /// <param name="unitInfo">配置单元信息</param>
        /// <returns>路径</returns>
        protected string GetConfigUnitFilePath(IClientInfo client, IConfigurationUnitInfo unitInfo)
        {
            string path = null;

            switch (unitInfo.Domain)
            {
                case EnumConfigDomain.Application:
                    path = client.SystemConfigurationDirectory;
                    break;

                case EnumConfigDomain.User:
                    path = client.UserConfigurationDirectory;
                    break;

                default:
                    throw new Exception("未知的配置文件域");
                    break;
            }

            return System.IO.Path.Combine(path, unitInfo.FileName);
        }

        /// <summary>
        /// 初始化ConfigUnitInfos属性
        /// </summary>
        protected void InitConfigUnitInfos()
        {
            Type typeUnitInfo = typeof(IConfigurationUnitInfo);
            Type type = this.GetType();

            FieldInfo[] fields = type.GetFields(BindingFlags.Instance | BindingFlags.NonPublic);
            List<IConfigurationUnitInfo> unitInfos = new List<IConfigurationUnitInfo>(fields.Length);
            
            foreach (FieldInfo field in fields)
            {
                if (typeUnitInfo.IsAssignableFrom(field.FieldType))
                {
                    object obj = field.GetValue(this);
                    if (obj != null) unitInfos.Add(obj as IConfigurationUnitInfo);
                }
            }

            _ConfigUnitInfos = unitInfos.ToArray();
        }

        public void Dispose()
        {
            if (!object.ReferenceEquals(null, this.ConfigurationObjectManager)) this.ConfigurationObjectManager.Dispose();
        }

        #endregion
    }
}
