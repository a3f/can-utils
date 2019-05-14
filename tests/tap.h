// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) 2019 Pengutronix, Ahmad Fatoum <a.fatoum@pengutronix.de>
 */

#ifndef TAP_H_
#define TAP_H_

#include <stdio.h>

static unsigned tap_fail __attribute__((unused));
static unsigned tap_count[1] __attribute__((unused));
static unsigned tap_planned __attribute__((unused));

#define fixture(func) \
	static __attribute__((constructor)) void FIXTURE_ ## func (void)

#define pr_diag(ok, name, diag, ...) ({                      \
	printf("%sok %d - " name diag "\n",                  \
	       ok ? "" : "not ", *tap_count, ##__VA_ARGS__); \
	ok;                                                  \
	})

#define pr_pass(name, ...) pr_diag(1, name, "", ##__VA_ARGS__)
#define pr_fail(name, ...) pr_diag(0, name, "", ##__VA_ARGS__)

#define ok(expr, msg, ...) ({                                 \
	++*tap_count;                                         \
	int ok = (expr);                                      \
	putchar('\t');                                        \
	pr_diag(ok, "`" #expr "'", " # " msg, ##__VA_ARGS__); \
	if (!ok) tap_fail++;                                  \
	ok;                                                   \
})

#define pass_no_tab(msg, ...) ({	\
	++*tap_count;			\
	pr_pass(msg, ##__VA_ARGS__);	\
	1;				\
})

#define pass(msg, ...) ({                \
	putchar('\t');                   \
	pass_no_tab(msg, ##__VA_ARGS__); \
})


#define fail_no_tab(msg, ...) ({     \
	++*tap_count;                \
	tap_fail++;                  \
	pr_fail(msg, ##__VA_ARGS__); \
	0;                           \
})


#define fail(msg, ...) ({                \
	putchar('\t');                   \
	fail_no_tab(msg, ##__VA_ARGS__); \
})

#define xok(expr, ...) do { if (!ok(expr, ##__VA_ARGS__)) return; } while (0)

#define subtest(func)                                                                                 \
	static void TEST_BODY_ ## func (unsigned *tap_count, unsigned *planned);                      \
	static __attribute__((constructor)) void TEST_ ## func (void) {                               \
		unsigned tap_fail_before = tap_fail;                                                  \
		int failed;                                                                           \
		{                                                                                     \
			unsigned tap_count = 0;                                                       \
			unsigned planned = 0;                                                         \
			puts("\t# Subtest: " #func);                                                  \
			TEST_BODY_ ## func (&tap_count, &planned);                                    \
			if (!planned)                                                                 \
				printf ("\t1..%d\n", tap_count);                                      \
		}                                                                                     \
		failed = tap_fail_before != tap_fail;                                                 \
		failed ? fail_no_tab(#func) : pass_no_tab(#func);                                     \
	}                                                                                             \
	static void TEST_BODY_ ## func (unsigned *tap_count, unsigned *planned __attribute((unused)))

#define subtest_plan(count) do { printf("\t1..%d\n", count); *planned = 1; } while(0)
#define plan(count)                                                                   \
	static __attribute__((constructor)) void FIXTURE_pre_plan (void) {            \
		printf("1..%d\n", count);                                             \
		tap_planned = 1;                                                      \
	}

static __attribute__((destructor)) void FIXTURE___post_plan (void) {
	if (!tap_planned)                       \
		printf ("1..%d\n", *tap_count); \
}

#define done_testing() int main(void) {         \
	return tap_fail != 0;                   \
}

#endif
