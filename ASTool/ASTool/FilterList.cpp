#include "StdAfx.h"
#include "FilterList.h"

FilterList::FilterList(void)
{
	for (int i = 0; i < ARRAYSIZE(items_) - 1; ++i)
	{
		items_[i].next = items_ + i + 1;
	}

	items_[ARRAYSIZE(items_) - 1].next = items_;

	for (int i = 1; i < ARRAYSIZE(items_); ++i)
	{
		items_[i].prev = items_ + i - 1;
	}

	items_[0].prev = items_ + ARRAYSIZE(items_) - 1;


}

FilterList::~FilterList(void)
{
}

bool FilterList::LoadFilters()
{
	CHAR ini_path[MAX_PATH * 2];

	SHGetSpecialFolderPathA(NULL, ini_path, CSIDL_APPDATA, FALSE);
	::PathAppendA(ini_path, /*TEXT*/("Zoom"));
	::PathAppendA(ini_path, /*TEXT*/("data"));
	::PathAppendA(ini_path, /*TEXT*/("LogView.ini"));

	Item* item = items_;
	first_ = item;

	CHAR key_name[30];
	for (int i = 0; i < ARRAYSIZE(items_); ++i)
	{
		itoa(i, key_name, 10);
		int len = GetPrivateProfileStringA(/*TEXT*/("LogViewFilter"), key_name, /*TEXT*/(""), item->buf, ARRAYSIZE(item->buf), ini_path);
		if (len <= 0)
			break;
		else
			item = item->next;
	}

	return true;
}

bool FilterList::SaveFilters()
{
	CHAR ini_path[MAX_PATH * 2];

	SHGetSpecialFolderPathA(NULL, ini_path, CSIDL_APPDATA, FALSE);
	::PathAppendA(ini_path, /*TEXT*/("Zoom"));
	::PathAppendA(ini_path, /*TEXT*/("data"));
	::PathAppendA(ini_path, /*TEXT*/("LogView.ini"));

	Item* item = first_;

	CHAR key_name[30];
	for (int i = 0; i < ARRAYSIZE(items_); ++i)
	{
		if (!*item->buf)
			break;

		itoa(i, key_name, 10);
		int len = WritePrivateProfileStringA(/*TEXT*/("LogViewFilter"), key_name, item->buf, ini_path);
		item = item->next;
	}

	return true;
}

bool FilterList::AddFilter( char* filter )
{
	Item* item = first_;

	for (int i = 0; i < ARRAYSIZE(items_); ++i)
	{
		if (!*item->buf)
			break;

		if (0 == strcmp(filter, item->buf))
		{
			return false;
		}
	}

	first_ = first_->prev;
	strcpy_s(first_->buf, filter);
	return true;
}
