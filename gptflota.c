 #include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

// Define las constantes para los resultados de los disparos
#define AGUA "AGUA"
#define TOCADO "TOCADO"
#define HUNDIDO "HUNDIDO"

// Define el nombre del archivo de disparos
#define ARCHIVO_DISPAROS "disparos.txt"

// Define el nombre del semáforo
#define SEMAFORO_NAME "/semaforo_disparos"

// Función para generar un número aleatorio en el rango [min, max]
int generar_aleatorio(int min, int max) {
    return min + rand() % (max - min + 1);
}

// Función para escribir en el archivo de disparos con el uso del semáforo
void escribir_disparo(sem_t *semaforo, const char *disparo) {
    sem_wait(semaforo);

    int fd = open(ARCHIVO_DISPAROS, O_WRONLY | O_APPEND);
    if (fd != -1) {
        write(fd, disparo, strlen(disparo));
        write(fd, "\n", 1);
        close(fd);
    }

    sem_post(semaforo);
}

// Función para leer el archivo de disparos y actualizar el estado del tablero
void actualizar_tablero(int pid, const char *resultado_disparo) {
    FILE *archivo = fopen(ARCHIVO_DISPAROS, "r");
    if (archivo != NULL) {
        char linea[100];
        char *token;

        while (fgets(linea, sizeof(linea), archivo) != NULL) {
            token = strtok(linea, ":");
            int pid_disparo = atoi(token);

            if (pid_disparo != pid) {
                // Obtener las coordenadas y el resultado del disparo
                token = strtok(NULL, ":");
                char *coordenadas = token;

                token = strtok(NULL, ":");
                char *resultado = token;

                // Actualizar el estado del tablero según el resultado del disparo
                // ...
            }
        }

        fclose(archivo);
    }
}

// Función para realizar el proceso de disparos
void disparar(int pid, sem_t *semaforo) {
    srand(time(NULL) + pid); // Inicializar la semilla aleatoria

    while (1) {
        // Realizar el disparo aleatorio
        // ...

        // Actualizar el tablero según el resultado del disparo
        // ...

        // Construir la cadena del disparo
        char disparo[100];
        sprintf(disparo, "%d:%d,%d:%s", pid, coordenada_x, coordenada_y, resultado_disparo);

        // Escribir el disparo en el archivo
        escribir_disparo(semaforo, disparo);

        // Esperar un tiempo entre 0 y 10 segundos antes del próximo disparo
        int espera = generar_aleatorio(0, 10);
        sleep(espera);
    }
}

int main() {
    // Crear el archivo de disparos si no existe
    int fd = open(ARCHIVO_DISPAROS, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    close(fd);

    // Crear el semáforo para controlar el acceso al archivo de disparos
    sem_t *semaforo = sem_open(SEMAFORO_NAME, O_CREAT | O_EXCL, 0666, 1);
    if (semaforo == SEM_FAILED) {
        perror("Error al crear el semáforo");
        return 1;
    }

    // Crear el proceso hijo 1
    pid_t pid_hijo1 = fork();
    if (pid_hijo1 == 0) {
        // Código del primer proceso hijo
        disparar(getpid(), semaforo);
        return 0;
    } else if (pid_hijo1 > 0) {
        // Crear el proceso hijo 2
        pid_t pid_hijo2 = fork();
        if (pid_hijo2 == 0) {
            // Código del segundo proceso hijo
            disparar(getpid(), semaforo);
            return 0;
        } else if (pid_hijo2 > 0) {
            // Código del proceso padre
            int status;
            waitpid(pid_hijo1, &status, 0);
            waitpid(pid_hijo2, &status, 0);

            // Obtener la fecha actual para el nombre del archivo de batalla
            time_t t = time(NULL);
            struct tm *tiempo = localtime(&t);
            char nombre_archivo[100];
            sprintf(nombre_archivo, "batalla %d vs %d.%d%d%d_%d%d.txt",
                    pid_hijo1, pid_hijo2,
                    tiempo->tm_year + 1900, tiempo->tm_mon + 1, tiempo->tm_mday,
                    tiempo->tm_hour, tiempo->tm_min);

                        // Copiar el archivo de disparos con el nombre de la batalla
            FILE *archivo_original = fopen(ARCHIVO_DISPAROS, "r");
            FILE *archivo_copia = fopen(nombre_archivo, "w");

            if (archivo_original == NULL || archivo_copia == NULL) {
                perror("Error al abrir los archivos");
                return 1;
            }

            char caracter;
            while ((caracter = fgetc(archivo_original)) != EOF) {
                fputc(caracter, archivo_copia);
            }

            fclose(archivo_original);
            fclose(archivo_copia);

            // Cerrar y eliminar el semáforo
            sem_close(semaforo);
            sem_unlink(SEMAFORO_NAME);

            return 0;
        } else {
            perror("Error al crear el segundo proceso hijo");
            return 1;
        }
    } else {
        perror("Error al crear el primer proceso hijo");
        return 1;
    }
}

