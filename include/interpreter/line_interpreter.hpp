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
    ~line_interpreter();

    int run();
    void print(std::vector<evaled_exp> const& exps, bool supp_in_print);

    void sigint_handler(int);
    void set_scope_file(const std::wstring &file);
    int interpret_command(const std::wstring &cmd);
    std::vector<evaled_exp> interpret_file(const std::wstring &file);
    std::vector<evaled_exp> interpret_line(std::wstring const& in);
    evaled_exp interpret_exp(std::wstring const& in);

    void doit(evaled_exp &ret, std::wstring& input);

    std::wstring scope;

private:
    interpreter inter;
    bool enable_trace = false;
    PAD(7);
};

#endif // LINE_INTERPRETER_HPP
