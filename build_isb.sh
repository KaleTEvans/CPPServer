# !/bin/sh

cd IntradayStrategyBuilder

rm -rf Build
mkdir Build && cd Build

cmake ..
make

