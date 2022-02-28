from ctypes import sizeof
import random
import socket
import struct
import time



class Player:

    def __init__(self, signature, auth):
        self.signature = signature
        self.auth= auth
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
            'PK_COL_ID':  2,
            'PK_POS_ID': 3,
            'PK_PLY_ID': 4,
            

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
                    self.players[sender] = Player(sender, auth_packet[1])
                    self.socket.sendto(packet,sender)
                    r,g, b=random.randbytes(3)
                    packet_color= struct.pack("BBBB", self.pk_ids['PK_COL_ID'],r,g,b)
                    self.socket.sendto(packet_color,sender)
                    for player in self.players:
                        if(player!= sender):
                            packet_player= struct.pack("4B2f",self.pk_ids['PK_PLY_ID'], self.players[player].auth,
                                                       0,0,
                                                       self.players[player].position[0],self.players[player].position[1])
                            self.socket.sendto(packet_player,sender)
            elif packet_id == self.pk_ids['PK_POS_ID']:
                pos_packet = struct.unpack('I2f', packet)
                if sender in self.players:
                    self.players[sender].update(pos_packet[1],pos_packet[2])
        except:
            print('packet discarded')
            

    def run(self):
        print('server running')
        random.seed()
        while True:
            self.run_once()


if __name__ == '__main__':
    server = Server()
    server.run()
