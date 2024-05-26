# Amazon Linux
FROM amazonlinux:latest

# Initialize
WORKDIR /home/ec2-user
RUN git clone https://github.com/4u-oss/4u-cdn.git
RUN yum update -y
RUN yum upgrade -y
RUN yum groupinstall -y "Development Tools"

# ImageMagick
RUN yum install -y wget
RUN wget https://imagemagick.org/archive/binaries/magick
RUN chmod +x magick
RUN ./magick --appimage-extract

# CMake
RUN yum install -y cmake

# AWS SDK for C++
RUN yum install -y libcurl-devel openssl-devel libuuid-devel pulseaudio-libs-devel git
RUN git clone --recursive https://github.com/aws/aws-sdk-cpp.git
RUN mkdir build
WORKDIR /home/ec2-user/4u-cdn/build
RUN cmake ../aws-sdk-cpp -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=/usr/local/ -DCMAKE_INSTALL_PREFIX=/usr/local/ -DBUILD_ONLY="s3"
RUN cmake --build . --config=Debug
RUN cmake --install . --config=Debug

# Clear CMake cache for AWS SDK build
WORKDIR /home/ec2-user/4u-cdn
RUN rm -rf build && mkdir build
WORKDIR /home/ec2-user/4u-cdn/build

# Run main.cpp
ADD ../CMakeLists.txt /home/ec2-user/4u-cdn/CMakeLists.txt
ADD ../main.cpp /home/ec2-user/4u-cdn/main.cpp
RUN cmake ..
RUN cmake --build .
CMD [ "./main" ]