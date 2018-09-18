#!/usr/bin/python
import socket
import sys
import thread
import numpy as np
import cv2

SERVER_IP = "127.0.0.1"
SERVER_PORT = 8888

MAX_UDP_SIZE = 64000

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

def init_video():
    cv2.namedWindow('frame', cv2.WINDOW_AUTOSIZE)

def udp_receiver(socket_udp):
    init_video()
    while(True):
        try:
            encoded, addr = socket_udp.recvfrom(MAX_UDP_SIZE)
        except:
            print "Udp dejo de recibir." 
            return 
        encoded = np.fromstring(encoded, np.uint8)
        frame = cv2.imdecode(encoded, cv2.IMREAD_COLOR)
        resize = cv2.resize(frame,(1000,500))
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        cv2.imshow('frame', frame)
        cv2.waitKey(1000/30)

def init_udp(socket_tcp, udp_port):
    message = 'init %d' % udp_port
    socket_tcp.sendall(message)

def send_msg(message):
    socket_tcp.sendall(message)

def menu(socket_udp):
    print "Thread menu creado"
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
        else:
            send_msg("close")
    print("Menu cerrado.")
    socket_udp.close()


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
    # init_video()
    # thread.start_new_thread(udp_receiver, (socket_udp,))
    thread.start_new_thread(menu, (socket_udp,))
    udp_receiver(socket_udp)
finally:
    #join thread?
    socket_tcp.close()
    print "Conexion tcp cliente cerrada"  
    socket_udp.close()
    print "Conexion udp cliente cerrada"  
    cv2.destroyAllWindows()
    print "Ventana video destruida"  
