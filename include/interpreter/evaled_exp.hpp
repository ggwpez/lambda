#ifndef EVALED_EXP_HPP
#define EVALED_EXP_HPP

#include "ast.hpp"
#include "interpreter.hpp"


#include <string>
#include <memory>

struct evaled_exp
{
	evaled_exp()
		: state(inter_ret_state_t::ERR), input(), tree(), info()
	{ }
	evaled_exp(inter_ret_state_t s, std::wstring in, ast_node_t& t, const inter_info_t &i)
		: state(s), input(in), tree(t), info(i)
	{ }
	~evaled_exp()
	{ }

	inter_ret_state_t state;
	PAD(4);
	std::wstring input;
	ast_traits::ptr_move_t tree;    //always auto free
	inter_info_t info;
};

#endif // EVALED_EXP_HPP
