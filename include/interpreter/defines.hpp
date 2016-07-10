#ifndef DEFINES_HPP
#define DEFINES_HPP

#define CLR L"\033[1;37m"
#define CLR_END L"\033[0m"

#define outnl(msg) do { std::wcout << L"> " << msg << std::endl; } while (0)
#define outl(msg) do { std::wcout << CLR << L"> " << msg << CLR_END << std::endl; } while (0)

#define PAD(s) union { char pad[s]; }

/*
 * Set to true, if you want to use threads, but the boost::object_pool isnt thread safe, so it uses
 * new/delete.
 *
 * Set to false, if you dont want to use threads, but the pool.
 *
 * One thread will be created for each semicolon separted expression, e.g.
 * "1;2;3" would create 3 threads.
 * But the pool allocator is faster on big terms.
 *
 */
#define USE_BOOST_THREADS 0

#endif // DEFINES_HPP
