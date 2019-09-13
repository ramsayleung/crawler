//
// Created by Ramsay on 2019/9/9.
//

#ifndef DOUBANCRAWLER_TEST_H
#define DOUBANCRAWLER_TEST_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual)                       \
    do {                                                               \
        test_count++;                                                  \
        if (equality)                                                  \
            test_pass++;                                               \
        else {                                                         \
            std::cerr << "expect: " << expect << "actual: " << actual; \
            main_ret = 1;                                              \
        }                                                              \
    } while (0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual)

#define EXPECT_EQ_BOOL(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual)

#define EXPECT_EQ_TRUE(actual) EXPECT_EQ_BASE((true) == (actual), true, actual)

#define EXPECT_EQ_STRING(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual)

#endif  //DOUBANCRAWLER_TEST_H
