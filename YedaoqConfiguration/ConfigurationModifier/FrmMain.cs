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
        /// �ͻ��б�İ�Դ
        /// </summary>
        private BindingSource BindSourceClients;

        /// <summary>
        /// Ӧ�ó������ü����ṩ��
        /// </summary>
        private IAppConfigurationProvider AppConfigurationProvider;

        /// <summary>
        /// ��ǰӦ�ó������ü�
        /// </summary>
        private IAppConfiguration CurrentAppConfiguration;

        /// <summary>
        /// ������PropertyGrid����ʾ����������ö�
        /// </summary>
        private ConfigurationUnitTypeDescripter Descripter = new ConfigurationUnitTypeDescripter();

        #endregion

        /// <summary>
        /// ��ʼ��
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
                MessageBox.Show("�ڵ�ǰ����Ŀ¼���Ҳ������������ṩ�ߣ�");
                return -1;
            }

            cbxClients.SelectedIndex = -1;
            BindSourceClients.DataSource = AppConfigurationProvider.Clients;            
            cbxClients.SelectedItem = AppConfigurationProvider.CurrentClient;

            return 1;
        }

        /// <summary>
        /// ����Ӧ�ó������ü�
        /// </summary>
        /// <param name="appConfiguration">Ӧ�ó������ü�</param>
        /// <returns>���/returns>
        protected int LoadAppConfiguration(IAppConfiguration appConfiguration)
        {
            if (object.ReferenceEquals(null, appConfiguration)) return -1;

            object obj = null;
            TreeNode node = null;
            IEnumerator<PropertyInfo> enumerator = null;

            //������ʹ���õ�Ԫ�ܹ��Զ�����
            foreach (IConfigurationUnitInfo unit in appConfiguration.ConfigUnitInfos)
            {
                unit.AutoInitWhenNonExist = true;
            }

            //����������������鵥Ԫ
            enumerator = ConfigurationHelper.GetDeclaredSubSectionGroups(appConfiguration.GetType());
            while (enumerator.MoveNext())
            {
                obj = enumerator.Current.GetValue(appConfiguration, null);
                node = NewConfigurationSectionGroupNode(enumerator.Current.Name, obj);
                node.ToolTipText = GetDescription(enumerator.Current);
                treeConfig.Nodes.Add(node);                
            }

            //��������������öε�Ԫ
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
        /// ����һ��������ڵ�
        /// </summary>
        /// <param name="obj">��������Ϣ</param>
        /// <returns></returns>
        protected TreeNode NewConfigurationSectionGroupNode(string name, object obj)
        {
            ExTreeNode node = new ExTreeNode(0, 0, name, null, obj, false);
            node.Nodes.Add("��ȡ��������Ϣʧ�ܣ�");
            return node;
        }

        /// <summary>
        /// ����һ�����öνڵ�
        /// </summary>
        /// <param name="obj">���ö���Ϣ</param>
        /// <returns></returns>
        protected TreeNode NewConfigurationSectionNode(string name, object obj)
        {
            TreeNode node = new TreeNode(name);
            node.Tag = obj;
            return node;
        }

        /// <summary>
        /// ��ȡ���͵�������Ϣ
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

            //����������������鵥Ԫ
            enumerator = ConfigurationHelper.GetDeclaredSubSectionGroups(group.GetType());
            while (enumerator.MoveNext())
            {
                obj = ConfigurationHelper.GetConfigurationSectionGroup(group, enumerator.Current.Name, enumerator.Current.PropertyType, true, true);
                node = NewConfigurationSectionGroupNode(enumerator.Current.Name, obj);
                node.ToolTipText = GetDescription(enumerator.Current);
                e.Node.Nodes.Add(eNode);
            }

            //��������������öε�Ԫ
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