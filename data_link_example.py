#!/bin/python3

import socket
import sys
import random

class Message:
    def __init__(self):
        self.session_id = random.randint(0, 256)
        self.session_id |= random.randint(0, 256) << 8
        self.session_id |= random.randint(0, 256) << 16
        self.session_id |= random.randint(0, 256) << 24

        self.session_type = 0
        self.channel = 0
        self.data = []

    def to_bytes(self):
        data_as_bytes = [
                self.session_id & 0xFF, 
                (self.session_id >> 8) & 0xFF,
                (self.session_id >> 16) & 0xFF,
                (self.session_id >> 24) & 0xFF
                ]

        data_as_bytes.extend([self.session_type])
        data_as_bytes.extend([self.channel])
        data_as_bytes.extend([len(self.data)])
        data_as_bytes.extend(self.data)

        return bytes(data_as_bytes)

    def from_bytes(self, data_bytes):
        self.session_id = data_bytes[0]
        self.session_id |= data_bytes[1] << 8
        self.session_id |= data_bytes[2] << 16
        self.session_id |= data_bytes[3] << 24

        self.session_type = data_bytes[4]
        self.channel = data_bytes[5]
        self.data = data_bytes[7: len(data_bytes)]

        self.data = [byt for byt in self.data]
        pass

def delete_file(sock):
    msg = Message()
    msg.session_type = 1
    msg.channel = 0
    msg.data = [
            ord('t'),
            ord('e'),
            ord('s'),
            ord('t')
            ]
    
    sock.send(msg.to_bytes())
    
    data = sock.recv(64)
    
    rx_msg = Message()
    rx_msg.from_bytes(data)

    if rx_msg.data[0] == 'O' and rx_msg.data[1] == 'K':
        # OPERATION WAS SUCCESSFULL
        pass


def waypoints_upload(sock):
    msg = Message()
    msg.session_type = 1
    msg.channel = 0
    msg.data = [
            ord('t'),
            ord('e'),
            ord('s'),
            ord('t')
            ]
    
    sock.send(msg.to_bytes())
    
    data = sock.recv(64)
    
    rx_msg = Message()
    rx_msg.from_bytes(data)

    if rx_msg.data[0] == 'O' and rx_msg.data[1] == 'K':
        # OPERATION WAS SUCCESSFULL
        pass



# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = ('127.0.0.1', 9000)
sock.connect(server_address)


    

print('closing socket')
sock.close()
