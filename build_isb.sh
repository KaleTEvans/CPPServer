# !/bin/sh

cd IntradayStrategyBuilder

protoc --cpp_out=generated/ messages.proto

cd Build

cmake ..
make

