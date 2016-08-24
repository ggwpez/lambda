#ifndef TOK_HPP
#define TOK_HPP

#include <defines.hpp>

#include <string>

typedef std::wstring var_t;

enum class tokt : unsigned
{
    LAM = 0,
    VAR,
    NUM,
    DOT,
    KA,
    KZ,
    LET,
    ASS,
    IN,
    SEMI,
    END
};

extern wchar_t const* const tokt_strings[];

struct tok
{
    tok(tokt t)
        : type(t), num(), var()
    { }
    tok(std::wstring const& v)
        : type(tokt::VAR), num(), var(v)
    { }
    tok(unsigned long n)
        : type(tokt::NUM), num(n), var()
    { }

    std::wstring to_str() const;

    tokt type;
    PAD(4);
    unsigned long num;
    std::wstring var;
};

#endif // TOK_HPP
