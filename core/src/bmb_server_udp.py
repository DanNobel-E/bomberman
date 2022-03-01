from ctypes import sizeof
import random
import socket
import struct
import time



class Player:

    def __init__(self, signature, auth):
        self.signature = signature
        self.auth= auth
        self.position = (100, 100)
        self.color = (0,0,0,255)

    def update(self, x, y):
        self.position = (x, y)
        #print(self.signature,self.position)


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
                if sender not in self.players:
                    self.new_player_authentication(sender,packet)
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
    
    def new_player_authentication(self, new_player, auth_packet):
        auth = struct.unpack('BB', auth_packet)
        self.players[new_player] = Player(new_player, auth[1])
        self.socket.sendto(auth_packet,new_player)
        r,g, b=random.randbytes(3)
        self.players[new_player].color= (r,g,b, 255)
        print("ciao")
        packet_color= struct.pack("BBBB", self.pk_ids['PK_COL_ID'],r,g,b)
        self.socket.sendto(packet_color,new_player)
        self.send_new_player_info(new_player)

                    
    
    def send_new_player_info(self, new_player):
        for player in self.players:
            if(player!= new_player):
                packet_to_player= struct.pack("4B2f",self.pk_ids['PK_PLY_ID'], self.players[new_player].color[0],
                                                self.players[new_player].color[1],self.players[new_player].color[2],
                                                self.players[new_player].position[0],self.players[new_player].position[1])
                packet_to_new_player= struct.pack("4B2f",self.pk_ids['PK_PLY_ID'], self.players[player].color[0],
                                                self.players[player].color[1],self.players[player].color[2],
                                                self.players[player].position[0],self.players[player].position[1])
                self.socket.sendto(packet_to_player,player)
                self.socket.sendto(packet_to_new_player,new_player)


if __name__ == '__main__':
    server = Server()
    server.run()
