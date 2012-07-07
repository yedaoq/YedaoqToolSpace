#include "StdAfx.h"
#include "CfgProOne.h"

xnode_t* CCfgMain::Serialize()
{
	xnode_t* node = new xnode_t();
	node->name(TEXT("Main"));
	xattr_t* attr;

	attr = new xattr_t();
	attr->name = TEXT("Name");
	attr->value = Name;
	node.append_attribute(attr);

	node.append_node(Sub1.Serialize());

	for(std::vector<CCfgSub2>::iterator iter = Sub2s.begin(); iter != Sub2s.end(); ++iter)
	{
		node.append_node(iter->Serialize());
	}

	return node;
}

void CCfgMain::Parse( xnode_t& node )
{
	xattr_t* attr;
	xnode_t* child;

	attr = node.first_attribute(TEXT("Name"));
	if(attr)
	{
		Name = attr->value();
	}

	child = node.first_node(TEXT("Sub1"));
	if(child)
	{
		Sub1.Parse(*child);
	}

	for(child = node.first_node(TEXT("Sub2")); child; child = child->next_sibling())
	{
		CCfgSub2 Sub2;
		Sub2s.push_back(Sub2);
	}
}
