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

#include "async-sockets/udpserver.hpp"
#include <iostream>

using namespace std;

int main()
{
    // Initialize server socket..
    UDPServer udpServer;

    // onMessageReceived will run when a message received with information of ip & port of sender:
    /*udpServer.onMessageReceived = [&](string message, string ipv4, uint16_t port) {
        //cout << ipv4 << ":" << port << " => " << message << endl;
        // Just send without control:
        udpServer.SendTo("A!", ipv4, port);
    };*/

    // If you want to use raw byte arrays:
    
    udpServer.onRawMessageReceived = [&](const char* message, int length, string ipv4, uint16_t port) {
        cout << ipv4 << ":" << port << " => " << message << "(" << length << ")" << endl;

        // Just send without control:
        udpServer.SendTo(message, length, ipv4, port);
    };
    

    // Bind the server to a port.
    udpServer.Bind(8888, [](int errorCode, string errorMessage) {
        // BINDING FAILED:
        cout << errorCode << " : " << errorMessage << endl;
    });

    // You should do an input loop so the program will not terminated immediately:
    string input;
    getline(cin, input);
    while (input != "exit")
    {
        getline(cin, input);
    }

    udpServer.Close();

    return 0;
}