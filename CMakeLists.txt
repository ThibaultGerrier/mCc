#Courtesy of alex-bl

cmake_minimum_required (VERSION 3.9.6)
project (mCc)

# set global compiler flags (append them)
# append -Wextra when all ast-implmentations are provided
string(APPEND CMAKE_C_FLAGS " -std=c11 -Wall -Werror")
string(APPEND CMAKE_CXX_FLAGS " -std=c++14 -Wall -Werror")

# enable this to see the exact commands form cmake
set(CMAKE_VERBOSE_MAKEFILE OFF)

# build-type
set(CMAKE_BUILD_TYPE Debug)

# gtest download-dest
set(GTEST_DOWNLOAD_DESTINATION "${PROJECT_SOURCE_DIR}/subprojects/cmake/googletest-release1.8.0")
set(GBENCHMARK_DOWNLOAD_DESTINATION "${PROJECT_SOURCE_DIR}/subprojects/cmake/googlebenchmark-release-1.3.0")

#=================================================================== building

# add the binary tree to the search path for include files
# so that we will find the headers
include_directories("${PROJECT_SOURCE_DIR}/include" ${CMAKE_CURRENT_BINARY_DIR})
# include test-utils-header also => separated from source
include_directories("${PROJECT_SOURCE_DIR}/test/include" ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(src)

# link libs with executables
add_executable(mCc src/bin/mCc.c)
target_link_libraries(mCc mCc_lib)

add_executable(mC_to_dot src/bin/mC_to_dot.c src/tac.c include/mCc/tac.h)
target_link_libraries(mC_to_dot mCc_lib)


