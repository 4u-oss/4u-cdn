# Amazon Linux
FROM amazonlinux:latest

# Initialize
WORKDIR /home/ec2-user
RUN yum update -y
RUN yum upgrade -y
RUN yum groupinstall -y "Development Tools"
RUN yum install -y git
RUN git clone https://github.com/4u-oss/4u-cdn.git 4u-cdn
WORKDIR /home/ec2-user/4u-cdn
COPY ./certificate.pem /home/ec2-user/4u-cdn/certificate.pem
COPY ./key.pem /home/ec2-user/4u-cdn/key.pem
EXPOSE 443

# ImageMagick
RUN yum install -y wget libjpeg-turbo-devel libpng-devel libtiff-devel libwebp-devel giflib-devel libxml2-devel
RUN wget https://imagemagick.org/archive/ImageMagick.tar.gz
RUN tar xvzf ImageMagick.tar.gz
WORKDIR /home/ec2-user/4u-cdn/ImageMagick-7.1.1-33
RUN yum install -y gcc g++
RUN ./configure
RUN make
RUN make install
WORKDIR /home/ec2-user/4u-cdn

# CMake
RUN yum install -y cmake

# AWS SDK for C++
RUN yum install -y libcurl-devel openssl-devel libuuid-devel pulseaudio-libs-devel
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
RUN cmake ..
RUN cmake --build .
CMD [ "./main" ]