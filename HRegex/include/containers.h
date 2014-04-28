#ifndef _HREG_CONTAINERS_
#define _HREG_CONTAINERS_

#include "globals.h"

template <typename T>
class SortedVectorSet
{
public:
	bool insert(const T& e)
	{
		auto iter = std::lower_bound(data.begin(), data.end(), e);
		if (iter == data.end() || *iter != e)
		{
			data.insert(iter, e);
			return true;
		}
		else
		{
			return false;
		}
	}
	void clear()
	{
		data.clear();
	}
	size_t size() const
	{
		return data.size();
	}
	bool operator==(const SortedVectorSet& other) const
	{
		return data == other.data;
	}
	bool operator!=(const SortedVectorSet& other) const
	{
		return data != other.data;
	}
	bool operator<(const SortedVectorSet& other) const
	{
		size_t s1 = size();
		size_t s2 = other.size();
		size_t smaller = s1 < s2 ? s1 : s2;
		for (size_t i = 0; i < smaller; ++i)
		{
			if (data[i] < other.data[i])
			{
				return true;
			}
			else if (other.data[i] < other.data[i])
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
		return std::binary_search(data.begin(), data.end(), e);
	}
	typename std::vector<T>::const_iterator begin() const
	{
		return data.begin();
	}
	typename std::vector<T>::const_iterator end() const
	{
		return data.end();
	}
private:
	std::vector<T> data;
};

#endif