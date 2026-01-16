# SplaT Middleware Protocol v1.0 Specification

**Last Updated: September 4th, 2025**

This document describes the SplaT Middleware Protocol (STMP).

STMP is used to build all the network applications I run which require communication with each other.

## Packet Structure

```
+------------------+------------------------+-----------------------------+
| Header (4 bytes) | Payload (< 1496 bytes) | Packet Termination (1 byte) |
+------------------+------------------------+-----------------------------+
```

## Header

STMP's packet header is **4 bytes**

```
+------------------+---------------+-------------------+----------------+
| Version (1 byte) | Type (1 byte) | Argument (1 byte) | Flags (1 byte) |
+------------------+---------------+-------------------+----------------+
```

### Version

STMP's current version is **2.0.0**. This is represented by the byte **0x02**. This is the only
byte available at the moment.

### Type

STMP supports five types of messages.

| Type | Byte | Description |
|:--:|:--:|:--:|
|  INIT  |  0x01  |  The initial handshake between clients  |
|  PING  |  0x02  |  Periodic keepalive message to ensure the connection is still alive  |
|  SEND  |  0x03  |  Send a message to another client  |
|  TERM  |  0x04  |  Notify client that the connection is terminated  |
|  INVALID  | 0x05   |  Notify client that an invalid message was sent   |

The **PING** message type is used to see if a client is still alive. By no means is it required in
an implementation, but ultimately up to you to use.

### Argument

For each **message type**, STMP requires a single **argument**. Below is a detailed list of the
arguments supported for each message type.

**INIT**

| Type | Argument | Byte | Direction |
|:--:|:--:|:--:|----|
| INIT |  INIT  | 0x01 | Client -> Server |
| INIT   | ACCEPT | 0x02 | Client <- Server |

**PING**

| Type | Argument | Byte | Direction |
|:--:|:--:|:--:|----|
| PING |  PING  | 0x00 | Client <-> Server |

**SEND**

| Type | Argument | Byte | Direction |
|:--:|:--:|:--:|----|
| SEND |  SEND  | 0x00 | Client <-> Server |

**TERM**

| Type | Argument | Byte | Direction |
|:--:|:--:|:--:|----|
| TERM |  CLEAN  | 0x01 | Client <-> Server |
| TERM   | BUSY | 0x02 | Client <-> Server |

**INVALID**

| Type | Argument | Byte | Direction |
|:--:|:--:|:--:|----|
| INVALID |  VERSION  | 0x01 | Client <- Server |
| INVALID   | TYPE | 0x02 | Client <- Server |
| INVALID |  MESSAGE  | 0x03 | Client <- Server |
| INVALID   | ARGUMENT | 0x04 | Client <- Server |
| INVALID |  FLAGS  | 0x05 | Client <- Server |
| INVALID   | PAYLOAD | 0x06 | Client <- Server |

### Flags

An STMP flags header byte is 1 byte, but is used for bitwise operations up to the implementation.

You can have up to eight different flags per packet. For example, if I use the the hexadecimal value
**0x07** to represent the bits **111**. You can use operations like `(1 << 0)` and `(1 << 1)` to define flags.

### Payload

Every STMP packet has a payload that can be at most 1496 bytes. Certain message types require an
empty payload. The maximum payload size ensures packets fit within a 1500-byte MTU.

Every STMP packet includes a payload of at least one byte. An "empty payload" is represented by a
single byte with the value `0x00`.

The message types that require an empty payload are **INIT** and **INVALID**.

Payloads are binary data. Interpretation of payload contents is left to the application.

### Packet Termination

**Every** STMP packet is terminated by the byte **0x7F**, DEL in ASCII.

The termination byte is not part of the payload.

### Memory Ownership

STMP does not allocate memory for packet payloads.

After deserialization, `stmp_packet.payload` points directly into the caller-provided buffer.
The buffer **must remain valid** for as long as the packet is in use.

Do **not** call `free()` on `stmp_packet.payload`.

If ownership is required, the caller must explicitly copy the payload.

### Closing

Future versions may extend this specification. Existing versions will remain backwards compatible.
