cmake_minimum_required(VERSION 2.8.12)
project(pyxpcs)

add_subdirectory(pybind11)
pybind11_add_module(pyxpcs src/main.cpp)
