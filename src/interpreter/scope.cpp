#include "scope.hpp"

void scope::define_value(var_t const& name, scope_entry_t value)
{
    defines[name] = value;
}

scope_entry_t* scope::get_value(var_t const& name)
{
    auto&& it = defines.find(name);

    if (it == defines.end())
        return nullptr;
    else
        return &it->second;
}
