#include <sys/resource.h>
#include <csignal>
#include <iostream>
#include <cstdlib>

#include <line_interpreter.hpp>
#include <traits_allocator.hpp>

#define STACK_SIZE_MB 32

__attribute__((noreturn)) void sigsegv_action(int num)
{
	std::wcerr << std::endl << L"-- SIGSEGV -- " << std::endl << L"Maybe out of stack?" << std::endl;
	signal(num, SIG_DFL);
	std::terminate();
}

void init_stack()
{
	rlim64_t const stack_s = STACK_SIZE_MB *1024 *1024;
	rlimit64 rl;
	int res = getrlimit64(RLIMIT_STACK, &rl);

	if (res == 0)
	{
		if (rl.rlim_cur < stack_s)
		{
			rl.rlim_cur = stack_s;
			res = setrlimit64(RLIMIT_STACK, &rl);

			if (res != 0)
				std::wcerr << L"Stack size set to: " << std::to_wstring(STACK_SIZE_MB) << L" failed, is now: " << std::to_wstring(rl.rlim_cur) << std::endl;
		}
	}
}

int main(int argc, char** argv)
{
	init_stack();
	signal(SIGSEGV, sigsegv_action);

	line_interpreter inter(argc, argv);

	return inter.run();
}
