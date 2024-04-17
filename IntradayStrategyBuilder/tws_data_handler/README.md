# Tws Data Handler

## Purpose

This serves as a wrapper to the Interactive Brokers TWS C++ Api. In virtually every example of the TWS Api usage I have seen in C++, a single instance of the EWrapper class is created to handle one-time requests for data. This is fine for many simple use cases, but the goal of this wrapper is to allow for simple distribution of the Api data across multiple components in a program. For example, an automated trading program will likely need several components to interact with the Api simultaneously. One component may want to receive and store tick data for analysis, while another may need to retrieve data for monitoring active trades. In many cases such as this, a single use EWrapper would not be applicable. 

### Methods

The wrapper has two primary methods for retreiving data. View the SampleDataSubscriber class for examples on how to use and retrieve data from each of these methods.

#### Single-Event Data

Any wrapper call that does not stream data indefinitely or until a cancellation request is sent falls under the single event category. For example, reqContractDetails and reqHistoricalData are two requests that would fall under this category. Note that the request functions created in the wrapper do not require a reqId, as the ID is auto generated inside the wrapper, and will be returned by the request function. This ID can then be used to check on the completion of a request, via the checkEventCompleted method in the wrapper. This allows for efficient organization of single event requests, allowing multiple requests to be made simultaneously across the program without the need for custom filtering of the data.

#### Streaming Data

The second primary method to this wrapper is the message bus, which handles and organizes all streaming data sent to the wrapper. The message bus handles several different types, such as real time bar data, tick data, etc. Any program component can subscribe to the bus using a specified type, and retrieve all data of that type that is posted to the message bus. The reasoning for using this callback method over the single event methods is so that only a single subscription needs to be made to listen for each callback type. This simplifies the code when needing to make several simultaneous requests, such as for tick data for an entire options chain. 

## Components

### TwsWrapper

Central interface with the TWS Api. Maintains socket connection with tws and distributes all client requests. Wrapper component translates all incoming messages to readable data and sends to the Event Handler in sequential order. Each single event method is contained in a hash map in the wrapper, which maps all received data by the ID of the request.

### TwsEventHandler

Cetnral data distribution mechanism for the Wrapper's streaming requests. Contains many user defined types that any component of the program can subscribe to. When these types are received by the wrapper and sent to the handler, any subscriber is immediately notified. The event handler does not contain any built-in filter mechanisms, so it will be the role of the subscriber to filter only the data that specific component needs. Every event will have a reqId attached to allow for easy filtering.