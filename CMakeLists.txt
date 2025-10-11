cmake_minimum_required(VERSION 3.5)
project(dspheaders VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Header only library
add_library(dspheaders INTERFACE)

target_include_directories(dspheaders INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
