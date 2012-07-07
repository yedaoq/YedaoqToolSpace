/* ___________________________________________________________________________
@ 
@ file - ListBumpkin.h
@ 
@ auth - yedaoq@gmail.com          http://blog.yedaoq.info
@ date - 2012-3-9
@ info -
@     as file name, this is the bumpkin implement of list , Without giving explanations
/* ___________________________________________________________________________*/

// nsListWithoutConstructor contain a list implement without constructor, which was instead by initialize function
namespace nsListWithoutConstructor
{

	template<typename T>
	class list_node
	{
	public:
		T			Val;
		list_node*	Prev;
		list_node*	Next;		

		list_node(const T& v, list_node *pre = 0, list_node *next = 0): Val(v), Prev(pre), Next(next) {}
		list_node(list_node *pre = 0, list_node *next = 0) : Prev(pre), Next(next) {}
	};

	template<typename T>
	class list
	{
	protected:
		list_node	Head_;		
		typedef		list_node* iterator;

	public:
		
		void initialize()
		{
			Head_.Next = &Head_;
			Head_.Prev = &Head_;
		}

		iterator begin()	{ return Head_.Next; }
		iterator end()	{ return &Head_; }

		void clear()
		{
			if(Head_.Next != &Head_)
			{
				list_node *pNode = Head_.Next;
				Head_.Next = &Head_;

				do 
				{
					list_node *pTmp = pNode;
					pNode = pNode->Next;
					delete pTmp;
				} while (pNode != &Head_);
			}
		}

		void insert(iterator where, const T& val)
		{
			where.Next = new list_node(val, where, where.Next);
		}

		void push_front(const T& val)
		{
			insert(&Head_, val);
		}

		void push_back(const T& val)
		{
			insert(Head_.Prev, val);
		}

		void erase(iterator iter)
		{
			iter.Prev->Next = iter.Next;
			iter.Next->Prev = iter.Prev;
			delete(iter);
		}

		bool remove(const T& val)
		{
			list_node* iter = Head_.Next;
			while(iter != &Head_)
			{
				if(iter->Val == val)
				{
					erase(iter);
					return true;
				}
			}
			return false;
		}

	};

}