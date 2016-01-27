# Zink
ZeroMQ pub-sub subscriber sink for testing and debugging purposes. Prints out full message and its parts or just a dot for each received message, depending on print verobosity. Zink can also pause or exit after certain number of received messages to test impact of ZeroMQ send and receive buffers.

## Dependencies

[ZeroMQ](http://zeromq.org)

[zmq.hpp](https://github.com/zeromq/cppzmq)

## Configuration
Zink is configured using environment variables.

Environment variable | Type | Default | Description
--- | --- | --- | ---
ZINK_PRINT | MESSAGES, PARTS, COMPACT | MESSAGES | Output print verobosity
ZINK_PAUSE_AFTER | number | -1 | Pauses after n complete messages
ZINK_EXIT_AFTER | number | -1 | Exits after n complete messages
ZINK_ENDPOINT | string | tcp://*:13415 | ZeroMQ endpoint
ZINK_CURVE | boolean | false | Use curve cryptography
ZINK_SECRET_KEY | string | ZeroMQ test key | Curve secret key

## Examples
```
$ ZINK_PRINT=COMPACT zink
....................................................................................................
100 ....................................................................................................
200 ....................................................................................................
```

```
$ ZINK_EXIT_AFTER=2 zink
Message 1 with 10 parts
Message 2 with 10 parts

Exiting after 2 messages
```

```
$ ZINK_PRINT=PARTS zink
PART 1: lorem
PART 2: ipsum
PART 3: 1453922746
Message 1 with 3 parts
```
