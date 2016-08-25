#include <interpreter.hpp>
#include <defines.hpp>

#include <iostream>
#include <sstream>
#include <array>

interpreter::interpreter(inter_ops_t ops)
    : op(ops), z(0), alphas(0), betas(0), etas(0)
{ }

ast_node_t interpreter::inter(ast_node_t& tree, inter_ret_state_t& state, uint64_t steps, inter_info_t& in)
{
    cancel = false;
    state = inter_ret_state_t::OK;
    ast_node_t& ret = tree;
    inter_info_t tmp;
    in = inter_info_t{0,0,0,0,0};
    bool changed = true;
    uint64_t sum_steps = 0;

    if (ast_traits::is_nullptr(tree))
        return ast_node_t();

    while (!cancel && changed && (sum_steps < steps))
    {
        ast_node_t&& nret(single_step(ret, changed, &tmp));
        in += tmp;

        if (! ast_traits::equals(ret, tree))
            ast_traits::free(ret);

        ret = std::move(nret);
        sum_steps++;
    }

    if (sum_steps >= steps)
        state = inter_ret_state_t::MAX_SUBST_ITER_REACHED;
    if (cancel)
        state = inter_ret_state_t::CANCEL;

    return ret;//(ast_traits::equals(ret, tree) ? ast_traits::alloc(tree) : ret);
}

ast_node_t interpreter::single_step(ast_node_t& tree, bool& changed, inter_info_t* info)
{
    if (ast_traits::is_nullptr(tree))
        return ast_node_t();

    alphas = betas = etas = mallocs = 0;
    int oz = z;

    changed = true;

    if (is_beta_reducable(tree))
    {
        beta_red(tree);
    }
    else if (is_eta_reducable(tree))
    {
       eta_red(tree);
    }
    else
    {
        changed = false;
    }

    if (info)
        *info = inter_info_t(alphas, betas, etas, z -oz, tree->get_node_count());
    return tree;
}

void interpreter::cancel_calculation()
{
    this->cancel = true;
}

void interpreter::alpha_red(ast_node_t& E, var_t& v, ast_node_t& N)
{
    alphas++;
    if (E->type == astt::VAR)
    {
        var_t& var = E->var;

        if (var == v)
            E = ast_traits::alloc(N);
        else
            return;    //###
    }
    else if (E->type == astt::APP)
    {
        alpha_red(E->c1, v, N);
        alpha_red(E->c2, v, N);
    }
    else if (E->type == astt::ABS)
    {
        var_t& x = E->c1->var;

        if (x == v)
        {  }
        else
        {
            if (is_not_free_in(N, x))
            {
                alpha_red(E->c2, v, N);
            }
            else
            {
                ast_node_t var_z = ast_traits::alloc(get_tmp());

                E->c1 = var_z;
                alpha_red(E->c2, x, var_z);
                alpha_red(E->c2, v, N);
            }
        }
    }

    return;
}

bool interpreter::is_beta_reducable(ast_node_t const& tree)
{
    if (tree->type == astt::ABS)
        return is_beta_reducable(tree->c2);
    else if (tree->type == astt::APP)
    {
        if (tree->c1->type == astt::ABS)
            return true;
        else
            return is_beta_reducable(tree->c1) || is_beta_reducable(tree->c2);
    }
    else
        return false;
}

void interpreter::beta_red(ast_node_t& tree)
{
    betas++;
    if (tree->type == astt::VAR)
    {  }
    else if (tree->type == astt::ABS)
    {
        beta_red(tree->c2);
    }
    else if (tree->type == astt::APP)
    {
        if (tree->c1->type == astt::ABS)        //lambda term gets called
        {
            ast_node_t& lambda = tree->c1;
            var_t& abst_var = lambda->c1->var;

            alpha_red(lambda->c2, abst_var, tree->c2);

            tree = lambda->c2;      // CARE ###
        }
        else
        {
            beta_red(tree->c1);

            if (!((int)this->op & (int)inter_ops_t::LAZY))
                beta_red(tree->c2);
        }
    }
    else
        std::terminate();
}

