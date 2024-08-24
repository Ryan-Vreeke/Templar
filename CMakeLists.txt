cmake_minimum_required(VERSION 3.11)  # Required for FetchContent
project(temple VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR}/includes)

set(SRCDIR "${PROJECT_SOURCE_DIR}/src")
file(GLOB SOURCES "${SRCDIR}/*.cpp")

include(FetchContent)

# Declare and fetch nlohmann_json
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)

FetchContent_MakeAvailable(json)

add_executable(${PROJECT_NAME} ${SOURCES})

# Link nlohmann_json
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

# Link pthread for the executable
target_link_libraries(${PROJECT_NAME} PRIVATE pthread)

# Specify output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
