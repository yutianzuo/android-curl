# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/main/cpp/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/main/cpp/include/curl)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -march=armv7-a")


if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    add_definitions(-DENABLE_LOG)
    add_definitions(-DCURL_DEBUG)
endif ()


if (${ANDROID_ABI} STREQUAL armeabi-v7a)
    link_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/lib/v7a)
elseif (${ANDROID_ABI} STREQUAL "arm64-v8a")
    link_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/lib/arm64)
elseif (${ANDROID_ABI} STREQUAL x86)
    link_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/lib/x86)
elseif (${ANDROID_ABI} STREQUAL x86_64)
    link_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/lib/x86_64)
endif ()


#add_definitions(-DENABLE_LOG)

add_library( # Sets the name of the library.
        native_net

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        src/main/cpp/jni.cpp src/main/cpp/manager/requestmanager.cpp)

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



#add_library(libssl
#        STATIC
#        IMPORTED)
#
#add_library(libcrypto
#        STATIC
#        IMPORTED)
#
#add_library(libcurl
#        STATIC
#        IMPORTED)
#
##add_library(libnghttp2
##        STATIC
##        IMPORTED)
#
#add_library(libmbedcrypto
#        STATIC
#        IMPORTED)
#
#add_library(libmbedtls
#        STATIC
#        IMPORTED)
#
#add_library(libmbedx509
#        STATIC
#        IMPORTED)

message(WARNING "ANDROID_ABI = ${ANDROID_ABI}")


