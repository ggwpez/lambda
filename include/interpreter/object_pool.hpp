#ifndef OBJECT_POOL_HPP
#define OBJECT_POOL_HPP

#include <cstdarg>
#include <memory>
#include <cstring>
#include <iostream>
#include <cstdlib>

#include <stack>
#include <list>
#include <vector>
#include <memory>
#include <array>

#define START UINT64_MAX

template<class T, std::size_t S>
class object_pool
{
public:
	explicit object_pool()
		: blocks(), frees(0, nullptr), it(START)
	{
		alloc(S);
	}

	~object_pool()
	{
		if (it +1 != frees.size())
			std::cerr << "Unfree objects found! (" << std::to_string(frees.size() -it -1) << ')' << std::endl;

		for (void* b : blocks)
			cfree(b);
	}

	template<class ...args>
	T* create(args&&... a)
	{
		if (it == START)
			re_alloc();

		T* addr = frees[it--];        // decrement here, otherwise recursive copy construtors wont work
		if (it == START)
			re_alloc();

		return new (addr) T(std::forward<args>(a)...);
	}

	inline void destroy(T* obj)
	{
		obj->~T();
		frees[++it] = obj;
	}

private:
	void alloc(std::size_t new_elems)
	{
		if (! new_elems)
			throw std::runtime_error("Internal alloc(size = 0) error.");

		frees.resize(frees.size() +new_elems);
		T* ptr = (T*)calloc(new_elems, sizeof(T));

		if (! ptr)
			throw std::bad_alloc();
		blocks.push_back(ptr);

		for (std::size_t i = 0; i < new_elems; i++)
			frees[++it] = ptr++;
	}

	inline void re_alloc()
	{
		alloc(S);
	}

private:
	std::vector<void*> blocks;
	std::vector<T*> frees;
	std::size_t/*typename std::iterator_traits<T*>::value_type*/ it;
};

#endif // OBJECT_POOL_HPP
