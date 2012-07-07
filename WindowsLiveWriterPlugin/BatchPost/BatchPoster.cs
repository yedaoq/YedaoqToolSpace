using System;
using System.Collections.Generic;
using System.Text;
using WindowsLive;
using WindowsLive.Writer.Api;
using WindowsLive.Writer.Passport;
using WindowsLive.Writer.BlogClient;
using WindowsLive.Writer.BlogClient.Clients;
using WindowsLive.Writer.Extensibility.BlogClient;
using System.Xml;
using System.Diagnostics;

namespace WindowsLiveWriter.Post
{
    [WindowsLive.Writer.Api.WriterPlugin("BC6107C2-1B35-4db5-8D04-1A97C5AF1BD0", "BatchPost")]
    public class BatchPoster:WindowsLive.Writer.Api.PublishNotificationHook
    {
        public override bool OnPrePublish(System.Windows.Forms.IWin32Window dialogOwner, IProperties properties, IPublishingContext publishingContext, bool publish)
        {
            bool Result = false;        //用于告诉Live Writer是否要post到当前帐户
            if (Posting) return true;   //由于向Server执行Post操作（IBlogClient.NewPost）会递归调用此过程，所以此处需要屏蔽后续的进入尝试

            BlogPost postinfo = publishingContext.PostInfo as BlogPost;
            BlogPostCategory[] categories = null;// = postinfo.Categories.Clone() as BlogPostCategory[];

            //选择帐户
            DialogBlogSelect.InitCategorys((publishingContext.PostInfo as WindowsLive.Writer.Extensibility.BlogClient.BlogPost).Categories);
            if (DialogBlogSelect.ShowDialog() != System.Windows.Forms.DialogResult.OK)
            {
                Posting = false;
                return false;
            }

            //排除当前帐户——当前帐户让Live Writer去完成
            for (int i = 0; i < DialogBlogSelect.SelectedBlogs.Count; i++)
            {
                if (DialogBlogSelect.SelectedBlogs[i].ID.ToString() == publishingContext.AccountId)
                {
                    postinfo.Categories = DialogBlogSelect.SelectedBlogs[i].PostCatalogs.ToArray();
                    DialogBlogSelect.SelectedBlogs.RemoveAt(i);
                    Result = true;                    
                    break;
                }
            }

            //发送到选择的所有帐户——所有的Post操作共用一个PostInfo对象，需要先备份PostInfo中的目录列表
            if(Result)
            {
                categories = postinfo.Categories.Clone() as BlogPostCategory[];
            }

            foreach (AccountItem blog in DialogBlogSelect.SelectedBlogs)
            {
                string str;
                XmlDocument document;
                BlogSettings settings = BlogSettings.ForBlogId(blog.ID);
                IBlogClient client = BlogClientManager.CreateClient(settings);
                postinfo.Categories = blog.PostCatalogs.ToArray();
                string postId = client.NewPost(settings.HostBlogId, postinfo, NullNewCategoryContext.Instance, true, out str, out document);
                OpenPostedPage(settings.HomepageUrl, postinfo.Permalink);
            }

            //恢复目录列表
            if(Result)
            {
                postinfo.Categories = categories;
            }

            Posting = false;
            return Result;
        }

        public void OpenPostedPage(string home, string page)
        {
            if (!Uri.IsWellFormedUriString(page, UriKind.Absolute))
            {
                page = new Uri(new Uri(home), new Uri(page, UriKind.Relative)).ToString();
            }
            ProcessStartInfo startInfo = new ProcessStartInfo(page);
            startInfo.UseShellExecute = true;
            Process.Start(startInfo);
        }

        public override void Initialize(IProperties pluginOptions)
        {
            base.Initialize(pluginOptions);
        }

        public override void EditOptions(System.Windows.Forms.IWin32Window dialogOwner)
        {
            base.EditOptions(dialogOwner);
        }

        private bool Posting = false;
        private FrmAccountSelect DialogBlogSelect = new FrmAccountSelect();
    }

    public class NullNewCategoryContext : INewCategoryContext
    {
        // Fields
        public static NullNewCategoryContext Instance;

        // Methods
        static NullNewCategoryContext()
        {
            Instance = new NullNewCategoryContext();
        }
        public NullNewCategoryContext()
        {

        }

        public void NewCategoryAdded(BlogPostCategory category)
        {
        }
    }

 

}
