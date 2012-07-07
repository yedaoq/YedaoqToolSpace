#pragma once
#include "CPP\tstring.h"
#include <tchar.h>

class xsNamespace
{
	struct Element
	{
		tchar*	str_;

		Element(tchar const* v);
		//Element(tchar (&v)[]);
		~Element();

		inline tchar const* str() const { return str_; }
		inline bool operator==(tchar const* v) const { return v && (_tcscmp(str_, v) || _tcscmp(str_ + 3, v)); }
	};

	xsNamespace(void);
	xsNamespace(const xsNamespace&);
	~xsNamespace(void);

public:
	static const Element all;
	static const Element annotation;
	static const Element any;
	static const Element anyAttribute;
	static const Element appInfo;
	static const Element attribute;
	static const Element attributeGroup;
	static const Element choice;
	static const Element complexContent;
	static const Element complexType;
	static const Element documentation;
	static const Element element;
	static const Element extension;
	static const Element field;
	static const Element group;
	static const Element import;
	static const Element include;
	static const Element key;
	static const Element keyref;
	static const Element list;
	static const Element notation;
	static const Element redefine;
	static const Element restriction;
	static const Element schema;
	static const Element selector;
	static const Element sequence;
	static const Element simpleContent;
	static const Element simpleType;
	static const Element union_;
	static const Element unique;
};
