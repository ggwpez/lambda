#include <iostream>
#include <parser.hpp>

#define ERR(msg) do { throw(L"Error: " + std::wstring(msg)); } while (0)
#define PAR_ERR(msg) do { ERR(L"Parsing: " + std::wstring(msg)); } while (0)
#define AWAITED(c) do { PAR_ERR(L"Awaited: '" + std::wstring(c) + L"'"); } while (0)
#define UNAWAITED(c) do { PAR_ERR(L"Unawaited: '" + std::wstring(c) + L"'"); } while (0)

#define assert(tok, t) do { if (tok.type != t) AWAITED(tokt_strings[(int)t]); } while (0)

ast_node_t parser::parse()
{
    unsigned l = 0;
    if (toks.size() < 2)
        return ast_node_t();

    try
    {
        ast_node_t ret = parse_exp(0, l);
        assert(toks[l], tokt::END);

        return ret;
    }
    catch (std::wstring const& str)
    {
        std::wcerr << L"Cought parsing error: " << str << std::endl;
        return ast_node_t();
    }
    catch (...)
    {
        std::wcerr << L"Internal parser exception." << std::endl;
        return ast_node_t();
    }
}

std::vector<ast_node_t> parser::parse_prog(unsigned const s, unsigned& l)
{
    std::vector<ast_node_t> ret;
    unsigned tmp_l = 0; l = 0;

    while (((s +l) +1) < toks.size())
    {
        tmp_l = 0;
        ast_node_t tmp = parse_exp(s +l, tmp_l);
        l += tmp_l;
        ret.push_back(tmp);

        while (toks[s +l].type == tokt::SEMI)
            l++;
    }

    return ret;
}

ast_node_t parser::parse_exp(unsigned const s, unsigned& l)
{
    unsigned exp_l = 0;

    ast_node_t ret = parse_ter(s, exp_l);

    while ((s +(l += exp_l) +1) < toks.size())
    {
        if ((toks[s +l].type == tokt::KZ) || (toks[s +l].type == tokt::IN) || (toks[s +l].type == tokt::SEMI))
            break;

        exp_l = 0;
        ast_node_t got = parse_ter(s +l, exp_l);

        ret = ast_traits::alloc(astt::APP, ret, got);
    }

    return ret;
}

ast_node_t parser::parse_ter(unsigned const s, unsigned& l)
{
    if (toks[s].type == tokt::VAR)
        return parse_var(s, l);
    else if (toks[s].type == tokt::LAM)
        return parse_abs(s, l);
    else if (toks[s].type == tokt::KA)
        return parse_par(s, l);
    else if (toks[s].type == tokt::LET)
        return parse_let(s, l);
    else if (toks[s].type == tokt::NUM)
        return parse_num(s, l);
    else
        UNAWAITED(tokt_strings[(int)toks[s].type]);
}

// parenthese term
ast_node_t parser::parse_par(unsigned const s, unsigned& l)
{
    unsigned exp_l = 0;

    ast_node_t ret = parse_exp(s +1, exp_l);
    l = 1 +exp_l +1;

    if (toks[s +l -1].type != tokt::KZ)
        AWAITED(L")");

    return ret;
}

ast_node_t parser::parse_abs(unsigned const s, unsigned& l)
{
    unsigned var_l = 0, exp_l = 0;

    assert(toks[s], tokt::LAM);                 l = 1;
    ast_node_t var = parse_var(s +l, var_l);    l += var_l;
    assert(toks[s +l], tokt::DOT);              l++;
    ast_node_t exp = parse_exp(s +l, exp_l);    l += exp_l;

    return ast_traits::alloc(astt::ABS, var, exp);
}

ast_node_t parser::parse_var(unsigned const s, unsigned& l)
{
    assert(toks[s], tokt::VAR);
    l = 1;

    return ast_traits::alloc(toks[s].var);
}

ast_node_t parser::parse_num(unsigned const s, unsigned& l)
{
    assert(toks[s], tokt::NUM);         l = 1;

    unsigned long n = toks[s].num;
    ast_node_t x = ast_traits::alloc(L"x"),
               f = ast_traits::alloc(L"f"),
               app = x;

    while(n--)
    {
        auto tmp = ast_traits::alloc(f);
        app = ast_traits::alloc(astt::APP, tmp, app);
    }

    auto tmp = ast_traits::alloc(x);
    ast_node_t abst_x = ast_traits::alloc(astt::ABS, tmp, app);
    ast_node_t abst_f = ast_traits::alloc(astt::ABS, f, abst_x);

    return abst_f;
}

// let var = value in exp
// becomes:
// ((\var. exp) value)
ast_node_t parser::parse_let(unsigned const s, unsigned& l)
{
    unsigned var_l = 0, val_l = 0, exp_l = 0;

    assert(toks[s], tokt::LET);                 l = 1;
    ast_node_t var = parse_var(s +l, var_l);    l += var_l;
    assert(toks[s +l], tokt::ASS);              l++;
    ast_node_t value = parse_exp(s +l, val_l);  l += val_l;
    assert(toks[s +l], tokt::IN);               l++;
    ast_node_t exp = parse_exp(s +l, exp_l);    l += exp_l;

    ast_node_t abst = ast_traits::alloc(astt::ABS, var, exp);
    return ast_traits::alloc(astt::APP, abst, value);
}