bool interpreter::is_eta_reducable(ast_node_t const& tree)
{
    if (tree->type == astt::VAR)
        return false;
    else if (tree->type == astt::APP)
        return is_eta_reducable(tree->c1) || is_eta_reducable(tree->c2);
    else if (tree->type == astt::ABS)
    {
        if (tree->c2->type == astt::APP &&
            tree->c2->c2->type == astt::VAR &&
            tree->c2->c2->var == tree->c1->var)
        {
            return is_not_free_in(tree->c2->c1, tree->c1->var);
        }
        else
            return is_eta_reducable(tree->c2);
    }
    else
        std::terminate();
}

void interpreter::eta_red(ast_node_t& tree)
{
    //if (! is_eta_reducable(tree))       //TODO: check speed
      //  return tree;

    etas++;
    if (tree->type == astt::VAR)
        return;     //###
    else if (tree->type == astt::APP)
    {
        eta_red(tree->c1);
        eta_red(tree->c2);

        tree = ast_traits::alloc(astt::APP, tree->c1, tree->c2);
    }
    else if (tree->type == astt::ABS)
    {
        if (tree->c2->type == astt::APP &&
            tree->c2->c2->type == astt::VAR &&
            tree->c2->c2->var == tree->c1->var)
        {
            tree = ast_traits::alloc(tree->c2->c1);
        }
        else
        {
            eta_red(tree->c1);
            eta_red(tree->c2);

            tree = ast_traits::alloc(astt::ABS, tree->c1, tree->c2);
        }
    }
    else
    {
        std::terminate();
    }
}

bool interpreter::is_term_closed(ast_node_t const& tree)
{
    //return FV(tree).empty();

    std::unordered_set<var_t> acc;
    return is_term_closed_rec(tree, acc);
}

bool interpreter::is_term_closed_rec(ast_node_t const& tree, std::unordered_set<var_t>& bound)
{
    if (tree->type == astt::VAR)
        return bound.count(tree->var);
    else if (tree->type == astt::APP)
    {
        if (! is_term_closed_rec(tree->c1, bound))
            return false;
        else
            return is_term_closed_rec(tree->c2, bound);
    }
    else//if (tree->type == astt::ABS)      //its a lambda
    {
        var_t& abst_var = tree->c1->var;
        bool is_in = bound.count(abst_var);
        if (! is_in)
            bound.insert(abst_var);

        bool ret = is_term_closed_rec(tree->c2, bound);

        if (! is_in)
            bound.erase(abst_var);
        return ret;
    }
}

bool interpreter::is_not_free_in(ast_node_t const& tree, var_t& x)
{
    if (tree->type == astt::VAR)
        return (tree->var != x);
    else if (tree->type == astt::APP)
        return is_not_free_in(tree->c1, x) && is_not_free_in(tree->c2, x);
    else
    {
        if (tree->c1->var == x)
            return true;

        return is_not_free_in(tree->c2, x);
    }
}

/*std::set<var_t> interpreter::FV(ast_node_t const& tree)
{
    std::set<var_t> ret;

    FV_rec(tree, ret);
    return ret;
}

// isempty acc
void interpreter::FV_rec(ast_node_t const& tree, std::set<var_t>& acc)
{
    if (tree->type == astt::VAR)
        acc.insert(tree->var);
    else if (tree->type == astt::APP)
    {
        FV_rec(tree->c1, acc);
        FV_rec(tree->c2, acc);
    }
    else//if (tree->type == astt::ABS)      //its a lambda
    {
        var_t abst_var = tree->c1->var;
        bool is_in = acc.count(abst_var);
        FV_rec(tree->c2, acc);

        if (! is_in)
            acc.erase(abst_var);
    }
}*/

void interpreter::set_mode(inter_ops_t new_op)
{
    this->op = new_op;
}

inline var_t interpreter::get_tmp()
{
    return std::wstring(1, L'τ') +std::to_wstring(z++);
}
