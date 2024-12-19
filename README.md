# Intraday Strategy Builder CPP Server

Communicates directly with the Trader Workstation API via socket to gather data and forward via websocket. The primary purpose is to link to a webserver that will collect and refine this data. This component is unnecessary for forwarding data alone, but the purpose is to have a stable back-end component capable of processing high frequency data, in the event of future implementations of algorithmic trading strategies. 

Contained inside is also a self developed wrapper to the TWS API, that tracks all requests and incoming data via a publisher/subscriber methodology, and is highly customizable. (See tws_data_handler)

All websocket and server implementations are are based on the CppServer library by [chronoxor](https://github.com/chronoxor)

# How to build with CMake

### Create build directory in /IntradayStrategyBuilder
```bash
cd IntradayStrategyBuilder && mkdir Build
```

### Run the build script via bash
```bash
./build_isb.sh
```

# Building with Docker

### Build the server with the docker file
```bash
docker build -t intraday_strategy_builder .
```

### Start the docker container and enter the shell
```bash
docker run -it --rm --name ISB_server intraday_strategy_builder /bin/bash
```

### Start the server manually via the executable
* Note: build directory is lowercase in docker container
```bash
cd IntradayStrategyBuilder/build
```

```bash
./TwsStrategyCppServer 8443 [SSL Certificate Path] [TWS IP]
```
