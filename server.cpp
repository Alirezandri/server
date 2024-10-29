#include <iostream>

#include <fstream>

#include <sys/socket.h>

#include <arpa/inet.h>

#include <cstring>

#include <unistd.h>

#include <dirent.h> 


#define PORT 8080


void listFiles(int clientSock) {

    char buffer[1024];

    std::string fileList;

    DIR *dir;

    struct dirent *ent;


    if ((dir = opendir(".")) != nullptr) {


        while ((ent = readdir(dir)) != nullptr) {


            if (ent->d_name[0] != '.') {

                fileList += std::string(ent->d_name) + "\n"; 

            }

        }

        closedir(dir); 

    } else {

        perror("Could not open directory");

    }


    send(clientSock, fileList.c_str(), fileList.size(), 0);

}

void handleClient(int clientSock) {

    char buffer[1024] = {0};


    while (true) {

        memset(buffer, 0, sizeof(buffer));

        int valread = read(clientSock, buffer, sizeof(buffer));


        if (valread <= 0) break; 


        std::string command(buffer);


        if (command.find("upload") == 0) {

            std::string filename = command.substr(7);

            std::ofstream outFile(filename);

            if (outFile) {


                while (read(clientSock, buffer, sizeof(buffer)) > 0) {


                    if (strncmp(buffer, "\n", 1) == 0) break;

                    outFile << buffer;

                    memset(buffer, 0, sizeof(buffer)); 

                }

                outFile.close();

                send(clientSock, "File uploaded successfully", 26, 0);

            } else {

                send(clientSock, "Failed to create file", 22, 0);

            }

        } else if (command.find("search") == 0) {

            std::string filename = command.substr(7);

            std::ifstream inFile(filename);

            if (inFile) {

                send(clientSock, "File found", 10, 0);

                std::string content((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());

                send(clientSock, content.c_str(), content.size(), 0);

                inFile.close();

            } else {

                send(clientSock, "File not found", 14, 0);

            }

        } else if (command.find("delete") == 0) {

            std::string filename = command.substr(7);

            if (remove(filename.c_str()) == 0) {

                send(clientSock, "File deleted successfully", 24, 0);

            } else {

                send(clientSock, "File not found", 14, 0);

            }

        } else if (command.find("list") == 0) {

            listFiles(clientSock); 

        }

    }

}

int main() {

    int serverSock, clientSock;

    struct sockaddr_in server, client;

    socklen_t clientLen = sizeof(client);

    if ((serverSock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {

        std::cerr << "Socket creation error" << std::endl;

        return -1;

    }

    server.sin_family = AF_INET;

    server.sin_addr.s_addr = INADDR_ANY;

    server.sin_port = htons(PORT);


    if (bind(serverSock, (struct sockaddr *)&server, sizeof(server)) < 0) {

        std::cerr << "Bind failed" << std::endl;

        return -1;

    }

    if (listen(serverSock, 3) < 0) {

        std::cerr << "Listen failed" << std::endl;

        return -1;

    }

    std::cout << "Server listening on port " << PORT << std::endl;

    while ((clientSock = accept(serverSock, (struct sockaddr *)&client, &clientLen))) {

        std::cout << "Connection accepted" << std::endl;

        handleClient(clientSock);

        close(clientSock);

    }
    return 0;
}
