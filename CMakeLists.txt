cmake_minimum_required(VERSION 3.16)
project(radxa_converter VERSION 3.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Поиск Qt5
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network Concurrent)

# Поиск libssh
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBSSH REQUIRED libssh)

# Добавляем пути для include
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${LIBSSH_INCLUDE_DIRS})

# Список исходных файлов
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/sshclient.cpp
    src/settingsmanager.cpp
    src/filemanagerdialog.cpp
    src/batchqueuedialog.cpp
    src/profilesdialog.cpp
    src/ffmpegparser.cpp
    src/conversionworker.cpp
)

# Создаем исполняемый файл
add_executable(radxa_converter ${SOURCES})

# Линковка библиотек
target_link_libraries(radxa_converter
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
    Qt5::Concurrent
    ${LIBSSH_LIBRARIES}
)

# Добавляем флаги компиляции для libssh
target_compile_options(radxa_converter PRIVATE ${LIBSSH_CFLAGS_OTHER})

message(STATUS "Configured successfully")
