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
	__int64 granularity;
	
};

void printUsage()
{
	print(	"Usage : filesplit [g<size>[G|M|K]] [r<ignore:count>] filepath\r\n"
			"	g - granularity\r\n"
			"	r - range\r\n"
			"		ignore - ignore first some subfile for a positive, or ignore last some subfile for a minus\r\n"
			"		count  - save some subfile, abandon others\r\n"
		);
}

void parseGranularity(LPCTSTR granularity)
{
	if (granularity)
	{
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

	for (int i = 0; i < argc - 1; ++i)
	{
		switch(argv[i][0])
		{
		case 'g':
		case 'G':

			break;

		case 'r':
		case 'R':

			break;
		}
	}

	return 0;
}

