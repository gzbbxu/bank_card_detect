# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
link_directories("${CMAKE_CURRENT_LIST_DIR}/../../main/jniLibs/${ANDROID_ABI}")
message("${CMAKE_CURRENT_LIST_DIR}../../main/jniLibs/${ANDROID_ABI}")
set(CMAKE_EXE_LINK_FLAGS "-Wl -hash-style=sysv")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")





include_directories("${CMAKE_CURRENT_LIST_DIR}/service")
include_directories("${CMAKE_CURRENT_LIST_DIR}/service/socket")
include_directories("${CMAKE_CURRENT_LIST_DIR}/service/container")

file(GLOB native_srcs
        "native-lib.cpp"

        "service/*.cpp"
        "service/socket/*.c"
        "service/container/*.hpp"

        )
message("=======================")
message(${native_srcs})

add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        ${native_srcs}
        # Provides a relative path to your source file(s).
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        native-lib


        ${log-lib}
        opencv_java3

        jnigraphics
        android
        z

        )