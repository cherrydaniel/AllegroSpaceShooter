set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_minimum_required(VERSION 3.10)

project(AllegroSpaceShooter)

find_package(Allegro CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)

add_executable(AllegroSpaceShooter
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/game.cpp
    ${CMAKE_SOURCE_DIR}/src/input.cpp
    ${CMAKE_SOURCE_DIR}/src/file.cpp
    ${CMAKE_SOURCE_DIR}/src/world.cpp
    ${CMAKE_SOURCE_DIR}/src/assetloader.cpp
    ${CMAKE_SOURCE_DIR}/src/weapons.cpp
    ${CMAKE_SOURCE_DIR}/src/userdata.cpp
    ${CMAKE_SOURCE_DIR}/src/settings.cpp
    ${CMAKE_SOURCE_DIR}/src/enemy.cpp
    ${CMAKE_SOURCE_DIR}/src/threadpool.cpp
    ${CMAKE_SOURCE_DIR}/src/eventloop.cpp
)

file(
    COPY ${CMAKE_CURRENT_LIST_DIR}/src/assets
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(AllegroSpaceShooter PRIVATE
    Allegro::allegro
    Allegro::allegro_primitives
    Allegro::allegro_image
    Allegro::allegro_font
    Allegro::allegro_ttf
    EnTT::EnTT)
