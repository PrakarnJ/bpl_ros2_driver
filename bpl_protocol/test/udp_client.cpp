/**
 * Copyright 2026 Prakarn Jaroonsorn <prakarn.jrs@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>

#include "async-sockets/udpsocket.hpp"

using namespace std;

int main()
{
  // Our constants:
  const string IP = "localhost";
  const uint16_t PORT = 8888;

  // Initialize socket.
  UDPSocket udpSocket(true);  // "true" to use Connection on UDP. Default is "false".
  udpSocket.Connect(IP, PORT);

  // Send String:
  udpSocket.Send("ABCDEFGH");
  //udpSocket.SendTo("ABCDEFGH", IP, PORT); // If you want to connectless

  udpSocket.onRawMessageReceived = [&](
                                     const char * message, int length, string ipv4, uint16_t port) {
    cout << ipv4 << ":" << port << " => " << message << "(" << length << ")" << endl;
  };

  // If you want to use std::string:
  /*
    udpSocket.onMessageReceived = [&](string message, string ipv4, uint16_t port) {
        cout << ipv4 << ":" << port << " => " << message << endl;
    };
    */

  // You should do an input loop so the program will not terminated immediately:
  string input;
  getline(cin, input);
  while (input != "exit") {
    udpSocket.Send(input);
    getline(cin, input);
  }

  // Close the socket.
  udpSocket.Close();

  return 0;
}