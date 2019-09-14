//
// Created by ramsay on 8/1/19.
//

#ifndef DOUBANCRAWLER_HTTP_H
#define DOUBANCRAWLER_HTTP_H

#include <string>
namespace doubanCrawler {

int tcp_connect(const char *host, const char *serv);

std::string handle_response(int sockfd);

void send_request(int sockfd, const char *message);

void error(const char *msg);

std::string http_get(const std::string &host);

}  // namespace doubanCrawler
#endif  //DOUBANCRAWLER_HTTP_H
