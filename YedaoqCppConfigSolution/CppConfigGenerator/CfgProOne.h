#pragma once

struct CCfgSub1
{
	xnode_t* Serialize();
	void Parse(xnode_t& node);

	int X;
	int Y;
};

struct CCfgSub2
{
	xnode_t* Serialize();
	void Parse(xnode_t& node);

	int X;
	int Y;
	int Width;
	int Height;
};

struct CCfgMain
{
	xnode_t* Serialize();
	void Parse(xnode_t& node);

	tstring Name;

	CCfgSub1	Sub1;
	std::vector<CCfgSub2>	Sub2s;
};