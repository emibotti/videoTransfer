#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>

#define PORT 33335
#define MY_IP "127.0.0.1"
#define MAX_QUEUE 10
#define MAX_MSG_SIZE 1024

int main()
{
   //primitiva SOCKET
   int server_socket = socket(AF_INET, SOCK_STREAM, 0);
   printf("\nprimitiva socket");
   
   //primitiva BIND
   struct sockaddr_in server_addr;
   socklen_t server_addr_size = sizeof server_addr;
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(PORT);
   server_addr.sin_addr.s_addr = inet_addr(MY_IP);
   bind(
      server_socket, 
      (struct sockaddr*)&server_addr, server_addr_size
   );
   printf("\n socketbindeado");


   //primitiva LISTEN
   listen(server_socket, MAX_QUEUE);
   printf("\n comienza a escuchar");
   

   while (1) {
      printf("\n espero una conexion - accept...");
      //primitiva ACCEPT
      struct sockaddr_in client_addr;
      socklen_t client_addr_size = sizeof client_addr;
      int socket_to_client = accept(
	     server_socket, 
		 (struct sockaddr *)&client_addr, &client_addr_size
      );
      printf("\nRecibio conexion\n");
   
      //primitiva RECEIVE
      char* data = malloc(MAX_MSG_SIZE);
      int data_size = MAX_MSG_SIZE;
      int received_data_size = recv(socket_to_client, data, data_size, 0);
      
      printf("Recibido del cliente (%d bytes): %s\n", received_data_size, data);
      
      int i;
      for (i = 0; i < received_data_size; i++) {
         data[i] = toupper(data[i]);
      }
      
      //primitiva SEND
      int sent_data_size = send(socket_to_client, data, received_data_size, 0);
      printf("Enviado al cliente (%d bytes): %s\n", sent_data_size, data);
      
      //primitiva CLOSE
      close(socket_to_client);
      close(server_socket);
      break;
   }

   //CLOSE del socket que espera conexiones

    return 0;
}

