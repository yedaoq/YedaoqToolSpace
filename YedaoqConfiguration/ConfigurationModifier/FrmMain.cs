using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Configuration;
using YedaoqConfiguration;
using System.IO;
using CommonLibrary;
using System.Reflection;

namespace ConfigurationModifier
{
    public partial class FrmMain : Form
    {
        public FrmMain()
        {
            InitializeComponent();

            BindSourceClients = new BindingSource();
            BindSourceClients.PositionChanged += new EventHandler(BindSourceClients_PositionChanged);
            cbxClients.DataSource = BindSourceClients;

            Init();
        }

        #region Fields

        /// <summary>
        /// 客户列表的绑定源
        /// </summary>
        private BindingSource BindSourceClients;

        /// <summary>
        /// 应用程序配置集的提供者
        /// </summary>
        private IAppConfigurationProvider AppConfigurationProvider;

        /// <summary>
        /// 当前应用程序配置集
        /// </summary>
        private IAppConfiguration CurrentAppConfiguration;

        /// <summary>
        /// 用于在PropertyGrid中显示配置组或配置段
        /// </summary>
        private ConfigurationUnitTypeDescripter Descripter = new ConfigurationUnitTypeDescripter();

        #endregion

        /// <summary>
        /// 初始化
        /// </summary>
        protected int Init()
        {
            try
            {
                AppConfigurationProvider = ConfigurationHelper.SearchForAppConfigurationProvider(Application.StartupPath);
            }
            catch(Exception ex)
            {
                MessageBox.Show(ex.Message);
                return -1;
            }
            if (object.ReferenceEquals(null, AppConfigurationProvider))
            {
                MessageBox.Show("在当前程序目录中找不到程序配置提供者！");
                return -1;
            }

            cbxClients.SelectedIndex = -1;
            BindSourceClients.DataSource = AppConfigurationProvider.Clients;            
            cbxClients.SelectedItem = AppConfigurationProvider.CurrentClient;

            return 1;
        }

        /// <summary>
        /// 加载应用程序配置集
        /// </summary>
        /// <param name="appConfiguration">应用程序配置集</param>
        /// <returns>结果/returns>
        protected int LoadAppConfiguration(IAppConfiguration appConfiguration)
        {
            if (object.ReferenceEquals(null, appConfiguration)) return -1;

            object obj = null;
            TreeNode node = null;
            IEnumerator<PropertyInfo> enumerator = null;

            //调整：使配置单元能够自动创建
            foreach (IConfigurationUnitInfo unit in appConfiguration.ConfigUnitInfos)
            {
                unit.AutoInitWhenNonExist = true;
            }

            //处理定义的所有配置组单元
            enumerator = ConfigurationHelper.GetDeclaredSubSectionGroups(appConfiguration.GetType());
            while (enumerator.MoveNext())
            {
                obj = enumerator.Current.GetValue(appConfiguration, null);
                node = NewConfigurationSectionGroupNode(enumerator.Current.Name, obj);
                node.ToolTipText = GetDescription(enumerator.Current);
                treeConfig.Nodes.Add(node);                
            }

            //处理定义的所有配置段单元
            enumerator = ConfigurationHelper.GetDeclaredSubSections(appConfiguration.GetType());
            while (enumerator.MoveNext())
            {
                obj = enumerator.Current.GetValue(appConfiguration, null);
                node = NewConfigurationSectionNode(enumerator.Current.Name, obj);
                node.ToolTipText = GetDescription(enumerator.Current);
                treeConfig.Nodes.Add(node);
            }

            return 1;
        }

        /// <summary>
        /// 创建一个配置组节点
        /// </summary>
        /// <param name="obj">配置组信息</param>
        /// <returns></returns>
        protected TreeNode NewConfigurationSectionGroupNode(string name, object obj)
        {
            ExTreeNode node = new ExTreeNode(0, 0, name, null, obj, false);
            node.Nodes.Add("获取配置组信息失败！");
            return node;
        }

