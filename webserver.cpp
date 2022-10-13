#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>

#include <unordered_map>

#include "basic_socket.h"

char version[2] = {0,0};

std::string siteByte, errorByte = "Version not matching with server!";

#define SITE_BYTE 10 //Site scale
#define ERROR_BYTE 6

bool handler(int socket)
{
	std::cout << "Connected socket:" << socket << "." << std::endl;

	char optV[5];
	if(!ISocketSv::Read(socket, optV, 5)) return false; // Getting optV data
	std::cout << "Version:" << (int)optV[0] << ":" << (int)optV[1] << std::endl;
	std::cout << "Directory:" << (int)optV[2] << ":" << (int)optV[3] << ":" << (int)optV[4] << std::endl;

	bool versionGood = false;
	if(optV[0] == version[0] && optV[1] == version[1]) versionGood = true;

	char preweb[2] = {(char)0,versionGood == true? (char)SITE_BYTE : (char)ERROR_BYTE}; // Sending preweb data
	if(!ISocketSv::Send(socket, preweb, 2)) return false; 

	char complete = 0;
	if(!ISocketSv::Read(socket, &complete, 1)) return false; // Reading complete data

	if(complete == (char)0) // If complete equals to 0, that is successfull
	{
		if(versionGood) ISocketSv::Send(socket, (char*)siteByte.c_str(), pow(2,SITE_BYTE)); // sending site
		else ISocketSv::Send(socket, (char*)errorByte.c_str(), errorByte.length()); // sending error
	} 

	return false;
}

int main(int argc, char *argv[])
{
	siteByte = "Welcome to AmmarOzkan Blog Site!";
	std::ifstream siteTxt; siteTxt.open("index");
	std::stringstream siteData; siteData << siteTxt.rdbuf();
	siteByte = siteData.str();

	ISocketSv server(SOCK_STREAM, 4545, 5);
	server.Begin(handler);

	return 0;
}
