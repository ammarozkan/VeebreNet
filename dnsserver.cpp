#include <iostream> // printing output to terminal
#include <fstream> // reading file
#include <vector> // vector for datas

#include <cstring> // strlen()

#include "basic_socket.h" // just for doing stuff with sockets
#include <unistd.h>

struct Address //just a struct
{
	std::string domain, address;
};

class DNSDatabase
{
	public:
	std::vector<Address> database;

	bool load(std::string DataName) //that will add data from file to database variable
	{
		std::fstream DataFile(DataName); Address add;
		if(DataFile.is_open())
		{
			while(DataFile >> add.domain >> add.address) 
			{
				std::cout << "Adding:" << add.domain << ":" << add.address << ":" << std::endl;
				database.push_back(add);
			}
			return true;
		}
		else 
		{
			std::cout << "Error while opening the data:" << DataName << std::endl;
			return false;
		}
	}
	
	std::string findIP(std::string Domain)
	{
		for(int i = 0;i<database.size();i++)
			if(database[i].domain == Domain) return database[i].address;

		 return "";
	}

	std::string findDomain(std::string IP)
	{
		for(int i = 0;i<database.size();i++)
			if(database[i].address == IP) return database[i].domain;
		return "";
	}
}mainDns;

bool handler(int socket)
{
	std::cout << "Connected:" << socket << std::endl;
	
	char msg[1024];
	if(!ISocketSv::Read(socket, msg, 1024)) return false;
	
	char *domain = (char*)calloc(strlen(msg),sizeof(char)); domain = msg;
	
	std::cout << "Searching IP for:" << domain << std::endl;
	std::string ip = mainDns.findIP( std::string(domain) );
	
	std::cout << "Sending finded IP for:" << domain << ":" << ip << std::endl;
	ISocketSv::Send(socket, (char*)ip.c_str(), strlen(ip.c_str()));

	close(socket);
	return false; // Returning false will disconnect and remove socket
}

int main(int argc, char *argv[])
{
	std::string path = "dnsBase.ddb";
	if(argc>1) path = argv[1];
	mainDns.load(path);

	ISocketSv DNSServer(SOCK_STREAM, 8000, 5);
	DNSServer.Begin(handler); // this ISocketSv class will run periodically this function for each connected socket
	return 0;
}