#if (${ANDROID_ABI} STREQUAL "x86")
#    message(WARNING "ANDROID_ABI is x86")
#    set_target_properties( # Specifies the target library.
#            libssl
#
#            # Specifies the parameter you want to define.
#            PROPERTIES IMPORTED_LOCATION
#
#            # Provides the path to the library you want to import.
#            #../../../../src/main/cpp/lib/libssl.a)
#            ${CMAKE_SOURCE_DIR}/src/main/cpp/lib/x86/libssl.a)
#
#    set_target_properties( # Specifies the target library.
#            libcrypto
#
#            # Specifies the parameter you want to define.
#            PROPERTIES IMPORTED_LOCATION
#
#            # Provides the path to the library you want to import.
#            #../../../../src/main/cpp/lib/libcrypto.a)
#            ${CMAKE_SOURCE_DIR}/src/main/cpp/lib/x86/libcrypto.a)
#
#
#    set_target_properties( # Specifies the target library.
#            libcurl
#
#            # Specifies the parameter you want to define.
#            PROPERTIES IMPORTED_LOCATION
#
#            # Provides the path to the library you want to import.
#            ${CMAKE_SOURCE_DIR}/src/main/cpp/lib/x86/libcurl.a)
#
#elseif (${ANDROID_ABI} STREQUAL "armeabi-v7a")
#    set_target_properties( # Specifies the target library.
#            libssl
#
#            # Specifies the parameter you want to define.
#            PROPERTIES IMPORTED_LOCATION
#
#            # Provides the path to the library you want to import.
#            #../../../../src/main/cpp/lib/libssl.a)
#            ${CMAKE_SOURCE_DIR}/src/main/cpp/lib/v7a/libssl.a)
#
#    set_target_properties( # Specifies the target library.
#            libcrypto
#
#            # Specifies the parameter you want to define.
#            PROPERTIES IMPORTED_LOCATION
#
#            # Provides the path to the library you want to import.
#            #../../../../src/main/cpp/lib/libcrypto.a)
#            ${CMAKE_SOURCE_DIR}/src/main/cpp/lib/v7a/libcrypto.a)
#
#    set_target_properties( # Specifies the target library.
#            libcurl
#
#            # Specifies the parameter you want to define.
#            PROPERTIES IMPORTED_LOCATION
#
#            # Provides the path to the library you want to import.
#            ${CMAKE_SOURCE_DIR}/src/main/cpp/lib/v7a/libcurl.a)
#
#    set_target_properties( # Specifies the target library.
#            libnghttp2
#
#            # Specifies the parameter you want to define.
#            PROPERTIES IMPORTED_LOCATION
#
#            # Provides the path to the library you want to import.
#            ${CMAKE_SOURCE_DIR}/src/main/cpp/lib/v7a/libnghttp2.a)
#elseif (${ANDROID_ABI} STREQUAL "arm64-v8a")
#    set_target_properties( # Specifies the target library.
#            libssl
#
#            # Specifies the parameter you want to define.
#            PROPERTIES IMPORTED_LOCATION
#
#            # Provides the path to the library you want to import.
#            #../../../../src/main/cpp/lib/libssl.a)
#            ${CMAKE_SOURCE_DIR}/src/main/cpp/lib/arm64/libssl.a)
#
#    set_target_properties( # Specifies the target library.
#            libcrypto
#
#            # Specifies the parameter you want to define.
#            PROPERTIES IMPORTED_LOCATION
#
#            # Provides the path to the library you want to import.
#            #../../../../src/main/cpp/lib/libcrypto.a)
#            ${CMAKE_SOURCE_DIR}/src/main/cpp/lib/arm64/libcrypto.a)
#
#    set_target_properties( # Specifies the target library.
#            libcurl
#
#            # Specifies the parameter you want to define.
#            PROPERTIES IMPORTED_LOCATION
#
#            # Provides the path to the library you want to import.
#            ${CMAKE_SOURCE_DIR}/src/main/cpp/lib/arm64/libcurl.a)
#
#    set_target_properties( # Specifies the target library.
#            libnghttp2
#
#            # Specifies the parameter you want to define.
#            PROPERTIES IMPORTED_LOCATION
#
#            # Provides the path to the library you want to import.
#            ${CMAKE_SOURCE_DIR}/src/main/cpp/lib/arm64/libnghttp2.a)
#
#    set_target_properties( # Specifies the target library.
#            libmbedcrypto
#
#            # Specifies the parameter you want to define.
#            PROPERTIES IMPORTED_LOCATION
#
#            # Provides the path to the library you want to import.
#            ${CMAKE_SOURCE_DIR}/src/main/cpp/lib/arm64/libmbedcrypto.a)
#
#    set_target_properties( # Specifies the target library.
#            libmbedtls
#
#            # Specifies the parameter you want to define.
#            PROPERTIES IMPORTED_LOCATION
#
#            # Provides the path to the library you want to import.
#            ${CMAKE_SOURCE_DIR}/src/main/cpp/lib/arm64/libmbedtls.a)
#
#    set_target_properties( # Specifies the target library.
#            libmbedx509
#
#            # Specifies the parameter you want to define.
#            PROPERTIES IMPORTED_LOCATION
#
#            # Provides the path to the library you want to import.
#            ${CMAKE_SOURCE_DIR}/src/main/cpp/lib/arm64/libmbedx509.a)
#elseif (${ANDROID_ABI} STREQUAL "x86_64")
#    message(WARNING "ANDROID_ABI is x86_64")
#    set_target_properties( # Specifies the target library.
#            libcurl
#
#            # Specifies the parameter you want to define.
#            PROPERTIES IMPORTED_LOCATION
#
#            # Provides the path to the library you want to import.
#            ${CMAKE_SOURCE_DIR}/src/main/cpp/lib/x86_64/libcurl.a)
#
#    set_target_properties( # Specifies the target library.
#            libmbedcrypto
#
#            # Specifies the parameter you want to define.
#            PROPERTIES IMPORTED_LOCATION
#
#            # Provides the path to the library you want to import.
#            ${CMAKE_SOURCE_DIR}/src/main/cpp/lib/x86_64/libmbedcrypto.a)
#
#    set_target_properties( # Specifies the target library.
#            libmbedtls
#
#            # Specifies the parameter you want to define.
#            PROPERTIES IMPORTED_LOCATION
#
#            # Provides the path to the library you want to import.
#            ${CMAKE_SOURCE_DIR}/src/main/cpp/lib/x86_64/libmbedtls.a)
#
#    set_target_properties( # Specifies the target library.
#            libmbedx509
#
#            # Specifies the parameter you want to define.
#            PROPERTIES IMPORTED_LOCATION
#
#            # Provides the path to the library you want to import.
#            ${CMAKE_SOURCE_DIR}/src/main/cpp/lib/x86_64/libmbedx509.a)
#endif ()


# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        native_net
#        libcurl
#        libmbedtls
#        libmbedcrypto
#        libmbedx509
        curl
        mbedtls
        mbedcrypto
        mbedx509
        nghttp2


#        libssl
#        libcrypto
#        libnghttp2

        z
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})