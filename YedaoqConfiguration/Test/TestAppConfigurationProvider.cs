using System;
using System.Collections.Generic;
using System.Text;
using YedaoqConfiguration;
using System.IO;
using System.Configuration;

namespace Test
{
    [AppConfigurationProvider("ID","Name")]
    public class TestAppConfigurationProvider : IAppConfigurationProvider
    {
        public TestAppConfigurationProvider()
        {

        }

        #region Fields

        /// <summary>
        /// 客户信息
        /// </summary>
        private System.Collections.Generic.List<IClientInfo> _Clients;

        /// <summary>
        /// 当前客户
        /// </summary>
        private ClientInfo _CurrentClient;

        /// <summary>
        /// 当前客户的配置
        /// </summary>
        private TestAppConfiguration _CurrentClientConfiguration;

        #endregion

        #region Properties

        public IEnumerable<IClientInfo> Clients
        {
            get
            {
                return _Clients;
            }
        }

        public int ClientCount
        {
            get
            {
                return _Clients.Count;
            }
        }

        public IClientInfo CurrentClient
        {
            get
            {
                return _CurrentClient;
            }
            set
            {
                _CurrentClient = value as ClientInfo;
                _CurrentClientConfiguration = null;
            }
        }

        public IAppConfiguration CurrentClientConfiguration
        {
            get
            {
                if (object.ReferenceEquals(null, _CurrentClientConfiguration) && !object.ReferenceEquals(null,_CurrentClient))
                {
                    _CurrentClientConfiguration = new TestAppConfiguration(_CurrentClient); 
                }
                return _CurrentClientConfiguration;
            }
        }

        #endregion

        #region Methods

        public int Initialize()
        {
            _Clients = new List<IClientInfo>();
            string directoryConfig = Path.Combine(Path.GetDirectoryName(this.GetType().Assembly.Location),"config");

            System.Diagnostics.Debug.WriteLine(false, directoryConfig);

            if (!Directory.Exists(directoryConfig)) return 1;

            foreach (string client in Directory.GetDirectories(directoryConfig))
            {
                _Clients.Add(new ClientInfo(_Clients.Count + 1, client.Substring(directoryConfig.Length + 1), client, client));
            }

            if (_Clients.Count > 0) _CurrentClient = _Clients[0] as ClientInfo;

            return 1;
        }

        #endregion
    }
}
