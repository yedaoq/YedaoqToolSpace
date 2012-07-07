#pragma once

class CXsdComplexTypeRender
{
public:
	CXsdComplexTypeRender(void);
	~CXsdComplexTypeRender(void);

	void Render(nsYedaoqXmlSchema::nsXsd2Cpp::Xsd2CppContext& ctx);
};
