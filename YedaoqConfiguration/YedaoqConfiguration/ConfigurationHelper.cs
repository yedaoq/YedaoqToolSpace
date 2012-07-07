using System;
using System.Collections.Generic;
using System.Text;
using System.Configuration;
using CommonLibrary;
using System.Reflection;
using System.Reflection.Emit;
using System.IO;

namespace YedaoqConfiguration
{
    public static class ConfigurationHelper
    {
        /// <summary>
        /// 从配置对象中移除配置单元
        /// </summary>
        /// <param name="configuration">配置对象</param>
        /// <param name="configUnitName">配置单元名称</param>
        public static int RemoveConfigUnit(ConfigurationSectionGroup configGroup, string configUnitName)
        {
            try
            {
                foreach (string sectionGroup in configGroup.SectionGroups.Keys)
                {
                    if (configUnitName.Equals(sectionGroup))
                    {
                        configGroup.SectionGroups.Remove(configUnitName);
                        return 1;
                    }
                }
                foreach (string section in configGroup.Sections.Keys)
                {
                    if (configUnitName.Equals(section))
                    {
                        configGroup.Sections.Remove(configUnitName);
                        return 1;
                    }
                }
            }
            catch { }
            return -1;
        }

        /// <summary>
        /// 填充指定的配置组:若配置组中存在空的子配置组或子配置段,将其初始化
        /// </summary>
        /// <param name="group">配置组</param>
        /// <returns></returns>
        public static int FillConfigurationSectionGroup(ConfigurationSectionGroup group, bool fillRecursion)
        {
            if (object.ReferenceEquals(null, group)) return -1;
            IEnumerator<PropertyInfo> enumerator = GetDeclaredSubSectionGroups(group);
            object obj;

            while (enumerator.MoveNext())
            {
                obj = GetConfigurationSectionGroup(group, enumerator.Current.Name, enumerator.Current.PropertyType, true, true);
                if (!object.ReferenceEquals(null, obj) && fillRecursion)
                {
                    FillConfigurationSectionGroup(obj as ConfigurationSectionGroup, fillRecursion);
                }
            }

            enumerator = GetDeclaredSubSections(group);

            while (enumerator.MoveNext())
            {
                obj = GetConfigurationSection(group, enumerator.Current.Name, enumerator.Current.PropertyType, true, true);
            }

            return 1;
        }

        /// <summary>
        /// 获取配置组中定义的所有子配置组信息
        /// </summary>
        /// <param name="group">配置组</param>
        /// <returns>包含配置组中声明的所有ConfigurationSectionGroup属性信息的迭代器</returns>
        public static IEnumerator<PropertyInfo> GetDeclaredSubSectionGroups(ConfigurationSectionGroup group)
        {
            if (object.ReferenceEquals(null, group)) return null;

            return GetDeclaredSubSectionGroups(group.GetType());
        }

        /// <summary>
        /// 获取配置组中定义的所有子配置段信息
        /// </summary>
        /// <param name="group">配置组</param>
        /// <returns>包含配置组中声明的所有ConfigurationSection属性信息的迭代器</returns>
        public static IEnumerator<PropertyInfo> GetDeclaredSubSections(ConfigurationSectionGroup group)
        {
            if (object.ReferenceEquals(null, group)) return null;

            return GetDeclaredSubSections(group.GetType());
        }

        /// <summary>
        /// 获取指定类型中定义的所有子配置组信息
        /// </summary>
        /// <param name="group">配置组</param>
        /// <returns>包含配置组中声明的所有ConfigurationSectionGroup属性信息的迭代器</returns>
        public static IEnumerator<PropertyInfo> GetDeclaredSubSectionGroups(Type type)
        {
            if (object.ReferenceEquals(null, type)) return null;

            Type typeSectionGroup = typeof(ConfigurationSectionGroup);
            PropertyInfo[] properties = type.GetProperties(BindingFlags.Public | BindingFlags.Instance);

            return new CommonLibrary.Enumerators.FilterEnumerator<PropertyInfo>(properties, delegate(PropertyInfo property)
            {
                return property.PropertyType.IsSubclassOf(typeSectionGroup);
            });
        }

        /// <summary>
        /// 获取指定类型中定义的所有子配置段信息
        /// </summary>
        /// <param name="group">配置组</param>
        /// <returns>包含配置组中声明的所有ConfigurationSection属性信息的迭代器</returns>
        public static IEnumerator<PropertyInfo> GetDeclaredSubSections(Type type)
        {
            if (object.ReferenceEquals(null, type)) return null;

            Type typeSection = typeof(ConfigurationSection);
            PropertyInfo[] properties = type.GetProperties(BindingFlags.Public | BindingFlags.Instance);

            return new CommonLibrary.Enumerators.FilterEnumerator<PropertyInfo>(properties, delegate(PropertyInfo property)
            {
                return property.PropertyType.IsSubclassOf(typeSection);
            });
        }

