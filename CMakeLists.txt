cmake_minimum_required(VERSION 3.20)
project("4u-cdn" VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)

include(FetchContent)

# uSockets & uWebSockets
FetchContent_Declare(
  uWebSockets_git
  GIT_REPOSITORY https://github.com/uNetworking/uWebSockets.git
  GIT_TAG v20.66.0
)
FetchContent_MakeAvailable(uWebSockets_git)
set(uSockets_src ${uwebsockets_git_SOURCE_DIR}/uSockets/src)
file(GLOB_RECURSE uSockets_sources ${uSockets_src}/*.c ${uSockets_src}/*.cpp ${uSockets_src}/*.h)
add_library(uSockets STATIC ${uSockets_sources})
target_compile_definitions(uSockets PRIVATE LIBUS_USE_OPENSSL)
target_link_libraries(uSockets PUBLIC ${OPENSSL_LIBRARIES} ${ZLIB_LIBRARIES})
target_include_directories(uSockets PUBLIC ${uSockets_src} ${OPENSSL_INCLUDE_DIR} ${ZLIB_INCLUDE_DIRS})

set(uWebSockets_src ${uwebsockets_git_SOURCE_DIR}/src)
add_library(uWebSockets INTERFACE)
target_include_directories(uWebSockets INTERFACE ${uWebSockets_src})
target_link_libraries(uWebSockets INTERFACE uSockets)

# AWS SDK
find_package(AWSSDK COMPONENTS REQUIRED s3)

# ImageMagick
add_definitions(-DMAGICKCORE_HDRI_ENABLE=1)
add_definitions(-DMAGICKCORE_QUANTUM_DEPTH=16)
find_path(ImageMagick_INCLUDE_DIR
  NAMES magick/magick-baseconfig.h Magick++.h
  PATHS
    "/usr/local/include"
    "/opt/homebrew/include"
  PATH_SUFFIXES
    ImageMagick ImageMagick-6 ImageMagick-7
  NO_DEFAULT_PATH
)
find_library(ImageMagick_Magick++_LIBRARY
  NAMES
    Magick++ CORE_RL_Magick++_
    Magick++-6 Magick++-7
    Magick++-Q8 Magick++-Q16 Magick++-Q16HDRI Magick++-Q8HDRI
    Magick++-6.Q64 Magick++-6.Q32 Magick++-6.Q64HDRI Magick++-6.Q32HDRI
    Magick++-6.Q16 Magick++-6.Q8 Magick++-6.Q16HDRI Magick++-6.Q8HDRI
    Magick++-7.Q64 Magick++-7.Q32 Magick++-7.Q64HDRI Magick++-7.Q32HDRI
    Magick++-7.Q16 Magick++-7.Q8 Magick++-7.Q16HDRI Magick++-7.Q8HDRI
  PATHS
    "/usr/local/lib"
    "/opt/homebrew/lib"
  DOC "Path to the ImageMagick Magick++ library."
  NO_DEFAULT_PATH
)
find_library(ImageMagick_MagickWand_LIBRARY
  NAMES
    MagickWand CORE_RL_wand_ CORE_RL_MagickWand_
    MagickWand-6 MagickWand-7
    MagickWand-Q16 MagickWand-Q8 MagickWand-Q16HDRI MagickWand-Q8HDRI
    MagickWand-6.Q64 MagickWand-6.Q32 MagickWand-6.Q64HDRI MagickWand-6.Q32HDRI
    MagickWand-6.Q16 MagickWand-6.Q8 MagickWand-6.Q16HDRI MagickWand-6.Q8HDRI
    MagickWand-7.Q64 MagickWand-7.Q32 MagickWand-7.Q64HDRI MagickWand-7.Q32HDRI
    MagickWand-7.Q16 MagickWand-7.Q8 MagickWand-7.Q16HDRI MagickWand-7.Q8HDRI
  PATHS
    "/usr/local/lib"
    "/opt/homebrew/lib"
  DOC "Path to the ImageMagick MagickWand library."
  NO_DEFAULT_PATH
)
find_library(ImageMagick_MagickCore_LIBRARY
  NAMES
    MagickCore CORE_RL_magick_ CORE_RL_MagickCore_
    MagickCore-6 MagickCore-7
    MagickCore-Q16 MagickCore-Q8 MagickCore-Q16HDRI MagickCore-Q8HDRI
    MagickCore-6.Q64 MagickCore-6.Q32 MagickCore-6.Q64HDRI MagickCore-6.Q32HDRI
    MagickCore-6.Q16 MagickCore-6.Q8 MagickCore-6.Q16HDRI MagickCore-6.Q8HDRI
    MagickCore-7.Q64 MagickCore-7.Q32 MagickCore-7.Q64HDRI MagickCore-7.Q32HDRI
    MagickCore-7.Q16 MagickCore-7.Q8 MagickCore-7.Q16HDRI MagickCore-7.Q8HDRI
  PATHS
    "/usr/local/lib"
    "/opt/homebrew/lib"
  DOC "Path to the ImageMagick MagickCore library."
  NO_DEFAULT_PATH
)
set(ImageMagick_LIBRARIES ${ImageMagick_Magick++_LIBRARY} ${ImageMagick_MagickWand_LIBRARY} ${ImageMagick_MagickCore_LIBRARY})
include_directories(${ImageMagick_INCLUDE_DIR})

# main
add_executable(main main.cpp)
target_link_libraries(main PRIVATE ${AWSSDK_LINK_LIBRARIES} ${ImageMagick_LIBRARIES} uWebSockets)