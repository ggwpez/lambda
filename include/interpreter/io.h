#ifndef IO_H
#define IO_H

#include <string>

class io
{
public:
    io();

    static std::string get_dir(std::string& file_name);
    static std::string get_file_name(std::string const& path);
    static const wchar_t* get_wc(const char* c);
    static const char*  get_c (const wchar_t* c);
    static int read_file(const char *path, std::wstringstream &str);
    static wchar_t const* read_file(const char *path);
};

#endif // IO_H
