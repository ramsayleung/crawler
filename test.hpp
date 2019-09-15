//
// Created by Ramsay on 2019/9/9.
//

#ifndef DOUBANCRAWLER_TEST_H
#define DOUBANCRAWLER_TEST_H

#include <assert.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format)                                                                                                                            \
    do {                                                                                                                                                                            \
        test_count++;                                                                                                                                                               \
        if (equality)                                                                                                                                                               \
            test_pass++;                                                                                                                                                            \
        else {                                                                                                                                                                      \
            fprintf(stderr, "function %s(): %s:%d  Assertion `%s` failed. expect: " format " actual: " format "\n", __func__, __FILE__, __LINE__, #equality, expect, actual); \
            main_ret = 1;                                                                                                                                                           \
        }                                                                                                                                                                           \
    } while (0)

#define PRINT_TEST_RESULT()                                                                        \
    do {                                                                                           \
        printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count); \
    } while (0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")

#define EXPECT_EQ_UNSIGNED_LONG(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%lu")

#define EXPECT_EQ_TRUE(actual) EXPECT_EQ_BASE(true == (actual), true, actual, "%d")

#define EXPECT_EQ_CSTRING(expect, actual) EXPECT_EQ_BASE(strcmp((expect),(actual))==0, expect, actual, "%s")

#endif  //DOUBANCRAWLER_TEST_H
