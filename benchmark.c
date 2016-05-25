#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define NUM_THREADS 8
#define NUM_RUNTIME 6
 
 
void *thr_func(void *arg) {
  for (int i = 0; i < NUM_RUNTIME; i++) {
    int arg1 = rand() % 255;
    sleep(arg1 % 3);
    char cmdbuff[1024];
    sprintf(cmdbuff, "./client.out 192.168.56.50 8888  %d %d %d", arg1, arg1, arg1);
    fprintf(stdout, "%s\n", cmdbuff);
    system(cmdbuff);
 }
  pthread_exit(NULL);
}
 
int main(int argc, char **argv) {
  pthread_t thr[NUM_THREADS];
  int i, rc;
  srand(time(NULL));
  fprintf(stdout, "NUM_THREADS %d NUM_RUNTIME %d\n", NUM_THREADS, NUM_RUNTIME);
  for (i = 0; i < NUM_THREADS; ++i) {
    if (rc = pthread_create(&thr[i], NULL, thr_func, NULL)) {
      fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
      return EXIT_FAILURE;
    }
  }
  /* block until all threads complete */
  for (i = 0; i < NUM_THREADS; ++i) {
    pthread_join(thr[i], NULL);
  }
  fprintf(stdout, "\nDONE\n");
  return EXIT_SUCCESS;
}