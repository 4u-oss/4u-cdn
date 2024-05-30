cmake_minimum_required(VERSION 3.20)
project("4u-cdn" VERSION 1.0.0 LANGUAGES CXX C)

add_executable(main main.cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)

# uWebSockets
if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/uWebSockets)
  execute_process(
    COMMAND git clone --recursive --depth 1 --branch v20.62.0 https://github.com/uNetworking/uWebSockets
  )
endif()
set(uWS_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/uWebSockets)
set(USOCKETS_DIR ${uWS_SOURCE_DIR}/uSockets/src)
set(UWEBSOCKETS_DIR ${uWS_SOURCE_DIR}/src)
# https://github.com/uNetworking/uWebSockets/discussions/1218#discussioncomment-431071
add_library(
  uSockets STATIC
  ${USOCKETS_DIR}/libusockets.h
  ${USOCKETS_DIR}/bsd.c
  ${USOCKETS_DIR}/context.c
  ${USOCKETS_DIR}/loop.c
  ${USOCKETS_DIR}/socket.c
  ${USOCKETS_DIR}/udp.c
  ${USOCKETS_DIR}/quic.c
  ${USOCKETS_DIR}/quic.h
  ${USOCKETS_DIR}/crypto/openssl.c
  ${USOCKETS_DIR}/crypto/sni_tree.cpp
  ${USOCKETS_DIR}/eventing/epoll_kqueue.c
  ${USOCKETS_DIR}/eventing/gcd.c
  ${USOCKETS_DIR}/eventing/libuv.c
  ${USOCKETS_DIR}/eventing/asio.cpp
)
target_compile_definitions(uSockets PRIVATE LIBUS_USE_OPENSSL)
target_compile_options(uSockets PUBLIC -flto)
target_link_options(uSockets
  PRIVATE -lstdc++
  PUBLIC -flto
)
target_link_libraries(uSockets ${OPENSSL_LIBRARIES} ${ZLIB_LIBRARIES})
target_include_directories(uSockets PUBLIC ${USOCKETS_DIR} ${OPENSSL_INCLUDE_DIR} ${ZLIB_INCLUDE_DIRS})
add_library(
  uWebSockets
  ${UWEBSOCKETS_DIR}/App.h
  ${UWEBSOCKETS_DIR}/AsyncSocketData.h
  ${UWEBSOCKETS_DIR}/AsyncSocket.h
  ${UWEBSOCKETS_DIR}/BloomFilter.h
  ${UWEBSOCKETS_DIR}/HttpContextData.h
  ${UWEBSOCKETS_DIR}/HttpContext.h
  ${UWEBSOCKETS_DIR}/HttpParser.h
  ${UWEBSOCKETS_DIR}/HttpResponseData.h
  ${UWEBSOCKETS_DIR}/HttpResponse.h
  ${UWEBSOCKETS_DIR}/HttpRouter.h
  ${UWEBSOCKETS_DIR}/LoopData.h
  ${UWEBSOCKETS_DIR}/Loop.h
  ${UWEBSOCKETS_DIR}/MessageParser.h
  ${UWEBSOCKETS_DIR}/MoveOnlyFunction.h
  ${UWEBSOCKETS_DIR}/Multipart.h
  ${UWEBSOCKETS_DIR}/PerMessageDeflate.h
  ${UWEBSOCKETS_DIR}/ProxyParser.h
  ${UWEBSOCKETS_DIR}/QueryParser.h
  ${UWEBSOCKETS_DIR}/TopicTree.h
  ${UWEBSOCKETS_DIR}/Utilities.h
  ${UWEBSOCKETS_DIR}/WebSocketContextData.h
  ${UWEBSOCKETS_DIR}/WebSocketContext.h
  ${UWEBSOCKETS_DIR}/WebSocketData.h
  ${UWEBSOCKETS_DIR}/WebSocketExtensions.h
  ${UWEBSOCKETS_DIR}/WebSocket.h
  ${UWEBSOCKETS_DIR}/WebSocketHandshake.h
  ${UWEBSOCKETS_DIR}/WebSocketProtocol.h
)
set_target_properties(uWebSockets PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(uWebSockets PUBLIC cxx_std_17)
target_link_libraries(uWebSockets uSockets)
target_include_directories(uWebSockets PUBLIC ${UWEBSOCKETS_DIR})

# AWS SDK
# find_package(AWSSDK COMPONENTS REQUIRED s3)

# ImageMagick
find_path(ImageMagick_INCLUDE_DIR
  NAMES magick/magick-baseconfig.h Magick++.h
  PATHS
    "/usr/local/include"
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
  DOC "Path to the ImageMagick MagickCore library."
  NO_DEFAULT_PATH
)
set(ImageMagick_LIBRARIES ${ImageMagick_Magick++_LIBRARY} ${ImageMagick_MagickWand_LIBRARY} ${ImageMagick_MagickCore_LIBRARY})
include_directories(${ImageMagick_INCLUDE_DIR})

# main
target_link_libraries(main PRIVATE ${AWSSDK_LINK_LIBRARIES} ${ImageMagick_LIBRARIES} uWebSockets)