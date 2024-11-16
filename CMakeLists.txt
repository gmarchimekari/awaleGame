cmake_minimum_required(VERSION 3.10)

# Set the project name
project(Awale)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

# Specify directories for client and server source files
set(CLIENT_DIR ${CMAKE_SOURCE_DIR}/client)
set(SERVER_DIR ${CMAKE_SOURCE_DIR}/server)

# Get the header files for the client
file(GLOB CLIENT_HEADERS ${CLIENT_DIR}/include/*.h)

# Get the .cpp files for the client
file(GLOB CLIENT_SOURCES ${CLIENT_DIR}/src/*.c)

# Get the header files for the server
file(GLOB SERVER_HEADERS ${SERVER_DIR}/include/*.h)

# Get the .cpp files for the server
file(GLOB SERVER_SOURCES ${SERVER_DIR}/src/*.c)

# Add include paths
include_directories(
  ${CLIENT_DIR/include/}  # Include the client headers
  ${SERVER_DIR/include/}  # Include the server headers
)

# SERVER
# Add the executable
add_executable(server ${SERVER_SOURCES})

# Add include directories for this target (awale executable)
target_include_directories(server PRIVATE
  ${SERVER_DIR}/include  # Include server headers
)

# CLIENT
# Add the executable
add_executable(client ${CLIENT_SOURCES})

# Add include directories for this target (client executable)
target_include_directories(client PRIVATE
  ${CLIENT_DIR}/include  # Include client headers
)