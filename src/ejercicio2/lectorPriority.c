#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

sem_t wrt;       // Semáforo para escritores
pthread_mutex_t mutex; // Mutex para proteger read_count
int read_count = 0;

void *reader(void *arg) {
    pthread_mutex_lock(&mutex);
    read_count++;
    if (read_count == 1) {
        sem_wait(&wrt); // Bloquear escritores si es el primer lector
    }
    pthread_mutex_unlock(&mutex);

    // Lectura del recurso
    printf("Lector %lu leyendo...\n", (unsigned long)pthread_self());

    pthread_mutex_lock(&mutex);
    read_count--;
    if (read_count == 0) {
        sem_post(&wrt); // Liberar escritores si es el último lector
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void *writer(void *arg) {
    sem_wait(&wrt); // Esperar acceso exclusivo
    // Escritura del recurso
    printf("Escritor %lu escribiendo...\n", (unsigned long)pthread_self());
    sem_post(&wrt); // Liberar el recurso
    return NULL;
}

int main() {
    sem_init(&wrt, 0, 1);
    pthread_mutex_init(&mutex, NULL);

    pthread_t r1, r2, w1, w2;
    pthread_create(&r1, NULL, reader, NULL);
    pthread_create(&w1, NULL, writer, NULL);
    pthread_create(&r2, NULL, reader, NULL);
    pthread_create(&w2, NULL, writer, NULL);

    pthread_join(r1, NULL);
    pthread_join(w1, NULL);
    pthread_join(r2, NULL);
    pthread_join(w2, NULL);

    sem_destroy(&wrt);
    pthread_mutex_destroy(&mutex);
    return 0;
}