        /// <summary>
        /// 从配置组集合中获取一个配置组
        /// </summary>
        /// <typeparam name="T">要获取的配置组类型</typeparam>
        /// <param name="groups">配置组集合</param>
        /// <param name="groupName">要获取的配置组名称</param>
        /// <param name="createWhenNonExist">不存在时创建</param>
        /// <param name="replaceWhenTypeConflict">获取到的配置组类型与指定类型不一致时是否覆盖</param>
        /// <returns>指定的配置组</returns>
        public static T GetConfigurationSectionGroup<T>(ConfigurationSectionGroup fatherConfigGroup, string groupName, bool createWhenNonExist, bool replaceWhenTypeConflict) where T : ConfigurationSectionGroup, new()
        {
            if (object.ReferenceEquals(null, fatherConfigGroup) || object.ReferenceEquals(null, groupName) || groupName.Length == 0) return null;

            ConfigurationSectionGroup group = null;
            bool typeConflict = false;

            try
            {
                group = fatherConfigGroup.SectionGroups[groupName];

                if (!object.ReferenceEquals(null, group))
                {
                    if (group is T)
                    {
                        return group as T;                        
                    }
                    else
                    {
                        typeConflict = true;
                        group = null;
                    }
                }
            }
            catch
            {
                typeConflict = true;
            }

            //根据参数创建新配置项
            if (createWhenNonExist || (typeConflict && replaceWhenTypeConflict))
            {
                RemoveConfigUnit(fatherConfigGroup, groupName);
                group = new T();
                fatherConfigGroup.SectionGroups.Add(groupName,group);
                FillConfigurationSectionGroup(group, true);
                return group as T;
            }

            return null;
        }

        /// <summary>
        /// 从配置组集合中获取一个配置组
        /// </summary>
        /// <typeparam name="T">要获取的配置组类型</typeparam>
        /// <param name="groups">配置组集合</param>
        /// <param name="groupName">要获取的配置组名称</param>
        /// <param name="createWhenNonExist">不存在时创建</param>
        /// <param name="replaceWhenTypeConflict">获取到的配置组类型与指定类型不一致时是否覆盖</param>
        /// <returns>指定的配置组</returns>
        public static T GetConfigurationSection<T>(ConfigurationSectionGroup fatherConfigGroup, string sectionName, bool createWhenNonExist, bool replaceWhenTypeConflict) where T : ConfigurationSection, new()
        {
            if (object.ReferenceEquals(null, fatherConfigGroup) || object.ReferenceEquals(null, sectionName) || sectionName.Length == 0) return null;

            ConfigurationSection section = null;
            bool typeConflict = false;

            try
            {
                section = fatherConfigGroup.Sections[sectionName];

                if (!object.ReferenceEquals(null, section))
                {
                    if (section is T)
                    {
                        return section as T;
                    }
                    else
                    {
                        typeConflict = true;
                        section = null;
                    }
                }
            }
            catch
            {
                typeConflict = true;
            }

            //根据参数创建新配置项
            if (createWhenNonExist || (typeConflict && replaceWhenTypeConflict))
            {
                RemoveConfigUnit(fatherConfigGroup, sectionName);
                section = new T();
                fatherConfigGroup.Sections.Add(sectionName, section);
                return section as T;
            }

            return null;
        }

        /// <summary>
        /// 从配置组集合中获取一个配置组
        /// </summary>
        /// <typeparam name="T">要获取的配置组类型</typeparam>
        /// <param name="groups">配置组集合</param>
        /// <param name="groupName">要获取的配置组名称</param>
        /// <param name="createWhenNonExist">不存在时创建</param>
        /// <param name="replaceWhenTypeConflict">获取到的配置组类型与指定类型不一致时是否覆盖</param>
        /// <returns>指定的配置组</returns>
        public static ConfigurationSectionGroup GetConfigurationSectionGroup(ConfigurationSectionGroup fatherConfigGroup, string groupName,Type groupType, bool createWhenNonExist, bool replaceWhenTypeConflict)
        {
            if (object.ReferenceEquals(null, fatherConfigGroup) || object.ReferenceEquals(null,groupType) || object.ReferenceEquals(null, groupName) || groupName.Length == 0) return null;

            ConfigurationSectionGroup group = null;
            bool typeConflict = false;

            try
            {
                group = fatherConfigGroup.SectionGroups[groupName];

                if (!object.ReferenceEquals(null, group))
                {
                    if (group.GetType().Equals(groupType))
                    {
                        return group;
                    }
                    else
                    {
                        typeConflict = true;
                        group = null;
                    }
                }
            }
            catch
            {
                typeConflict = true;
            }

            //根据参数创建新配置项
            if (createWhenNonExist || (typeConflict && replaceWhenTypeConflict))
            {
                RemoveConfigUnit(fatherConfigGroup, groupName);
                group = CreateInstance(groupType) as ConfigurationSectionGroup;
                if (!object.ReferenceEquals(null, group))
                {
                    fatherConfigGroup.SectionGroups.Add(groupName, group);
                    FillConfigurationSectionGroup(group, true);
                }
            }

            return group;
        }

