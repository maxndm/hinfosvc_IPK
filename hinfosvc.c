#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

void GetHostname(char *buffer) {
  FILE *fp;
  memset(buffer,0,strlen(buffer));
  fp = fopen("/proc/sys/kernel/hostname","r");
  fscanf(fp,"%s",buffer);
  strcat(buffer,"\n");
}

void GetCPU(char *buffer) {
  FILE *fpipe;
  char *command = "cat /proc/cpuinfo | grep 'model name' | head -n 1 | awk -F ':' '{printf $2}'\n ";
  char c;
  int i = 0;
  memset(buffer,0,strlen(buffer));
  fpipe = popen(command,"r");
  if(fpipe == NULL){
    printf("Unable to process\n" );
  }else{
    while((c=fgetc(fpipe))!= EOF ){
      if (i==0 && c==' '){
        continue;
      }
      buffer[i] = c;
      i++;
    }
  }
  pclose(fpipe);
  buffer[i]='\0';
  strcat(buffer,"\n");

}

void GetCPUloadDATA(char *buffer, double *now) {
  char *token;
  FILE *fpipe;
  char *command ="cat /proc/stat | grep 'cpu' | head -n 1";
  char c;
  int i = 0;
  memset(buffer,0,strlen(buffer));
  fpipe = popen(command,"r");
  if(fpipe == NULL){
    printf("Unable to process\n" );
  }else{
    while((c=fgetc(fpipe))!= EOF ){
      if (i==0 && c==' '){
        continue;
      }
      buffer[i] = c;
      i++;
    }
  }
  pclose(fpipe);
  i = 0;
  token = strtok(buffer, " ");
  while( token != NULL){
    if(!strcmp(token,"cpu")){
      token = strtok(NULL, " ");
      continue;
    };
    now[i] = strtod(token,NULL);
    i++;
    token = strtok(NULL, " ");
  }
  memset(buffer,0,strlen(buffer));
}

void GetCPUload(char *buffer) {
  memset(buffer,0,strlen(buffer));
  double now[10];
  double prev[10];
  double PrevIdle,PrevNonIdle,PrevTotal;
  double Idle,NonIdle,Total;
  double TotalDif,IdleDif;
  double Percentage;
  GetCPUloadDATA(buffer,prev);
  sleep(0.5);
  GetCPUloadDATA(buffer,now);
  PrevIdle = prev[3] + prev[4];
  Idle = now[3] + now[4];

  PrevNonIdle = prev[0] + prev[1] + prev[2] + prev[5] + prev[6] + prev[7];
  NonIdle = now[0] + now[1] + now[2] + now[5] + now[6] + now[7];

  PrevTotal = PrevIdle + PrevNonIdle;
  Total = Idle + NonIdle;

  TotalDif = Total - PrevTotal;
  IdleDif = Idle - PrevIdle;

  Percentage = (TotalDif - IdleDif)*100/TotalDif;
  // printf("%0.1f\n",Percentage );
  sprintf(buffer,"%0.0f%c",Percentage,'%' );
  strcat(buffer,"\n");
}

// void GetCPUload(char *buffer) {
//   double Total;
//   double now[500];
//   char *token;
//   FILE *fpipe;
//   char *command ="top -b -n 1| tail -n +6 | head -n 200 |awk -F ' ' '{printf \"%s \",$9 }'";
//   char c;
//   int i = 0;
//   memset(buffer,0,strlen(buffer));
//   fpipe = popen(command,"r");
//   if(fpipe == NULL){
//     printf("Unable to process\n" );
//   }else{
//     while((c=fgetc(fpipe))!= EOF ){
//       if (i==0 && c==' '){
//         continue;
//       }
//       buffer[i] = c;
//       i++;
//     }
//   }
//   pclose(fpipe);
//   i = 0;
//   token = strtok(buffer, " ");
//   while( token != NULL){
//     if(!strcmp(token,"%CPU")){
//       token = strtok(NULL, " ");
//       continue;
//     };
//     now[i] = strtod(token,NULL);
//     i++;
//     token = strtok(NULL, " ");
//   }
//   memset(buffer,0,strlen(buffer));
//   for (size_t i = 0; i < 10; i++) Total += now[i] ;
//   sprintf(buffer,"%1.f%c",Total,'%' );
//   strcat(buffer,"\n");
//
// }

int main(int argc, char *argv[]) {

  if (argc>2) {
    printf("Too many arguments..\n" );
    return 0;
  }else if (argc<2){
    printf("usage: ./server_http <port> \n" );
    return 0;
  }

  char request[100];
  char buffer[500];
  int port = strtol(argv[1],&argv[1], 10);
  char *http_header_pattern = "HTTP/1.1 200 OK\r\nContent-Type:text/plain;\r\n\r\n";
  char *http_header_pattern1 = "HTTP/1.1 400 Bad request\r\nContent-Type:text/plain;\r\n\r\n";
  char http_header[100] = "\0";

  //create a socket
  int server_socket;
  server_socket = socket(AF_INET,SOCK_STREAM,0);
  if(server_socket == -1){
    printf("Socket creation failed..\n");
    exit(0);
  }

  //server address information
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);
  server_address.sin_addr.s_addr = INADDR_ANY; //0.0.0.0 - localhost

  //bind the socket to our IP and sin_port
  bind(server_socket, (struct sockaddr*) &server_address,sizeof(server_address));
  int client_socket;
  listen(server_socket,5);

  while(1){
    client_socket = accept(server_socket,NULL,NULL);
    read(client_socket,request,sizeof(request));
    // printf("Client said: %s\n",request );
    char *token = strtok(request, " ");
    token = strtok(NULL, " ");
    if(strcmp(token,"/hostname")==0){
      GetHostname(buffer);
      strcpy(http_header,http_header_pattern);
      strcat(http_header,buffer);
    }else if(strcmp(token,"/cpu-name")==0){
      GetCPU(buffer);
      strcpy(http_header,http_header_pattern);
      strcat(http_header,buffer);
    }else if(strcmp(token,"/load")==0){
      GetCPUload(buffer);
      strcpy(http_header,http_header_pattern);
      strcat(http_header,buffer);
    }else{
      strcpy(http_header,http_header_pattern1);
    }
    write(client_socket, http_header, sizeof(http_header));
    memset(http_header,0,strlen(http_header));
    close(client_socket);
  }
  close(server_socket);
  return 0;
}
