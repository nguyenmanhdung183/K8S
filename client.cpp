#include <iostream>
#include <cstring>
#include <thread>
#include <winsock2.h> // Sử dụng winsock2.h thay vì unistd.h và arpa/inet.h
//    g++ -o client client.cpp -lws2_32
#define PORT 55634
#define BUFFER_SIZE 1024

void receiveMessages(SOCKET sock) {
    char buffer[BUFFER_SIZE];
    while (true) {
        int valread = recv(sock, buffer, BUFFER_SIZE, 0); // Sử dụng recv thay cho read
        if (valread > 0) {
            std::cout << " " << std::string(buffer, valread) << std::endl;
        }
        memset(buffer, 0, BUFFER_SIZE);
    }
}

int main(int argc, char ** argv) {
    // Khởi tạo Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Winsock initialization failed" << std::endl;
        return -1;
    }

    if (argc < 2) {
        std::cerr << "Usage: ./client <ID>" << std::endl;
        return -1;
    }

    int id = std::stoi(argv[1]); // Lấy ID từ tham số dòng lệnh
    SOCKET sock = 0; // Sử dụng SOCKET thay vì int cho socket
    struct sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Sử dụng inet_addr thay cho inet_pton

    // Tạo socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cerr << "Socket creation error" << std::endl;
        WSACleanup(); // Dọn dẹp Winsock
        return -1;
    }

    // Kết nối đến server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        closesocket(sock); // Đóng socket
        WSACleanup(); // Dọn dẹp Winsock
        return -1;
    }

    std::cout << "Connected to the server as Client ID: " << id << std::endl;

    // Gửi ID của client đến server ngay sau khi kết nối
    std::string idMessage = "Client " + std::to_string(id) + " has joined the chat.";
    send(sock, idMessage.c_str(), idMessage.size(), 0);

    // Tạo một luồng để nhận tin nhắn từ server
    std::thread(receiveMessages, sock).detach();

    // Gửi tin nhắn
    while (true) {
        std::string message;
        std::cout << "YOU : ";
        std::getline(std::cin, message);
        std::string fullMessage = "Client " + std::to_string(id) + ": " + message;
        send(sock, fullMessage.c_str(), fullMessage.size(), 0);
    }

    closesocket(sock); // Đóng socket
    WSACleanup(); // Dọn dẹp Winsock
    return 0;
}
