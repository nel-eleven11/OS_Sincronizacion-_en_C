#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

sem_t wrt, read_try; // Semáforos para escritores y control de lectores
pthread_mutex_t mutex_read, mutex_write;
int read_count = 0, write_count = 0;

void *reader(void *arg) {
    sem_wait(&read_try); // Bloquear si hay escritores esperando
    sem_post(&read_try); // Liberar para otros lectores

    pthread_mutex_lock(&mutex_read);
    read_count++;
    if (read_count == 1) {
        sem_wait(&wrt); // Bloquear escritores si es el primer lector
    }
    pthread_mutex_unlock(&mutex_read);

    // Lectura del recurso
    printf("Lector %lu leyendo...\n", (unsigned long)pthread_self());

    pthread_mutex_lock(&mutex_read);
    read_count--;
    if (read_count == 0) {
        sem_post(&wrt); // Liberar escritores si es el último lector
    }
    pthread_mutex_unlock(&mutex_read);
    return NULL;
}

void *writer(void *arg) {
    pthread_mutex_lock(&mutex_write);
    write_count++;
    if (write_count == 1) {
        sem_wait(&read_try); // Bloquear nuevos lectores
    }
    pthread_mutex_unlock(&mutex_write);

    sem_wait(&wrt); // Esperar acceso exclusivo
    // Escritura del recurso
    printf("Escritor %lu escribiendo...\n", (unsigned long)pthread_self());
    sem_post(&wrt);

    pthread_mutex_lock(&mutex_write);
    write_count--;
    if (write_count == 0) {
        sem_post(&read_try); // Permitir nuevos lectores
    }
    pthread_mutex_unlock(&mutex_write);
    return NULL;
}

int main() {
    sem_init(&wrt, 0, 1);
    sem_init(&read_try, 0, 1);
    pthread_mutex_init(&mutex_read, NULL);
    pthread_mutex_init(&mutex_write, NULL);

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
    sem_destroy(&read_try);
    pthread_mutex_destroy(&mutex_read);
    pthread_mutex_destroy(&mutex_write);
    return 0;
}