#ifndef OMFG_TCP_H
#define OMFG_TCP_H

#include <ctime>

namespace TCP
{
	
struct Socket
{
	static int const bufferSize = 1024;
	
	enum Error
	{
		ErrorNone,
		ErrorConnect,
		ErrorSend,
		ErrorRecv,
		ErrorDisconnect,
		ErrorTimeout,
	};
	
	struct ResumeSend
	{
		ResumeSend(Socket* sock_, char const* b_, char const* e_)
		: sock(sock_), b(b_), e(e_), error(ErrorNone)
		{
		}
		
		Socket* sock;
		char const* b;
		char const* e;
		Error error;
		
		bool resume();
	};
	
	Socket(int s_)
	: s(s_), connected(false), connecting(true)
	, error(ErrorNone)
	{
		resetTimer();
	}
	
	void checkTimeout()
	{
		if(time(0) > t + 10)
			error = ErrorTimeout;
	}
	
	void resetTimer()
	{
		t = time(0);
	}
	
	bool think();
	
	bool readChunk();
	
	ResumeSend* send(char const* b, char const* e);
	
	ResumeSend* send(ResumeSend* r, char const* b, char const* e);
	
	bool trySend(char const*& b, char const* e);
	
	Error getError() { return error; }
	
	~Socket();
	
	void close();
	
protected:
	int s;
	bool connected;
	bool connecting;
	char staticBuffer[bufferSize];
	char const* dataBegin;
	char const* dataEnd;
	Error error;
	time_t t;
};

}

#endif //OMFG_TCP_H
