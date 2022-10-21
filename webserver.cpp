#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>

#include <unordered_map>

#include "basic_socket.h"

char version[2] = {0,1};

struct PageInfo
{
	char size; // Data will be pow(2,size) bytes
	std::string path,opTypes;
	std::string pageByte;

	char getSize() { for(int i = 0;i<255;i++) if(pow(2,i)>pageByte.size()) return i; return '?';}
	PageInfo(std::string path, std::string opTypes = "")
	{

		std::ifstream PageText; PageText.open(path);
		std::stringstream PageStream; PageStream << PageText.rdbuf(); PageText.close();
		pageByte = PageStream.str();
		size = getSize();
		this->opTypes = opTypes;

		std::cout << path << " readed for Ops:" << opTypes <<  ":" << std::endl << pageByte << std::endl;
	}
};

std::vector<PageInfo> Pages;
#define MAINPAGE 0
#define ERRORPAGE 1

int getPage(int p1, int p2, int p3)
{
	return MAINPAGE;
}

int readInt(int socket)
{
	char* integerVal = (char*)calloc(sizeof(char), 4);
	ISocketSv::Read(socket, integerVal, 4);
	return *(int*)(integerVal);
}

char readChar(int socket)
{
	char charVal = 0;
	ISocketSv::Read(socket, &charVal, 1);
	return charVal;
}

bool handler(int socket)
{
	std::cout << "Connected socket:" << socket << "." << std::endl;

	char optV[5];
	if(!ISocketSv::Read(socket, optV, 5)) return false; // Getting optV data
	std::cout << "Version:" << (int)optV[0] << ":" << (int)optV[1] 
		<< "(should be " << (int)version[0] << ":" << (int)version[1] << ")" << std::endl;

	std::cout << "Directory:" << (int)optV[2] << ":" << (int)optV[3] << ":" << (int)optV[4] << std::endl;
	
	char preweb[2] = {0,0}; // Sending preweb data
	
	int page = getPage((int)optV[2], (int)optV[3],(int)optV[4]); preweb[1] = Pages[page].size;
	if(optV[0] != version[0] || optV[1] != version[1]) { preweb[0] = page = ERRORPAGE; }

	std::cout << "Sending preweb:" << (int)preweb[0] << ":" << (int)preweb[1] << std::endl;
	if(!ISocketSv::Send(socket, preweb, 2)) return false; 
	if(preweb[0] != 0) return false; // Stop Veepre if a error corrupted.

	char complete = 0;
	if(!ISocketSv::Read(socket, &complete, 1)) return false; // Reading complete data
	
	char opTypeSize = Pages[page].opTypes.size();
	std::cout << "Sending Op Size:" << (int)opTypeSize 
		<< " and type:"<<Pages[page].opTypes<<"."<< std::endl;
	if(!ISocketSv::Send(socket, &opTypeSize,1)) return false; // Sending OpSize
	if((int)opTypeSize > 0) 
	{
		if(!ISocketSv::Send(socket, (char*)Pages[page].opTypes.c_str(), (int)opTypeSize )) return false; // Sending OpTypes
		std::cout << "Readed Opts:" ;
		for(int i = 0 ;i<(int)opTypeSize;i++)
		{
			if(Pages[page].opTypes[i]=='c') std::cout << readChar(socket);//reading char
			else if(Pages[page].opTypes[i]=='i') std::cout << readInt(socket); //reading integer
		}
	} std::cout << std::endl << (int)opTypeSize << " op reading ended." << std::endl;

	if(complete == (char)0) // If complete equals to 0, that is successfull
	{
		ISocketSv::Send(socket, (char*)Pages[page].pageByte.c_str(),pow(2,Pages[page].size));
		std::cout << "Sending PageId:" << page << "." << std::endl;
	}
	std::cout << "Communication end for socket " << socket << "." << std::endl << std::endl;

	return false;
}

int main(int argc, char *argv[])
{
	Pages.push_back(PageInfo("index", "ccccccc")); // MainPage
	Pages.push_back(PageInfo("error", "")); // ErrorPage
	Pages.push_back(PageInfo("info", "")); // 2 

	ISocketSv server(SOCK_STREAM, 4545, 5);
	server.Begin(handler);

	return 0;
}
