cmake_minimum_required (VERSION 3.0)

include_directories (../liblwsav/)
add_executable (lwreader main.cpp)
target_link_libraries (lwreader liblwsav)