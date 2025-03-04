#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 5  

typedef enum {PENSANDO, HAMBRIENTO, COMIENDO} Estado;

Estado estados[N];
pthread_mutex_t mutex; 
sem_t tenedores[N];   

void pensar(int id) {
    printf("Filósofo %d está pensando...\n", id);
    sleep(1);
}

void comer(int id) {
    printf("Filósofo %d está comiendo...\n", id);
    sleep(2);
}

void tomar_tenedores(int id) {
    pthread_mutex_lock(&mutex);
    estados[id] = HAMBRIENTO;

    if (estados[id] == HAMBRIENTO &&
        estados[(id + N - 1) % N] != COMIENDO &&
        estados[(id + 1) % N] != COMIENDO) {
        estados[id] = COMIENDO;
        sem_post(&tenedores[id]); 
    }
    pthread_mutex_unlock(&mutex);
    sem_wait(&tenedores[id]); 
}

void dejar_tenedores(int id) {
    pthread_mutex_lock(&mutex);
    estados[id] = PENSANDO;

    int left = (id + N - 1) % N;
    if (estados[left] == HAMBRIENTO &&
        estados[(left + N - 1) % N] != COMIENDO &&
        estados[(left + 1) % N] != COMIENDO) {
        estados[left] = COMIENDO;
        sem_post(&tenedores[left]);
    }

    int right = (id + 1) % N;
    if (estados[right] == HAMBRIENTO &&
        estados[(right + N - 1) % N] != COMIENDO &&
        estados[(right + 1) % N] != COMIENDO) {
        estados[right] = COMIENDO;
        sem_post(&tenedores[right]);
    }

    pthread_mutex_unlock(&mutex);
}

void *filosofo(void *num) {
    int id = *(int *)num;

    while (1) {
        pensar(id);
        tomar_tenedores(id);
        comer(id);
        dejar_tenedores(id);
    }
}

int main() {
    pthread_t hilos[N];
    int ids[N];

    pthread_mutex_init(&mutex, NULL);
    for (int i = 0; i < N; i++) {
        sem_init(&tenedores[i], 0, 0); 
        estados[i] = PENSANDO; 
    }

    for (int i = 0; i < N; i++) {
        ids[i] = i;
        pthread_create(&hilos[i], NULL, filosofo, &ids[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(hilos[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    for (int i = 0; i < N; i++) {
        sem_destroy(&tenedores[i]);
    }

    return 0;
}
