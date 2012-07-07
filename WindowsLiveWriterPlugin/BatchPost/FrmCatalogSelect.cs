using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using WindowsLive.Writer.Extensibility.BlogClient;

namespace WindowsLiveWriter.Post
{
    public partial class FrmCatalogSelect : Form
    {
        private AccountItem _AccountItem;
        private WindowsLive.Writer.BlogClient.Blog Blog;
        bool InitFlag = false;
    
        public FrmCatalogSelect()
        {
            InitializeComponent();
        }

        public AccountItem AccountItem
        {
            get
            {
                return _AccountItem;
            }
            set
            {
                if (_AccountItem != null && _AccountItem.ID == value.ID) return;

                _AccountItem = value;
                Blog = new WindowsLive.Writer.BlogClient.Blog(value.ID.ToString());
                InitFlag = false;
            }
        }

        public DialogResult ShowDialog(AccountItem account)
        {
            this.AccountItem = account;

            if (!InitFlag)
            {
                InitCatalogs();
            }

            InitCheckState();

            return this.ShowDialog();
        }

        private void btnCatalog_Click(object sender, EventArgs e)
        {
            Blog.RefreshCategories();

            InitCheckState();
            InitCheckState();
        }

        public void InitCatalogs()
        {
            this.lsvCatalog.Items.Clear();

            foreach (BlogPostCategory catalog in Blog.Categories)
            {
                ListViewItem item = new ListViewItem(catalog.Name);
                item.Tag = catalog;
                this.lsvCatalog.Items.Add(item);

                System.Diagnostics.Debug.Assert(false, catalog.Name);
            }

            InitFlag = true;
        }

        public void InitCheckState()
        {
            foreach (ListViewItem item in this.lsvCatalog.Items)
            {
                item.Checked = false;
            }

            foreach (BlogPostCategory catalog in AccountItem.PostCatalogs)
            {
                ListViewItem item = this.lsvCatalog.FindItemWithText(catalog.Name);
                if (item != null)
                {
                    item.Checked = true;
                }
            }
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            AccountItem.PostCatalogs.Clear();
            foreach (ListViewItem  item in this.lsvCatalog.Items)
            {
                if (item.Checked)
                {
                    AccountItem.PostCatalogs.Add(item.Tag  as BlogPostCategory);
                }
            }

            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }
    }
}
