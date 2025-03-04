#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5  // Capacidad máxima del buffer

int buffer[BUFFER_SIZE];
int in = 0;   // Índice para la próxima producción
int out = 0;  // Índice para la próxima consumición

// Semáforos para la sincronización
sem_t mutex;  // Exclusión mutua (inicializado a 1)
sem_t empty;  // Espacios libres (inicializado a BUFFER_SIZE)
sem_t full;   // Espacios ocupados (inicializado a 0)

// Función que imprime el estado actual del buffer.
// E imprime dichos elementos en orden.
void print_buffer() {
    printf("Estado del buffer: ");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        int index = (i) % BUFFER_SIZE;
        printf("%d ", buffer[index]);
    }
    printf("\n");
}

void* producer(void* arg) {
    int item;
    while (1) {
        // Genera un elemento aleatorio
        item = rand() % 100;

        // Espera hasta que haya un espacio libre
        sem_wait(&empty);
        // Entra en la sección crítica
        sem_wait(&mutex);

        // Inserta el elemento en el buffer y actualiza el índice
        buffer[in] = item;
        printf("--- Productor produjo %d en la posición %d\n", item, in);
        in = (in + 1) % BUFFER_SIZE;
        
        // Imprime el estado actual del buffer
        print_buffer();

        // Sale de la sección crítica y notifica que hay un nuevo elemento
        sem_post(&mutex);
        sem_post(&full);

        sleep(1);  // Simula el tiempo de producción
    }
    return NULL;
}

void* consumer(void* arg) {
    int item;
    while (1) {
        // Espera hasta que haya al menos un elemento en el buffer
        sem_wait(&full);
        // Entra en la sección crítica
        sem_wait(&mutex);

        // Extrae el elemento del buffer y actualiza el índice
        item = buffer[out];
        printf(">>Consumidor consumió %d de la posición %d\n", item, out);
        buffer[out] = 0;  // Marcamos la posición como vacía (asumiendo que los elementos son >= 0)
        out = (out + 1) % BUFFER_SIZE;
        
        // Imprime el estado actual del buffer
        print_buffer();

        // Sale de la sección crítica y notifica que se liberó un espacio
        sem_post(&mutex);
        sem_post(&empty);

        sleep(5);  // Simula el tiempo de consumo
    }
    return NULL;
}

int main() {
    pthread_t prodThread, consThread;

    // Inicialización de los semáforos
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

    // Creación de los hilos productor y consumidor
    pthread_create(&prodThread, NULL, producer, NULL);
    pthread_create(&consThread, NULL, consumer, NULL);

    // Espera a que los hilos terminen (en este ejemplo se ejecutan indefinidamente)
    pthread_join(prodThread, NULL);
    pthread_join(consThread, NULL);

    // Destrucción de los semáforos (nunca se alcanza en este ejemplo)
    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}
