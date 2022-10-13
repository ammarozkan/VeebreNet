#include <iostream>
#include <cstring>
#include <cmath>
#include "basic_socket.h"

#define PROT_OPT(v1, v2, d1, d2, d3) {(char)v1, (char)v2, (char)d1, (char)d2, (char)d3}

int main(int argc, char *argv[])
{
	char ip[16]; // Answer from DNS Server
	int dir[3] = {0,0,0};
	std::string dns = "veepreweb";
	if(argc>4) for(int i = 0;i<3;i++) dir[i] = atoi(argv[i+2]);
	{
		ISocketCl client(SOCK_STREAM);
		client.Connect("192.168.88.128",8000); //Connecting to DNS with 8000 port
						       //
		if(argc>1) dns = argv[1]; // if entered a domain, program will search entered domain (like basic_browser sitedomain)
					  // else program will search veepreweb

		client.Send((char*)dns.c_str(),dns.size()); // Client will send the domain
		
		client.Read(ip, 16); // reading answer
		
		client.Close();
	}
	
	std::cout << "Answer From DNS Server:" << ip << std::endl;
	if(std::string(ip)!="") // if IP is filled with data
	{
		std::cout << "Trying to reach website (" << dns << ":" << ip << ")." << std::endl;
		ISocketCl websiteClient(SOCK_STREAM);
		websiteClient.Connect(ip,4545); // Connecting website with port 4545

		char optV[5] = PROT_OPT(0, 0, dir[0], dir[1], dir[2]);
		websiteClient.Send(optV,5);

		char *preWeb = (char*)calloc(2,sizeof(char));
		websiteClient.Read(preWeb,2);
		
		if(preWeb[0]==0)
		{
			char complete = 0;
			websiteClient.Send(&complete, 1);

			char *web = (char*)calloc(pow(2,(int)preWeb[1]), sizeof(char));
			websiteClient.Read(web, pow(2,(int)preWeb[1]));
			std::cout << "domain<" << dns << ">" << std::endl;
			std::cout << web << std::endl;
		}
	}
	return 0;
}
