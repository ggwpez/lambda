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

ast::ast(ast_node_t&& p)
{
    this->type = p->type;

    if (p->type == astt::VAR)
        this->var = std::move(p->var);
    else if (p->type == astt::ABS || p->type == astt::APP)
    {
        this->c1 = std::move(p->c1);
        this->c2 = std::move(p->c2);
    }
}

std::wstring ast::to_str()
{
    std::wostringstream ss;

    this->to_str_rec(ss);

    return ss.str();
}

void ast::to_str_rec(std::wostringstream& ss)
{
    if (type == astt::VAR)
        ss << var;
    else
    {
        ss << L'(';
        if (type == astt::ABS)
        {
            ast_node_t& mb_abs = c2;

            ss << L'λ', c1->to_str_rec(ss);
            while (mb_abs->type == astt::ABS)
            {
                ss << L' ';
                mb_abs->c1->to_str_rec(ss);
                mb_abs = mb_abs->c2;
            }
            ss << L". ", mb_abs->to_str_rec(ss);
        }
        else if (type == astt::APP)
            c1->to_str_rec(ss), ss << L' ', c2->to_str_rec(ss);
        else
            ss << L"define ", c1->to_str_rec(ss), ss << L" = ", c2->to_str_rec(ss);
        ss << L')';
    }
}
