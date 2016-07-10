#ifndef SPLIT_STRING_HPP
#define SPLIT_STRING_HPP

#include <vector>
#include <string>

void split(std::wstring const& s, wchar_t delim, std::vector<std::wstring>& elems);
std::vector<std::wstring> split(std::wstring const& s, wchar_t delim);

#endif // SPLIT_STRING_HPP
