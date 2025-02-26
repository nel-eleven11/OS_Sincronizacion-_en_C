#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>  // para sleep()

#define BUFFER_SIZE 5  // Tamaño máximo del buffer

int buffer[BUFFER_SIZE];
int in = 0;   // Índice para el próximo elemento producido
int out = 0;  // Índice para el próximo elemento a consumir

// Semáforos:
// - mutex: para la exclusión mutua al acceder al buffer (inicializado a 1).
// - empty: para controlar los espacios libres (inicializado a BUFFER_SIZE).
// - full: para controlar los espacios ocupados (inicializado a 0).
sem_t mutex;
sem_t empty;
sem_t full;

void* producer(void* arg) {
    int item;
    while (1) {
        // Produce un elemento 
        item = rand() % 100;

        // Espera a que haya un espacio libre en el buffer.
        sem_wait(&empty);
        // Entra en la sección crítica.
        sem_wait(&mutex);

        // Inserta el elemento en el buffer.
        buffer[in] = item;
        printf("Productor produjo %d en la posición %d\n", item, in);
        in = (in + 1) % BUFFER_SIZE;  // Actualiza el índice circularmente

        // Sale de la sección crítica.
        sem_post(&mutex);
        // Indica que hay un nuevo elemento disponible.
        sem_post(&full);

        sleep(1);  // Simula el tiempo de producción
    }
    return NULL;
}

void* consumer(void* arg) {
    int item;
    while (1) {
        // Espera a que haya al menos un elemento en el buffer.
        sem_wait(&full);
        // Entra en la sección crítica.
        sem_wait(&mutex);

        // Extrae el elemento del buffer.
        item = buffer[out];
        printf("Consumidor consumió %d de la posición %d\n", item, out);
        out = (out + 1) % BUFFER_SIZE;  // Actualiza el índice circularmente

        // Sale de la sección crítica.
        sem_post(&mutex);
        // Indica que se ha liberado un espacio en el buffer.
        sem_post(&empty);

        sleep(3);  // Simula el tiempo de consumo
    }
    return NULL;
}

int main() {
    pthread_t prodThread, consThread;

    // Inicialización de los semáforos
    sem_init(&mutex, 0, 1);           // Exclusión mutua
    sem_init(&empty, 0, BUFFER_SIZE); // Espacios libres disponibles
    sem_init(&full, 0, 0);            // No hay elementos inicialmente

    // Creación de los hilos productor y consumidor
    pthread_create(&prodThread, NULL, producer, NULL);
    pthread_create(&consThread, NULL, consumer, NULL);

    // Espera a que los hilos finalicen (en este ejemplo, se ejecutan indefinidamente)
    pthread_join(prodThread, NULL);
    pthread_join(consThread, NULL);

    // Destrucción de los semáforos (nunca se alcanza en este ejemplo, pero es buena práctica)
    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}
