#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>

#include "tok.hpp"

class lexer
{
public:
    lexer(std::wstring const& in)
        : input(in) {}

    std::vector<tok>* lex();

private:
    tok lex_num(unsigned const s, unsigned& l);
    tok lex_var(unsigned const s, unsigned& l);
    tok lex_str(unsigned const s, unsigned& l);
    tok inter_string(std::wstring const& str);

    std::wstring input;     //has to be a copy
};

#endif // LEXER_HPP
