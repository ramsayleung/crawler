//
// Created by Ramsay on 2019/9/9.
//
#include <unistd.h> /* read, write, close */
#include "test.h"
#include "dom.h"
#include "html.h"
#include "http.h"
#include "utils.h"

void testSplit() {
  std::string classes = "item.pic.info";
  std::set<std::string> classSet;
  doubanCrawler::split(classes, classSet, '.');
  EXPECT_EQ_INT(3, classSet.size());
}

void testStartsWith() {
  const std::string source = "tititoto";
  std::string prefix = "titi";
  EXPECT_EQ_INT(1, doubanCrawler::startsWith(prefix, source));
  prefix = "tito";
  EXPECT_EQ_INT(true, doubanCrawler::startsWith(prefix, source, 2, 6));
}

void testParse() {
  const std::string source = R"(
  <html>
      <body>
          <h1>Title</h1>
          <div id="main" class="test">
              <p>Hello <em>world</em>!</p>
          </div>
      </body>
  </html>)";

  doubanCrawler::Node node = doubanCrawler::parse(source);
}

void testHttp() {
  /* first where are we going to send it? */
  const std::string host = "stackoverflow.com";
  const std::string service = "http";
  const std::string method = doubanCrawler::method::GET;
  const std::string path = "/";
  int sockfd;
  const char *httpRequestFormat = "%s %s HTTP/1.1\r\n";
  char buffer[1024];

  std::map<std::string, std::string> headers;
  headers.insert(std::pair<std::string, std::string>(doubanCrawler::header::HOST, host));
  headers.insert(std::pair<std::string, std::string>(doubanCrawler::header::CONNECTION, "close"));
  /* allocate space for the message */
  sprintf(buffer, httpRequestFormat, method.c_str(), path.c_str());
  std::string requestBody = buffer;

  /* fill in the parameters */
  /* If client sends request without `Connection: close` header, the server with the connection open, and read block*/
  for (auto const &element: headers) {
    requestBody += element.first + doubanCrawler::constants::HTTP_COLON + element.second
        + doubanCrawler::constants::HTTP_SEPARATOR;
  }
  requestBody += doubanCrawler::constants::HTTP_SEPARATOR;
  /* What are we going to send? */
  printf("Request:\n%s\n", requestBody.c_str());

  /* create the socket */
  sockfd = doubanCrawler::tcp_connect(host.c_str(), service.c_str());

  /* send request*/
  doubanCrawler::send_request(sockfd, requestBody.c_str());

  /* receive the response */
  doubanCrawler::handle_response(sockfd);

  /* close the socket */
  close(sockfd);
}

int main(int argc, char *argv[]) {
  testSplit();
  testStartsWith();
  testParse();
  testHttp();
}
