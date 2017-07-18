// FileSplitterSC.cpp : Defines the entry point for the console application.
//

#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows XP.
#define _WIN32_WINNT 0x0501     // Change this to the appropriate value to target other versions of Windows.
#endif

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

struct split_config
{
	int     sub_file_count;
	__int64 sub_file_size;
	
};

void printUsage()
{
	print(	"Usage : filesplit [g<size>[G|M|K]] [r<ignore:count>] filepath outdir\r\n"
			"	g - granularity\r\n"
			"	r - range\r\n"
			"		ignore - ignore first some subfile for a positive, or ignore last some subfile for a minus\r\n"
			"		count  - save some subfile, abandon others\r\n"
		);
}

void parseGranularity(LPCTSTR granularity, split_config* cfg)
{
	if (granularity || *granularity)
	{
		if ('-' == *granularity)
		{
			cfg->sub_file_count = _ttoi(granularity + 1);
		}
		else
		{
			cfg->sub_file_size = _ttoi(granularity);
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	print(	"****************************************************\r\n"
			"*************simple file splitter (console)*********\r\n");

	if (argc < 1)
	{
		printUsage();
		return 0;
	}

	split_config cfg = { 10, 0 };

	LPCTSTR file_to_split = NULL;
	TCHAR   dir_ouput[MAX_PATH * 2] = TEXT("");

	for (int i = 0; i < argc - 1; ++i)
	{
		DWORD file_attr = GetFileAttributes(argv[i]);
		if (INVALID_FILE_ATTRIBUTES != file_attr)
		{
			if (FILE_ATTRIBUTE_DIRECTORY & file_attr)
			{
				StringCchCopy(dir_ouput, ARRAYSIZE(dir_ouput) - 1, argv[i])
			}
			else
			{
				file_to_split = argv[i];	
			}
			continue;
		}

		switch(argv[i][0])
		{
		case 'g':
		case 'G':
			parseGranularity(argv[i] + 1, &cfg);
			break;

		case 'r':
		case 'R':

			break;
		}
	}

	if (!file_to_split)
	{
		printUsage();
		return 0;
	}

	_tprintf(TEXT("target : %s\r\n"), file_to_split);

	HANDLE file_handle = CreateFile(file_to_split, FILE_READ_ACCESS, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

	if (INVALID_HANDLE_VALUE == file_handle)
	{
		_tprintf(TEXT("error  : open file fail\r\n"));
		return 0;
	}

	LARGE_INTEGER file_size;
	GetFileSizeEx(file_handle, &file_size);

	if (cfg.sub_file_size)
	{
		if (cfg.sub_file_size >= file_size.QuadPart)
		{
			_tprintf(TEXT("error : open file fail - %s\r\n"), file_to_split);
			return 0;
		}
	}


	return 0;
}

