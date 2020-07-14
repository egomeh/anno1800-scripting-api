import struct
import socket
import atexit

s = None

def connect():

    global s

    port = 1800
    host_ip = socket.gethostbyname("localhost")

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((host_ip, port))

    atexit.register(exit)

def exchange(data):

    global s

    outgoingPayload = data
    outgoingLength = struct.pack("<q", len(outgoingPayload) + 8)

    s.send(outgoingLength + outgoingPayload)

    chunk = s.recv(0x1000)

    if len(chunk) < 8:

        return None

    messageSize = struct.unpack("<q", chunk[:8])[0]

    message = chunk

    while len(message) < messageSize:

        chunk = s.recv(0x1000)

        message += chunk

    # Cut off the message size
    payload = message[8:]

    # Take the success of the message
    success = struct.unpack("<B", payload[:1])[0]

    if success == 0:

        return None

    return payload[1:]


def serialize_bool(value):

    if not value == 0 or value == False:

        return struct.pack("<B", 1)

    return struct.pack("<B", 0)


def deserialize_bool(data, offset):

    value = (struct.unpack("<B", data[offset:offset + 1]), )

    offset = offset + 1

    if not value == 0:

        return True, offset

    return False, offset


def serialize_uint8_t(value):

    return struct.pack("<B", value)


def deserialize_uint8_t(data, offset):

    (value, ) = struct.unpack("<B", data[offset:offset + 1])
    return value, offset + 1


def serialize_float(value):

    return struct.pack("<f", value)


def deserialize_float(data, offset):

    (value, ) = struct.unpack("<f", data[offset:offset + 4])
    return value, offset + 4


def serialize_double(value):

    return struct.pack("<d", value)


def deserialize_double(data, offset):

    (value, ) = struct.unpack("<d", data[offset:offset + 8])
    return value, offset + 8


def serialize_uint64_t(value):

    return struct.pack("<q", value)


def deserialize_uint64_t(data, offset):

    (value, ) = struct.unpack("<q", data[offset:offset + 8])
    return value, offset + 8


def serialize_string(value):

    size = len(value)

    data = serialize_uint64_t(size)
    data += struct.pack("{{}}s".format(size), value.encode())

    return data


def deserialize_string(data, offset):

    (size, ) = struct.unpack("<q", data[offset:offset + 8])
    offset += 8

    (value, ) = struct.unpack("{{}}s".format(size), data[offset:offset + size])
    offset += size

    return value.decode(), offset

def serialize_list(values, serial_fn):

    size = len(values)

    data = serialize_uint64_t(size)

    for value in values:

        data += serial_fn(value)

    return data


def deserialize_list(data, offset, deserial_fn):

    size, offset = deserialize_uint64_t(data, offset)

    result = []

    for i in range(size):

        element, offset = deserial_fn(data, offset)

        result.append(element)

    return result, offset


{classes}

{functions}
