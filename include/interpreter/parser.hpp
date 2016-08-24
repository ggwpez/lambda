#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>

#include "tok.hpp"
#include "ast.hpp"

class parser
{
public:
    parser(std::vector<tok> const& input)
        : toks(input)
    { }

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
};

#endif // PARSER_HPP
