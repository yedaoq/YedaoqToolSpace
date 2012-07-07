using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using WindowsLive;
using WindowsLive.Writer.Api;
using WindowsLive.Writer.Passport;
using WindowsLive.Writer.BlogClient;
using WindowsLive.Writer.BlogClient.Clients;
using WindowsLive.Writer.Extensibility.BlogClient;

namespace WindowsLiveWriter.Post
{
    public partial class FrmAccountSelect : Form
    {
        /// <summary>
        /// 选择的帐户
        /// </summary>
        private System.Collections.Generic.List<AccountItem> _SelectedBlogs = new List<AccountItem>();

        private List<AccountItem> AccountItems = new List<AccountItem>();

        private BindingSource SourceAccounts = new BindingSource();

        private FrmCatalogSelect FrmCatalogSelect = new FrmCatalogSelect();

        public FrmAccountSelect()
        {
            InitializeComponent();

            this.dgvAccount.AutoGenerateColumns = false;
            ColSelect.FalseValue = 0;
            ColSelect.TrueValue = 1;

            InitAccounts();
        }

        public void InitAccounts()
        {
            try
            {
                /*System.Diagnostics.Debug.Assert(false, "Load");*/
                AccountItems.Clear();
                foreach (BlogDescriptor blog in WindowsLive.Writer.BlogClient.BlogSettings.GetBlogs(true))
                {
                    AccountItems.Add(new AccountItem(blog));
                }
            }
            catch (Exception ex) { MessageBox.Show(ex.Message); }
        }

        public void InitCategorys(IEnumerable<BlogPostCategory> categorys)
        {
            foreach (AccountItem item in AccountItems)
            {
                item.SetCategorysByNames(categorys);
            }
        }

        /// <summary>
        /// 选择的帐户
        /// </summary>
        public List<AccountItem> SelectedBlogs
        {
            get
            {
                return _SelectedBlogs;
            }
            set
            {
                _SelectedBlogs.Clear();
                _SelectedBlogs.AddRange(value);
            }
        }

        private void FrmAccountSelect_Load(object sender, EventArgs e)
        {
            SourceAccounts.DataSource = AccountItems;
            dgvAccount.DataSource = SourceAccounts;
        }

        private void buttonAll_Click(object sender, EventArgs e)
        {
            foreach (DataGridViewRow item in dgvAccount.Rows)
            {
                item.Cells[ColSelect.Index].Value = "1";
            }
        }

        private void BtnReverse_Click(object sender, EventArgs e)
        {
            foreach (DataGridViewRow item in dgvAccount.Rows)
            {
                item.Cells[ColSelect.Index].Value = ( item.Cells[ColSelect.Index].Value as string == "1") ? "0" : "1";
            }
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            SelectedBlogs.Clear();
            foreach (DataGridViewRow item in dgvAccount.Rows)
            {
                if ((int)item.Cells[ColSelect.Index].Value == 1)
                {
                    SelectedBlogs.Add(item.DataBoundItem as AccountItem);
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

        private void dgvAccount_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {
            try
            {
                if (e.ColumnIndex == ColCatalog.Index && e.RowIndex >= 0)
                {
                    System.Diagnostics.Debug.Assert(false, dgvAccount.Rows[e.RowIndex].DataBoundItem.ToString());
                    AccountItem item = dgvAccount.Rows[e.RowIndex].DataBoundItem as AccountItem;
                    if (this.FrmCatalogSelect.ShowDialog(item) == DialogResult.OK)
                    {
                        item.Tag = 1;
                        SourceAccounts.ResetCurrentItem();
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.StackTrace);
            }
        }
    }
}
