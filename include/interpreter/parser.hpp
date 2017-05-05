#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <unordered_map>
#include <tuple>

#include "tok.hpp"
#include "ast.hpp"

class parser
{
public:
	parser(std::vector<tok> const& input)
		: toks(input)
	{ }
	~parser()
	{
		for (auto& fv : free_vars)
			ast_traits::free(fv.second);
	}

	std::vector<ast_node_t> parse_prog(unsigned const s, unsigned& l);
	ast_node_t parse();

private:
	ast_node_t parse_exp(unsigned const s, unsigned& l);
	ast_node_t parse_ter(unsigned const s, unsigned& l);
	ast_node_t parse_par(unsigned const s, unsigned& l);
	ast_node_t parse_abs(unsigned const s, unsigned& l);
	ast_node_t parse_app(unsigned const s, unsigned& l);
	ast_node_t parse_var(unsigned const s, unsigned& l);
	ast_node_t parse_let(unsigned const s, unsigned& l);
	ast_node_t parse_num(unsigned const s, unsigned& l);

	ast_node_t parse_abs_rest(unsigned const s, unsigned& l);

	std::vector<tok> const& toks;

	void bind_var(std::wstring const& var);
	void unbind_var(std::wstring const& var);
	bool is_bound(std::wstring const& var);
	var_t get_bound_var(std::wstring const& var);
	ast_node_t get_free_var(std::wstring const& var);


	std::vector<std::tuple<std::wstring, var_t>> bound_vars;
	std::unordered_map<std::wstring, ast_node_t> free_vars;
};

#endif // PARSER_HPP
