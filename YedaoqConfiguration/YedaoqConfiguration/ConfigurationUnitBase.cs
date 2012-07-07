using System;
using System.Collections.Generic;
using System.Text;

namespace YedaoqConfiguration
{
    public abstract class ConfigurationUnitBase : IConfigurationUnitInfo
    {
        public ConfigurationUnitBase(string configName, Type unitType, EnumConfigDomain domain, string fileName, bool autoInitFlag)
        {
            this.ConfigName = configName;
            this.UnitType = unitType;
            this.Domain = domain;
            this.FileName = fileName;
            this.AutoInitWhenNonExist = autoInitFlag;
        }

        #region Fields

        private bool _AutoInitWhenNonExist;
        private EnumConfigDomain _Domain;
        private string _FileName;
        private string _ConfigName;
        private Type _UnitType;
        private object _Value;
        private bool _InitFlag;

        #endregion

        #region  Properties

        public bool AutoInitWhenNonExist
        {
            get
            {
                return _AutoInitWhenNonExist;
            }
            set
            {
                _AutoInitWhenNonExist = value;
            }
        }

        public EnumConfigDomain Domain
        {
            get
            {
                return _Domain;
            }
            set
            {
                _Domain = value;
            }
        }

        public string FileName
        {
            get
            {
                return _FileName;
            }
            set
            {
                _FileName = value;
            }
        }

        public string ConfigName
        {
            get
            {
                return _ConfigName;
            }
            set
            {
                _ConfigName = value;
            }
        }

        public Type UnitType
        {
            get
            {
                return _UnitType;
            }
            set
            {
                _UnitType = value;
            }
        }

        public object Value
        {
            get
            {
                return _Value;
            }
            set
            {
                _Value = value;
            }
        }

        public bool InitFlag
        {
            get { return _InitFlag; }
            set { _InitFlag = value; }
        }

        #endregion
    }
}
