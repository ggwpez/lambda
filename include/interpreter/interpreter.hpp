#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <cstdint>
#include <vector>
#include <set>

#include "inter_info.hpp"
#include "ast.hpp"

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
    OK,
    MAX_SUBST_ITER_REACHED,
    ERR
};

class interpreter
{
public:
    interpreter(inter_ops_t ops = inter_ops_t::DEFAULT);
    ast_node_t inter(ast_node_t &tree, inter_ret_state_t& state, uint64_t steps, inter_info_t &in);
    ast_node_t single_step(ast_node_t &tree, bool &changed, inter_info_t* info);

private:
    ast_node_t alpha_red(ast_node_t& E, var_t v, ast_node_t& N);
    ast_node_t beta_red(ast_node_t& tree);
    ast_node_t eta_red(ast_node_t& tree);

    bool is_beta_reducable(ast_node_t const& tree);
    bool is_eta_reducable(ast_node_t const& tree);

    bool is_term_closed(ast_node_t const& tree);
    bool is_term_closed_rec(ast_node_t const& tree, std::set<var_t>& acc);

    bool is_free_in(ast_node_t const& tree, var_t &x);
    //std::set<var_t> FV(ast_node_t const& tree);
    //void FV_rec(ast_node_t const& tree, std::set<var_t>& acc);

    var_t get_z();

    inter_ops_t op;
    unsigned z, alphas, betas, etas;
    PAD(4);
};

#endif // INTERPRETER_HPP
