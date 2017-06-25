#include <iostream>
#include <cassert>
#include <parser.hpp>

#define ERR(msg) do { throw(L"Error: " + std::wstring(msg)); } while (0)
#define PAR_ERR(msg) do { ERR(L"Parsing: " + std::wstring(msg)); } while (0)
#define AWAITED(c) do { PAR_ERR(L"Awaited: '" + std::wstring(c) + L"'"); } while (0)
#define UNAWAITED(c) do { PAR_ERR(L"Unawaited: '" + std::wstring(c) + L"'"); } while (0)

#undef assert
#define assert(tok, t) do { if (tok.type != t) AWAITED(tokt_strings[(int)t]); } while (0)

void parser::bind_var(std::wstring const& var)
{
	bound_vars.push_back(std::make_tuple(var, bound_vars.size()));
}

bool parser::is_bound(std::wstring const& var)
{
	for (auto it(bound_vars.rbegin()); it != bound_vars.rend(); ++it)
	{
		if (std::get<0>(*it) == var)
			return true;
	}

	return false;
}

var_t parser::get_bound_var(std::wstring const& var)
{
	for (auto it(bound_vars.rbegin()); it != bound_vars.rend(); ++it)
	{
		if (std::get<0>(*it) == var)
			return bound_vars.size() -std::get<1>(*it);
	}

	throw "Error\n";
}

ast_node_t parser::get_free_var(std::wstring const& var)
{
	auto it(free_vars.find(var));

	if (it != free_vars.end())
		return ast_traits::alloc(it->second);

	return ast_traits::alloc((free_vars[var] = ast_traits::alloc(bound_vars.size() +free_vars.size())));
}

void parser::unbind_var(std::wstring const& var)
{
	if(var != std::get<0>(bound_vars.back()))
		throw "Error\n";
	bound_vars.pop_back();
}

ast_node_t parser::parse()
{
	bound_vars = std::vector<std::tuple<std::wstring, var_t>>();
	free_vars  = std::unordered_map<std::wstring, ast_node_t>();

	unsigned l(0);
	if (toks.size() < 2)
		return ast_traits::alloc();

	try
	{
		ast_node_t ret(parse_exp(0, l));
		assert(toks[l], tokt::END);

		return ret;
	}
	catch (std::wstring const& str)
	{
		std::wcerr << L"Caught Exception: " << str << std::endl;
		return ast_node_t();
	}
	catch (std::exception const& e)
	{
		std::wcerr << L"Caught Exception: " << e.what() << std::endl;
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
	unsigned tmp_l(0); l = 0;

	while (((s +l) +1) < toks.size())
	{
		tmp_l = 0;
		ast_node_t tmp(parse_exp(s +l, tmp_l));
		l += tmp_l;
		ret.push_back(tmp);

		while (toks[s +l].type == tokt::SEMI)
			l++;
	}

	return ret;
}

ast_node_t parser::parse_exp(unsigned const s, unsigned& l)
{
	unsigned exp_l(0);

	ast_node_t ret(parse_ter(s, exp_l));

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
	unsigned exp_l(0);

	ast_node_t ret(parse_exp(s +1, exp_l));
	l = 1 +exp_l +1;

	if (toks[s +l -1].type != tokt::KZ)
		AWAITED(L")");

	return ret;
}

// lambda
ast_node_t parser::parse_abs(unsigned const s, unsigned& l)
{
	unsigned abs_l(0);

	assert(toks[s], tokt::LAM);                     l = 1;
	ast_node_t abs(parse_abs_rest(s +l, abs_l));	l += abs_l;

	return abs;
}

ast_node_t parser::parse_abs_rest(unsigned const s, unsigned& l)
{
	assert(toks[s], tokt::VAR);
	std::wstring const binder(toks[s].var);			l = 1;
	bind_var(binder);

	// is it an abstraction with multiple bound variabled, aka. '\x y.' ?
	if (toks[s +l].type == tokt::VAR)
	{
		unsigned abs_l(0);
		ast_node_t abs(parse_abs_rest(s +l, abs_l)); l += abs_l;

		unbind_var(binder);
		return ast_traits::alloc(astt::ABS, abs);
	}
	else
	{
		unsigned exp_l(0);
		assert(toks[s +l], tokt::DOT);              l++;
		ast_node_t exp(parse_exp(s +l, exp_l));		l += exp_l;

		unbind_var(binder);
		return ast_traits::alloc(astt::ABS, exp);
	}

	unbind_var(binder);
	exit(-1);
}

ast_node_t parser::parse_var(unsigned const s, unsigned& l)
{
	assert(toks[s], tokt::VAR);
	l = 1;

	if (is_bound(toks[s].var))
		return ast_traits::alloc(get_bound_var(toks[s].var));
	else
		return get_free_var(toks[s].var);
}

ast_node_t parser::parse_num(unsigned const s, unsigned& l)
{
	assert(toks[s], tokt::NUM);         l = 1;

	unsigned long n(toks[s].num);
	ast_node_t x(ast_traits::alloc(1)),
			   app(x);

	while(n--)
	{
		ast_node_t tmp(ast_traits::alloc(2));
		app = ast_traits::alloc(astt::APP, tmp, app);
	}

	ast_node_t abst_x(ast_traits::alloc(astt::ABS, app));
	ast_node_t abst_f(ast_traits::alloc(astt::ABS, abst_x));

	return abst_f;
}

// let var = value in exp
// becomes:
// ((\var. exp) value)
ast_node_t parser::parse_let(unsigned const s, unsigned& l)
{
	unsigned val_l(0), exp_l(0);

	assert(toks[s], tokt::LET);                 l = 1;
	assert(toks[s +l], tokt::VAR);
	std::wstring const binder(toks[s +l].var);	l++;

	assert(toks[s +l], tokt::ASS);              l++;
	ast_node_t value(parse_exp(s +l, val_l));	l += val_l;

	bind_var(binder);
	assert(toks[s +l], tokt::IN);               l++;
	ast_node_t exp(parse_exp(s +l, exp_l));		l += exp_l;

	ast_node_t abst(ast_traits::alloc(astt::ABS, exp));

	unbind_var(binder);
	return ast_traits::alloc(astt::APP, abst, value);
}
