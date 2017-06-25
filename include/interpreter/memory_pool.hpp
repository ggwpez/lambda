#ifndef MEMORY_POOL_HPP
#define MEMORY_POOL_HPP

#include "ast.hpp"

#include <cassert>
#include <array>
#include <memory>
#include <vector>
#include <iostream>
#include <type_traits>

template<typename T, std::size_t s>
class pool
{
public:
	pool()
		: data_ptr(reinterpret_cast<char*>(calloc(count, t_size))), m_free_pos(0)
	{
		assert(all_size);
		std::wclog << L"Pool allocating " << all_size << L" bytes in " << count << L" objects of type " << typeid(T).name() << std::endl;

		if (! data_ptr)
			throw std::bad_alloc();

		for (std::size_t i(0); i < count; ++i)
			m_free[i] = reinterpret_cast<void*>(data_ptr +i *sizeof(T));
	}

	~pool()
	{
		cfree(data_ptr);
		data_ptr = nullptr;

		if (m_free_pos)
			std::wcerr << L"Warning " << m_free_pos << L" non-destructed objects detected (all pool memory got freed)" << std::endl;
	}

	template<typename... Args, typename = std::enable_if_t<std::is_constructible<T, Args...>::value>>
	T* _new(Args&&... args)
	{
		// Is there still something free ?
		if (m_free_pos +1 >= m_free.size())
			throw std::bad_alloc();

		T* ret(reinterpret_cast<T*>(m_free[m_free_pos++]));
		assert(ret && is_ptr_valid(ret));

		return new (ret) T(std::forward<Args>(args)...);
	}

	void _delete(T* obj)
	{
		if (! is_ptr_valid(obj))
			throw std::runtime_error("Invalid free address");
		if (! m_free_pos)
			throw std::runtime_error("_delete called more often than _new");

		obj->~T();
		m_free[--m_free_pos] = reinterpret_cast<void*>(obj);
	}

	constexpr bool is_ptr_valid(void* obj)
	{
		return obj && !((reinterpret_cast<char*>(obj) -data_ptr) %sizeof(T));
	}

	typedef T value_type;
	std::size_t const count = s,
					  t_size = sizeof(T),
					  all_size = s *sizeof(T);

private:
	char* data_ptr;
	std::array<void*, s> m_free;
	std::size_t m_free_pos;
};

#endif // MEMORY_POOL_HPP
