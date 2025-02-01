cmake_minimum_required(VERSION 3.10)
project(TramPathMapping)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск OpenCV
find_package(OpenCV REQUIRED)

# Поиск Eigen3
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIRS})

# Поиск GridMap Core (используем только grid_map_core)
find_package(grid_map_core REQUIRED)
include_directories(${grid_map_core_INCLUDE_DIRS})

# Включаем пути к вашим заголовочным файлам
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Собираем исполняемый файл, включив все исходники
add_executable(TramPathMapping
    main.cpp
    src/TrajectoryReader.cpp
    src/Camera.cpp
    src/GridMapHandler.cpp
)

# Линкуем с необходимыми библиотеками:
# - OpenCV (из переменной ${OpenCV_LIBS})
# - GridMap Core (добавляем grid_map_core)
target_link_libraries(TramPathMapping
    ${OpenCV_LIBS}
    grid_map_core
)
