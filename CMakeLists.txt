cmake_minimum_required(VERSION 3.10)

project(LocalFileServer)



set(CMAKE_CXX_STANDARD 17)



# سرور

add_executable(server server.cpp)

# کلاینت

add_executable(client client.cpp)
