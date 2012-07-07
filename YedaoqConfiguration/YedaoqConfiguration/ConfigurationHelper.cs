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
        /// �����ö������Ƴ����õ�Ԫ
        /// </summary>
        /// <param name="configuration">���ö���</param>
        /// <param name="configUnitName">���õ�Ԫ����</param>
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
        /// ���ָ����������:���������д��ڿյ���������������ö�,�����ʼ��
        /// </summary>
        /// <param name="group">������</param>
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
        /// ��ȡ�������ж������������������Ϣ
        /// </summary>
        /// <param name="group">������</param>
        /// <returns>����������������������ConfigurationSectionGroup������Ϣ�ĵ�����</returns>
        public static IEnumerator<PropertyInfo> GetDeclaredSubSectionGroups(ConfigurationSectionGroup group)
        {
            if (object.ReferenceEquals(null, group)) return null;

            return GetDeclaredSubSectionGroups(group.GetType());
        }

        /// <summary>
        /// ��ȡ�������ж�������������ö���Ϣ
        /// </summary>
        /// <param name="group">������</param>
        /// <returns>����������������������ConfigurationSection������Ϣ�ĵ�����</returns>
        public static IEnumerator<PropertyInfo> GetDeclaredSubSections(ConfigurationSectionGroup group)
        {
            if (object.ReferenceEquals(null, group)) return null;

            return GetDeclaredSubSections(group.GetType());
        }

        /// <summary>
        /// ��ȡָ�������ж������������������Ϣ
        /// </summary>
        /// <param name="group">������</param>
        /// <returns>����������������������ConfigurationSectionGroup������Ϣ�ĵ�����</returns>
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
        /// ��ȡָ�������ж�������������ö���Ϣ
        /// </summary>
        /// <param name="group">������</param>
        /// <returns>����������������������ConfigurationSection������Ϣ�ĵ�����</returns>
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
        /// �������鼯���л�ȡһ��������
        /// </summary>
        /// <typeparam name="T">Ҫ��ȡ������������</typeparam>
        /// <param name="groups">�����鼯��</param>
        /// <param name="groupName">Ҫ��ȡ������������</param>
        /// <param name="createWhenNonExist">������ʱ����</param>
        /// <param name="replaceWhenTypeConflict">��ȡ����������������ָ�����Ͳ�һ��ʱ�Ƿ񸲸�</param>
        /// <returns>ָ����������</returns>
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

            //���ݲ���������������
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
        /// �������鼯���л�ȡһ��������
        /// </summary>
        /// <typeparam name="T">Ҫ��ȡ������������</typeparam>
        /// <param name="groups">�����鼯��</param>
        /// <param name="groupName">Ҫ��ȡ������������</param>
        /// <param name="createWhenNonExist">������ʱ����</param>
        /// <param name="replaceWhenTypeConflict">��ȡ����������������ָ�����Ͳ�һ��ʱ�Ƿ񸲸�</param>
        /// <returns>ָ����������</returns>
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

            //���ݲ���������������
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
        /// �������鼯���л�ȡһ��������
        /// </summary>
        /// <typeparam name="T">Ҫ��ȡ������������</typeparam>
        /// <param name="groups">�����鼯��</param>
        /// <param name="groupName">Ҫ��ȡ������������</param>
        /// <param name="createWhenNonExist">������ʱ����</param>
        /// <param name="replaceWhenTypeConflict">��ȡ����������������ָ�����Ͳ�һ��ʱ�Ƿ񸲸�</param>
        /// <returns>ָ����������</returns>
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

            //���ݲ���������������
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
        /// �������鼯���л�ȡһ��������
        /// </summary>
        /// <typeparam name="T">Ҫ��ȡ������������</typeparam>
        /// <param name="groups">�����鼯��</param>
        /// <param name="groupName">Ҫ��ȡ������������</param>
        /// <param name="createWhenNonExist">������ʱ����</param>
        /// <param name="replaceWhenTypeConflict">��ȡ����������������ָ�����Ͳ�һ��ʱ�Ƿ񸲸�</param>
        /// <returns>ָ����������</returns>
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

            //���ݲ���������������
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
        /// ��ָ��Ŀ¼�еĳ����л�ȡһ��IAppConfigurationProviderʵ��
        /// </summary>
        /// <param name="directoryPath">Ŀ��Ŀ¼</param>
        /// <returns>ʵ��</returns>
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
        /// ��ָ�������л�ȡһ��IAppConfigurationProviderʵ��
        /// </summary>
        /// <param name="assembly">����</param>
        /// <returns>ʵ��</returns>
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
                    throw new Exception(string.Format("�ҵ���Ӧ�ó��������ṩ�ࡶ{0}������δ�ܳɹ����������\r\n��ȷ��������Ĭ�Ϲ��캯��������Initialize��Ա����", type));
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
