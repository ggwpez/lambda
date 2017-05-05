#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <cstdint>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "inter_info.hpp"
#include "scope.hpp"
#include "ast.hpp"

#define MAX_STEPS (1ull << 32)

enum class inter_ops_t : unsigned
{
	NONE = 0,
	DEFAULT = 8,
	TRACE = 1,
	TRACE_ALL = 2,
	LAZY = 4,
	AUTO_REP_ON_OV = 8,
};

enum class inter_ret_state_t : unsigned
{
	OK = 0,
	TIMEOUT,
	CANCEL,
	ERR
};

class interpreter
{
public:
	interpreter(inter_ops_t ops = inter_ops_t::DEFAULT);

	inter_ret_state_t inter(ast_node_t& tree, inter_info_t& info);
	bool eval(ast_node_t& tree);
	void free_change(ast_node_t& tree, int dir, int rec);
	void substitute_and_dec_free(ast_node_t& M, ast_node_t const& N, int rec, int abs);
	bool is_bound_in(ast_node_t const& tree, var_t var);

	void cancel_calculation();
	void set_mode(inter_ops_t new_op);

	inter_ops_t op;

	std::uint64_t betas, etas, steps;
	bool cancel = false;
};

#endif // INTERPRETER_HPP
