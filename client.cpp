#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>

#define PORT 8082
#define BUFFER_SIZE 1024

void receiveMessages(int sock) {
    char buffer[BUFFER_SIZE];
    while (true) {
        int valread = read(sock, buffer, BUFFER_SIZE);
        if (valread > 0) {
            std::cout << " " << std::string(buffer, valread) << std::endl;
        }
        memset(buffer, 0, BUFFER_SIZE);
    }
}

int main(int argc, char ** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./client <ID>" << std::endl;
        return -1;
    }

    int id = std::stoi(argv[1]); // Lấy ID từ tham số dòng lệnh
    int sock = 0;
    struct sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Chuyển đổi địa chỉ IPv4 từ văn bản sang nhị phân  192.168.49.2
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address" << std::endl;
        return -1;
    }

    // Tạo socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    // Kết nối đến server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
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

    close(sock);
    return 0;
}
