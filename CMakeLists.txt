cmake_minimum_required(VERSION 3.10)
project(TramPathMapping)

set(CMAKE_CXX_STANDARD 17)

# Поиск OpenCV (если нужно)
find_package(OpenCV REQUIRED)

# Включаем директорию заголовков
include_directories(include)

# Добавляем исполняемый файл, указывая main.cpp И TrajectoryReader.cpp
add_executable(TramPathMapping
    main.cpp
    src/TrajectoryReader.cpp
)

# Линкуем (если нужно) с OpenCV или другими библиотеками
target_link_libraries(TramPathMapping ${OpenCV_LIBS})
