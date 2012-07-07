#pragma once
#include "TransParams.h"
#include "Xsd2CppContext.h"

namespace nsYedaoqXmlSchema{ namespace nsXsd2Cpp
{
	class CXsd2Cpp
	{
	public:
		CXsd2Cpp(CXmlSchema* schema, const tstring& project, CTransParams* params = 0)
			: Context(schema, params, project)
		{}

		virtual bool Transform();
		virtual bool ContextVerify();

	protected:
		Xsd2CppContext Context;
	};

}}