import socket

players=[]
s= socket.socket(socket.AF_INET, socket.SOCK_DGRAM,socket.IPPROTO_UDP)

s.bind(('127.0.0.1',9999))

while True:
    message, sender= s.recvfrom(4096)
    
    if message == b'quit':
        break
    print(message,sender)