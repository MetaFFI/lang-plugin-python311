cmake_minimum_required(VERSION 3.16.3)

project(lang-plugin-python3 VERSION 0.0.1)

include(CTest)
enable_testing()
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# To allow CMake to find boost on windows these environment variables must be set:
if(WIN32)
	set(BOOST_INCLUDEDIR    $ENV{Boost_INCLUDE_DIR})
	set(BOOST_LIBRARYDIR    $ENV{Boost_INCLUDE_DIR}/lib64-msvc-14.3)
	set(BOOST_ROOT          $ENV{Boost_INCLUDE_DIR}/boost)
else()
	set(BOOST_INCLUDEDIR    /usr/include)
	set(BOOST_LIBRARYDIR    /usr/lib)
	set(BOOST_ROOT          /usr/include/boost)
endif()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/compiler/)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/runtime/)