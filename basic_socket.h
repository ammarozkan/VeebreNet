#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>

#define SOCKETERROR(x) std::cout << "Error[" << x << "] " << errno << std::endl;
#define SOCKETERRORCOM(x,y) {std::cout << "Error[" << x << "] " << errno << std::endl; y;}

class ISocketSv
{
	private:
		int sock;
		std::vector<int> connectedSockets;
		std::vector<std::thread> thds;
	public:
		ISocketSv(int type,int port, int listenSize)
		{
			if( (sock = socket(AF_INET,type, 0)) == 0 ) SOCKETERROR("socket()");

			int opt = 1;
			if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) SOCKETERROR("setsockopt()");

			sockaddr_in address; address.sin_family = AF_INET; 
			address.sin_port = htons(port); address.sin_addr.s_addr = INADDR_ANY;
			if(bind(sock, (sockaddr*)&address, sizeof(address)) < 0) SOCKETERROR("bind()");

			if(listen(sock, listenSize) < 0) SOCKETERROR("listen()");
		}

		bool Accept() // getting function that handles given socket number. 
		{
			int newConnect = 0;
			/*if((newConnect = accept(sock, NULL, NULL)) < 0) SOCKETERRORCOM("accept()",return false);
			std::thread shThr(socketHandler,newConnect);
			shThr.join();*/
			
			while((newConnect = accept(sock, NULL, NULL)) >= 0)
				connectedSockets.push_back(newConnect);

			return true;
		}
		
		void Threader()
		{
			
		}

		void Do(bool (*socketHandler)(int))
		{
			/*int i = 0,c = 0;
			while(true) for(;i<connectedSockets.size();i++) 
			{
				thds.push_back(std::thread(socketHandler, connectedSockets[i]));
				thds[i].join();
				std::cout << "THREAD " << i << " ACTIVE!" << std::endl;
			}*/
			
			while(true) for(int i = 0;i<connectedSockets.size();i++)
				if(!socketHandler(connectedSockets[i])) connectedSockets.erase(connectedSockets.begin()+i);
			
		}
		
		void Begin(bool (*socketHandler)(int))
		{
			std::thread AckThd(&ISocketSv::Accept, this) , DoThd(&ISocketSv::Do, this, socketHandler)
				/*,ThdThd(&ISocketSv::Threader, this)*/;
			AckThd.join(); DoThd.join(); /*ThdThd.join();*/
		}

		void Close()
		{
			for(int i = 0;i<connectedSockets.size();i++) 
			{
				close(connectedSockets[i]); connectedSockets.erase(connectedSockets.begin()+i);
			}
			shutdown(sock, SHUT_RDWR);
			close(sock);
		}

		static bool Read(int socket, char* msg, unsigned int size)
		{
			if(read(socket,msg,size) < 0) SOCKETERRORCOM("read()", return false); return true;
		}

		static bool Send(int socket, char* msg, unsigned int size)
		{
			if(send(socket,msg,size,0) < 0) SOCKETERRORCOM("send()", return false); return true;
		}
};

class ISocketCl
{
	private:
		int sock;
	public:
		ISocketCl(int type)
		{
			if( (sock = socket(AF_INET, type, 0)) == 0 ) SOCKETERROR("socket()");

			int opt = 1;
			if( setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) SOCKETERROR("setsockopt()");
		}

		bool Connect(std::string ip, int port)
		{
			sockaddr_in addr; addr.sin_family = AF_INET;
			addr.sin_port = htons(port); addr.sin_addr.s_addr = inet_addr(ip.c_str());
			if( connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) SOCKETERRORCOM("connect()", return false);
			return true;
		}

		bool Send(char* msg, unsigned int size)
		{
			if(send(sock,msg,size,0) < 0) SOCKETERRORCOM("send()", return false); return true;
		}
		
		bool Read(char* msg, unsigned int size)
		{
			if(read(sock,msg,size) < 0) SOCKETERRORCOM("read()", return false); return true;
		}

		void Close()
		{
			shutdown(sock, SHUT_RDWR);
			close(sock);
		}
};
