//
// Created by ramsay on 8/1/19.
//

#ifndef DOUBANCRAWLER_HTTP_H
#define DOUBANCRAWLER_HTTP_H

namespace doubanCrawler {

int tcp_connect(const char *host, const char *serv);

void handle_response(int sockfd);

void send_request(int sockfd, const char *message);

void error(const char *msg);


}
#endif //DOUBANCRAWLER_HTTP_H
