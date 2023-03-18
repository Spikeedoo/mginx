namespace HttpUtils {
  void sendStatus(int socket, int statusCode);
  void sendStandardHeaders(int socket, int contentLength);
}