        /// <summary>
        /// 创建一个配置段节点
        /// </summary>
        /// <param name="obj">配置段信息</param>
        /// <returns></returns>
        protected TreeNode NewConfigurationSectionNode(string name, object obj)
        {
            TreeNode node = new TreeNode(name);
            node.Tag = obj;
            return node;
        }

        /// <summary>
        /// 获取类型的描述信息
        /// </summary>
        /// <param name="type"></param>
        /// <returns></returns>
        protected string GetDescription(PropertyInfo property)
        {
            Attribute a = Attribute.GetCustomAttribute(property, typeof(DescriptionAttribute));

            if (object.ReferenceEquals(null, a)) return string.Empty;
            return (a as DescriptionAttribute).Description;
        }

        void BindSourceClients_PositionChanged(object sender, EventArgs e)
        {
            System.Diagnostics.Debug.WriteLine("Client Selected Changed");

            if (object.ReferenceEquals(null, AppConfigurationProvider)) return;

            if(!object.ReferenceEquals(null,CurrentAppConfiguration))
            {
                CurrentAppConfiguration.Dispose();
                treeConfig.Nodes.Clear();
                gridConfigItem.SelectedObject = null;
            }

            AppConfigurationProvider.CurrentClient = cbxClients.SelectedItem as IClientInfo ;

            CurrentAppConfiguration = AppConfigurationProvider.CurrentClientConfiguration;

            if (!object.ReferenceEquals(null, CurrentAppConfiguration))
            {
                LoadAppConfiguration(CurrentAppConfiguration);
            }
        }

        private void FrmMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (!object.ReferenceEquals(null, CurrentAppConfiguration))
            {
                CurrentAppConfiguration.Dispose();
            }
        }

        private void treeConfig_BeforeExpand(object sender, TreeViewCancelEventArgs e)
        {
            if (!(e.Node is ExTreeNode)) return;

            ExTreeNode eNode = e.Node as ExTreeNode;

            if (eNode.ChildInitFlag) return;

            eNode.ChildInitFlag = true;

            if (object.ReferenceEquals(null, eNode.Tag) || !(eNode.Tag is ConfigurationSectionGroup)) return;

            ConfigurationSectionGroup group = eNode.Tag as ConfigurationSectionGroup;
            object obj = null;
            IEnumerator<PropertyInfo> enumerator = null;
            TreeNode node = null;

            eNode.Nodes.Clear();

            //处理定义的所有配置组单元
            enumerator = ConfigurationHelper.GetDeclaredSubSectionGroups(group.GetType());
            while (enumerator.MoveNext())
            {
                obj = ConfigurationHelper.GetConfigurationSectionGroup(group, enumerator.Current.Name, enumerator.Current.PropertyType, true, true);
                node = NewConfigurationSectionGroupNode(enumerator.Current.Name, obj);
                node.ToolTipText = GetDescription(enumerator.Current);
                e.Node.Nodes.Add(eNode);
            }

            //处理定义的所有配置段单元
            enumerator = ConfigurationHelper.GetDeclaredSubSections(group.GetType());
            while (enumerator.MoveNext())
            {
                obj = ConfigurationHelper.GetConfigurationSection(group, enumerator.Current.Name, enumerator.Current.PropertyType, true, true);
                node = NewConfigurationSectionNode(enumerator.Current.Name, obj);
                node.ToolTipText = GetDescription(enumerator.Current);
                e.Node.Nodes.Add(node);
            }
        }

        private void treeConfig_BeforeSelect(object sender, TreeViewCancelEventArgs e)
        {
            if (e.Node is ExTreeNode) return;

            gridConfigItem.SelectedObject = e.Node.Tag;
        }

        private void gridConfigItem_SelectedObjectsChanged(object sender, EventArgs e)
        {
            
        }
    }
}