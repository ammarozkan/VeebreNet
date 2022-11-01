#include <iostream>
#include <cmath>

#include "basic_socket.h"

void sendInt(ISocketCl *client, int val)
{
	char *cval = (char*)calloc(sizeof(char),4); cval = (char*)&val;
	client->Send(cval, 4);
}

void sendChar(ISocketCl *client, char val)
{
	client->Send(&val,1);
}

char version[2] = {0,2};

int main(int argc, char *argv[])
{
	char ip[16];
	std::string servername = "veepreweb", nameserver = "192.168.88.128", nameserver_port = "8000"; int dir[3] = {0,0,0};
	std::vector<int> intOpts; std::vector<char> charOpts; std::string opts;
	for (int i = 1;i<argc;i++)
	{
		if(argv[i] == "--name" && i+1 < argc) servername = argv[++i];
		else if(argv[i] == "--dir" && i+3 < argc) for (int j = 0;j<3;j++) dir[j] = atoi(argv[++i]);
		else if(argv[i] == "--nameserver" && i+1 < argc) nameserver = argv[++i];
		else if(argv[i] == "--nameserverport" && i+1 < argc) nameserver_port = argv[++i];
		else if(argv[i] == "--opts" && i+1 < argc)
		{
			opts = argv[++i];
			if(i+opts.size()<argc) for(int j = 1;j<opts.size();j++) 
			{
				if(opts[j]=='c') charOpts.push_back(argv[++i][0]);
				else if(opts[j] == 'i') intOpts.push_back( atoi(argv[++i]) );
				else SOCKETERRORCOM("What th' f is "<<opts[j]<<" parameter type?",return 1);
			}
		}
	}
	{ // Nameserver Connection
		ISocketCl client(SOCK_STREAM);
		if( !client.Connect(nameserver,std::stoi(nameserver_port)) ) SOCKETERRORCOM("Unable to connect nameserver.",return -1);
		if( !client.Send((char*)servername.c_str(),servername.size()) ) SOCKETERRORCOM("Unable to send server's name to nameserver.",return -2);
		if( !client.Read(ip, 16) ) SOCKETERRORCOM("Unable to read ip address from nameserver.",return -3);
		client.Close();
	}
	std::cout << "Answer From Name Server:" << ip << std::endl;
	if(std::string(ip)!="")
	{
		char optV[5] = {version[0],version[1],(char)dir[0],(char)dir[1],(char)dir[2]};
		ISocketCl websiteClient(SOCK_STREAM);
		if(!websiteClient.Connect(ip,4545)) SOCKETERRORCOM("Website connection failed.",return -4);
		if(!websiteClient.Send(optV,5)) SOCKETERRORCOM("optV sending failed.",return -5);
		
		char *preWeb = (char*)calloc(sizeof(char),2);
		if(!websiteClient.Read(preWeb,2)) SOCKETERRORCOM("preWeb reading failed.",return -6);
		std::cout << "PreWeb:" << (int)preWeb[0] << " " << (int)preWeb[1] << std::endl;
		if(preWeb[0]!=0) SOCKETERRORCOM("Server error,"<<preWeb[0]<<", received from preWeb.", return -7);
		
		//Parameter sent
		char optSize = (char)opts.size();
		if(!websiteClient.Send(&optSize,1)) SOCKETERRORCOM("Option size sending failed.",return -8);
		if(!websiteClient.Send((char*)opts.c_str(),opts.size())) SOCKETERRORCOM("Option type sending failed.",return -9);
		for(int counter = 0,in=0,ch=0;counter<opts.size();counter++)
		{
			if(opts[counter]=='c') sendChar(&websiteClient,charOpts[ch++]);
			else if(opts[counter]=='i') sendInt(&websiteClient,intOpts[in++]);
		}
		char *page = (char*)calloc(sizeof(char),pow(2,(int)preWeb[1]));
		if(!websiteClient.Read(page,pow(2,(int)preWeb[1]))) SOCKETERRORCOM("Page reading failed.",return -10);
		std::cout << "Domain<"<< servername <<">["<< dir[0] << "," << dir[1] << "," << dir[2] << "]" << std::endl;
		std::cout << page << std::endl;

	}
}
