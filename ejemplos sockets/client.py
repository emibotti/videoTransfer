import socket
import sys

IP = "172.16.133.17"
PORT = 3494

#Se crea socket tcp en internet

s = socket.socket( socket.AF_INET, socket.SOCK_STREAM)

#conexion
s.connect((IP,PORT))

try:
    message='hola'
    s.sendall(message)

    data = s.recv(128)

    print data

finally:
    s.close()
