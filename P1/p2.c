#include<signal.h>
#include<stdlib.h>

int main(int argc, char* argv[]){
	int signal=atoi(argv[2]);
	int pid=atoi(argv[1]);
	kill(pid,signal);
	return 0;
}
