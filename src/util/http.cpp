#include <iostream>
#include <sys/socket.h>
#include <string>
#include <sstream>
#include <map>

#include "http.h"
#include "files.h"
#include "../lib/yaml-cpp/yaml.h"

YAML::Node config = YAML::LoadFile("../config.yaml");

// Set up the ability to parse gateways with a custom schema
template<>
struct YAML::convert<HttpUtils::GatewayItem> {
  static YAML::Node encode(const HttpUtils::GatewayItem& rhs) {
    YAML::Node node;
    node.push_back(rhs.path);
    node.push_back(rhs.service);
    return node;
  }

  static bool decode(const YAML::Node& node, HttpUtils::GatewayItem& rhs) {
    if (!node.IsSequence() || node.size() != 2) {
      return false;
    }

    rhs.path = node[0].as<std::string>();
    rhs.service = node[0].as<std::string>();
    return true;
  }
};

void HttpUtils::sendStatus(int socket, int statusCode = 200) {
  std::string response;
  if (statusCode == 200) response = "HTTP/1.1 200 OK\r\n";
  if (statusCode == 404) response = "HTTP/1.1 404 Not Found\r\n";
  const char* responseConverted = response.data();
  send(socket, responseConverted, response.size(), 0);
}

void HttpUtils::sendStandardHeaders(int socket, std::string mimeType, int contentLength) {
  std::string response = "Content-Type: " + mimeType + "; encoding=utf8\r\nContent-Length: " + std::to_string(contentLength) + "\r\nConnection: keep-alive\r\n\r\n";
  const char* responseConverted = response.data();
  send(socket, responseConverted, response.size(), 0);
}

HttpUtils::HttpRequest HttpUtils::parseRequest(std::vector<std::string> reqLines) {
  HttpUtils::HttpRequest newRequest;
  std::string rawHttpRequestLine = reqLines[0];
  if (rawHttpRequestLine.size() > 0) {
    std::istringstream iss(rawHttpRequestLine);

    // Parsing based off of https://stackoverflow.com/questions/28268236/parse-http-request-without-using-regexp
    std::string method;
    std::string query;
    std::string protocol;

    // Split up request
    if (!(iss >> method >> query >> protocol)) {
      throw "Error parsing request";
    }

    iss.clear();
    iss.str(query);

    // Parse URL
    std::string url;
    if (!std::getline(iss, url, '?')) {
      throw "Error parsing url";
    }

    // Parse params
    std::map<std::string, std::string> params;
    std::string keyval, key, val;

    while(std::getline(iss, keyval, '&')) {
      std::istringstream iss(keyval);
      if (std::getline(std::getline(iss, key, '='), val)) {
        params[key] = val;
      }
    }
    
    
    newRequest.method = method;
    newRequest.protocol = protocol;
    newRequest.url = url;
    newRequest.params = params;
  }
  return newRequest;
}

void HttpUtils::send404(int socket) {
  FileUtils::FileInfo fileInfo;
    try {
      fileInfo = FileUtils::getFileMetadata("../default/404.html");
    } catch (std::string err) {
      return;
    }
    // Send status code
    HttpUtils::sendStatus(socket, 404);
    // Send response headers
    HttpUtils::sendStandardHeaders(socket, "text/html", fileInfo.contentLength);
    // Send content
    FileUtils::sendFileOverSocket(socket, "../default/404.html");
}

void HttpUtils::handleGatewayRequest(int socket, std::vector<char> req, HttpUtils::GatewayItem gateItem) {
  std::cout << "Gateway request!" << std::endl;
}

void HttpUtils::handleRequest(int socket, std::vector<char> req) {
  std::vector<std::string> reqLines;
  std::string cursor;
  std::string requestString = req.data();
  std::stringstream stream(requestString);

  while (getline(stream, cursor, '\n')) {
    cursor.erase(std::remove(cursor.begin(), cursor.end(), '\n'), cursor.cend());
    reqLines.push_back(cursor);
  }
  
  if (reqLines.size() > 0) {
    HttpUtils::HttpRequest requestParsed = HttpUtils::parseRequest(reqLines);
    HttpUtils::GatewayItem selectedGateway {};

    // Look through configured gateways
    for (auto element : config["gateway"]) {
      HttpUtils::GatewayItem gate = element.as<HttpUtils::GatewayItem>();
      if (gate.path == requestParsed.url) {
        selectedGateway = gate;
        break;
      }
    }

    if (!selectedGateway.path.empty() && !selectedGateway.service.empty()) {
      HttpUtils::handleGatewayRequest(socket, req, selectedGateway);
      return;
    }

    // No other conditional met--serve from basedir
    std::string basedir = config["basedir"].as<std::string>();
    std::string targetFilePath = basedir + requestParsed.url;

    // Pre read the file
    FileUtils::FileInfo fileInfo;
    try {
      fileInfo = FileUtils::getFileMetadata(targetFilePath);
    } catch (const char* err) {
      send404(socket);
      return;
    }
    // Send status code
    HttpUtils::sendStatus(socket, 200);
    // Send response headers
    HttpUtils::sendStandardHeaders(socket, fileInfo.mimeType, fileInfo.contentLength);
    // Send content
    FileUtils::sendFileOverSocket(socket, targetFilePath);
  }
}