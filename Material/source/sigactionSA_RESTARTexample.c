#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int counter = 0;

void handler(int signal) {
  printf("Signal!!!\n");
  if (signal == SIGINT) {
    counter++;
  } else if (signal == SIGUSR1) {
    printf("coutner %d\n", counter);
    counter = 0;
  }
}

void main() {
  struct sigaction sa;
  sa.sa_handler = handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_flags = SA_RESTART;

  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGUSR1, &sa, NULL);
  while (1) {
    char buffer[4096];
    while (1) {
      scanf("%s", &buffer);
      /* if (errno == EINTR) { */
      /*   errno = 0; */
      /*   continue; */
      /* } */
      printf("Output: %s\n", buffer);
    }
  }
}