#pragma once

#include "vector"
#include "Enumerator.h"
#include <boost\smart_ptr\scoped_ptr.hpp>

//////////////////////////////////////////////////////////////////////////
// objvector
//    - a vector maintain a set of obj of type T.
//      since it's designed for contain the objects in a class hierarchy tree, it hold
//      the pointer of the obj instead of the obj itself.
//      since it's designed for the Semantics of Copy by value, it will maintain the life
//      of the obj.
//      it owns all the obj in it exclusively.
//      in order to copy and delete the member obj, the obj should supply a Clone and a 
//      Destroy function.
//////////////////////////////////////////////////////////////////////////
template<typename T>
class objvector : public ICloneable, std::vector<T*>
{
public:

	// although the destructor of vector is not a virtual function, but some one should never use 
	// the vector* pointer to delete a objvector.
	~objvector(void)	
	{
		ReleaseObj(begin(), end());
	}

	objvector(void);

	objvector(const objvector& other)
	{
		Import(other);
	}

	objvector& operator=(const objvector& other)
	{
		Clear();
		Import(other);
	}

	virtual ICloneable* Clone() const
	{
		return new objvector(*this);
	}

	void Add(const T& obj)
	{
		push_back(static_cast<T*>(obj.Clone()));
	}

	void Add(T* obj)
	{
		push_back(obj);
	}

	void Import(const objvector& source)
	{
		boost::scoped_ptr<IEnumerator<T>> etor = source.Enum();
		Import(etor);
	}

	void Import(IEnumerator<T>* source)
	{
		while(source->MoveNext())
		{
			Add(source->Current());
		}
	}

	void RemoveAt(int i)
	{
		iterator iter = this->begin() + i;
		iter->Destroy();
		erase(iter);
	}

	void SetAt(int i, const T& obj)
	{
		SetAt(i, static_cast<T*>(obj.Clone()));
	}

	void SetAt(int i, T* obj)
	{
		iterator iter = begin() + i;
		ReleaseObj(iter, iter + 1);
		*iter = obj;
	}

	void Clear()
	{
		ReleaseObj(begin(), end());
		clear();
	}

	IEnumerator<T>* Enum() const
	{
		return new_convert_enumerator<T>(
			make_iterator_enumerator(begin(), end()),
			CCastFunctor_Dereference<T&, T*>());
	}

protected:
	void ReleaseObj(iterator first, iterator last)
	{
		for(iterator iter = first; iter != last; ++iter) (*iter)->Destroy();
	}
};
