#include <ast.hpp>
#include <sstream>

ast::ast(ast_node_t const& p)
    : type(p->type), c1(), c2(), var()
{
    this->type = p->type;

    if (p->type == astt::VAR)
        this->var = p->var;
    else if (p->type == astt::ABS || p->type == astt::APP)
    {
        c1 = ast_traits::alloc(p->c1);
        c2 = ast_traits::alloc(p->c2);
    }
}

std::wstring ast::to_str()
{
    if (type == astt::VAR)
        return var;

    std::wostringstream ss;

    ss << L'(';
    if (type == astt::ABS)
        ss << L'λ', c1->to_str_rec(ss), ss << L". ", c2->to_str_rec(ss);
    else
        c1->to_str_rec(ss), ss << L' ', c2->to_str_rec(ss);

    ss << L')';
    return ss.str();
}

void ast::to_str_rec(std::wostringstream& ss)
{
    if (type == astt::VAR)
    {
        ss << var;
        return;
    }

    ss << L'(';
    if (type == astt::ABS)
        ss << L'λ', c1->to_str_rec(ss), ss << L". ", c2->to_str_rec(ss);
    else
        c1->to_str_rec(ss), ss << L' ', c2->to_str_rec(ss);
    ss << L')';
}
