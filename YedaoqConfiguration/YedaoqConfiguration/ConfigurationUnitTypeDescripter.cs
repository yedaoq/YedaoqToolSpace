using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using CommonLibrary;
using System.Reflection;

namespace YedaoqConfiguration
{
    public class ConfigurationUnitTypeDescripter : ICustomTypeDescriptor
    {
        private Singleton<ConfigurationUnitTypeDescripter> Singleton;

        public ConfigurationUnitTypeDescripter Instance
        {
            get
            {
                if (object.ReferenceEquals(null, Singleton)) Singleton = new Singleton<ConfigurationUnitTypeDescripter>();
                return Singleton.Instance;
            }
        }

        /// <summary>
        /// 配置单元
        /// </summary>
        private object _ConfigUnit = new object();

        /// <summary>
        /// 配置单元
        /// </summary>
        public object ConfigUnit
        {
            get
            {
                return _ConfigUnit;
            }
            set
            {
                _ConfigUnit = value;
            }
        }

        #region ICustomTypeDescriptor 成员

        AttributeCollection ICustomTypeDescriptor.GetAttributes()
        {
            return TypeDescriptor.GetAttributes(ConfigUnit, true);
        }

        string ICustomTypeDescriptor.GetClassName()
        {
            return TypeDescriptor.GetClassName(ConfigUnit, true);
        }

        string ICustomTypeDescriptor.GetComponentName()
        {
            return TypeDescriptor.GetComponentName(ConfigUnit, true);
        }

        TypeConverter ICustomTypeDescriptor.GetConverter()
        {
            return TypeDescriptor.GetConverter(ConfigUnit, true);
        }

        EventDescriptor ICustomTypeDescriptor.GetDefaultEvent()
        {
            return TypeDescriptor.GetDefaultEvent(ConfigUnit, true);
        }

        PropertyDescriptor ICustomTypeDescriptor.GetDefaultProperty()
        {
            return TypeDescriptor.GetDefaultProperty(ConfigUnit, true);
        }

        object ICustomTypeDescriptor.GetEditor(Type editorBaseType)
        {
            return TypeDescriptor.GetEditor(ConfigUnit, editorBaseType, true);
        }

        EventDescriptorCollection ICustomTypeDescriptor.GetEvents(Attribute[] attributes)
        {
            return TypeDescriptor.GetEvents(ConfigUnit, attributes, true);
        }

        EventDescriptorCollection ICustomTypeDescriptor.GetEvents()
        {
            return TypeDescriptor.GetEvents(ConfigUnit, true);
        }

        PropertyDescriptorCollection ICustomTypeDescriptor.GetProperties(Attribute[] attributes)
        {
            PropertyDescriptorCollection properties = TypeDescriptor.GetProperties(ConfigUnit, attributes, true);

            return Filter(properties);
        }

        PropertyDescriptorCollection ICustomTypeDescriptor.GetProperties()
        {
            PropertyDescriptorCollection properties = TypeDescriptor.GetProperties(ConfigUnit, true);

            return Filter(properties);
        }

        object ICustomTypeDescriptor.GetPropertyOwner(PropertyDescriptor pd)
        {
            return this;
        }

        #endregion

        protected PropertyDescriptorCollection Filter(PropertyDescriptorCollection properties)
        {
            PropertyInfo[] DeclaredProperties = ConfigUnit.GetType().GetProperties(BindingFlags.DeclaredOnly | BindingFlags.Public | BindingFlags.Instance);
            PropertyDescriptorCollection Result = new PropertyDescriptorCollection(null, false);

            bool Existed;
            for (int i = properties.Count - 1; i >= 0; --i)
            {
                Existed = false;
                foreach (PropertyInfo p in DeclaredProperties)
                {
                    if (properties[i].Name == p.Name)
                    {
                        Existed = true;
                        break;
                    }
                }
                if (Existed)
                {
                    Result.Add(properties[i]);
                }
            }

            return Result;
        }
    }
}
