#include <lexer.hpp>

#include <iostream>

#define ERR(msg) do { throw std::wstring(msg); /*std::wcerr << L"Error: " << msg << std::endl; std::exit(EXIT_FAILURE);*/   } while (0)

std::vector<tok>* lexer::lex()
{
    wchar_t c;
    unsigned s = 0, i = 0;
    std::vector<tok>* ret = new std::vector<tok>();

    try
    {
        while ((s += i) < input.length() && (c = input[s]))
        {
            i = 1;
            if (c == L'#' || c == L'λ' || c == L'\\')
                ret->push_back(tok(tokt::LAM));
            else if (std::iswalpha(c))                      //actually λ is a walpha, wtf
                ret->push_back(lex_var(s, i));
            else if (std::iswdigit(c))
                ret->push_back(lex_num(s, i));
            else if (c == L'"')
                ret->push_back(lex_str(s, i));
            else if (c == L';')
                continue;
            else if (c == L'(')
                ret->push_back(tok(tokt::KA));
            else if (c == L')')
                ret->push_back(tok(tokt::KZ));
            else if (c == L'.')
                ret->push_back(tok(tokt::DOT));
            else if (c == L'=')
                ret->push_back(tok(tokt::ASS));
            else if (std::isspace(c))
                continue;
            else
                ERR(L"Unawaited: " +std::wstring(1, c));
        }

        if (s != input.length())
            ERR(L"Awaited EOF");

        ret->push_back(tok(tokt::END));
        return ret;
    }
    catch (std::wstring const& str)
    {
        std::wcerr << L"Cought lexing error: " << str << std::endl;
        ret->clear();
        ret->push_back(tok(tokt::END));
        return ret;
    }
    catch (...)
    {
        std::wcerr << L"Internal lexer exception." << std::endl;
        ret->clear();
        ret->push_back(tok(tokt::END));
        return ret;
    }
}

tok lexer::inter_string(std::wstring const& str)
{
    if (str == L"let")
        return tok(tokt::LET);
    else if (str == L"in")
        return tok(tokt::IN);
    else
        return tok(str);
}

tok lexer::lex_num(unsigned const s, unsigned& l)
{
    l = 0;

    while ((s +l) < input.length() && std::iswdigit(input[s +l]))
        l++;

    if (!l)
        ERR(L"Awaited: number");

    unsigned long n = std::stoul(input.substr(s, l));
    return tok(n);
}

tok lexer::lex_var(unsigned const s, unsigned& l)
{
    l = 0;

    while ((s +l) < input.length() && (std::iswalnum(input[s +l]) || input[s +l] == L'_'))
        l++;

    if (!l)
        ERR(L"Awaited: variable");

    return inter_string(input.substr(s, l));
}

tok lexer::lex_str(unsigned const s, unsigned& l)
{
    l = 1;

    while (((s +l) < input.length()) && input[s +l] != L'"')
        l++;

    if (input[l +s] != L'"')
        ERR(L"Awaited: '\"'");

    return tok(input.substr(s, ++l));
}
