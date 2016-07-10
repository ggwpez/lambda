#include <split_string.hpp>

#include <sstream>

void split(std::wstring const& s, wchar_t delim, std::vector<std::wstring>& elems)
{
    std::wstringstream ss(s);
    std::wstring item;

    while (std::getline(ss, item, delim))
        elems.push_back(item);
}

std::vector<std::wstring> split(std::wstring const& s, wchar_t delim)
{
    std::vector<std::wstring> elems;
    split(s, delim, elems);
    return std::move(elems);
}
