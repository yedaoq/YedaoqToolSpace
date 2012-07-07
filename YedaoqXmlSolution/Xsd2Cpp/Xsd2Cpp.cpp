#include "Xsd2Cpp.h"
#include "Singlton.h"
#include "XmlSchema.h"
#include "TypeCollecter.h"

using namespace nsYedaoqXmlSchema;
using namespace nsYedaoqXmlSchema::nsXsd2Cpp;

#define RETatFAIL(expr) if(!(expr)) return;
#define RETatFAILr(expr, ret) if(!(expr)) return ret;

bool CXsd2Cpp::Transform()
{
	RETatFAILr(ContextVerify(), false);	
	RETatFAILr(CTypeCollector().Collect(Context), false);
}

bool CXsd2Cpp::ContextVerify()
{
	do 
	{
		if(!Context.Schema) break;
		Context.TypeMask.resize(Context.Schema->TypeCount(), false);

		if(Context.ProjectName.length() <= 0) Context.ProjectName = "Proj";
		if(Context.Params)
		{
			if(!Context.Params->NameMapper.get())
			{
				Context.Params->NameMapper = std::auto_ptr<INameMapper>(new CNameMapperBase());
			}
		}
		else
		{
			Context.Params = &CSingleton<CTransParams>::GetInstance();
		}

		return true;
	} while (false);

	return false;
}
