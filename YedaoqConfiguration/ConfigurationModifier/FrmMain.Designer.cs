namespace ConfigurationModifier
{
    partial class FrmMain
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.cbxClients = new System.Windows.Forms.ComboBox();
            this.treeConfig = new System.Windows.Forms.TreeView();
            this.gridConfigItem = new System.Windows.Forms.PropertyGrid();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.cbxClients);
            this.splitContainer1.Panel1.Controls.Add(this.treeConfig);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.gridConfigItem);
            this.splitContainer1.Size = new System.Drawing.Size(523, 376);
            this.splitContainer1.SplitterDistance = 174;
            this.splitContainer1.TabIndex = 0;
            // 
            // cbxClients
            // 
            this.cbxClients.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cbxClients.DisplayMember = "Name";
            this.cbxClients.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbxClients.FormattingEnabled = true;
            this.cbxClients.Location = new System.Drawing.Point(3, 4);
            this.cbxClients.Name = "cbxClients";
            this.cbxClients.Size = new System.Drawing.Size(168, 20);
            this.cbxClients.TabIndex = 1;
            // 
            // treeConfig
            // 
            this.treeConfig.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.treeConfig.Location = new System.Drawing.Point(3, 26);
            this.treeConfig.Name = "treeConfig";
            this.treeConfig.ShowNodeToolTips = true;
            this.treeConfig.Size = new System.Drawing.Size(168, 347);
            this.treeConfig.TabIndex = 0;
            this.treeConfig.BeforeExpand += new System.Windows.Forms.TreeViewCancelEventHandler(this.treeConfig_BeforeExpand);
            this.treeConfig.BeforeSelect += new System.Windows.Forms.TreeViewCancelEventHandler(this.treeConfig_BeforeSelect);
            // 
            // gridConfigItem
            // 
            this.gridConfigItem.Dock = System.Windows.Forms.DockStyle.Fill;
            this.gridConfigItem.Location = new System.Drawing.Point(0, 0);
            this.gridConfigItem.Name = "gridConfigItem";
            this.gridConfigItem.Size = new System.Drawing.Size(345, 376);
            this.gridConfigItem.TabIndex = 0;
            this.gridConfigItem.SelectedObjectsChanged += new System.EventHandler(this.gridConfigItem_SelectedObjectsChanged);
            // 
            // FrmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(523, 376);
            this.Controls.Add(this.splitContainer1);
            this.Name = "FrmMain";
            this.Text = "ÅäÖÃÎÄ¼þÐÞ¸ÄÆ÷";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FrmMain_FormClosing);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.TreeView treeConfig;
        private System.Windows.Forms.PropertyGrid gridConfigItem;
        private System.Windows.Forms.ComboBox cbxClients;
    }
}

