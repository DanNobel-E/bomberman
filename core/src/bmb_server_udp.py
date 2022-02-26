from http import server
import socket
import struct

class Player:
    
    def __init__(self,signature):
        self.signature= signature
        self.position= (0,0)
        
    def update(self,x,y):
        self.position= (x,y)
        #print(self.signature,self.x,self.y)
        
class Server:

    def __init__(self, address='127.0.0.1', port=9999):
        self.address = address
        self.port = port
        self.socket= socket.socket(socket.AF_INET,socket.SOCK_DGRAM,socket.IPPROTO_UDP)
        self.socket.bind((address,port))
        self.players= {}
        self.pk_ids = {
            'PK_AUTH_ID':  1,
            'PK_POS_ID': 2,

        }
        
    def run_once(self):
        try:
            packet, sender= self.socket.recv(64)
            print(packet[0])
            if len(packet) >16:
                raise Exception()
            if packet[0] == self.pk_ids['PK_AUTH_ID']:
                id,auth= struct.unpack('BB',packet)
                if sender not in self.players:
                    self.players[sender]= Player(sender)
                print(auth)
            if packet[0] == self.pk_ids['PK_POS_ID']:
                id,x,y= struct.unpack('4Bff',packet)
                self.players[sender].update(x,y)
        except:
            print('packet discarded')

    def run(self):
        print('server running')
        print(self.pk_ids['PK_AUTH_ID'])
        
        while True:
            self.run_once()

        

if __name__ == '__main__':
    server = Server()
    server.run()

