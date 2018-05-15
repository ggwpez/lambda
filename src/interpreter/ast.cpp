#include <ast.hpp>
#include <sstream>
#include <unordered_map>

ast::ast(ast_node_t const& p)
	: type(p->type)
{
	if (p->type == astt::VAR)
		this->var = p->var;
	else if (p->type == astt::APP)
	{
		c1 = ast_traits::alloc(p->c1);
		c2 = ast_traits::alloc(p->c2);
	}
	else if (p->type == astt::ABS)
		c2 = ast_traits::alloc(p->c2);

}

ast::ast(ast_node_t&& p)
{
	this->type = p->type;

	if (p->type == astt::VAR)
		this->var = std::move(p->var);
	else if (p->type == astt::APP)
	{
		this->c1 = std::move(p->c1);
		this->c2 = std::move(p->c2);
	}
	else if (p->type == astt::ABS)
		this->c2 = std::move(p->c2);
}

ast::~ast()
{
	if (this->type == astt::APP)
	{
		if (c1) ast_traits::free(c1);
		if (c2) ast_traits::free(c2);
	}
	else if (this->type == astt::ABS && c2)
		ast_traits::free(c2);
}

static std::unordered_map<var_t, std::wstring> de_brujin;
std::wstring ast::to_str() const
{
	std::wostringstream ss;

	this->to_str_rec(ss, false, 0);

	return ss.str();
}

std::wstring var_name(int h)
{
	if (h < 26)
		return std::wstring(1, h +L'a');
	else
		return std::wstring(L"t") +std::to_wstring(h);
}

void ast::to_str_rec(std::wostringstream& ss, bool place_bracket, int rec) const
{
	if (type == astt::VAR)
		ss << var;
	else
	{
		if (place_bracket)
			ss << L'(';
		if (type == astt::ABS)
		{
			ss << L"λ ";
			c2->to_str_rec(ss, false, rec +1);
		}
		else if (type == astt::APP)
			c1->to_str_rec(ss, true, rec), ss << L' ', c2->to_str_rec(ss, true, rec);
		if (place_bracket)
			ss << L')';
	}
}
