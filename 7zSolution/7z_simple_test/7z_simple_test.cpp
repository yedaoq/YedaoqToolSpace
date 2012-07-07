// 7z_simple_test.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <tchar.h>
#include "..\7z_simple\7z_simple.h"
#include "..\7z_simple\7zVersion.h"

int _tmain(int argc, _TCHAR* argv[])
{
	printf("\n7z ANSI-C Decoder " MY_VERSION_COPYRIGHT_DATE "\n\n");
	if (argc == 1)
	{
		printf("Usage: 7zDec <archive_name> <outdir>\n\n");
		return 0;
	}
	if (argc < 3)
	{
		PrintError("incorrect command");
		return 1;
	}

	decompress_by_path(argv[1], argv[2]);

	getchar();

	return 0;
}

