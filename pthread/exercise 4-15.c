
#include <pthread.h>
#include <stdio.h>

/*
Consider the following code segment:
a. How many unique processes are created?
b. How many unique threads are created?
*/
int main()
{
	pid t pid;
	pid = fork();
	if (pid == 0) { /* child process */
		fork();
		pthread_create( . . .);
	}
	fork();
}