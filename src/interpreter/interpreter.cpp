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
    ast_node_t ret;

    changed = true;

    if (is_beta_reducable(tree))
        ret = beta_red(tree);
    else if (is_eta_reducable(tree))
        ret = eta_red(tree);
    else
    {
        changed = false;
        ret = tree;
    }

    if (info)
        *info = inter_info_t(alphas, betas, etas, z -oz, tree->get_node_count());
    return ret;
}

void interpreter::cancel_calculation()
{
    this->cancel = true;
}

ast_node_t interpreter::alpha_red(ast_node_t& E, var_t& v, ast_node_t& N)
{
    //if (is_term_closed(E))
      //  return E;

    alphas++;
    if (E->type == astt::VAR)
    {
        var_t& var = E->var;

        if (var == v)
            return ast_traits::alloc(N);            //+++
        else
            return E;    //###
    }
    else if (E->type == astt::APP)
    {
        /*ast_node_t c1 = alpha_red(E->c1, v, N);           //+++
        ast_node_t c2 = alpha_red(E->c2, v, N);

        return ast_traits::alloc(astt::APP, c1, c2);*/

        E->c1 = alpha_red(E->c1, v, N);
        E->c2 = alpha_red(E->c2, v, N);

        return E;
    }
    else if (E->type == astt::ABS)
    {
        var_t& x = E->c1->var;

        if (x == v)
            return E;    //###
        else
        {
            if (is_not_free_in(N, x))
            {
                /*ast_node_t ny = ast_traits::alloc(E->c1);     //+++
                ast_node_t c2 = alpha_red(E->c2, v, N);

                return ast_traits::alloc(astt::ABS, ny, c2);*/

                E->c2 = alpha_red(E->c2, v, N);

                return E;
            }
            else
            {
                ast_node_t var_z = ast_traits::alloc(get_tmp());

                /*ast_node_t c2_o = alpha_red(E->c2, x, var_z);     //+++
                ast_node_t c2_n = alpha_red(c2_o, v, N);*/

                E->c1 = var_z;
                E->c2 = alpha_red(E->c2, x, var_z);
                E->c2 = alpha_red(E->c2, v, N);

                //ast_traits::free(var_z); //c2_o ###

                //return ast_traits::alloc(astt::ABS, var_z, c2_n);
                return E;
            }
        }
    }

    return nullptr;
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

ast_node_t interpreter::beta_red(ast_node_t& tree)
{
    ast_node_t ret = nullptr;

    betas++;
    if (tree->type == astt::VAR)
        ret = tree;  //###
    else if (tree->type == astt::ABS)
    {
        /*ast_node_t c1 = ast_traits::alloc(tree->c1);      //+++
        ast_node_t c2 = beta_red(tree->c2);

        return ast_traits::alloc(astt::ABS, c1, c2);*/

        tree->c2 = beta_red(tree->c2);
        return tree;
    }
    else if (tree->type == astt::APP)
    {
        if (tree->c1->type == astt::ABS)        //lambda term gets called
        {
            ast_node_t& lambda = tree->c1;
            var_t& abst_var = lambda->c1->var;

            return alpha_red(lambda->c2, abst_var, tree->c2);
        }
        else
        {
            /*ast_node_t func = beta_red(tree->c1);     //+++
            ast_node_t arg = !((int)this->op & (int)inter_ops_t::LAZY) ? beta_red(tree->c2) : ast_traits::alloc(tree->c2);

            return ast_traits::alloc(astt::APP, func, arg);*/

            tree->c1 = beta_red(tree->c1);
            tree->c2 = !((int)this->op & (int)inter_ops_t::LAZY) ? beta_red(tree->c2) : tree->c2;

            return tree;
        }
    }

    return ret;
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

ast_node_t interpreter::eta_red(ast_node_t& tree)
{
    //if (! is_eta_reducable(tree))       //TODO: check speed
      //  return tree;

    etas++;
    if (tree->type == astt::VAR)
        return tree;     //###
    else if (tree->type == astt::APP)
        return ast_traits::alloc(astt::APP, eta_red(tree->c1), eta_red(tree->c2));
    else if (tree->type == astt::ABS)
    {
        if (tree->c2->type == astt::APP &&
            tree->c2->c2->type == astt::VAR &&
            tree->c2->c2->var == tree->c1->var)
        {
            return ast_traits::alloc(tree->c2->c1);
        }
        else
            return ast_traits::alloc(astt::ABS, eta_red(tree->c1), eta_red(tree->c2));
    }
    else
        return nullptr;
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
