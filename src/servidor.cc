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
#include <signal.h>

using namespace cv;
using namespace std;

#define MY_IP "192.168.1.112"
#define MAX_QUEUE 10
#define MAX_MSG_SIZE 1024
#define TRUE 1
#define MAX_CLIENTS 10
#define VIDEO_PATH "../video_prueba.mp4"

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
bool has_received_port(std::string, std::string); 
int get_port_cmd(std::string, std::string);
void exit_error(std::string);

bool for_debug = false;

struct Estados {
   int  status;
   char* ip;
   int  port;
   pthread_t tcpThreadID;
   pthread_t udpThreadID;
};

struct args_struct {
    int socketTCP;
	int socketUDP;
    int client_index;
};

struct Estados estados[MAX_CLIENTS];
struct args_struct argumentos[MAX_CLIENTS];

int server_socket;

int assign_free_position(){
	//Busca un espacio sin cliente asignado 
	for(int i=0; i<MAX_CLIENTS; i++){
		if(estados[i].status == POS_LIBRE){
			estados[i].status = WAITINGPORT_STATUS;
			printf("Fin busqueda OK.\n");
			return i;
		}		
	}
	printf("Fin busqueda ERR.\n");
	return -1;
}

void my_handler(int s){
	printf("Caught close signal \n");
	close(server_socket);
	exit(1); 
}

void force_exit_handler(){
	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = my_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);
}

void init_estados(){
	for(int i=0; i<MAX_CLIENTS; i++){
		estados[i].status = POS_LIBRE;
		estados[i].ip;
		estados[i].port = -1;
		estados[i].tcpThreadID = 0;
		estados[i].udpThreadID = 0;
	}
}

int main(){

	//Handler para el CTRL + C
	force_exit_handler();

	//Inicializo estructura estados
	init_estados();

    //Primitiva SOCKET
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
        exit_error("Error al crear socket TCP.\n");
    else
        printf("Socket TCP creado.\n");

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
		exit_error("Error al bindear socket TCP.\n");
	else
		printf("Socket TCP binded.\n");
	
    // Listening
    int listening = listen(server_socket, MAX_QUEUE);
     if (listening == -1)
        exit_error("Error al intentar escuchar en socket TCP.\n");
    else
        printf("Socket TCP escuchando en %s %d.\n", MY_IP, PORT);

    while (1){
        printf("Esperando un accept (TCP)...\n");
        
		//Primitiva ACCEPT
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof client_addr;
        int socket_to_client = accept(
            server_socket,
            (struct sockaddr *)&client_addr, &client_addr_size);

		//Buscar index_libre
		int index = assign_free_position();
		printf("Index libre: %d\n", index);

		if (index == -1){
			printf("Max clients reached.\n");
		}
		else {
			
			argumentos[index].socketTCP = socket_to_client;
			argumentos[index].client_index = index;
			estados[index].ip = inet_ntoa(client_addr.sin_addr);

			printf("%d Socket cliente TCP: %d\n", index,socket_to_client);

			//Thread control (TCP)
			pthread_create(&estados[index].tcpThreadID, NULL, tcp_handler, (void *)&argumentos[index]);

			//Creo socket UDP
			int udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (udp_socket == -1)
				exit_error("Error al crear socket UDP.\n");
			else
				printf("Socket UDP creado.\n");
			

			printf("%d Socket cliente UDP: %d\n",index,udp_socket);
		

			// Thread datos (UDP)			
			argumentos[index].socketUDP = udp_socket;
			pthread_create(&estados[index].udpThreadID, NULL, udp_handler, (void *) &argumentos[index]);
		}
    }
    pthread_exit(NULL);
    close(server_socket);

    return 0;
}

