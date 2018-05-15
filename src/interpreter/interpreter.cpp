#include "interpreter.hpp"
#include "defines.hpp"

#include <iostream>
#include <sstream>
#include <cassert>
#include <array>

interpreter::interpreter(inter_ops_t ops)
	: op(ops), betas(0), etas(0), steps(0)
{ }

void interpreter::free_change(ast_node_t& tree, int dir, int rec)
{
	if (tree->type == astt::APP)
	{
		free_change(tree->c1, dir, rec);
		free_change(tree->c2, dir, rec);
	}
	else if (tree->type == astt::VAR)
	{
		if (tree->var > rec)		// is it free?
			tree->var += dir;
	}
	else if (tree->type == astt::ABS)
	{
		free_change(tree->c2, dir, rec +1);
	}

	else
		assert("Unreachable" && 0);
}

void interpreter::substitute_and_dec_free(ast_node_t& M, ast_node_t const& N, int rec, int abs = 0)
{
	if (M->type == astt::APP)
	{
		substitute_and_dec_free(M->c2, N, rec, abs);
		substitute_and_dec_free(M->c1, N, rec, abs);
	}
	else if (M->type == astt::VAR)
	{
		if (M->var == rec)				// bound by the outermost lambda?
		{
			ast_traits::free(M);
			M = ast_traits::alloc(N);
			free_change(M, abs, 0);
		}
		else if (M->var > rec)			// is it free?
			--M->var;
	}
	else if (M->type == astt::ABS)
		substitute_and_dec_free(M->c2, N, rec +1, abs +1);
	else
		assert("Unreachable" && 0);
}

bool interpreter::is_bound_in(ast_node_t const& tree, var_t var)
{
	if (tree->type == astt::APP)
	{
		return (is_bound_in(tree->c1, var)
			 || is_bound_in(tree->c2, var));
	}
	else if (tree->type == astt::VAR)
		return (tree->var == var);
	else if (tree->type == astt::ABS)
		return is_bound_in(tree->c2, var +1);
	else
		assert("Unreachable" && 0);
}

inter_ret_state_t interpreter::inter(ast_node_t& tree, inter_info_t& info)
{
	this->betas = this->etas = this->steps = 0;
	this->cancel = false;
	while (eval(tree));

	info = inter_info_t(this->betas, this->etas, this->steps, 0);
	if (this->steps >= MAX_STEPS)
		return inter_ret_state_t::TIMEOUT;
	else if (cancel)
		return inter_ret_state_t::CANCEL;
	else
		return inter_ret_state_t::OK;
}


bool interpreter::eval(ast_node_t& tree)
{
	assert(tree);

	if (cancel || this->steps++ >= MAX_STEPS)
		return false;

	if (tree->type == astt::VAR)
		return false;
	else if (tree->type == astt::APP)
	{
		if (tree->c1->type == astt::ABS)			// BETA: ((\ M) N)
		{
			ast_node_t& M = tree->c1->c2;
			ast_node_t& N = tree->c2;

			substitute_and_dec_free(M, N, 1);
			// Anti leak magic
			ast_node_t new_M(M);
			M = nullptr;
			ast_traits::free(tree);

			tree = new_M;
			return ++this->betas;
		}
		else
		{
			bool r1(eval(tree->c1)),
				 r2(false);

			if (tree->c1->type == astt::VAR)	// eval args after call
				r2 = eval(tree->c2);

			return (r1 || r2);
		}
	}
	else if (tree->type == astt::ABS)
	{
		if (tree->c2->type == astt::APP
			&& tree->c2->c2->type == astt::VAR
			&& tree->c2->c2->var == 1
			&& ! is_bound_in(tree->c2->c1, 1))		// ETA: (\ (M 1))
		{
			ast_node_t redex(tree->c2->c1);
			tree->c2->c1 = nullptr;
			ast_traits::free(tree);

			tree = redex;
			free_change(tree, -1, 0);

			return ++this->etas;
		}
		else
			return eval(tree->c2);
	}
	else
		assert("Unreachable" && 0);
}

void interpreter::cancel_calculation()
{
	this->cancel = true;
}

void interpreter::set_mode(inter_ops_t new_op)
{
	this->op = new_op;
}
