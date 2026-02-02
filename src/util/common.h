#ifndef _GAME_COMMON_H_
#define _GAME_COMMON_H_

#include <cstdio>

#define ANSI_COLOR_RESET    "\x1b[0m"
#define ANSI_COLOR_BOLD     "\x1b[1m"
#define ANSI_COLOR_RED      "\x1b[31m"
#define ANSI_COLOR_GREEN    "\x1b[32m"
#define ANSI_COLOR_YELLOW   "\x1b[33m"
#define ANSI_COLOR_BLUE     "\x1b[34m"
#define ANSI_COLOR_MAGENTA  "\x1b[35m"
#define ANSI_COLOR_CYAN     "\x1b[36m"

#define BIT(n) (1<<(n))

#define PRINT_DEBUG(v, ...) {do {fprintf(stderr, v "\n", ##__VA_ARGS__);} while(0);}

#define DIE(v, ...) {do {PRINT_DEBUG(v, ##__VA_ARGS__); exit(1);} while(0);}

#define ASSERT(v, msg, ...) {do {if (!v) DIE(msg, ##__VA_ARGS__);} while(0);}

#define CHECK(v) {do {if (!v) return false;} while (0);}

#define CHECK_MSG(v, msg, ...) {do {if (!v) {PRINT_DEBUG(msg, ##__VA_ARGS__); return false;}} while (0);}

#define FOR_RANGE(a, b, i) for (size_t i = a; i<b; i++)

#define FOR_TIMES(v, i) FOR_RANGE(0, v, i)

#endif
