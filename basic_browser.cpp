#include <iostream>
#include <cstring>
#include <cmath>
#include "basic_socket.h"

#define PROT_OPT(v1, v2, d1, d2, d3) {(char)v1, (char)v2, (char)d1, (char)d2, (char)d3}

void sendInt(ISocketCl *client, int val)
{
	char *cval = (char*)calloc(sizeof(char),4); cval = (char*)&val;
	client->Send(cval, 4);
}

void sendChar(ISocketCl *client, char val)
{
	client->Send(&val,1);
}

int main(int argc, char *argv[])
{
	char ip[16]; // Answer from DNS Server
	int dir[3] = {0,0,0};
	std::string servername = "veepreweb";
	if(argc>4) for(int i = 0;i<3;i++) dir[i] = atoi(argv[i+2]);
	{
		ISocketCl client(SOCK_STREAM);
		client.Connect("192.168.88.128",8000); //Connecting to Name Server with 8000 port
						       //
		if(argc>1) servername = argv[1]; // if entered a server name, program will search entered name (like basic_browser sitedomain)
					  // else program will search veepreweb

		client.Send((char*)servername.c_str(),servername.size()); // Client will send the domain
		
		client.Read(ip, 16); // reading answer
		
		client.Close();
	}
	
	std::cout << "Answer From Name Server:" << ip << std::endl;
	if(std::string(ip)!="") // if IP is filled with data
	{
		std::cout << "Trying to reach website (" << servername << ":" << ip << ")." << std::endl;
		ISocketCl websiteClient(SOCK_STREAM);
		websiteClient.Connect(ip,4545); // Connecting website with port 4545

		char optV[5] = PROT_OPT(0, 1, dir[0], dir[1], dir[2]);
		websiteClient.Send(optV,5);

		char *preWeb = (char*)calloc(sizeof(char),2);
		websiteClient.Read(preWeb,2);
		std::cout << "PreWeb:" << preWeb[0] << " " << preWeb[1] << std::endl;

		if(preWeb[0]==0)
		{
			char complete = 0;
			websiteClient.Send(&complete, 1);
			char opSize = 0;
			websiteClient.Read(&opSize,1);
			char *opTypes = (char*)calloc(sizeof(char), (int)opSize);
			websiteClient.Read(opTypes, (int)opSize);
			std::cout << opTypes << std::endl;
			for(int i = 0;i<strlen(opTypes);i++)
			{
				if(opTypes[i]=='i') sendInt(&websiteClient, 0);
				else if(opTypes[i]=='c') sendChar(&websiteClient, '0');
			}
			std::cout << strlen(opTypes) << " ops sended." << std::endl;

			char *web = (char*)calloc(pow(2,(int)preWeb[1]), sizeof(char));
			websiteClient.Read(web, pow(2,(int)preWeb[1]));
			std::cout << "domain<" << servername << ">" << std::endl;
			std::cout << web << std::endl;
		} else std::cout << "VeebreWeb Error:" << (int)preWeb[0] << std::endl;
	}
	return 0;
}
