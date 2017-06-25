#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <memory>

#include <tok.hpp>
#include <defines.hpp>
#include <tuple>

enum class astt : uint8_t
{
	VAR,
	ABS,
	APP
};

class ast;
typedef ast* ast_node_t;

struct ast
{
	ast()
		: var(0), type(astt::VAR)
	{ }
	ast(var_t v)
		: var(v), type(astt::VAR)
	{ }
	ast(astt t, ast_node_t const& p2)
		: c2(p2), c1(), type(t)
	{ }
	ast(astt t, ast_node_t const& p1, ast_node_t const& p2)
		: c2(p2), c1(p1), type(t)
	{ }
	ast(ast_node_t const& p);
	ast(ast_node_t&& p);
	~ast();

	inline std::size_t get_height() const
	{
		if (this->type == astt::VAR)
			return 1;
		else if (this->type == astt::ABS)
			return (this->c2->get_height() +1);
		else
			return std::max(this->c1->get_height(), this->c2->get_height()) +1;
	}

	inline std::size_t get_width_r() const
	{
		if (type == astt::VAR)
			return 1;
		else
			return c2->get_width_r() +c2->get_width_l();
	}

	inline std::size_t get_width_l() const
	{
		if (type == astt::VAR)
			return 1;
		else if (type == astt::ABS)
			return c2->get_width_l() +c2->get_width_r();
		else
			return c1->get_width_l() +c1->get_width_r();
	}

	inline std::size_t get_node_count() const
	{
		if (this->type == astt::VAR)
			return 1;
		else if (type == astt::ABS)
			return this->c2->get_node_count() +1;
		else
			return this->c1->get_node_count() +this->c2->get_node_count() +1;
	}
	std::wstring to_str() const;

	union
	{
		struct
		{
			ast_node_t c2, c1;
		};
		var_t var;
	};
	astt type;

private:
	void to_str_rec(std::wostringstream& ss, bool place_bracket, int rec) const;
};

#include "traits_allocator.hpp"

#endif // AST_HPP
