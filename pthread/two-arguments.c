#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

struct arg_struct {
    int arg1;
    int arg2;
};

void *print_the_arguments(void *arguments)
{
    struct arg_struct *args = (struct arg_struct *)arguments;
    printf("%d\n", args -> arg1);
    printf("%d\n", args -> arg2);
    pthread_exit(NULL);
    return NULL;
}

int main()
{
    pthread_t some_thread;
    struct arg_struct args;
    args.arg1 = 10;
    args.arg2 = 70;

    if (pthread_create(&some_thread, NULL, &print_the_arguments, (void *)&args) != 0) {
        printf("Uh-oh!\n");
        return -1;
    }

    return pthread_join(some_thread, NULL); /* Wait until thread is finished */
}