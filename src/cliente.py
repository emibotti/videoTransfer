#!/usr/bin/python
import socket
import sys
import thread

SERVER_IP = "127.0.0.1"
SERVER_PORT = 8888

# Menu options
CLOSE_menu = 0
PLAY_menu = 1
STOP_menu = 2
PAUSE_menu = 3

menu_actions = {
    1: "play",
    2: "stop",
    3: "pause"
}

INIT = "init"


def udp_receiver(socket_udp):
    while(True):
        data, addr = socket_udp.recvfrom(1024)
        print "UDP received message: %s" % data

def init_udp(socket_tcp, udp_port):
    message = 'init %d' % udp_port
    socket_tcp.sendall(message)

def send_msg(message):
    socket_tcp.sendall(message)

def menu():
    action = -1
    while (action != CLOSE_menu):
        print """Seleccionar una opcion:\n
                1- PLAY
                2- STOP
                3- PAUSE
                0- SALIR
            """
        action = input("")
        message = menu_actions.get(action, "close")
        if message != "close":
            send_msg(message)
    print("Menu cerrado.")
            



# Se crea socket tcp y udp en internet

socket_tcp = socket.socket( socket.AF_INET, socket.SOCK_STREAM)
socket_udp = socket.socket( socket.AF_INET, socket.SOCK_DGRAM)

# Se bindea socket udp
socket_udp.bind(("127.0.0.1", 0)) #socket.gethostname()
udp_ip, udp_port = socket_udp.getsockname()

print udp_ip
print udp_port

#conexion
socket_tcp.connect((SERVER_IP,SERVER_PORT))

try:
    init_udp(socket_tcp, udp_port)
    thread.start_new_thread(udp_receiver, (socket_udp,))
    menu()
finally:
    #join thread?
    socket_tcp.close()
    print "Conexion tcp cliente cerrada"  
    socket_udp.close()
    print "Conexion udp cliente cerrada"  