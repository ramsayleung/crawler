//
// Created by ramsay on 8/1/19.
//

#ifndef DOUBANCRAWLER_UTILS_H
#define DOUBANCRAWLER_UTILS_H

namespace {
// http header constants
namespace header {
const char *const CONNECTION = "Connection";
const char *const HOST = "Host";
}
// http method constants
namespace method {
const char *const GET = "GET";
const char *const POST = "POST";
}
namespace constants {
const char *const HTTP_SEPARATOR = "\r\n";
const char *const HTTP_COLON = ": ";
}
}
#define MAXLINE 4096

#endif //DOUBANCRAWLER_UTILS_H