void *udp_handler(void * arguments){
	
	printf("---------- UDP entra\n");
	args_struct args = *(args_struct*) arguments;
	printf("---------- UDP client index: %d\n", args.client_index);
	
	
	//int udp_sock = args.socketUDP;
	int udp_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	int recv_len;
	char data[MAX_MSG_SIZE];
	struct sockaddr_in udp_destino;
	int datos_enviados = 0;

	//Open the video file for reading
	VideoCapture cap(VIDEO_PATH); 

	//If not successful, exit program
	if (cap.isOpened() == false) {
		cout << "Cannot open the video file" << endl;
		cin.get(); //wait for any key press
		datos_enviados = 1;
	}

	
	String window_name;

	if (for_debug){
		window_name = "Client" + to_string(args.client_index);
		namedWindow(window_name, WINDOW_NORMAL);
	} 

	socklen_t udp_destino_len;

	while(!datos_enviados) {

		if(estados[args.client_index].status == PLAY_STATUS)
		{
			Mat frame;
			bool bSuccess = cap.read(frame);

			//Breaking the while loop at the end of the video
			if (bSuccess == false) {
				cout << "Found the end of the video." << endl;
				printf("%d ---------- UDP fin del video.\n", args.client_index);
				cap.set(1, 0);
				estados[args.client_index].status = 0;
			}
			else {

				//Show the frame in the created window
				if (for_debug){
					imshow(window_name, frame);
					printf("%d ----------Waitkey\n", args.client_index);
				}

				usleep(30000);
				//waitKey(1000/30);

				vector<uchar> encoded; //vector para almacenar el frame codificado en jpg
				vector <int> compression_params;
				compression_params.push_back(IMWRITE_JPEG_QUALITY);
				compression_params.push_back(80);
								
				bool is_encoded = imencode(".jpg", frame, encoded, compression_params); 
				if (!is_encoded)
					exit_error("Error al hacer encode de frame.");
				if (encoded.size() < 1)
					exit_error("Frame encoded generado es null.");
 
				if (sendto(udp_sock, encoded.data(), encoded.size(), 0, (struct sockaddr*) &udp_destino, udp_destino_len) == -1)
					exit_error("Error en sendto.");
			}
		}
		else if(estados[args.client_index].status == PAUSE_STATUS){
			printf("%d ---------- UDP pauso video.\n", args.client_index);
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
			printf("%d ---------- UDP le dio stop\n", args.client_index);
			cap.set(1, 0);
			estados[args.client_index].status = 0;
		}
		else if (estados[args.client_index].status == CLOSE_STATUS){
			string hostIP = estados[args.client_index].ip;
			printf("Se cierra UDP %s:%d\n", hostIP.c_str(), estados[args.client_index].port);
			datos_enviados = 1;
		}
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
	printf("\n----- TCP Recibio conexion.\n");
	args_struct args = *(args_struct*) argument;
	printf("\n----- TCP client index: %d\n", args.client_index);
	int sock = args.socketTCP;

	//Primitiva Receive
	char data[MAX_MSG_SIZE];
	int data_size = MAX_MSG_SIZE;
	int is_connected = 1;

	while (is_connected){
		printf("----- TCP Hilo receive.\n");
		int received_data_size = recv(sock, data, data_size, 0);
		printf("----- TCP Hilo paso.\n");

		std::string message = data;

		if (has_received_port(message, INIT)){
			int port = get_port_cmd(message, INIT);
			printf("----- TCP INIT: %d\n", port);
			estados[args.client_index].port = port;
			estados[args.client_index].status = INIT_STATUS;
		}else if (has_received(message, PLAY)){
			printf("----- TCP Envio play\n");
			estados[args.client_index].status = PLAY_STATUS;
		}else if (has_received(message, PAUSE)){
			printf("----- TCP Envio pause\n");
			estados[args.client_index].status = PAUSE_STATUS;
		}else if (has_received(message, STOP)){
			printf("----- TCP Envio stop\n");
			estados[args.client_index].status = STOP_STATUS;
		}else if (has_received(message, CLOSE)){
			string hostIP = estados[args.client_index].ip;
			printf("Se cierra TCP %s:%d\n", hostIP.c_str(), estados[args.client_index].port);
			estados[args.client_index].status = CLOSE_STATUS;
			is_connected = 0;
		}else
			printf("----- TCP Mensaje no reconocido.\n");

		memset(data, 0, sizeof(data)); 
	}

	close(sock);
	return 0;
}

bool has_received(std::string message, std::string command){

	return (message.compare(0, message.length(), command) == 0);
}

bool has_received_port(std::string message, std::string command){

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