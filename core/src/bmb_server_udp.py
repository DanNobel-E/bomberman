from ctypes import sizeof
from genericpath import exists
import random
import socket
import struct
import time


class InvalidPacketSize(Exception):
    pass


class DosAttemptDetected(Exception):
    pass


class Player:

    def __init__(self, signature, auth, index):
        self.signature = signature
        self.auth = auth
        self.index = index
        self.position = (100, 100)
        self.color = (0, 0, 0, 255)
        self.last_update = time.time()

    def update(self, x, y):
        now = time.time()
        self.position = (x, y)
        self.last_update = time.time()


class Server:

    def __init__(self, address='127.0.0.1', port=9999, tolerance=20):
        self.address = address
        self.port = port
        self.tolerance = tolerance
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.socket.bind((self.address, self.port))
        self.packet = None
        self.sender = None
        self.players = {}
        self.current_player_index = 0
        self.pk_ids = {
            'PK_AUTH_ID':  1,
            'PK_COL_ID':  2,
            'PK_POS_ID': 3,
            'PK_PLY_ID': 4,
            'PK_DSC_ID': 5,

        }

    def run_once(self):
        try:
            packet, sender = self.socket.recvfrom(64)
            self.packet = packet
            self.sender = sender
            if len(packet) > 20:
                raise InvalidPacketSize()
            if sender is self.players and self.players[sender] is None:
                self.kick_out_player(sender, False)
                return
            if sender is self.players:
                self.check_dos_attack(sender)
            self.read_current_packet(packet, sender)
            self.packet = None
            self.sender = None
        except InvalidPacketSize:
            print('Invalid packet size detected from {0}'.format(sender))
            if self.sender is not None:
                self.kick_out_player(sender,False)
        except DosAttemptDetected:
            print('Dos detected from {0}, kicking it out'.format(sender))
            # del(self.players[sender]) #TO DO blacklist
            if self.sender is not None:
                self.kick_out_player(sender, False)
        except OSError:
            print('packet discarded')
            if self.sender is not None:
                self.kick_out_player(sender, False)

    def run(self):
        print('server running')
        random.seed()
        while True:
            self.run_once()

    def new_player_authentication(self, new_player, auth_packet):
        auth = struct.unpack('BBB', auth_packet)
        self.players[new_player] = Player(new_player, auth[1], self.current_player_index)
        self.current_player_index += 1
        auth_packet = struct.pack('BBB', self.pk_ids['PK_AUTH_ID'], auth[1], self.players[new_player].index)
        self.socket.sendto(auth_packet, new_player)
        r, g, b = random.randbytes(3)
        self.players[new_player].color = (r, g, b, 255)
        packet_color = struct.pack("BBBB", self.pk_ids['PK_COL_ID'], r, g, b)
        self.socket.sendto(packet_color, new_player)
        self.send_new_player_info(new_player)
        

    def send_new_player_info(self, new_player):
        for player in self.players:
            if player != new_player:
                packet_to_player = struct.pack("4B2f4B", self.pk_ids['PK_PLY_ID'], self.players[new_player].color[0],
                                               self.players[new_player].color[1], self.players[new_player].color[2],
                                               self.players[new_player].position[0], self.players[new_player].position[1],
                                               self.players[new_player].index, 0, 0, 0)
                packet_to_new_player = struct.pack("4B2f4B", self.pk_ids['PK_PLY_ID'], self.players[player].color[0],
                                                   self.players[player].color[1], self.players[player].color[2],
                                                   self.players[player].position[0], self.players[player].position[1],
                                                   self.players[player].index, 0, 0, 0)
                self.socket.sendto(packet_to_player, player)
                self.socket.sendto(packet_to_new_player, new_player)

    def check_dos_attack(self, sender):
        now = time.time()
        if sender in self.players:
            delta_time_to_last_packet = now-self.players[sender].last_update
            if delta_time_to_last_packet < (1/self.tolerance):
                raise DosAttemptDetected()

    def read_current_packet(self, packet, sender):
        packet_id = packet[0]
        if packet_id == self.pk_ids['PK_AUTH_ID']:
            if sender not in self.players:
                self.new_player_authentication(sender, packet)
        elif packet_id == self.pk_ids['PK_POS_ID']:
            pos_packet = struct.unpack('I2f', packet)
            if sender in self.players:
                x = pos_packet[1]
                y = pos_packet[2]
                self.players[sender].update(x, y)
                self.broadcast_position(sender, x, y)
        elif packet_id == self.pk_ids['PK_DSC_ID']:
            if sender in self.players:
                self.disconnect_player(packet, sender, True)

    def broadcast_position(self, sender, x, y):
        for player in self.players:
            if self.players[player] is not None:
                if player != sender:
                    pos_packet = struct.pack(
                        'BBBB2f', self.pk_ids['PK_POS_ID'], self.players[sender].index, 0, 0, x, y)
                    self.socket.sendto(pos_packet, player)

    def disconnect_player(self, packet, sender, self_disconnect):
        disc_packet = struct.unpack('BB', packet)
        if self.players[sender].index== disc_packet[1]:
            if self_disconnect:
                del self.players[sender]
            else:
                self.players[sender]=None
        for player in self.players:
            if self.players[player] is not None:
                self.socket.sendto(packet, player)
        self.packet = None
        self.sender = None

    def kick_out_player(self, sender, self_disconnect):
        if sender is self.players:
            if self.players[sender] is not None:
                disc_packet = struct.pack('BB', self.pk_ids['PK_DSC_ID'], self.players[sender].index)
                self.disconnect_player(disc_packet, sender, self_disconnect)


if __name__ == '__main__':
    server = Server()
    server.run()
