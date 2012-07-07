using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.Configuration;
using System.IO;

namespace YedaoqConfiguration
{
    public class ConfigurationObjectManager : IConfigurationObjectManager,IDisposable
    {
        public ConfigurationObjectManager()
        {
            TableConfigurationObject = new Hashtable();
        }

        #region Fields

        /// <summary>
        /// 存储Configuration对象的哈希表
        /// </summary>
        private Hashtable TableConfigurationObject;

        #endregion

        #region Methods

        public System.Configuration.Configuration GetConfiguration(string file, bool createWhenNonExist)
        {
            object Config = TableConfigurationObject[file];

            if (Config != null && Config is Configuration) return Config as Configuration;

            if (!TableConfigurationObject.ContainsKey(file))
            {
                Config = OpenConfigurationFile(file, createWhenNonExist);
                TableConfigurationObject.Add(file, Config);
            }
            else if (createWhenNonExist)
            {
                Config = OpenConfigurationFile(file, createWhenNonExist);
                TableConfigurationObject[file] = Config;
            }

            return Config as Configuration;
        }

        public int Save()
        {
            foreach (Configuration config in TableConfigurationObject.Values)
            {
                if (!object.ReferenceEquals(null, config))
                {
                    try
                    {
                        config.Save();
                    }
                    catch { }
                }
            }

            return 1;
        }

        public int Save(string file)
        {
            object Config = TableConfigurationObject[file];
            if (object.ReferenceEquals(null, Config) || !(Config is Configuration)) return -1;
            try
            {
                (Config as Configuration).Save();
            }
            catch
            {
                return -1;
            }
            return 1;
        }

        public void Dispose()
        {
            Save();
            TableConfigurationObject.Clear();
        }

        private int CreateConfigurationFile(string file)
        {
            try
            {
                File.Create(file);
            }
            catch
            {
                return -1;
            }

            return 1;
        }

        private int DropConfigurationFile(string file)
        {
            try
            {
                File.Delete(file);
            }
            catch
            {
                return -1;
            }

            return 1;
        }

        private Configuration OpenConfigurationFile(string file, bool createWhenNonExist)
        {
            Configuration Config = null;

            try
            {
                Config = ConfigurationManager.OpenExeConfiguration(file);
                return Config;
            }
            catch { }

            if (!File.Exists(file))
            {
                if (createWhenNonExist)
                {
                    if (CreateConfigurationFile(file) != 1) return Config;
                    return OpenConfigurationFile(file, false);
                }
                return Config;
            }

            string configFile = file + ".config";
            if (File.Exists(configFile))
            {
                if (DropConfigurationFile(configFile) != 1) return Config;
                return OpenConfigurationFile(file, false);
            }

            return Config;
        }

        #endregion
    }
}
