cmake_minimum_required (VERSION 3.0)
# to find our openvdb cmake file
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

project (openvdbtest)

find_package(OPENVDB REQUIRED)
include_directories(${OPENVDB_INCLUDE_DIRS})
list(APPEND LIBS ${OPENVDB_LIBRARIES})

find_package(Boost REQUIRED COMPONENTS program_options)
include_directories(${Boost_INCLUDE_DIRS})
list(APPEND LIBS ${Boost_LIBRARIES})

add_executable(openvdbtest src/main.cpp)
set_property(TARGET openvdbtest PROPERTY CXX_STANDARD 14)
target_link_libraries(openvdbtest ${LIBS})

