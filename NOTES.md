# stmp - SplaT Message Protocol

Need a request acknowledge / non acknowledgment

Success / Failure after work is done

Some sort of identifier for applications per request

Flags in the header (for broker, silent message, etc)

include retry message type, in case of corruption

checksum, need to figure out what to add

payload:

application part of the header

---

version (1 byte) - 1

type of message (1 byte)
INIT - intialize connection
PING - sonar ping as connection stays open
SEND - send a message to an application
TERM - terminate connection (responder must send this, not the client, this should be sent as a response)
INVALID - payload would be 0x7F

message argument (1 byte)
INIT:
INIT - first send
ACCEPT - handshake

PING (no arguments | hexadecimal: 0x00)

SEND (no arguments | hexadecimal: 0x00)

TERM:
CLEAN - clean termination
BUSY - too many requests

INVALID:
VERSION - bad version
TYPE - bad type
MESSAGE - bad message
ARGUMENT - bad argument
FLAGS - bad flag(s)
PAYLOAD - bad payload

flags (1 byte)
NONE - no flags
LIGHT - relay messages sent to the broker (personal)
DARK - keep messages private (personal)

payload (n bytes)
ASCII is the payload type, using the first 127 characters to be unicode complient (UTF-8)

packet terminate (1 byte) 0x7F (DEL in ascii, 127th char)


It is required you free the payload after deserialization
CALLER frees


Central message broker - this has endpoints defined. Each message sent to the message
broker will have an ID regarding where it came from. This is like an RPC system

STMPEndpoint:
- endpoint id
- name of the endpoint
- socket of the endpoint


No public release until I release the P2P version of stmp


