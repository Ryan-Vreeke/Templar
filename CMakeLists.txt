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

# Create a static library for the project
add_library(${PROJECT_NAME} STATIC ${SOURCES})

# Link nlohmann_json to the static library
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

# Find and link pthread to the static library
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# Specify output directory for the library
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Optional: Create an executable for testing purposes
# add_executable(${PROJECT_NAME}_test ${SOURCES})

# Link the static library and nlohmann_json to the test executable
# target_link_libraries(${PROJECT_NAME}_test PRIVATE ${PROJECT_NAME} nlohmann_json::nlohmann_json Threads::Threads)

# Ensure the test executable has the necessary include directories
# target_include_directories(${PROJECT_NAME}_test PRIVATE ${PROJECT_SOURCE_DIR}/includes)

# Specify output directory for the test executable
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#Export temple library as a reusable target
