cmake_minimum_required (VERSION 3.0)
# to find our openvdb cmake file
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

project (openvdbmeshunion)

find_package(OPENVDB REQUIRED)
include_directories(${OPENVDB_INCLUDE_DIRS})
list(APPEND LIBS ${OPENVDB_LIBRARIES})

add_executable(openvdbmeshunion src/main.cpp src/objmodel.cpp)
set_property(TARGET openvdbmeshunion PROPERTY CXX_STANDARD 14)
target_link_libraries(openvdbmeshunion ${LIBS})

