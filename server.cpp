#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <strings.h>
#include <stdlib.h>
#include <string>
#include <pthread.h>
#include <sys/errno.h>
using namespace std;

void *task1(void *);

//static int connFd;

int create_skfd()
{
	int pId, portNo, listenFd;
	int ret;
	socklen_t len; //store size of the address
	bool loop = false;
	struct sockaddr_in svrAdd, clntAdd;   
	portNo = 5020;

	cout << "create socket..." << endl;
	listenFd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(listenFd < 0)
	{
		 cerr << "Cannot open socket" << endl;
		 return -1;
	}
	cout << "set socket reuse" << endl;
	int on = 1;
	ret = setsockopt (listenFd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));
	if (ret < 0)
	{
		cerr << "Cannot set socket reuse." << endl;
		return -1;	  
	}

	bzero((char*) &svrAdd, sizeof(svrAdd));
	svrAdd.sin_family = AF_INET;
	svrAdd.sin_addr.s_addr = INADDR_ANY;
	svrAdd.sin_port = htons(portNo);
	
	cout << "bind socket..." << endl;
	//bind socket
	if(bind(listenFd, (struct sockaddr *)&svrAdd, sizeof(svrAdd)) < 0)
	{
		 cerr << "Cannot bind" << endl;
		 return -1;
	}
	cout << "listen socket..." << endl;
	ret = listen(listenFd, 5);
	if (ret < 0)
	{
		cerr << "listen fail" << endl;
		return -1;	  
	}
	return listenFd;
}

void *server_thread (void* ptr)
{
	int skfd;
	int rskfd;
	socklen_t len; //store size of the address
	struct sockaddr_in svrAdd, clntAdd;
	pthread_t m_thread;

	pthread_detach (pthread_self ());

	skfd = create_skfd ();
	if (skfd < 0) 
	{
		cerr << "create socket fail" << endl;
		//return;
		pthread_exit ( (void*)0);
	}
	cout << "create socket success..." << endl;
	while (true)
	{
		cout << "wait accept..." << endl;
		memset(&clntAdd, 0, sizeof(sockaddr_in));
		len = 0;
		rskfd = accept(skfd, (struct sockaddr *)&clntAdd, &len);
		if (rskfd < 0)
		{
			 cerr << "Cannot accept connection: " << strerror (errno) << endl;
			 pthread_exit ( (void*)0);
		}
		cout << "accept success..." << endl;
		int *arg = (int *)malloc (sizeof (*arg));
		if ( arg == NULL ) {
			 fprintf(stderr, "Couldn't allocate memory for thread arg.\n");
			 exit(EXIT_FAILURE);
		}

		*arg = rskfd;
		pthread_create(&m_thread, NULL, task1, arg);
	}
}

int main(int argc, char* argv[])
{ 
   pthread_t m_thread;
   cout << "create server thread" << endl;
	pthread_create(&m_thread, NULL, server_thread, NULL);
	while (true)
	{
		sleep(1);
	}
   return 0;
}

void *task1 (void *rskfd)
{
	pthread_detach (pthread_self ());
	 cout << "Thread No: " << pthread_self() << endl;
    char test[300];
    bzero(test, 301);
	 bool loop = false;

	 int connFd =*((int *) rskfd);
	 free(rskfd);
    while(!loop)
    {    
        bzero(test, 301);
        
        
        read(connFd, test, 300);
        
        string tester (test);
        cout << tester << endl;
        
        
        if(tester == "exit")
            break;
    }
    cout << "\nClosing thread and conn" << endl;
	 close(connFd);
	 pthread_exit ( (void*)0);
}
