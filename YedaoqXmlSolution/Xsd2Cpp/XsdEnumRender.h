#pragma once
#include "Xsd2CppContext.h"

class CXsdEnumRender
{
public:
	CXsdEnumRender(void);
	~CXsdEnumRender(void);

	void Render(nsYedaoqXmlSchema::nsXsd2Cpp::Xsd2CppContext& ctx);
};
