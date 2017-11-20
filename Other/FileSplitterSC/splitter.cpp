// FileSplitterSC.cpp : Defines the entry point for the console application.
//

#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows XP.
#define _WIN32_WINNT 0x0501     // Change this to the appropriate value to target other versions of Windows.
#endif

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <shlwapi.h>
#include <Strsafe.h>

#pragma comment(lib, "shlwapi.lib")

struct split_config
{
	int     sub_file_count;
	__int64 sub_file_size;
	
};

void printUsage()
{
	printf(	"Usage : filesplit [g<size>[G|M|K]] [r<ignore:count>] filepath outdir\r\n"
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
			TCHAR buf[128] = {0};
			StringCchCopy(buf, ARRAYSIZE(buf), granularity);

			int len = _tcslen(buf);
			char last_char = buf[len - 1];
			__int64 unit = 1;

			if (last_char < '0' || last_char > '9')
			{
				buf[len - 1] = 0;

				switch(last_char)
				{
				case 'K':
				case 'k':
					unit = 1024;
					break;
				case 'M':
				case 'm':
					unit = 1024 * 1024;
					break;
				case 'g':
				case 'G':
					unit = 1024 * 1024 * 1024;
					break;
				default:
					printf("error : can not parse param '%s'", granularity);
					break;
				}
			}

			cfg->sub_file_size = unit * _ttoi(buf);
		}
	}
}

void copy_file_to_map( HANDLE file_handle, HANDLE sub_file_map_handle, __int64 sub_file_size ) 
{
	LARGE_INTEGER offset = {0};
	DWORD map_size = 0;
	while ((map_size = sub_file_size - offset.QuadPart) > 0)
	{
		if (map_size > 1024 * 1024 * 100)
		{
			map_size = 1024 * 1024 * 100;
		}

		LPVOID sub_file_buf = MapViewOfFile(sub_file_map_handle, FILE_MAP_WRITE, offset.HighPart, offset.LowPart, map_size);

		DWORD bytes_readed = 0;
		if (!::ReadFile(file_handle, sub_file_buf, map_size, &bytes_readed, NULL) || bytes_readed != map_size)
		{
			printf("error : fail to copy file - %d", GetLastError());
			exit(0);
		}

		UnmapViewOfFile(sub_file_buf);

		offset.QuadPart += map_size;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	printf(	"\r\n*************  simple file splitter (console)  *********\r\n");

	if (argc < 1)
	{
		printUsage();
		return 0;
	}

	split_config cfg = { 10, 0 };

	LPCTSTR file_to_split = NULL;
	TCHAR   dir_ouput[MAX_PATH * 2] = TEXT("");

	for (int i = 1; i < argc; ++i)
	{
		DWORD file_attr = GetFileAttributes(argv[i]);
		if (INVALID_FILE_ATTRIBUTES != file_attr)
		{
			if (FILE_ATTRIBUTE_DIRECTORY & file_attr)
			{
				StringCchCopy(dir_ouput, ARRAYSIZE(dir_ouput) - 1, argv[i]);
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
		default:
			break;
		}
	}

	if (!file_to_split)
	{
		printUsage();
		return 0;
	}

	if (!*dir_ouput)
	{
		StringCchCopy(dir_ouput, ARRAYSIZE(dir_ouput), file_to_split);
		::PathRemoveExtension(dir_ouput);
	}

	int dir_ouput_prefix_len = _tcslen(dir_ouput);

	_tprintf(TEXT("target : %s\r\n"), file_to_split);

	HANDLE file_handle = CreateFile(file_to_split, FILE_READ_ACCESS, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

	if (INVALID_HANDLE_VALUE == file_handle)
	{
		_tprintf(TEXT("error  : fail to open file\r\n"));
		return 0;
	}

	// determine sub file size
	LARGE_INTEGER file_size;
	GetFileSizeEx(file_handle, &file_size);

	if (cfg.sub_file_size)
	{
		if (cfg.sub_file_size >= file_size.QuadPart)
		{
			_tprintf(TEXT(" target file is small than split granularity \r\n"), file_to_split);
			return 0;
		}
	}
	else if (cfg.sub_file_count > 0)
	{
		cfg.sub_file_size = file_size.QuadPart / cfg.sub_file_count;
		if (file_size.QuadPart % cfg.sub_file_count)
		{
			++cfg.sub_file_size;
		}
	}

	// split
	::PathAppend(dir_ouput, ::PathFindFileName(file_to_split));
	::PathRemoveExtension(dir_ouput);

	LPCTSTR file_ext = ::PathFindExtension(file_to_split);
	if (NULL == file_ext)
		file_ext = TEXT("");

	__int64 left_size = file_size.QuadPart;
	int		sub_file_idx = 1;

	DWORD tick_count = GetTickCount();

	while(left_size > 0)
	{
		printf("wirte #%d sub file : ", sub_file_idx);

		__int64 sub_file_size = cfg.sub_file_size;
		if (left_size < cfg.sub_file_size)
		{
			sub_file_size = left_size;
		}
		left_size -= sub_file_size;

		StringCchPrintf(dir_ouput + dir_ouput_prefix_len, MAX_PATH, TEXT("-%d%s"), sub_file_idx, file_ext);

		HANDLE sub_file_handle = CreateFile(dir_ouput, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE != sub_file_handle && NULL != sub_file_handle)
		{
			HANDLE sub_file_map_handle = CreateFileMapping(sub_file_handle, NULL, PAGE_READWRITE | SEC_COMMIT, (DWORD)(sub_file_size >> 32), (DWORD)sub_file_size, NULL);
			
			if (INVALID_HANDLE_VALUE != sub_file_map_handle && NULL != sub_file_map_handle)
			{
				copy_file_to_map(file_handle, sub_file_map_handle, sub_file_size);
				::CloseHandle(sub_file_map_handle);
			}
			else
			{
				printf("error : fail to create file map %d\r\n", GetLastError());
				exit(1);
			}
		}
		else
		{
			printf("error : fail to open subfile %d, %s\r\n", GetLastError(), dir_ouput);
			exit(1);
		}

		printf("ok\r\n");

		++sub_file_idx;
	}

	printf("split complete in %dms\r\n", GetTickCount() - tick_count);

	getchar();

	return 0;
}

