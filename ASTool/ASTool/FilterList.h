#pragma once

class FilterList
{
public:
	struct Item
	{
		char buf[1024];
		Item* next;
		Item* prev;

		Item()
		{
			buf[0] = 0;
		}
	};

public:
	FilterList(void);
	~FilterList(void);

	bool LoadFilters();
	bool SaveFilters();

	bool AddFilter(char* filter);

	Item* First() const { return first_; }

protected:
	Item	items_[10];
	Item*	first_;
};
