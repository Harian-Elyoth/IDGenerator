# Minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Project name
project(ClientApp)

# Set C++ standard (C++11 in this case, you can change it to C++14, C++17, etc.)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories for built binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Collect source files from the src/ directory
file(GLOB_RECURSE CLIENT_SOURCES "src/*.cpp")

# Create executable
add_executable(${PROJECT_NAME} ${CLIENT_SOURCES})

# Add include directories
target_include_directories(ClientApp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Optionally, you can add additional libraries or dependencies here
# Example:
# find_package(YourLibrary REQUIRED)
# target_link_libraries(${PROJECT_NAME} YourLibrary::YourLibrary)

# Set the output directory for the compiled executable
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Optionally, you can add specific compiler flags
# Example:
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)

# Optionally, enable testing (add_subdirectory(test) and add_test sections)
# enable_testing()
# add_subdirectory(test)