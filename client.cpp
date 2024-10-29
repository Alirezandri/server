#include <iostream>

#include <fstream>

#include <sys/socket.h>

#include <arpa/inet.h>

#include <cstring>

#include <unistd.h>

#define PORT 8080

void uploadFile(int sock) {

    std::string filename;

    std::cout << "Enter filename to upload: ";

    std::cin >> filename;

    std::ofstream outFile(filename); 

    if (!outFile) {

        std::cerr << "Failed to create file: " << filename << std::endl;

        return;
    }

    std::cout << "Enter message to upload: ";

    std::cin.ignore();

    std::string message;

    std::getline(std::cin, message);

    std::string request = "upload " + filename;

    send(sock, request.c_str(), request.size(), 0);

    send(sock, message.c_str(), message.size(), 0);

    const char *endMarker = "\n";

    send(sock, endMarker, strlen(endMarker), 0);

    outFile << message; 

    outFile.close(); 

    std::cout << "File uploaded successfully" << std::endl;

    char buffer[1024] = {0};

    read(sock, buffer, sizeof(buffer)); 

    std::cout << buffer << std::endl; 

}

void searchFile(int sock) {

    std::string filename;

    std::cout << "Enter filename to search: ";

    std::cin >> filename;
    
    std::string request = "search " + filename;

    send(sock, request.c_str(), request.size(), 0);

    char buffer[1024] = {0};

    read(sock, buffer, sizeof(buffer)); 

    std::cout << buffer << std::endl; 

    if (strncmp(buffer, "File found", 10) == 0) {

        std::cout << "Content of " << filename << ":\n";

        read(sock, buffer, sizeof(buffer)); 

        std::cout << buffer << std::endl; 

    }
}

void deleteFile(int sock) {

    std::string filename;

    std::cout << "Enter filename to delete: ";

    std::cin >> filename;

    std::string request = "delete " + filename;

    send(sock, request.c_str(), request.size(), 0);

    char buffer[1024] = {0};

    read(sock, buffer, sizeof(buffer)); 

    std::cout << buffer << std::endl; 

}

void listFiles(int sock) {

    std::string request = "list";

    send(sock, request.c_str(), request.size(), 0);

    char buffer[1024] = {0};

    read(sock, buffer, sizeof(buffer));

    std::cout << "Files on server:\n" << buffer << std::endl; 

}

int main() {

    int sock;

    struct sockaddr_in server;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {

        std::cerr << "Socket creation error" << std::endl;

        return -1;
    }
    server.sin_family = AF_INET;

    server.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) <= 0) {

        std::cerr << "Invalid address / Address not supported" << std::endl;

        return -1;
    }

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {

        std::cerr << "Connection Failed" << std::endl;

        return -1;

    }
    while (true) {

        std::cout << "Choose an option:\n";

        std::cout << "1. Upload File\n";

        std::cout << "2. Search File\n";

        std::cout << "3. Delete File\n";

        std::cout << "4. List Files\n";  

        std::cout << "5. Exit\n";  

        int choice;

        std::cin >> choice;

        switch (choice) {

            case 1:

                uploadFile(sock);

                break;

            case 2:

                searchFile(sock);

                break;

            case 3:

                deleteFile(sock);
break;

            case 4:

                listFiles(sock);  

                break;

            case 5:  

                close(sock);

                return 0;

            default:

                std::cout << "Invalid option. Please try again." << std::endl;
        }

    }
    return 0;
}
