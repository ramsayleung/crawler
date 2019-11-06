//
// Created by ramsay on 8/1/19.
//

#ifndef DOUBANCRAWLER_UTILS_H
#define DOUBANCRAWLER_UTILS_H

#include <iostream>

namespace crawler {
// http header constants
namespace header {
const char *const CONNECTION = "Connection";
const char *const HOST = "Host";
}  // namespace header
// http method constants
namespace method {
const char *const GET = "GET";
const char *const POST = "POST";
}  // namespace method
namespace constants {
const char *const HTTP_SEPARATOR = "\r\n";
const char *const HTTP_COLON = ": ";
}  // namespace constants
}  // namespace crawler
#define MAXLINE 4096

#ifdef DEBUG
#define debug_print(fmt, ...)                                    \
    do {                                                         \
        if (DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                           __LINE__, __func__, __VA_ARGS__);     \
    } while (0)

#define TRACE(x)                  \
    do {                          \
        if (DEBUG) debug_print x; \
    } while (0)
#else
#define TRACE(x) \
    do {         \
    } while (0)

#endif  //DEBUG

#endif  //DOUBANCRAWLER_UTILS_H
