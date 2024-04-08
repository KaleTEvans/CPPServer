# Tws Data Handler

Handles requests and distributes all incoming data from the Interactive Brokers Trader Workstation. 

## Components

### TwsWrapper

Central interface with the TWS Api. Maintains socket connection with tws and distributes all client requests. Wrapper component translates all incoming messages to readable data and sends to the Event Handler in sequential order.

### TwsEventHandler

Cetnral data distribution mechanism for the Wrapper. Contains many user defined types that any component of the program can subscribe to. When these types are received by the wrapper and sent to the handler, any subscriber is immediately notified. The event handler does not contain any built-in filter mechanisms, so it will be the role of the subscriber to filter only the data that specific component needs. Every event will have a reqId attached to allow for easy filtering.