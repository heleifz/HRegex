#ifndef _HREG_CONTAINERS_
#define _HREG_CONTAINERS_

#include "globals.h"

// a copy-on-write set container
// set of automata states are used and copied everywhere during regex compilation
template <typename T>
class SortedVectorSet
{
public:
	SortedVectorSet()
	{
		data = std::make_shared<std::vector<T>>();
	}
	SortedVectorSet(typename std::vector<T>::const_iterator b,
					typename std::vector<T>::const_iterator e)
	{
		data = std::make_shared<std::vector<T>>(b, e);
	}
	bool insert(const T& e)
	{
		if (data.use_count() > 1)
		{
			// make a copy
			data = std::make_shared<std::vector<T>>(data->begin(), data->end());
		}
		auto iter = std::lower_bound(data->begin(), data->end(), e);
		if (iter == data->end() || *iter != e)
		{
			data->insert(iter, e);
			return true;
		}
		else
		{
			return false;
		}
	}
	bool remove(const T& e)
	{
		if (data.use_count() > 1)
		{
			// make a copy
			data = std::make_shared<std::vector<T>>(data->begin(), data->end());
		}
		auto iter = std::lower_bound(data->begin(), data->end(), e);
		if (iter == data->end() || *iter != e)
		{
			return false;
		}
		else
		{
			data->erase(iter);
			return true;
		}
	}
	void popBack()
	{
		if (data.use_count() > 1)
		{
			// make a copy
			data = std::make_shared<std::vector<T>>(data->begin(), data->end());
		}
		if (!data->empty())
		{
			data->pop_back();
		}
		else
		{
			throw EmptyContainerError();
		}
	}
	void clear()
	{
		if (data.use_count() == 1)
		{
			data->clear();
		}
		else
		{
			data = std::make_shared<std::vector<T>>();
		}
	}
	const T& last() const
	{
		if (size() == 0)
		{
			throw EmptyContainerError();
		}
		return *(end() - 1);
	}
	bool isEmpty() const
	{
		return size() == 0;
	}
	size_t size() const
	{
		return data->size();
	}
	SortedVectorSet<T> operator||(const SortedVectorSet<T>& other) const
	{
		SortedVectorSet<T> ret;
		auto thisHead = begin();
		auto thatHead = other.begin();
		while (thisHead != end() && thatHead != other.end())
		{
			if (*thisHead == *thatHead)
			{
				ret.data->push_back(*thisHead);
				thisHead++;
				thatHead++;
			}
			else if (*thisHead > *thatHead)
			{
				ret.data->push_back(*thatHead);
				thatHead++;
			}
			else
			{
				ret.data->push_back(*thisHead);
				thisHead++;
			}
		}
		if (thisHead != end())
		{
			ret.data->insert(ret.data->end(), thisHead, end());
		}
		else if (thatHead != other.end())
		{
			ret.data->insert(ret.data->end(), thatHead, other.end());
		}
		return ret;
	}
	SortedVectorSet<T> operator&&(const SortedVectorSet<T>& other) const
	{
		SortedVectorSet<T> ret;
		auto thisHead = begin();
		auto thatHead = other.begin();
		while (thisHead != end() && thatHead != other.end())
		{
			if (*thisHead == *thatHead)
			{
				ret.data->push_back(*thisHead);
				thisHead++;
				thatHead++;
			}
			else if (*thisHead > *thatHead)
			{
				thatHead++;
			}
			else
			{
				thisHead++;
			}
		}
		return ret;
	}
	SortedVectorSet<T> operator-(const SortedVectorSet<T>& other) const
	{
		SortedVectorSet<T> ret;
		auto thisHead = begin();
		auto thatHead = other.begin();
		while (thisHead != end() && thatHead != other.end())
		{
			if (*thisHead == *thatHead)
			{
				thisHead++;
				thatHead++;
			}
			else if (*thisHead > *thatHead)
			{
				thatHead++;
			}
			else
			{
				ret.data->push_back(*thisHead);
				thisHead++;
			}
		}
		if (thisHead != end())
		{
			ret.data->insert(ret.data->end(), thisHead, end());
		}
		return ret;
	}
	bool operator==(const SortedVectorSet& other) const
	{
		return (data == other.data || (*data) == (*other.data));
	}
	bool operator!=(const SortedVectorSet& other) const
	{
		return !(operator==(other));
	}
	bool operator<(const SortedVectorSet& other) const
	{
		if (data == other.data)
		{
			return false;
		}
		size_t s1 = size();
		size_t s2 = other.size();
		size_t smaller = s1 < s2 ? s1 : s2;
		for (size_t i = 0; i < smaller; ++i)
		{
			if ((*data)[i] < (*other.data)[i])
			{
				return true;
			}
			else if ((*other.data)[i] < (*data)[i])
			{
				return false;
			}
		}
		if (s1 < s2)
		{
			return true;
		}
		return false;
	}
	bool contains(const T& e) const
	{
		return std::binary_search(data->begin(), data->end(), e);
	}
	typename std::vector<T>::const_iterator begin() const
	{
		return data->begin();
	}
	typename std::vector<T>::const_iterator end() const
	{
		return data->end();
	}
private:
	std::shared_ptr<std::vector<T>> data;
};

#endif