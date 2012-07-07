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
        /// �ͻ���Ϣ
        /// </summary>
        private IClientInfo _ClientInfo;

        /// <summary>
        /// ���õ�Ԫ��Ϣ����
        /// </summary>
        private IConfigurationUnitInfo[] _ConfigUnitInfos;

        /// <summary>
        /// Configuration���������
        /// </summary>
        private IConfigurationObjectManager _ConfigurationObjectManager;

        #endregion               

        #region Properties

        /// <summary>
        /// ��ȡָ�����õ�Ԫ��������Ϣ
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
        /// ���õ�Ԫ��Ϣ����
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
        /// �ͻ���Ϣ
        /// </summary>
        public IClientInfo ClientInfo
        {
            get
            {
                return _ClientInfo;
            }
        }

        /// <summary>
        /// ���õ�Ԫ����
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
        /// Configuration���������
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
        /// ��ȡָ�������õ�Ԫ
        /// </summary>
        /// <param name="ConfigInfo">���õ�Ԫ��Ϣ</param>
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
        /// ��ȡָ�����õ�Ԫ
        /// </summary>
        /// <param name="configuration">���ö���</param>
        /// <param name="unitInfo">���õ�Ԫ��Ϣ</param>
        /// <returns>��ص�������Ϣ</returns>
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
        /// ��ȡָ�����õ�Ԫ���ļ�·��
        /// </summary>
        /// <param name="client">�ͻ���Ϣ</param>
        /// <param name="unitInfo">���õ�Ԫ��Ϣ</param>
        /// <returns>·��</returns>
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
                    throw new Exception("δ֪�������ļ���");
                    break;
            }

            return System.IO.Path.Combine(path, unitInfo.FileName);
        }

        /// <summary>
        /// ��ʼ��ConfigUnitInfos����
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
