#ifndef SCOPE_HPP
#define SCOPE_HPP

#include <unordered_map>
#include <type_traits>

#include "tok.hpp"
#include "ast.hpp"

class interpreter;
typedef ast_node_t(*inbuild_ptr_t)(interpreter*, ast_node_t&);

enum class scope_entry_tt : unsigned
{
    NONE = 0,
    INBUILD,
    AST
};

struct scope_entry_t
{
    scope_entry_t()
        : type(scope_entry_tt::NONE), ptr(nullptr), ast()
    { }
    scope_entry_t(inbuild_ptr_t fptr)
        : type(scope_entry_tt::INBUILD), ptr(fptr), ast()
    { }
    scope_entry_t(ast_node_t const& value)
        : type(scope_entry_tt::AST), ptr(nullptr), ast(value)
    { }

    scope_entry_tt type;
    PAD(4);
    inbuild_ptr_t ptr;
    ast_node_t ast;
};

static_assert(std::is_same<bool, decltype(std::declval<var_t&>() == std::declval<var_t&>())>::value, "var_t musst be == compareable");  // i like traitors
class scope
{
public:
    scope()
        : defines()
    { }

    void define_value(var_t const& name, scope_entry_t value);
    scope_entry_t* get_value(var_t const& name);

private:
    std::unordered_map<var_t, scope_entry_t> defines;
};

#endif // SCOPE_HPP
