#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <semaphore.h>

#define FILENAME "disparos.txt"

// Función para generar un número aleatorio entre min y max
int generarAleatorio(int min, int max) {
    return min + rand() % (max - min + 1);
}

int main() {
    // Código para crear y configurar el semáforo
    sem_t *sem;
    sem = sem_open("/disparos_sem", O_CREAT | O_EXCL, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("Error al crear el semáforo");
        exit(1);
    }

    // Código para crear y configurar el archivo de disparos
    int fd;
    fd = open(FILENAME, O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if (fd == -1) {
        perror("Error al abrir el archivo de disparos");
        exit(1);
    }
    close(fd);

    // Código para crear los procesos hijos y realizar los disparos
    pid_t pid;
    srand(time(NULL));

    for (int i = 0; i < 2; i++) {
        pid = fork();

        if (pid == -1) {
            perror("Error al crear el proceso hijo");
            exit(1);
        } else if (pid == 0) {
            // Código para el proceso hijo

            // Lógica de los disparos
            for (int j = 0; j < num_disparos; j++) {
                // Generar las coordenadas aleatorias del disparo
                int coordenada_x = generarAleatorio(0, tamano_tablero_x - 1);
                int coordenada_y = generarAleatorio(0, tamano_tablero_y - 1);

                // Esperar un tiempo aleatorio antes de disparar
                int tiempo_espera = generarAleatorio(0, 10);
                sleep(tiempo_espera);

                // Bloquear el semáforo antes de escribir en el archivo
                sem_wait(sem);

                // Escribir el disparo en el archivo
                fd = open(FILENAME, O_WRONLY | O_APPEND);
                if (fd == -1) {
                    perror("Error al abrir el archivo de disparos");
                    exit(1);
                }

                char resultado[100];
                sprintf(resultado, "Jugador%d: %d,%d\n", i + 1, coordenada_x, coordenada_y);
                write(fd, resultado, strlen(resultado));
                close(fd);

                // Desbloquear el semáforo después de escribir en el archivo
                sem_post(sem);
            }

            exit(0);
        }
    }

    // Código para esperar a que los procesos hijos terminen
    for (int i = 0; i < 2; i++) {
        wait(NULL);
    }

    // Cerrar y eliminar el semáforo
    sem_close(sem);
    sem_unlink("/disparos_sem");

    return 0;
}
