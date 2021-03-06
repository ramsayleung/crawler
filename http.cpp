#include "http.hpp"
#include "utils.hpp"
#include <cstdarg>
#include <cstdio>  /* printf, sprintf */
#include <cstdlib> /* exit, atoi, malloc, free */
#include <cstring> /* memcpy, memset */
#include <map>
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <string>
#include <sys/socket.h> /* socket, connect */
#include <syslog.h>
#include <unistd.h> /* read, write, close */

void crawler::error(const char *msg) {
  perror(msg);
  exit(0);
}

/**
 * 建立tcp连接, 对getaddrinfo 函数进行封装, 适用于Ipv4和Ipv6
 * @param host 主机名字
 * @param serv 服务
 * @return socketfd
 */
int crawler::tcp_connect(const char *host, const char *serv) {
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

std::string crawler::handle_response(int sockfd) {
  int n = 0;
  char recvline[MAXLINE + 1];
  /* process response */
  std::string buffer;

  while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
    recvline[n] = 0; /* null terminate */
    buffer += recvline;
  }
  return std::string(buffer);
}

void crawler::send_request(int sockfd, const char *message) {
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

std::string crawler::http_get(const std::string &host) {
  const std::string protocol = "http";
  const std::string method = crawler::method::GET;
  const std::string path = "/";
  int sockfd;
  const char *httpRequestFormat = "%s %s HTTP/1.1\r\n";
  char buffer[1024];

  std::map<std::string, std::string> headers;
  headers.insert(
      std::pair<std::string, std::string>(crawler::header::HOST, host));
  headers.insert(std::pair<std::string, std::string>(
      crawler::header::CONNECTION, "close"));
  /* allocate space for the message */
  sprintf(buffer, httpRequestFormat, method.c_str(), path.c_str());
  std::string requestBody = buffer;

  /* fill in the parameters */
  /* If client sends request without `Connection: close` header, the server with
   * the connection open, and read block*/
  for (auto const &element : headers) {
    requestBody += element.first + crawler::constants::HTTP_COLON +
                   element.second + crawler::constants::HTTP_SEPARATOR;
  }
  requestBody += crawler::constants::HTTP_SEPARATOR;

  /* What are we going to send? */
  TRACE(("Request:%s\n", requestBody.c_str()));

  /* create the socket */
  sockfd = crawler::tcp_connect(host.c_str(), protocol.c_str());

  /* send request*/
  crawler::send_request(sockfd, requestBody.c_str());

  /* receive the response */
  std::string response = crawler::handle_response(sockfd);

  /* close the socket */
  close(sockfd);

  return std::string(response);
}