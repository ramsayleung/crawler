#include <cstdio> /* printf, sprintf */
#include <cstdlib> /* exit, atoi, malloc, free */
#include <unistd.h> /* read, write, close */
#include <cstring> /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h> /* struct hostent, gethostbyname */
#include <cstdarg>
#include <syslog.h>
#include <string>
#include "http.h"
#include <map>
#include "utils.h"

void doubanCrawler::error(const char *msg) {
  perror(msg);
  exit(0);
}

/**
 * 建立tcp连接, 对getaddrinfo 函数进行封装, 适用于Ipv4和Ipv6
 * @param host 主机名字
 * @param serv 服务
 * @return socketfd
 */
int doubanCrawler::tcp_connect(const char *host, const char *serv) {
  int sockfd, n;
  struct addrinfo hints, *res, *ressave;
  bzero(&hints, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  if ((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
    printf("tcp_connect error for %s %s: %s", host, serv, gai_strerror(n));
    exit(0);
  }
  ressave = res;
  do {
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
      continue; /* ignore this one */
    }
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0) {
      break; /* success */
    }
    close(sockfd); /* ignore this one */
  } while ((res = res->ai_next) != nullptr);
  if (res == nullptr) {
    printf("tcp_connect error for %s, %s", host, serv);
    exit(0);
  }
  freeaddrinfo(ressave);
  return sockfd;
}

void doubanCrawler::handle_response(int sockfd) {
  int n = 0;
  char recvline[MAXLINE + 1];
  /* process response */
  printf("Response:\n");

  while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
    recvline[n] = 0; /* null terminate */
    fputs(recvline, stdout);
  }
  exit(0);
}

void doubanCrawler::send_request(int sockfd, const char *message) {
  int total = 0, sent = 0, bytes = 0;
  /* send the request */
  total = strlen(message);
  do {
    bytes = write(sockfd, message + sent, total - sent);
    if (bytes < 0)
      error("ERROR writing message to socket");
    if (bytes == 0)
      break;
    sent += bytes;
  } while (sent < total);
}