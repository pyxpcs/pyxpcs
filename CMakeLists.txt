cmake_minimum_required(VERSION 2.8.12)
project(libpyxpcs)

set(SOURCE_DIR "src/libpyxpcs")

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

add_subdirectory(lib/pybind11)
pybind11_add_module(libpyxpcs "${SOURCE_DIR}/sparse_data.cpp" "${SOURCE_DIR}/corr.cpp")
