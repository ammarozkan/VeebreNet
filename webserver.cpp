#include <iostream> // std::cout
#include <vector> // std::vector
#include <cmath> // pow(x,y)

#include <fstream>
#include <sstream>

#include "basic_socket.h"

char version[2] = {0,2};

struct Page
{
	char size;
	std::string pageByte;
	std::string work(std::vector<int>,std::vector<char>)
	{
		return pageByte; // Will define a language for programming
				 // pages.
	}
	char getSize() { for(int i = 0;i<255;i++) if(pow(2,i)>pageByte.size()) return i; return '?'; }
	Page(std::string path)
	{
		std::ifstream PageText; PageText.open(path);
		std::stringstream PageStream; PageStream << PageText.rdbuf(); PageText.close();
		pageByte = PageStream.str();
		size = getSize();
		std::cout << path << std::endl << pageByte << std::endl;
	}
};

class PageManager
{
	private:
		std::vector<Page>* pages;
		int CPage;
	public:
		PageManager(std::vector<Page>* pages) : pages(pages)
		{

		}

		char readChar(int socket)
		{
			char charVal = 0; ISocketSv::Read(socket,&charVal,1); return charVal;
		}
		int readInt(int socket)
		{
			char* integerVal = (char*)calloc(sizeof(char),4);
			ISocketSv::Read(socket,integerVal,4);
			return *(int*)(integerVal);
		}

		Page &operator[](int i){ if(i<0) return pages[0][1]; else return pages[0][i]; }
		int GetPage(char* directory)
		{
			return directory[2];
		}
		char* optVRead(char* optV)//reads optV and returns preweb
		{
			char* preweb = (char*)calloc(sizeof(char),2);
			if(optV[0] != version[0] || optV[1] != version[1]) {preweb[0] = 1; return preweb;}
			int page = CPage = GetPage((preweb+2)); if(page<0) {preweb[0]=2;return preweb;}
			preweb[1] = pages[0][page].size;
			return preweb;
		}
		std::string optsRead(int socket) //reads opts and returns page
		{
			std::vector<char> param;
			char *size = calloc(sizeof(char),1);
			if(!ISocketSv::Read(socket,size,1)) return nullptr;
			for(int i = 0;i<(int)size[0];i++)
			{
				char *val; if(!ISocketSv::Read(socket,val,1)) return nullptr;
				param.push_back(*val);
			}
			std::vector<int> integers; std::vector<char> characters;
			for(int i = 0;i<(int)size[0];i++)
			{
				if(param[i]=='i') integers.push_back(readInt(socket));
				else if(param[i] == 'c') integers.push_back(readChar(socket));
			}
			return pages[0][CPage].work(integers,characters);
		}

};

std::vector<Page> pages;
bool handler(int socket)
{
	PageManager PM(&pages);
	char optV[5];
	if(!ISocketSv::Read(socket,optV,5)) return false;
	char* preweb = PM.optVRead(optV);
	if(!ISocketSv::Send(socket,preweb,2)) return false;
	std::string page = PM.optsRead(socket);
	if(!ISocketSv::Send(socket,(char*)page.c_str(),pow(2,preweb[1]) )) return false;
	return false;

}

int main(int argc, char *argv[])
{
	pages.push_back(Page("index.vbnt"));

	ISocketSv server(SOCK_STREAM,4545,5);
	server.Begin(handler);
}
