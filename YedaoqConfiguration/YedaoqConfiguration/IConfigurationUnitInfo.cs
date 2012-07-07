using System;
using System.Collections.Generic;
using System.Text;
using System.Configuration;

namespace YedaoqConfiguration
{
    public interface IConfigurationUnitInfo
    {
        /// <summary>
        /// ���������Ƿ��Զ���ʼ��
        /// </summary>
        bool AutoInitWhenNonExist
        {
            get;
            set;
        }

        /// <summary>
        /// ������
        /// </summary>
        EnumConfigDomain Domain
        {
            get;
            set;
        }

        /// <summary>
        /// ���������ļ���
        /// </summary>
        string FileName
        {
            get;
            set;
        }

        /// <summary>
        /// ������
        /// </summary>
        string ConfigName
        {
            get;
            set;
        }

        /// <summary>
        /// ���õ�Ԫ�������
        /// </summary>
        Type UnitType
        {
            get;
            set;
        }

        /// <summary>
        /// ������Ϣ�Ƿ��Ѷ�ȡ�ı��
        /// </summary>
        bool InitFlag
        {
            get;
            set;
        }

        /// <summary>
        /// ������Ϣ
        /// </summary>
        object Value
        {
            get;
            set;
        }
    }
}
