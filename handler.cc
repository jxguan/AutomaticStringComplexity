#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

typedef void (*handler_t)(int);

handler_t installSignalHandler(int signum, handler_t handler) {
	struct sigaction action, old_action;  
	action.sa_handler = handler;  
	sigemptyset(&action.sa_mask); // block sigs of type being handled
	action.sa_flags = SA_RESTART; // restart syscalls if possible  
	sigaction(signum, &action, &old_action);
	return old_action.sa_handler;
}