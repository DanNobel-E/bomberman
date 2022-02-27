from ctypes import sizeof
import socket
import struct
import time



class Player:

    def __init__(self, signature):
        self.signature = signature
        self.position = (0, 0)

    def update(self, x, y):
        self.position = (x, y)
        print(self.signature,self.position)


class Server:

    def __init__(self, address='127.0.0.1', port=9999):
        self.address = address
        self.port = port
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.socket.bind((self.address, self.port))
        self.players = {}
        self.pk_ids = {
            'PK_AUTH_ID':  1,
            'PK_POS_ID': 2,

        }

    def run_once(self):
        try:
            packet, sender = self.socket.recvfrom(64)
            if len(packet) > 16:
                raise Exception()
            packet_id = packet[0]
            if packet_id == self.pk_ids['PK_AUTH_ID']:
                auth_packet = struct.unpack('BB', packet)
                if sender not in self.players:
                    self.players[sender] = Player(sender)
                    self.socket.sendto(packet,sender)
            elif packet_id == self.pk_ids['PK_POS_ID']:
                pos_packet = struct.unpack('I2f', packet)
                if sender in self.players:
                    self.players[sender].update(pos_packet[1],pos_packet[2])
        except:
            print('packet discarded')
            

    def run(self):
        print('server running')
        while True:
            self.run_once()


if __name__ == '__main__':
    server = Server()
    server.run()
