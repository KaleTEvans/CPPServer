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

# Set up SSL Certificate

# Create CA Private key
RUN openssl genrsa -passout pass:querty -out ca-secret.key 4096
# Remove passphrase
RUN openssl rsa -passin pass:qwerty -in ca-secret.key -out ca.key
# Create CA self-signed certificate
RUN openssl req -new -x509 -days 3650 -subj '/C=US/ST=Texas/L=Austin/O=ISB_Server/OU=dev/CN=isb_server' -key ca.key -out ca.crt
# Convert CA self-signed certificate to PFX
RUN openssl pkcs12 -export -passout pass:qwerty -inkey ca.key -in ca.crt -out ca.pfx
# Convert CA self-signed certificate to PEM
RUN openssl pkcs12 -passin pass:qwerty -passout pass:qwerty -in ca.pfx -out ca.pem

# Create private key for server
RUN openssl genrsa -passout pass:qwerty -out server-secret.key 4096
# Remove passphrase
RUN openssl rsa -passin pass:qwerty -in server-secret.key -out server.key
# Create CSR for server
RUN openssl req -new -subj '/C=US/ST=Texas/L=Austin/O=ISB_Server/OU=dev/CN=isb_server' -key server.key -out server.csr
# Create certificate for the server
RUN openssl x509 -req -days 3650 -in server.csr -CA ca.crt -CAkey ca.key -set_serial 01 -out server.crt
# Convert the server certificate to PFX
RUN openssl pkcs12 -export -passout pass:qwerty -inkey server.key -in server.crt -out server.pfx
# Convert the server certificate to PEM
RUN openssl pkcs12 -passin pass:qwerty -passout pass:qwerty -in server.pfx -out server.pem

# Create private key for client
RUN openssl genrsa -passout pass:qwerty -out client-secret.key 4096
# Remove passphrase
RUN openssl rsa -passin pass:qwerty -in client-secret.key -out client.key
# Create CSR for client
RUN openssl req -new -subj '/C=US/ST=Texas/L=Austin/O=ISB_Server/OU=dev/CN=isb_server' -key client.key -out client.csr
# Create the client certificate
RUN openssl x509 -req -days 3650 -in client.csr -CA ca.crt -CAkey ca.key -set_serial 01 -out client.crt
# Convert the client certificate to PFX
RUN openssl pkcs12 -export -passout pass:qwerty -inkey client.key -in client.crt -out client.pfx
# Convert the client certificate to PEM
RUN openssl pkcs12 -passin pass:qwerty -passout pass:qwerty -in client.pfx -out client.pem

# Create DH parameters
RUN openssl dhparam -out dh4096.pem 4096
 
# Build the main project
RUN mkdir -p /IntradayStrategyBuilder/build \
    && cd /IntradayStrategyBuilder/build \
    && cmake .. \
    && make -j$(nproc)

# Expose the application port (e.g., 8443)
EXPOSE 8443

WORKDIR /IntradayStrategyBuilder/build