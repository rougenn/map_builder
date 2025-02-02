cmake_minimum_required(VERSION 3.10)
project(TramPathMapping)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIRS})

find_package(grid_map_core REQUIRED)
include_directories(${grid_map_core_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(TramPathMapping
    main.cpp
    src/TrajectoryReader.cpp
    src/Camera.cpp
    src/GlobalGridMapHandler.cpp
    src/QuadrantMap.cpp
)

target_link_libraries(TramPathMapping
    ${OpenCV_LIBS}
    grid_map_core
)
