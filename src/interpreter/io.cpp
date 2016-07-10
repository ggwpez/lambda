#include <io.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

io::io()
{
}

std::string io::get_dir(std::string& file_name)
{
    std::string directory;
    const size_t last_slash_idx = file_name.rfind('/');
    if (std::string::npos != last_slash_idx)
    {
        directory = file_name.substr(0, last_slash_idx) +std::string("/");
    }

    return directory;
}

struct MatchPathSeparator
{
    bool operator()( char ch ) const
    {
        return ch == '\\' || ch == '/';
    }
};

std::string io::get_file_name(std::string const& path)
{
    return std::string(
        std::find_if( path.rbegin(), path.rend(),
                      MatchPathSeparator() ).base(),
        path.end() );
}

const wchar_t* io::get_wc(const char* c)
{
    const size_t cSize = mbstowcs(NULL, c, 0) +sizeof(char);
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs (wc, c, cSize);

    return wc;
}

const char* io::get_c(const wchar_t* c)
{
    const size_t cSize = wcslen(c) +sizeof(wchar_t);
    char* wc = new char[cSize];
    wcstombs(wc, c, cSize);

    return wc;
}

int io::read_file(char const* path, std::wstringstream& str)
{
    wchar_t const* tem = read_file(path);
    if (!tem)
        return -1;

    str << tem;
    delete[] tem;
    return 0;
}

wchar_t const* io::read_file(char const* path)
{
    setlocale( LC_ALL, "" );

    FILE* f = fopen(path, "r");
    if (!f)
        return nullptr;

    size_t l;
    char* buf;

    fseek(f, 0, SEEK_END);
    l = ftell(f) +1;
    rewind(f);

    buf = new char[l];

    if (fread(buf, 1, l, f) +1 != l)        //+1, because l takes \0 into account
    {
        delete[] buf;
        return NULL;
    }

    buf[l -1] = 0;

    fclose(f);

    wchar_t const* ret = get_wc(buf);
    delete[] buf;

    return ret;
}
