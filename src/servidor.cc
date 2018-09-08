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

using namespace std;

#define MY_IP "127.0.0.1"
#define MAX_QUEUE 10
#define MAX_MSG_SIZE 1024
#define TRUE 1

void *tcp_handler(void *);
void *udp_handler(void *);
bool has_received(string, string);
int get_port_cmd(string, string);
void exit_error(string);

int ext_port = 8888;;
const char* ext_ip = "127.0.0.1";

int main(){
    //primitiva SOCKET
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
        exit_error("Error al crear socket tcp");
    else
        printf("Socket tcp creado..");

    int udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udp_socket == -1)
        exit_error("Error al crear socket udp");
    else
        printf("Socket udp creado..");

    // Config bind
    struct sockaddr_in server_addr;
    socklen_t server_addr_size = sizeof server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(MY_IP);

    // Binding

    int binding = bind(
        server_socket,
        (struct sockaddr *)&server_addr,
         server_addr_size);
    if (binding == -1)
        exit_error("Error al bindear socket tcp");
    else
        printf("Socket tcp bindeado..");

    binding = bind(
        udp_socket,
        (struct sockaddr *)&server_addr,
         server_addr_size);

    if (binding == -1)
        exit_error("Error al bindear socket udp");
    else
        printf("Socket udp bindeado..");

    // Listening
    int listening = listen(server_socket, MAX_QUEUE);
     if (listening == -1)
        exit_error("Error al intentar escuchar en socket tcp");
    else
        printf("Socket tcp escuchando en %s %d..", MY_IP, PORT);
    
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

        // Thread datos (UDP)
        // Ver si pasar por parametro id del host, para sincronizar udp y tcp con mismo host.
        pthread_create(&thread_id_2, NULL, udp_handler, (void*) &udp_socket);
  
    }
     //CLOSE del socket que espera conexiones
    close(server_socket);

    return 0;
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



        /*
		if(estados.status[i] == 1)
		{
			if(cap no abierto)
				abrir cap

			cap.read();
			sock.sendto(estados.port[i], currentFrame)
			waitKey()

			else if(estados.status[i] == 0)
				sleep 1
			else if(estados.status[i] == 2)
				cerrar cap
		}
		*/

    return 0;
}

void *tcp_handler(void *socket_desc){
    printf("\nRecibio conexion\n");

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
        }else if (has_received(message, PAUSE)){
            printf("Envio pause\n");
        }else if (has_received(message, STOP)){
            printf("Envio stop\n");
        }else if (has_received(message, INIT)){
            int port = get_port_cmd(message, INIT);
            printf("%d\n", port);
        }else
            printf("Escribio cualquier cosa\n");

        //primitiva SEND
        int sent_data_size = send(sock, data, received_data_size, 0);
        printf("Enviado al cliente (%d bytes): %s\n", sent_data_size, data);
        is_connected = 0;
    } // Funciona con muchas terminales. Ahora que el servidor sepa el port.

    close(sock);
    return 0;
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