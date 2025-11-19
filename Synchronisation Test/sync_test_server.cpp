#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>

std::string getLatestDmesgLine() {
    // Open a pipe to run a shell command
    FILE* pipe = popen("sudo /bin/dmesg | grep 'GPIO_16_IRQ' | tail -1", "r");
    if (!pipe) return "ERROR";
    char buffer[256];
    std::string result;
    // Read command output line by line into result string
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);
    return result;
}

long extractTimestamp(const std::string& dmesgLine) {
    // Find the position of the marker "GPIO_16_IRQ:"
    size_t pos = dmesgLine.find("GPIO_16_IRQ:");
    if (pos == std::string::npos) return -1;
    // Extract substring after "GPIO_16_IRQ:" (12 characters long)
    std::string ts_str = dmesgLine.substr(pos + 12);
    return std::stod(ts_str);
}

int main() {
    int server_fd, new_socket;
    sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Create and configure socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(12345);

    // Bind socket to the specified address and port
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));

    // Listen for incoming connections (queue size = 3)
    listen(server_fd, 3);

    while (true) {
        // Accept a new client connection
        new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        std::cout << "Request received from client." << std::endl;

        // Read data from client (trigger message)
        read(new_socket, buffer, 1024);

        std::string msg = std::string(buffer);
        std::string response;

        // Get latest dmesg line containing "GPIO_16_IRQ" and extract timestamp
        response = getLatestDmesgLine();
        double time_Pi = extractTimestamp(response);
        printf("Sending timestamps %+f ns\n\n", time_Pi);

        // Send the full dmesg line back to client
        send(new_socket, response.c_str(), response.size(), 0);
        close(new_socket);
    }
}