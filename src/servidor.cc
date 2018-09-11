#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <arpa/inet.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#include "protocolo.h"

#include "opencv2/opencv.hpp"

using namespace std;

#define MY_IP "127.0.0.1"
#define MAX_QUEUE 10
#define MAX_MSG_SIZE 1024
#define TRUE 1
#define MAX_CLIENTS 5
#define VIDEO_PATH "/home/video.avi"

void *tcp_handler(void *);
void *udp_handler(void *);
bool has_received(string, string);
int get_port_cmd(string, string);
void exit_error(string);

int ext_port = 8888;;
const char* ext_ip = "127.0.0.1";

struct Estados {
   int  status[MAX_CLIENTS];
   int  port[MAX_CLIENTS];
};
 
int main(){
	
	struct Estados estados;

	for(int i=0; i<MAX_CLIENTS; i++){
		estados.status[i] = -1;
		estados.port[i] = -1;
	}

    //primitiva SOCKET
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
        exit_error("Error al crear socket tcp");
    else
        printf("Socket tcp creado..");

    // Config bind
    struct sockaddr_in server_addr;
    socklen_t server_addr_size = sizeof server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(MY_IP);

    // Binding
	PrimitivaBind(server_socket, &server_addr, server_addr_size, "TCP");
	
    // Listening
    int listening = listen(server_socket, MAX_QUEUE);
     if (listening == -1)
        exit_error("Error al intentar escuchar en socket TCP");
    else
        printf("Socket TCP escuchando en %s %d..", MY_IP, PORT);
    
    pthread_t thread_id;
    pthread_t thread_id_2; 
    // posteriormente crear un arreglo e ir poniendo los threads_id ahi
    // ver ejemplos http://man7.org/linux/man-pages/man3/pthread_create.3.html

    while (1){
        printf("Esperando un accept (TCP)...\n");
        //primitiva ACCEPT
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof client_addr;
        int socket_to_client = accept(
            server_socket,
            (struct sockaddr *)&client_addr, &client_addr_size);
        //Thread control (TCP)
        pthread_create(&thread_id, NULL, tcp_handler, (void *)&socket_to_client);


		//Creo socket UDP
		int udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (udp_socket == -1)
			exit_error("Error al crear socket udp");
		else
			printf("Socket udp creado..");

		PrimitivaBind(udp_socket, &server_addr, server_addr_size, "UDP");
		
        // Thread datos (UDP)
        // Ver si pasar por parametro id del host, para sincronizar udp y tcp con mismo host.
        pthread_create(&thread_id_2, NULL, udp_handler, (void*) &udp_socket);
  
    }
     //CLOSE del socket que espera conexiones
    close(server_socket);

    return 0;
}

void PrimitivaBind(int socket, (struct sockaddr *) sv_addr, int size, string type){
	int binding = bind(
		socket,
		(struct sockaddr *)&sv_addr,
			size);
	if (binding == -1)
		exit_error("Error al bindear socket " + "type");
	else
		printf("Socket " + type + " bindeado.");
}

void *udp_handler(void * socket_desc){

	int udp_sock = *(int *)socket_desc;
	int recv_len;
	char data[MAX_MSG_SIZE];

	struct sockaddr_in si_other;
	socklen_t slen = sizeof(si_other);
	int datos_enviados = 0;


	while(!datos_enviados)
	{
		printf("Waiting for data...\n");
		fflush(stdout);
         
		//try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(udp_sock, data, MAX_MSG_SIZE, 0, (struct sockaddr *) &si_other, &slen)) == -1)
		{
			exit_error("recvfrom()");
		}
         
		//print details of the client/peer and the data received
		printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		printf("Data: %s\n" , data); 
         
		/*  printf("Ingrese data a enviar:\n");
		gets(data); */

		//En i se tiene que pasar el numero de cliente que esta atendiendo este thread
		int i = 0;

		if(estados.status[i] == 1)
		{
			VideoCapture cap(VIDEO_PATH);
				
			if(cap.isOpened() == false)
				return;

			Mat frame;
			cap.read(frame);
			
			udp_sock.sendto(estados.port[i], frame)
			waitKey()	
		}
		else if(estados.status[i] == 0)
			sleep 1;
		else if(estados.status[i] == 2)
			cap.close();


		// Hay que ver como el cliente sabe en que puerto udp para recibir.
		// Tambien hay que ver como se sincronizan TCP y UDP para hablar con un host en particular.
		ext_port = ntohs(si_other.sin_port);

		sockaddr_in dest_addr;
		socklen_t slen_dst = sizeof(dest_addr);
		dest_addr.sin_family=AF_INET;
		dest_addr.sin_port= htons(ext_port);
		dest_addr.sin_addr.s_addr=inet_addr(ext_ip);

		//now reply the client with the same data
		if (sendto(udp_sock, data, strlen(data), 0, (struct sockaddr*) &dest_addr, slen_dst) == -1)
		{
			exit_error("sendto()");
		}
		printf("Sent packet to %s:%d\n", inet_ntoa(dest_addr.sin_addr), ntohs(dest_addr.sin_port));
		datos_enviados = 1;
	}

	close(udp_sock);





	return 0;
}

void *tcp_handler(void *socket_desc){
	printf("\nRecibio conexion\n");
	int client;
	int sock = *(int *)socket_desc;

	//primitiva RECEIVE
	char data[MAX_MSG_SIZE];
	int data_size = MAX_MSG_SIZE;
	int is_connected = 1;

	while (is_connected){
		int received_data_size = recv(sock, data, data_size, 0);

		string message = data;

		if (has_received(message, PLAY)){
			printf("Envio play\n");
			estados[client].status = 1;
		}else if (has_received(message, PAUSE)){
			printf("Envio pause\n");
			estados[client].status = 0;
		}else if (has_received(message, STOP)){
			printf("Envio stop\n");
			estados[client].status = 2;
		}else if (has_received(message, INIT)){
			int port = get_port_cmd(message, INIT);
			printf("%d\n", port);
			client = assign_port(port);
		}else
			printf("Mensaje no reconocido\n");

		//primitiva SEND
		int sent_data_size = send(sock, data, received_data_size, 0);
		printf("Enviado al cliente (%d bytes): %s\n", sent_data_size, data);
		is_connected = 0;
	} // Funciona con muchas terminales. Ahora que el servidor sepa el port.

	close(sock);
	return 0;
}

int assign_port(int port){
	//Busca un espacio sin cliente asignado 
	for(int i=0; i<MAX_CLIENTS; i++){
		if(estados.port[i] == -1){
			estados.port[i] = port;
			return i;
		}		
	}
	printf("Max Clients reached");
}

bool has_received(string message, string command){
	return (message.compare(0, command.length(), command) == 0);
}

int get_port_cmd(string message, string command){
	string port_str = message.substr(command.length() + 1, message.length() - 2).c_str();
	int port = stoi(port_str);
	return port;
}

void exit_error(string err){
	printf("%s\n", err.c_str());
	exit(1);
}