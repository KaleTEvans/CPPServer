# Use an official lightweight Linux image
FROM ubuntu:20.04

# Set non-interactive mode for automated apt operations
ENV DEBIAN_FRONTEND=noninteractive

# Install necessary dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    g++ \
    git \
    libprotobuf-dev \
    protobuf-compiler \
    libssl-dev \
    libpthread-stubs0-dev \
    libc-dev \
    uuid-dev \
    binutils-dev \
    doxygen \
    libbfd-dev \
    wget \
    ca-certificates \
    gnupg \
    && rm -rf /var/lib/apt/lists/*

# Install Python3 and pip
RUN apt-get update && apt-get install -y python3 python3-pip

# Install gil via pip
RUN pip3 install gil

# Add Kitware APT repository for the latest CMake
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc | gpg --dearmor -o /usr/share/keyrings/kitware.gpg \
    && echo "deb [signed-by=/usr/share/keyrings/kitware.gpg] https://apt.kitware.com/ubuntu/ focal main" > /etc/apt/sources.list.d/kitware.list \
    && apt-get update \
    && apt-get install -y cmake \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory in the container
WORKDIR /IntradayStrategyBuilder

# Copy the entire prroject into the container
COPY ./IntradayStrategyBuilder /IntradayStrategyBuilder

# Ensure the `generated` directory exists
RUN mkdir -p /IntradayStrategyBuilder/generated

# Compile Protobuf files
RUN protoc --cpp_out=generated/ messages.proto

# Navigate to the third-party libs directory
WORKDIR /IntradayStrategyBuilder/third_party_libs

# Clone the CppServer repository
RUN rm -rf CppServer && git clone https://github.com/KaleTEvans/CppServerLibrary.git CppServer

# Navigate to the CppServer directory and update dependencies using gil
WORKDIR /IntradayStrategyBuilder/third_party_libs/CppServer
RUN gil update

# Build the main project
RUN mkdir -p /IntradayStrategyBuilder/build \
    && cd /IntradayStrategyBuilder/build \
    && cmake .. \
    && make -j$(nproc)

# Expose the application port (e.g., 8443)
EXPOSE 8443