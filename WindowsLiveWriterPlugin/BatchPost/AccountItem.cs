using System;
using System.Collections.Generic;
using System.Text;
using WindowsLive;
using System.Collections;
using WindowsLive.Writer.BlogClient;
using WindowsLive.Writer.BlogClient.Clients;
using WindowsLive.Writer.Extensibility.BlogClient;

namespace WindowsLiveWriter.Post
{
    public class AccountItem
    {
        public AccountItem(WindowsLive.Writer.BlogClient.BlogDescriptor blog)
        {
            _Description = blog;
        }

        private WindowsLive.Writer.BlogClient.BlogDescriptor _Description;
        private List<WindowsLive.Writer.Extensibility.BlogClient.BlogPostCategory> _PostCatalogs = new List<WindowsLive.Writer.Extensibility.BlogClient.BlogPostCategory>();
        private int _Tag;

        public string Name
        {
            get
            {
                return _Description.Name;
            }
        }

        public string ID
        {
            get
            {
                return _Description.Id;
            }
        }

        public List<WindowsLive.Writer.Extensibility.BlogClient.BlogPostCategory> PostCatalogs
        {
            get
            {
                return _PostCatalogs;
            }
            set
            {
                _PostCatalogs.Clear();
                _PostCatalogs.AddRange(value);
            }
        }

        public string PostCatalogStr
        {
            get
            {
                if (PostCatalogs.Count == 0) return "请选择目录...";
                return StringSplice(",", PostCatalogs);
            }
        }

        public int Tag
        {
            get
            {
                return _Tag;
            }
            set
            {
                _Tag = value;
            }
        }

        public void SetCategorysByNames(IEnumerable<BlogPostCategory> categories)
        {
            this.PostCatalogs.Clear();
            BlogSettings settings = BlogSettings.ForBlogId(ID);
            foreach (BlogPostCategory ori in categories)
            {
                BlogPostCategory cur = GetAccountCategoryByString(settings, ori.ToString());
                if(cur != null)
                {
                    this.PostCatalogs.Add(cur);
                }
            }
        }

        /// <summary>
        /// 根据目录字符串获取指定帐户中指定的目录信息
        /// </summary>
        /// <param name="settings">帐户信息</param>
        /// <param name="categoryStr">目录名称</param>
        /// <returns>不存在指定名称的目录时返回null</returns>
        public BlogPostCategory GetAccountCategoryByString(BlogSettings settings, string categoryStr)
        {
            foreach (BlogPostCategory item in settings.Categories)
            {
                if (item.ToString().Equals(categoryStr))
                {
                    return item;
                }
            }
            return null;
        }

        /// <summary>
        /// 将多个数据拼接为一个字符串
        /// </summary>
        /// <param name="Linker">用于连接的字符串</param>
        /// <param name="Source">源字符串</param>
        /// <returns>拼接得到的字符串</returns>
        public string StringSplice(string Linker, IEnumerable Source)
        {
            if (Source == null) return string.Empty;

            IEnumerator Enumerator = Source.GetEnumerator();

            StringBuilder Result = new StringBuilder();

            while (Enumerator.MoveNext())
            {
                Result.Append(Linker);
                Result.Append(Enumerator.Current.ToString());
            }

            if (Result.Length > 0) Result.Remove(0, Linker.Length);
            return Result.ToString();
        }
    }
}
