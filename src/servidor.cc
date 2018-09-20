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
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

#define MY_IP "192.168.1.114"
#define MAX_QUEUE 10
#define MAX_MSG_SIZE 1024
#define TRUE 1
#define MAX_CLIENTS 10
#define VIDEO_PATH "../videoplayback"

#define POS_LIBRE -1
#define PAUSE_STATUS 0
#define PLAY_STATUS 1
#define STOP_STATUS 2
#define WAITINGPORT_STATUS 3
#define INIT_STATUS 4
#define CLOSE_STATUS -1

void *tcp_handler(void *);
void *udp_handler(void *);
bool has_received(std::string, std::string);
int get_port_cmd(std::string, std::string);
void exit_error(std::string);

bool for_debug = false;

struct Estados {
   int  status;
   char* ip;
   int  port;
};

struct Estados estados[MAX_CLIENTS];

struct args_struct {
    int socket;
    int client_index;
};

int assign_free_position(){
	//Busca un espacio sin cliente asignado 
	for(int i=0; i<MAX_CLIENTS; i++){
		if(estados[i].status == POS_LIBRE){
			estados[i].status = WAITINGPORT_STATUS;
			printf("fin busqueda OK\n");

			return i;
		}		
	}
	printf("fin busqueda ERR\n");
	return -1;
}


int main(){

	for(int i=0; i<MAX_CLIENTS; i++){
		estados[i].status = POS_LIBRE;
		estados[i].ip = "";
		estados[i].port = -1;
	}

    //primitiva SOCKET
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
        exit_error("Error al crear socket tcp\n");
    else
        printf("Socket tcp creado..\n");

    // Config bind
    struct sockaddr_in server_addr;
    socklen_t server_addr_size = sizeof server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(MY_IP);

    // Binding
	// ref https://stackoverflow.com/questions/10035294/compiling-code-that-uses-socket-function-bind-with-libcxx-fails
	int binding = ::bind(
		server_socket,
		(struct sockaddr *)&server_addr,
			server_addr_size);
	if (binding == -1)
		exit_error("Error al bindear socket TCP\n");
	else
		printf("Socket TCP binded\n");
	
    // Listening
    int listening = listen(server_socket, MAX_QUEUE);
     if (listening == -1)
        exit_error("Error al intentar escuchar en socket TCP\n");
    else
        printf("Socket TCP escuchando en %s %d..\n", MY_IP, PORT);
    
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

		//BUSCAR index_libre
		int index = assign_free_position();
		printf("Index libre: %d\n", index);

		if (index == -1){
			printf("Max clients reached\n");
		}
		else {
			args_struct args_tcp;
			args_tcp.socket = socket_to_client;
			args_tcp.client_index = index;
			estados[index].ip = inet_ntoa(client_addr.sin_addr);

			printf("Socket cliente TCP: %d\n", socket_to_client);

			//Thread control (TCP)
			pthread_create(&thread_id, NULL, tcp_handler, (void *)&args_tcp);

			
			//Creo socket UDP
			int udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (udp_socket == -1)
				exit_error("Error al crear socket udp\n");
			else
				printf("Socket udp creado..\n");
			

			printf("Socket cliente UDP: %d\n", udp_socket);
			

			// Thread datos (UDP)
			// Ver si pasar por parametro id del host, para sincronizar udp y tcp con mismo host.
			args_struct args_udp;
			args_udp.socket = udp_socket;
			args_udp.client_index = index;

			printf("args_udp: %d\n", args_udp.socket);
			pthread_create(&thread_id_2, NULL, udp_handler, (void *) &args_udp);
		}
    }
    pthread_exit(NULL);
     //CLOSE del socket que espera conexiones
    close(server_socket);

    return 0;
}

