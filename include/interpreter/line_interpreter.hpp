#ifndef LINE_INTERPRETER_HPP
#define LINE_INTERPRETER_HPP

#include "defines.hpp"
#include "ast.hpp"
#include "interpreter.hpp"
#include "evaled_exp.hpp"

#include <vector>
#include <string>
#include <array>

#include <traits_allocator.hpp>


class line_interpreter
{
public:
    line_interpreter(int argc, char** argv);

    int run();
    void print(std::vector<evaled_exp> const& exps, bool supp_in_print);

    void set_scope_file(const std::wstring &file);
    int interpret_command(const std::wstring &cmd);
    std::vector<evaled_exp> interpret_file(const std::wstring &file);
    std::vector<evaled_exp> interpret_line(std::wstring const& in);
    evaled_exp interpret_exp(std::wstring const& in);

    void doit(evaled_exp &ret, std::wstring& input);

    inter_ops_t mode;
    PAD(4);
    std::wstring scope;
};

#endif // LINE_INTERPRETER_HPP
