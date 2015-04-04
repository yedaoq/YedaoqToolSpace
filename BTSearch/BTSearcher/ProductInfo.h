#ifndef ProductInfo_h__
#define ProductInfo_h__

#define TXT_NAME_DEFAULT

#ifdef TXT_NAME_DEFAULT 

#define _UNICODESTR(_name)		L##_name
#define UNICODESTR(_name)		_UNICODESTR(_name)

#define COPYDATE_U_STR			1
#define COPYDATE_A_STR			0

#define PRODUCT_ID				TEXT("MicroWord")
#define PRODUCT_NAME			TEXT("Î¢ÐÍWord")
//#define PRODUCT_WND_CLASS		TEXT("WND") PRODUCT_ID TEXT("Class")
#define WNDCLASSNAME			"WNDMicroWordClass"
#define PRODUCT_WND_CLASSW		UNICODESTR(WNDCLASSNAME)
#define PRODUCT_WND_CLASSA		WNDCLASSNAME
#if defined(UNICODE) || defined(_UNICODE)
#define PRODUCT_WND_CLASS		PRODUCT_WND_CLASSW
#else
#define PRODUCT_WND_CLASS		PRODUCT_WND_CLASSA
#endif
#define PRODUCT_TIP_CLASS		TEXT("WND") PRODUCT_ID TEXT("TipClass")
#define PRODUCT_CONFIG			TEXT("config.ini")
#define PRODUCT_APPDATA_FOLDER	PRODUCT_ID

#define PRODUCT_FOLDER			PRODUCT_ID
#define PRODUCT_WND_TITLE		PRODUCT_NAME
#define PROGRAMPARAM_HIDE		TEXT("auto")
#define AUTORUN_REGKEY			PRODUCT_ID
#define PRODUCT_BIND_INI		TEXT("friend.dat")

#define PROGRAMPARAM_SHELL_REPORT	TEXT("srun")
#define PROGRAMPARAM_START_JOBOT	TEXT("jstart")

#define PRODUCT_DOMAIN			TEXT(".microword.cn/")
//#define PRODUCT_HELP_URL		TEXT("http://soft") PRODUCT_DOMAIN
#define PRODUCT_HOME_URL		TEXT("http://soft") PRODUCT_DOMAIN
#define PRODUCT_CONTENT_URL		TEXT("http://s") PRODUCT_DOMAIN
#define PRODUCT_UPDATE_URL		TEXT("http://u") PRODUCT_DOMAIN TEXT("u.jpg")
#define PRODUCT_REPORT_URL		TEXT("http://st") PRODUCT_DOMAIN TEXT("p.jpg")
#define PRODUCT_BIND_URL		TEXT("http://st") PRODUCT_DOMAIN TEXT("b.gif")
#define PRODUCT_FEEDBACK_URL	TEXT("http://support") PRODUCT_DOMAIN TEXT("feedback.html")

#define PRODUCT_SHELL_GUID		{ 0x2bd98d43, 0x1d1f, 0x4eef, { 0xb5, 0xb, 0x42, 0xc6, 0xf8, 0x52, 0xab, 0x49 } };

#define PRODUCT_SHELL_FILE_NAME				TEXT("libgapi.dll")

#endif

#endif // ProductInfo_h__

