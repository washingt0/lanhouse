cmake_minimum_required(VERSION 3.22)
project(lan_house)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(CURL_LIBRARY "-lcurl")
find_package(CURL REQUIRED)
find_package(Mimetic REQUIRED)
find_package(Cups REQUIRED)
include_directories(${CURL_INCLUDE_DIR})
include_directories(${CUPS_INCLUDE_DIRS})

add_executable(
        lan_house main.cpp
        curlpp/imap_curl.cpp
        cupspp/print_cups.cpp
        b64/b64.cpp utils/utils.cpp utils/utils.h log/log.cpp log/log.h)

target_link_libraries(lan_house ${CURL_LIBRARIES} ${CUPS_LIBRARIES} Mimetic::Mimetic)
