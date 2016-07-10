#include <tok.hpp>

wchar_t const* const tokt_strings[] = { L"λ", L"var", L"num", L".", L"(", L")", L"let", L"=", L"in", L";", L"END" };
static_assert((sizeof(tokt_strings) / sizeof(tokt_strings[0])) == (int)tokt::END +1, "tokt_strings is missing a string value");

std::wstring tok::to_str() const
{
    if (type == tokt::VAR)
        return var;
    else if (type == tokt::NUM)
        return std::to_wstring(num);

    return std::wstring(tokt_strings[(unsigned)type]);
}
