# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

link_directories("src/main/jniLibs/${ANDROID_ABI}")


#设置生成的so动态库最后输出的路径 方式1
#set(jnilibs "${CMAKE_SOURCE_DIR}/src/main/jniLibs")
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${jnilibs}/${ANDROID_ABI})


#[[include_directories("${CMAKE_CURRENT_LIST_DIR}/src/main/cpp")
include_directories("${CMAKE_CURRENT_LIST_DIR}/src/main/cpp/inferxlite/include")
include_directories("${CMAKE_CURRENT_LIST_DIR}/src/main/cpp/openblas/include")
include_directories("${CMAKE_CURRENT_LIST_DIR}/src/main/cpp/face_veri_3399/face_attr")
include_directories("${CMAKE_CURRENT_LIST_DIR}/src/main/cpp/face_veri_3399/face_detector")
include_directories("${CMAKE_CURRENT_LIST_DIR}/src/main/cpp/face_veri_3399/face_veri")
include_directories("${CMAKE_CURRENT_LIST_DIR}/src/main/cpp/face_veri_3399/utils")]]
include_directories("${CMAKE_CURRENT_LIST_DIR}/src/main/cpp/service")
include_directories("${CMAKE_CURRENT_LIST_DIR}/src/main/cpp/service/socket")
include_directories("${CMAKE_CURRENT_LIST_DIR}/src/main/cpp/service/container")

cmake_minimum_required(VERSION 3.4.1)


set(CMAKE_EXE_LINK_FLAGS "-Wl -hash-style=sysv")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

file(GLOB native_srcs
   #[[ "src/main/cpp/*.cpp"
        "src/main/cpp/face_veri_3399/face_veri/*.cpp"
    "src/main/cpp/face_veri_3399/face_attr/*.cpp"
    "src/main/cpp/face_veri_3399/face_detector/*.cpp"
    "src/main/cpp/face_veri_3399/utils/*.cpp"]]
        "src/main/cpp/*.cpp"
        "src/main/cpp/service/*.cpp"
        "src/main/cpp/service/socket/*.c"
   )
message("hello world")
message(${native_srcs})
add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             ${native_srcs}
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
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                       opencv_java3
                     #[[  inferxlite]]
                   #[[     bitanswer-jni]]
                       jnigraphics
                       android
                       z
                       )