        /// <summary>
        /// 从配置组集合中获取一个配置组
        /// </summary>
        /// <typeparam name="T">要获取的配置组类型</typeparam>
        /// <param name="groups">配置组集合</param>
        /// <param name="groupName">要获取的配置组名称</param>
        /// <param name="createWhenNonExist">不存在时创建</param>
        /// <param name="replaceWhenTypeConflict">获取到的配置组类型与指定类型不一致时是否覆盖</param>
        /// <returns>指定的配置组</returns>
        public static ConfigurationSection GetConfigurationSection(ConfigurationSectionGroup fatherConfigGroup, string sectionName,Type sectionType, bool createWhenNonExist, bool replaceWhenTypeConflict)
        {
            if (object.ReferenceEquals(null, fatherConfigGroup) || object.ReferenceEquals(null, sectionType) || object.ReferenceEquals(null, sectionName) || sectionName.Length == 0) return null;

            ConfigurationSection section = null;
            bool typeConflict = false;

            try
            {
                section = fatherConfigGroup.Sections[sectionName];

                if (!object.ReferenceEquals(null, section))
                {
                    if (section.GetType().Equals(sectionType))
                    {
                        return section;
                    }
                    else
                    {
                        typeConflict = true;
                        section = null;
                    }
                }
            }
            catch
            {
                typeConflict = true;
            }

            //根据参数创建新配置项
            if (createWhenNonExist || (typeConflict && replaceWhenTypeConflict))
            {
                RemoveConfigUnit(fatherConfigGroup, sectionName);
                section = CreateInstance(sectionType) as ConfigurationSection;
                if (!object.ReferenceEquals(null, section))
                {
                    fatherConfigGroup.Sections.Add(sectionName, section);
                }
            }

            return section;
        }

        /// <summary>
        /// 从指定目录中的程序集中获取一个IAppConfigurationProvider实例
        /// </summary>
        /// <param name="directoryPath">目标目录</param>
        /// <returns>实例</returns>
        public static IAppConfigurationProvider SearchForAppConfigurationProvider(string directoryPath)
        {
            if (object.ReferenceEquals(null, directoryPath) || directoryPath.Length == 0 || !Directory.Exists(directoryPath)) return null;

            Assembly assembly = null;
            IAppConfigurationProvider result = null;

            foreach (string file in Directory.GetFiles(directoryPath, "*.dll"))
            {
                try
                {
                    assembly = Assembly.LoadFile(file);
                }
                catch { }

                if (object.ReferenceEquals(null, assembly)) continue;

                result = SearchForAppConfigurationProvider(assembly);
                if (!object.ReferenceEquals(null, result))
                {
                    return result;
                }
            }

            return null;
        }

        /// <summary>
        /// 从指定程序集中获取一个IAppConfigurationProvider实例
        /// </summary>
        /// <param name="assembly">程序集</param>
        /// <returns>实例</returns>
        public static IAppConfigurationProvider SearchForAppConfigurationProvider(Assembly assembly)
        {
            if (object.ReferenceEquals(assembly, null)) return null;

            IAppConfigurationProvider result = null;

            Type typeAppConfigurationProvider = typeof(AppConfigurationProviderAttribute);

            foreach (Type type in assembly.GetTypes())
            {
                if (!type.IsClass || type.IsAbstract) continue;
                if (object.ReferenceEquals(null, Attribute.GetCustomAttribute(type, typeAppConfigurationProvider))) continue;
                if (!typeof(IAppConfigurationProvider).IsAssignableFrom(type)) continue;

                try
                {
                    ConstructorInfo m = type.GetConstructor(new Type[] { });
                    result = m.Invoke( null) as IAppConfigurationProvider;
                    if (!object.ReferenceEquals(null, result)) result.Initialize();
                }
                catch(Exception ex)
                {
                    throw new Exception(string.Format("找到了应用程序配置提供类《{0}》，但未能成功创建其对象！\r\n请确保类型有默认构造函数并且其Initialize成员无误。", type));
                }

                return result;
            }

            return null;
        }

        public static object CreateInstance(Type type)
        {
            if (object.ReferenceEquals(null, type)) return null;

            ConstructorInfo constructor = type.GetConstructor(new Type[] { });

            if (object.ReferenceEquals(null, constructor)) return null;

            return constructor.Invoke(null);
        }
    }
}
