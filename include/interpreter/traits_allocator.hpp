#ifndef AST_TRAITOR_HPP
#define AST_TRAITOR_HPP

#include "ast.hpp"
#include "defines.hpp"
#include "object_pool.hpp"

#define USE_POOL 0
#define POOL_SIZE 30000

extern object_pool<ast, POOL_SIZE> pool;

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
            return pool.create(std::forward<args>(a)...);
        else
            return new ast(std::forward<args>(a)...);
    }

    static inline void free(ast* obj)
    {
        if (USE_POOL)
            pool.destroy(obj);
        else
            delete obj;
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

template<>
struct allocator<std::shared_ptr<ast>>
{
    template<class ...args>
    static inline std::shared_ptr<ast> alloc(args&&... a)
    {
        mallocs++;
        if (USE_POOL)
            return std::shared_ptr<ast>(pool.create(std::forward<args>(a)...), allocator<ast*>::free);
        else
            return std::make_shared<ast>(std::forward<args>(a)...);
    }

    static inline void free(std::shared_ptr<ast>& obj)
    {
        (void)obj;
        //obj.reset();
    }

    static inline bool equals(std::shared_ptr<ast> const& obj1, std::shared_ptr<ast> const& obj2)
    {
        return (obj1.get() == obj2.get());
    }

    static inline bool is_nullptr(std::shared_ptr<ast> const& ptr)
    {
        return !bool(ptr);
    }

    typedef std::shared_ptr<ast> ptr_move_t;
};
typedef allocator<ast_node_t> ast_traits;


#endif // AST_TRAITOR_HPP
