#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>

using namespace std;

typedef unsigned long ip_address;
typedef unsigned short uint16;
static const int BUFFER_SIZE = 4*1024*1024;

static void UsageExit()
{
   printf("Usage:  xfer (send/receive) filename [host:Port]\n");
   exit(10);
}

static ip_address GetHostByName(const char * name)
{
   struct hostent * he = gethostbyname(name);
   return ntohl(he ? *((ip_address*)he->h_addr) : 0);
}

static bool SendFile(ip_address ipAddress, uint16 port, const char * fileName)
{
   bool flag = true;
   FILE * fpIn = fopen(fileName, "r");
   if (fpIn)
   {
      int s = socket(AF_INET, SOCK_STREAM, 0);
      if (s >= 0)
      {
         struct sockaddr_in saAddr; memset(&saAddr, 0, sizeof(saAddr));
         saAddr.sin_family      = AF_INET;
         saAddr.sin_addr.s_addr = htonl(ipAddress);
         saAddr.sin_port        = htons(port);
		 

         if (connect(s, (struct sockaddr *) &saAddr, sizeof(saAddr)) == 0)
         {
            printf("Connected to remote host, sending file [%s]\n", fileName);

            char buf[BUFFER_SIZE];
            while(1)
            {
               ssize_t bytesRead = fread(buf, 1, sizeof(buf), fpIn);
               if (bytesRead <= 0) break;  // EOF

               //printf("Read %i bytes from file, sending them to network...\n", (int)bytesRead);
               if (send(s, buf, bytesRead, 0) != bytesRead)
               {
                  perror("send");
				  flag = false;
                  break;
               }
            }
         }
         else 
		 {
			 perror("connect");
			 flag = false;
		 }

         close(s);
      }
      else 
	  {
		  perror("socket");
		  flag = false;  
	  }

      fclose(fpIn);
   }
   else 
   {
	   printf("Error, couldn't open file [%s] to send!\n", fileName);
	   flag = false; 
   }
   
   return flag;
}

static bool SendMessage(ip_address ipAddress, uint16 port, const char * message)
{
    bool flag = true;
      int s = socket(AF_INET, SOCK_STREAM, 0);
      if (s >= 0)
      {
         struct sockaddr_in saAddr; memset(&saAddr, 0, sizeof(saAddr));
         saAddr.sin_family      = AF_INET;
         saAddr.sin_addr.s_addr = htonl(ipAddress);
         saAddr.sin_port        = htons(port);
		 

         if (connect(s, (struct sockaddr *) &saAddr, sizeof(saAddr)) == 0)
         {
            printf("Connected to remote host, sending message [%s] \n", message);

            char buf[100];
			
			strcpy(buf, message);
		
			if (send(s, buf, strlen(message), 0) != strlen(message))
               {
                  perror("send");
				  flag = false;
                  //break;
               }
         }
         else 
		 {
			 perror("connect");
			 flag = false;
		 }

         close(s);
      }
      else 
	  {
		  perror("socket");
		  flag = false;
	  }  

	return flag; 
}

static bool ReceiveFile(uint16 port, const char * fileName)
{
   bool flag = true;
   int s = socket(AF_INET, SOCK_STREAM, 0);
   if (s >= 0)
   {
#ifndef WIN32
      // (Not necessary under windows -- it has the behaviour we want by default)
      const int trueValue = 1;
      (void) setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &trueValue, sizeof(trueValue));
#endif

      struct sockaddr_in saAddr; memset(&saAddr, 0, sizeof(saAddr));
      saAddr.sin_family      = AF_INET;
      saAddr.sin_addr.s_addr = htonl(0);  // (IPADDR_ANY)
      saAddr.sin_port        = htons(port);

      if ((bind(s, (struct sockaddr *) &saAddr, sizeof(saAddr)) == 0)&&(listen(s, 10) == 0))
      {
         memset(&saAddr, 0, sizeof(saAddr));
         socklen_t len = sizeof(saAddr);
         printf("Waiting for incoming TCP connection on port %u, to write file [%s]\n", port, fileName);
         int connectSocket = accept(s, (struct sockaddr *) &saAddr, &len);
         if (connectSocket >= 0)
         {
            FILE * fpIn = fopen(fileName, "w");
            if (fpIn)
            {
               char buf[BUFFER_SIZE];
               while(1)
               {
                  ssize_t bytesReceived = recv(connectSocket, buf, sizeof(buf), 0);
                  if (bytesReceived < 0) perror("recv");  // network error?
                  if (bytesReceived == 0) break;   // sender closed connection, must be end of file

                  //printf("Received %i bytes from network, writing them to file...\n", (int) bytesReceived);
                  if (fwrite(buf, 1, bytesReceived, fpIn) != (size_t) bytesReceived)
                  {
                     perror("fwrite");
					 flag = false;
                     break;
                  }
               }

               fclose(fpIn);
            }
            else 
			{
				printf("Error, couldn't open file [%s] to receive!\n", fileName);
				flag = false;
			}

            close(connectSocket);
         }
      }
      else 
	  {
		  perror("bind");
		  flag = false;
	  }

      close(s);
   }
   else 
   {
	   perror("socket");
	   flag = false;
   }
   
   return flag;
}


static void ReceiveMessage(uint16 port)
{
   //std::string recv_msg = "";
   int s = socket(AF_INET, SOCK_STREAM, 0);
   if (s >= 0)
   {
#ifndef WIN32
      // (Not necessary under windows -- it has the behaviour we want by default)
      const int trueValue = 1;
      (void) setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &trueValue, sizeof(trueValue));
#endif

      struct sockaddr_in saAddr; memset(&saAddr, 0, sizeof(saAddr));
      saAddr.sin_family      = AF_INET;
      saAddr.sin_addr.s_addr = htonl(0);  // (IPADDR_ANY)
      saAddr.sin_port        = htons(port);

      if ((bind(s, (struct sockaddr *) &saAddr, sizeof(saAddr)) == 0)&&(listen(s, 10) == 0))
      {
         memset(&saAddr, 0, sizeof(saAddr));
         socklen_t len = sizeof(saAddr);
         printf("Waiting for incoming TCP connection on port %u, for message \n", port);
         int connectSocket = accept(s, (struct sockaddr *) &saAddr, &len);
         if (connectSocket >= 0)
         {
            char buf[100];
               
            ssize_t bytesReceived = recv(connectSocket, buf, sizeof(buf), 0);
            if (bytesReceived < 0) perror("recv");  // network error?
            //if (bytesReceived == 0) break;   // sender closed connection, must be end of file	  
		    buf[bytesReceived] = '\0';
		    std::string recv_msg1(buf);
            printf("Received %i bytes from network, message [%s]...\n", (int) bytesReceived, recv_msg1.c_str()); 
		    //recv_msg = recv_msg1;
			//strcpy((char *)recv_msg.c_str(), recv_msg1.c_str());
			//printf("content of recv_msg %s \n", recv_msg.c_str());
            close(connectSocket);
         }
      }
      else 
	  {
		  perror("bind");
	  }

      close(s);
   }
   else 
   {
	   perror("socket");
   }
   
   //printf("content of recv_msg %s \n", recv_msg.c_str());
   
   return;// recv_msg;
}

