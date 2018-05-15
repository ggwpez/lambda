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
#include <csignal>

#define TEST 0

line_interpreter* ptr = nullptr;
static void sigint_action(int signum)
{
	if (ptr)
		ptr->sigint_handler(signum);
	else
		std::wcerr << L"Unhandled 'SIGINT'" << std::endl;
}

line_interpreter::line_interpreter(int argc, char** argv)
	: scope(), inter()
{
	ptr = this;
	signal(SIGINT, sigint_action);

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
			std::wcout << BANG_IN;
			std::getline(std::wcin, input);
			if (! input.size())
			{
				std::wcout << std::endl;
				continue;
			}

			if (input[0] == L':' || input[0] == L'.' || input[0] == L'-')
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
		std::vector<evaled_exp> exps = interpret_line(L"6 6");//L"(map sqr (list-n 5))");
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
		this->scope.clear();
		outl(MSG_B << L"Scope reset" << MSG_E);
	}
	else if (cmd[1] == L'p')
		outl(this->scope);
	else if (cmd[1] == L's')
		set_scope_file(cmd.substr(2, std::wstring::npos));
	else if (cmd[1] == L't')
		this->enable_trace ^= true;
	else
		outl(MSG_B << L"Unknown command: '" << cmd << L'\'' << MSG_E);

	return 0;
}

void line_interpreter::print(std::vector<evaled_exp> const& exps, bool supp_in_print)
{
	for (evaled_exp const& e : exps)
	{
		if (! supp_in_print)
			std::wcout << BANG_IN << e.input << std::endl;

		if (e.tree)
			outl(e.tree->to_str());
		else
			outl(MSG_B << L"<NULL>" << MSG_E);

		if (e.state == inter_ret_state_t::TIMEOUT)
			outl(MSG_B << L"Max Step count reached" << MSG_E);
		else if (e.state == inter_ret_state_t::CANCEL)
			outl(MSG_B << L"Canceled after " << e.info.steps << L" steps" << MSG_E);
		else if (e.state == inter_ret_state_t::ERR)
			outl(MSG_B << L"Unknown error" << MSG_E);
		else if (enable_trace)
		{
			outl(MSG_B << e.info.to_str() << MSG_E);
			outl(MSG_B << L"Size: " << e.tree->get_node_count() << L" Heigth: " << e.tree->get_height() << MSG_E);
		}
	}
}

void line_interpreter::set_scope_file(std::wstring const& file)
{
	std::string filen(file.begin(), file.end());

	std::wstringstream ss;
	if (! io::read_file(filen.c_str(), ss))
	{
		this->scope = ss.str() + std::wstring(L"\n");
		outl(MSG_B << L"Scope set (" << file << L')' << MSG_E);
	}
	else
		outl(MSG_B << L"File not found: " << file << MSG_E);
}

std::vector<evaled_exp> line_interpreter::interpret_file(std::wstring const& file)
{
	std::string filen(file.begin(), file.end());
	std::wstringstream ss;

	if (! io::read_file(filen.c_str(), ss))
	{
		std::wstring const& input = ss.str();

		outl(MSG_B << L"Interpreting file: " << file << MSG_E);
		return interpret_line(input);
	}
	else
		outl(MSG_B << L"File not found: " << file << MSG_E);

	return std::vector<evaled_exp>();
}

std::vector<evaled_exp> line_interpreter::interpret_line(std::wstring const& in)
{
	std::vector<std::wstring> exps = split(in, L';');
	std::vector<evaled_exp> ret(exps.size());

	for (size_t i = 0; i < exps.size(); i++)
		ret[i] = interpret_exp(exps[i]);

	return ret;
}

evaled_exp line_interpreter::interpret_exp(std::wstring const& in)
{
	lexer lex = lexer(this->scope +in);
	std::vector<tok>* toks = lex.lex();

	parser par = parser(*toks);
	ast_node_t tree = par.parse();

	inter_ret_state_t state;
	inter_info_t sum, info;

	while (((state = inter.inter(tree, info)) == inter_ret_state_t::TIMEOUT))
	{
		sum += info;
		std::wcout << L"Max step count(" << MAX_STEPS << L") reached, proceed? (y/n)";
		wchar_t c;
		std::wcin >> c;
		if (c == L'y')
			continue;
		else
			break;
	}
	if (state != inter_ret_state_t::TIMEOUT)
		sum += info;

	delete toks;
	return evaled_exp(state, in, tree, sum);
}

void line_interpreter::sigint_handler(int)
{
	inter.cancel_calculation();
}

line_interpreter::~line_interpreter()
{ }
