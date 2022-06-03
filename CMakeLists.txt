cmake_minimum_required(VERSION 3.0)

project("EDACUP")

set (CMAKE_CXX_STANDARD 11)

# From "Working with CMake" documentation:
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin" OR ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # AddressSanitizer (ASan)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif()
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # UndefinedBehaviorSanitizer (UBSan)
    add_compile_options(-fsanitize=undefined)
    add_link_options(-fsanitize=undefined)
endif()

# raylib-cpp
find_package(raylib_cpp QUIET)
if (NOT raylib_cpp_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib_cpp
        GIT_REPOSITORY https://github.com/RobLoach/raylib-cpp.git
        GIT_TAG d3d8a4f250efb572dbd7d245ed50116c0e6dc70d
    )
    FetchContent_MakeAvailable(raylib_cpp)
endif()

add_executable(edacup main.cpp MQTTClient2.cpp GameController.cpp Player.cpp AStar.cpp)

# Packages
find_package(raylib CONFIG REQUIRED)
find_package(mosquitto REQUIRED)
find_package(OpenSSL REQUIRED)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_include_directories(edacup PRIVATE ${raylib_INCLUDE_DIRS} ${MOSQUITTO_INCLUDE_DIRS})
    target_link_libraries(edacup PRIVATE ${raylib_LIBRARIES} raylib_cpp ${MOSQUITTO_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_include_directories(edacup PRIVATE ${raylib_INCLUDE_DIRS} raylib_cpp ${MOSQUITTO_INCLUDE_DIRS})
    target_link_libraries(edacup PRIVATE ${raylib_LIBRARIES} raylib_cpp ${MOSQUITTO_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_include_directories(edacup PRIVATE ${RAYLIB_INCLUDE_DIRS} raylib_cpp ${MOSQUITTO_INCLUDE_DIRS})
    target_link_libraries(edacup PRIVATE ${RAYLIB_LIBRARIES} raylib_cpp ${MOSQUITTO_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # From "Working with CMake" documentation:
    target_link_libraries(edacup PRIVATE "-framework IOKit" "-framework Cocoa" "-framework OpenGL")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(edacup PRIVATE m ${CMAKE_DL_LIBS} raylib raylib_cpp pthread GL rt X11)
endif()
