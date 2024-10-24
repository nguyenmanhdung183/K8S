#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <mutex>
#include <algorithm> // Thêm thư viện này để sử dụng std::remove_if

#define PORT 8080
#define BUFFER_SIZE 1024

std::vector<int> clients;
std::mutex clients_mutex;

void broadcastMessage(const std::string &message, int sender_socket) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (int client_socket : clients) {
        if (client_socket != sender_socket) {
            send(client_socket, message.c_str(), message.size(), 0);
        }
    }
}

void handleClient(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    while (true) {
        int valread = read(client_socket, buffer, BUFFER_SIZE);
        if (valread > 0) {
            std::string message(buffer, valread);
            std::cout << "Message from client " << client_socket << ": " << message << std::endl;
            broadcastMessage(" " + message, client_socket);
            memset(buffer, 0, BUFFER_SIZE);
        } else {
            // Khi client ngắt kết nối
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
            close(client_socket);
            break;
        }
    }
}

int main(int argc, char ** argv) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Tạo socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Thiết lập lại các tùy chọn socket
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Thiết lập thông tin địa chỉ
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Gán địa chỉ và port cho socket
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    std::cout << "Server is listening on port " << PORT << std::endl;

    while (true) {
        // Chấp nhận kết nối mới
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.push_back(new_socket);
        }
        std::cout << "New client connected: " << new_socket << std::endl;

        // Tạo một luồng để xử lý client
        std::thread(handleClient, new_socket).detach();
    }

    return 0;
}
