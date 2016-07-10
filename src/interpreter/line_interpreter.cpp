#include <line_interpreter.hpp>
#include <interpreter.hpp>
#include <lexer.hpp>
#include <parser.hpp>
#include <tok.hpp>
#include <split_string.hpp>
#include <io.h>

#include <array>
#include <sstream>
#include <fstream>
#include <locale>
#include <string>
#include <iostream>
#include <unistd.h>

#define TEST 0

line_interpreter::line_interpreter(int argc, char** argv)
    : mode(inter_ops_t::DEFAULT), scope()
{
    int c = 0;
    while ((c = getopt(argc, argv, "s:i:f:l:")) != -1)
    {
        switch (c)
        {
            case 's':
            {
                std::string tmp(optarg);
                std::wstring filen(tmp.begin(), tmp.end());

                set_scope_file(filen);
                break;
            }
            case 'i': case 'f': case 'l':
            {
                std::string tmp(optarg);
                std::wstring filen(tmp.begin(), tmp.end());

                std::vector<evaled_exp> evaled = interpret_file(filen);
                print(evaled, false);
                break;
            }
        default:
            // getopt already handles this case, just for -Wswitch-default
            break;
        }
    }
}

int line_interpreter::run()
{
    setlocale(LC_ALL, "");

    if (! TEST)
    {
        while (1)
        {
            std::wstring input;
            std::wcout << L"< ";
            std::getline(std::wcin, input);
            if (! input.size())
            {
                std::wcout << std::endl;
                continue;
            }

            if (input[0] == L':' || input[0] == L'.')
            {
                if (interpret_command(input))
                    return 0;
                else
                    continue;
            }

            std::vector<evaled_exp> exps = interpret_line(input);
            print(exps, (exps.size() == 1));
        }
    }
    else
    {
        std::vector<evaled_exp> exps = interpret_line(L"40000");
        print(exps, (exps.size() == 1));
    }

    return 0;
}

int line_interpreter::interpret_command(std::wstring const& cmd)
{
    if (cmd[1] == L'l' || cmd[1] == L'f' || cmd[1] == L'i')
    {
        auto exps = interpret_file(cmd.substr(2, std::wstring::npos));
        print(exps, false);
    }
    else if (cmd[1] == L'q')
        return 1;
    else if (cmd[1] == L'c')
        std::wcout << "\033c";
    else if (cmd[1] == L'r')
    {
        this->scope = L"";
        outl(L"< Scope reset >");
    }
    else if (cmd[1] == L's')
        set_scope_file(cmd.substr(2, std::wstring::npos));
    else if (cmd[1] == L't')
        this->mode = inter_ops_t::TRACE;

    return 0;
}

void line_interpreter::print(std::vector<evaled_exp> const& exps, bool supp_in_print)
{
    for (evaled_exp const& e : exps)
    {
        if (! supp_in_print)
            std::wcout << L"< " << e.input << std::endl;

        if (e.state == inter_ret_state_t::OK)
        {
            if (e.tree)
            {
                outl(e.tree->to_str());
                std::wcout << e.info.to_str() << std::endl;
            }
            else
                std::wcout << L"< NULL >";
        }
        else if (e.state == inter_ret_state_t::MAX_SUBST_ITER_REACHED)
            outl(L"< NULL > Step reached.");
        else
            outl(L"< NULL > Unknown error.");
    }
}

void line_interpreter::set_scope_file(std::wstring const& file)
{
    std::string filen(file.begin(), file.end());

    std::wstringstream ss;
    if (! io::read_file(filen.c_str(), ss))
    {
        this->scope = ss.str() + std::wstring(L"\n");
        outl(L"< Scope set (" << file << ") >");
    }
    else
        outl(L"< File not found: " << file << L" >");
}

std::vector<evaled_exp> line_interpreter::interpret_file(std::wstring const& file)
{
    std::string filen(file.begin(), file.end());
    std::wstringstream ss;

    if (! io::read_file(filen.c_str(), ss))
    {
        std::wstring const& input = ss.str();

        outl(L"< Interpreting file: " << file << L" >");
        return interpret_line(input);
    }
    else
        outl(L"File not found: " << file);

    return std::vector<evaled_exp>();
}

void line_interpreter::doit(evaled_exp& ret, std::wstring& input)
{
    ret = interpret_exp(input);
}

std::vector<evaled_exp> line_interpreter::interpret_line(std::wstring const& in)
{
    std::vector<std::wstring> exps = split(in, L';');
    std::vector<evaled_exp> ret(exps.size());
#if USE_BOOST
    boost::thread_group threads;
#endif

    for (size_t i = 0; i < exps.size(); i++)
    {
#if USE_BOOST_THREADS
        threads.add_thread(new boost::thread(&line_interpreter::doit, this, boost::ref(ret[i]), boost::ref(exps[i])));
#endif

        ret[i] = interpret_exp(exps[i]);
    }

#if USE_BOOST_THREADS
    threads.join_all();
#endif
    return ret;
}

evaled_exp line_interpreter::interpret_exp(std::wstring const& in)
{
    lexer lex = lexer(this->scope +in);
    std::vector<tok>* toks = lex.lex();

    parser par = parser(*toks);
    ast_node_t tree = par.parse();

    interpreter inter = interpreter(mode);
    inter_ret_state_t state;
    inter_info_t info;
    ast_node_t evaled = inter.inter(tree, state, UINT64_MAX, info);

    delete toks;
    ast_traits::free(tree);
    return evaled_exp(state, in, evaled, info);
}
