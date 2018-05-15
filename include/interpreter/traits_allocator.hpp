#ifndef AST_TRAITOR_HPP
#define AST_TRAITOR_HPP

#include "ast.hpp"
#include "defines.hpp"
#include "memory_pool.hpp"

#define USE_POOL 0
#define POOL_SIZE 300000

extern pool<ast, POOL_SIZE> global_pool;

static unsigned mallocs = 0;

template<typename T>
struct allocator{};

template<>
struct allocator<ast*>
{
	template<class ...args>
	static inline ast* alloc(args&&... a)
	{
		mallocs++;

		if (USE_POOL)
			return global_pool._new(std::forward<args>(a)...);
		else
			return new ast(std::forward<args>(a)...);
	}

	static inline void free(ast* obj)
	{
#ifndef SLEAZY_CLEANUP
		if (USE_POOL)
			global_pool._delete(obj);
		else
			delete obj;
#else
		(void)obj;
#endif
	}

	static inline bool equals(ast* const& obj1, ast* const& obj2)
	{
		return (obj1 == obj2);
	}

	static inline bool is_nullptr(ast* ptr)
	{
		return !bool(ptr);
	}

	typedef std::shared_ptr<ast> ptr_move_t;
};

typedef allocator<ast_node_t> ast_traits;


#endif // AST_TRAITOR_HPP