void *udp_handler(void * arguments){
	
	printf("---------- UDP entra\n");
	args_struct args = *(args_struct*) arguments;
	printf("---------- UDP client index: %d\n", args.client_index);
	
	
	// int udp_sock = args.socket;
	int udp_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	int recv_len;
	char data[MAX_MSG_SIZE];
	struct sockaddr_in udp_destino;
	int datos_enviados = 0;

	//open the video file for reading
	VideoCapture cap(VIDEO_PATH); 

	// if not success, exit program
	if (cap.isOpened() == false) {
		cout << "Cannot open the video file" << endl;
		cin.get(); //wait for any key press
		datos_enviados = 1;
	}

	String window_name = "Client" + to_string(args.client_index);
	if (for_debug){
		namedWindow(window_name, WINDOW_NORMAL); //create a window
	} 

	socklen_t udp_destino_len;

	while(!datos_enviados) {

		if(estados[args.client_index].status == PLAY_STATUS)
		{
			// printf("---------- UDP tiene que enviar frame\n");

			Mat frame;
			Mat newFrame;
			
			bool bSuccess = cap.read(frame); // read a new frame from video 

			//Breaking the while loop at the end of the video
			if (bSuccess == false) {
				cout << "Found the end of the video" << endl;
				printf("---------- UDP fin del video\n");
				cap.set(1, 0);
				estados[args.client_index].status = 0;

			}
			else {

				//show the frame in the created window
				if (for_debug){
					imshow(window_name, frame);
				}

				vector<uchar> encoded; //vector para almacenar el frame codificado en jpeg
				vector <int> compression_params;
				compression_params.push_back(IMWRITE_JPEG_QUALITY);
				compression_params.push_back(80);
				imencode(".jpg", frame, encoded, compression_params); 
				if (sendto(udp_sock, encoded.data(), encoded.size(), 0, (struct sockaddr*) &udp_destino, udp_destino_len) == -1)
					exit_error("Error en sendto");

				//wait for for 10 ms until any key is pressed.  
				//If the 'Esc' key is pressed, break the while loop.
				//If the any other key is pressed, continue the loop 
				//If any key is not pressed withing 10 ms, continue the loop
				if (waitKey(1000/20) == 27) {
					cout << "Esc key is pressed by user. Stoppig the video" << endl;
				}
				// erase encoded?
			}
		}
		else if(estados[args.client_index].status == PAUSE_STATUS){
			printf("---------- UDP pauso video\n");
			sleep(1);
		}
		else if(estados[args.client_index].status == INIT_STATUS){
			udp_destino.sin_family = AF_INET;
			udp_destino.sin_port = htons(estados[args.client_index].port);
			udp_destino.sin_addr.s_addr = inet_addr(estados[args.client_index].ip);

			udp_destino_len = sizeof(udp_destino);
			estados[args.client_index].status = PAUSE_STATUS;
		}
		else if(estados[args.client_index].status == STOP_STATUS){
			printf("---------- UDP le dio stop\n");
			cap.set(1, 0);
			estados[args.client_index].status = 0;
			//cap.close();
			//datos_enviados = 1; // SACAR DE ACA
		}
		else if (estados[args.client_index].status == CLOSE_STATUS){
			string hostIP = estados[args.client_index].ip;
			printf("Se cierra UDP %s:%d\n", hostIP.c_str(), estados[args.client_index].port);
			datos_enviados = 1;
		}
		//datos_enviados = 1;
	}
	if (for_debug){
		std::string x( "Client" + std::to_string(args.client_index));
		char *y = new char[x.length() + 1];
		std::strcpy(y, x.c_str());	
		cv::destroyWindow(y);
	}
	

	return 0;
}


void *tcp_handler(void * argument){
	printf("\n----- TCP Recibio conexion\n");
	args_struct args = *(args_struct*) argument;
	printf("\n----- TCP client index: %d\n", args.client_index);
	int sock = args.socket;

	//primitiva RECEIVE
	char data[MAX_MSG_SIZE];
	int data_size = MAX_MSG_SIZE;
	int is_connected = 1;

	while (is_connected){
		printf("----- TCP Hilo TCP receive\n");
		int received_data_size = recv(sock, data, data_size, 0);
		if (received_data_size <= 0) {
			// received_data_size = 0 -> connection closed
			// received_data_size = 1 -> error
			break;

		}
		printf("----- TCP Hilo TCP paso\n");

		std::string message = data;

		if (has_received(message, PLAY)){
			printf("----- TCP Envio play\n");
			estados[args.client_index].status = PLAY_STATUS;
		}else if (has_received(message, PAUSE)){
			printf("----- TCP Envio pause\n");
			estados[args.client_index].status = PAUSE_STATUS;
		}else if (has_received(message, STOP)){
			printf("----- TCP Envio stop\n");
			estados[args.client_index].status = STOP_STATUS;
		}else if (has_received(message, INIT)){
			int port = get_port_cmd(message, INIT);
			printf("----- TCP INIT: %d\n", port);
			estados[args.client_index].port = port;
			estados[args.client_index].status = INIT_STATUS;
		}else if (has_received(message, CLOSE)){
			string hostIP = estados[args.client_index].ip;
			printf("Se cierra TCP %s:%d\n", hostIP.c_str(), estados[args.client_index].port);
			estados[args.client_index].status = CLOSE_STATUS;
			is_connected = 0; // break?
		}else
			printf("----- TCP Mensaje no reconocido\n");

		//primitiva SEND
		//int sent_data_size = send(sock, data, received_data_size, 0);
		//printf("Enviado al cliente (%d bytes): %s\n", sent_data_size, data);
		//is_connected = 0;
	} // Funciona con muchas terminales. Ahora que el servidor sepa el port.

	close(sock);
	return 0;
}

bool has_received(std::string message, std::string command){
	return (message.compare(0, command.length(), command) == 0);
}

int get_port_cmd(std::string message, std::string command){
	std::string port_str = message.substr(command.length() + 1, message.length() - 2).c_str();
	int port = stoi(port_str);
	return port;
}

void exit_error(std::string err){
	printf("%s\n", err.c_str());
	exit(1);
}