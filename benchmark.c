#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 30
 
 
void *thr_func(void *arg) {
  // thread_data_t *data = (thread_data_t *)arg;
  char arg1 = rand() %255;
  char cmbuff[1024];
  // sprintf(cmbuff, "./client.out 192.168.56.50 8888  %c", arg1);
  sprintf(cmbuff, "./client.out 127.0.0.1 8888  %c %d", arg1, arg1);
  // sprintf(cmbuff, "./client.out 192.168.56.50 8888  %c %d", arg1, arg1);
  system(cmbuff);
 
  pthread_exit(NULL);
}
 
int main(int argc, char **argv) {
  pthread_t thr[NUM_THREADS];
  int i, rc;
  srand(time(NULL));
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