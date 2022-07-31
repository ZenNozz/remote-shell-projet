#include "headers/client.h"
#include "headers/server.h"
#include<stdio.h>
#include<stdlib.h>


int main(int argc, char* argv[]) {
    char* command = cmd();
    printf("%s", command);
    char* shellReturn = doCommand(command);
    printf("%s", shellReturn);
    return EXIT_SUCCESS;
}