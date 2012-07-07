// XmlSchemaParser.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "fstream"
#include "..\YedaoqXmlSchema\XmlSchemaSerializer.h"

using namespace nsYedaoqXmlSchema;
using namespace nsYedaoqXmlSchema::nsSerialize;

int _tmain(int argc, _TCHAR* argv[])
{
	if(argc < 2)
	{
		return 0;
	}

	tchar buf[65535] = {0};
	std::tfstream stream(argv[1], std::ios_base::in);
	stream.read(buf, count_of(buf));
	CXmlSchemaSerializer	loader;
	CXmlSchema				schema;
	loader.Load(schema, buf);
	return 0;
}

