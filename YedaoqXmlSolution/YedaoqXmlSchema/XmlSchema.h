/* ___________________________________________________________________________
@ 
@ file - XmlSchema.h
@ 
@ auth - yedaoq@gmail.com          http://blog.yedaoq.info
@ date - 2012-2-28
@ info -
@     
/* ___________________________________________________________________________*/

#pragma once

#include <vector>
#include "CPP\tstring.h"
#include "CPP\Enumerator.h"
#include "CPP\microtype.h"
#include "XmlSchemaCommon.h"
#include "XmlSchemaEnum.h"
#include "XmlSchemaTypeDefinition.h"

namespace nsYedaoqXmlSchema
{
	class CXmlSchema : public CCloneable<CXmlSchema, IXmlSchemaObject>
	{
	public:

		typedef cloned_container<XmlSchemaTypeHeader, EnumStdContainer::Vector> TypeCollection;
		typedef std::vector<XmlSchemaElement>									ElementCollection;

	public:
		CXmlSchema();

		virtual EnumXmlSchemaObject			ObjectCategory() const { return EnumXmlSchemaObject::Schema; }

		IEnumerator<XmlSchemaTypeHeader>*	EnumType() const;
		IEnumerator<XmlSchemaSimpleType>*	EnumSimpeType() const;		
		IEnumerator<XmlSchemaTypeHeader>*	EnumUserType() const;

		XmlSchemaTypeSN_t					AddVirtualType(const tstring& name );
		XmlSchemaTypeSN_t					AddType(const XmlSchemaTypeHeader& header);
		XmlSchemaTypeSN_t					AddType(XmlSchemaTypeHeader* header);
		void								SetType(XmlSchemaTypeSN_t sn, const XmlSchemaTypeHeader& header);
		void								SetType(XmlSchemaTypeSN_t sn, XmlSchemaTypeHeader* header);

		XmlSchemaTypeHeader const*			GetType(XmlSchemaTypeSN_t sn) const;
		XmlSchemaTypeHeader*				GetType(XmlSchemaTypeSN_t sn);
		XmlSchemaTypeHeader const*			GetType(const tstring& name) const;
		XmlSchemaTypeHeader*				GetType(const tstring& name, bool autoCreate = false);

		unsigned int						TypeCount() const { return Types_.size(); }

		IEnumerator<XmlSchemaElement>*		EnumGlobalElement() const;

		void								AddElement(const XmlSchemaElement& header);
		void								SetElement(const tstring& name, const XmlSchemaElement* header);

		XmlSchemaElement const*				GetElement(const tstring& name) const;
		XmlSchemaElement*					GetElement(const tstring& name);

		unsigned int						ElementCount() const { return Elements_.size(); }

		void								Clear();
		bool								NameConflict(const tstring& name, XmlSchemaTypeSN_t sn) const;

		void								PreFillBuildinTypes();

	protected:
		TypeCollection						Types_;
		ElementCollection					Elements_;
	};
}