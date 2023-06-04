#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>

#define MAX_DIMENSION 10
#define MIN_DIMENSION 5
#define MAX_BARCOS 5

#define AGUA 0
#define TOCADO 1
#define HUNDIDO 2

typedef struct {
    int x;
    int y;
} Coordenada;

typedef struct {
    char tipo[20];
    Coordenada** barcos;
    int num_barcos;
    int num_barcos_hundidos; // Número de barcos hundidos
} Barco;

typedef struct {
    Barco barcos[MAX_BARCOS]; // Array de barcos
    int num_barcos;
    int dimensionX;
    int dimensionY;
} Tablero;

vvoid cargarTablero(const char* archivo, Tablero* tablero) {
    FILE* file = fopen(archivo, "r");
    if (file == NULL) {
        printf("Error al abrir el archivo\n");
        return;
    }

    // ...

    while (!feof(file)) {
        Barco barco;
        int num_barcos;
        if (fscanf(file, "%s %d", barco.tipo, &num_barcos) != 2)
            break;

        // ...

        if (num_barcos > 0) {
            barco.num_barcos = num_barcos;
            barco.num_barcos_hundidos = 0;
            barco.barcos = malloc(num_barcos * sizeof(Coordenada*));

            for (int j = 0; j < num_barcos; j++) {
                int x, y;
                fscanf(file, "%d %d", &x, &y);
                barco.barcos[j] = malloc(MAX_DIMENSION * sizeof(Coordenada));
                int dimension = MAX_DIMENSION;

                barco.barcos[j][0].x = x;
                barco.barcos[j][0].y = y;

                // Leer y almacenar las coordenadas intermedias
                int num_intermedias = 0;
                while (fscanf(file, ", %d %d", &x, &y) == 2) {
                    if (num_intermedias + 1 >= dimension) {
                        // Si se alcanza el límite de coordenadas, aumentar la dimensión
                        dimension *= 2;
                        barco.barcos[j] = realloc(barco.barcos[j], dimension * sizeof(Coordenada));
                    }
                    barco.barcos[j][num_intermedias + 1].x = x;
                    barco.barcos[j][num_intermedias + 1].y = y;
                    num_intermedias++;
                }

                barco.num_barcos_hundidos++;
            }
            tablero->barcos[tablero->num_barcos] = barco;
            tablero->num_barcos++;
        }
        // ...
    }

    // ...

    fclose(file);
}



void imprimirTablero(const Tablero* tablero) {
    printf("Dimensiones del tablero: %d x %d\n", tablero->dimensionX, tablero->dimensionY);

    for (int i = 0; i < tablero->num_barcos; i++) {
        Barco barco = tablero->barcos[i];
        printf("Tipo de barco: %s\n", barco.tipo);
        printf("Número de barcos: %d\n", barco.num_barcos);
        printf("Número de barcos hundidos: %d\n", barco.num_barcos_hundidos);
        printf("Coordenadas de los barcos:\n");
        
            // ...
    for (int j = 0; j < barco.num_barcos; j++) {
        printf("Barco %d: ", j + 1);
        int k = 0;

        while (k < MAX_DIMENSION && barco.barcos[j][k].x != -1) {
            printf("(%d, %d) ", barco.barcos[j][k].x, barco.barcos[j][k].y);
            k++;
        }

        printf("\n");
    }
    // ...

        
        printf("\n");
    }
}


void reiniciarArchivo(const char* archivo) {
    FILE* file = fopen(archivo, "w");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return;
    }
    fclose(file);
}

void escribirDisparo(const char* archivo, const char* mensaje, int pid, int jugador) {
    sem_t* semaforo = sem_open("/disparos_semaphore", O_CREAT, 0666, 1);
    if (semaforo == SEM_FAILED) {
        perror("Error al abrir el semáforo");
        return;
    }

    int fd = open(archivo, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("Error al abrir el archivo de disparos");
        sem_close(semaforo);
        return;
    }

    if (sem_wait(semaforo) == -1) { // Bloquear el semáforo
        perror("Error al bloquear el semáforo");
        close(fd);
        sem_close(semaforo);
        return;
    }

    char buffer[1024];
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);
    if (bytesRead == -1) {
        perror("Error al leer el archivo de disparos");
        sem_post(semaforo); // Desbloquear el semáforo
        sem_close(semaforo);
        close(fd);
        return;
    }

    buffer[bytesRead] = '\0'; // Agregar el terminador de cadena

    lseek(fd, 0, SEEK_SET); // Mover el cursor al inicio del archivo

    char resultado_disparo[256];
    sprintf(resultado_disparo, "PID %d Jugador %d: %s\n", pid, jugador, mensaje);

    ssize_t bytesWritten = write(fd, resultado_disparo, strlen(resultado_disparo));
    if (bytesWritten == -1) {
        perror("Error al escribir en el archivo de disparos");
        sem_post(semaforo); // Desbloquear el semáforo
        sem_close(semaforo);
        close(fd);
        return;
    }

    bytesWritten = write(fd, buffer, strlen(buffer));
    if (bytesWritten == -1) {
        perror("Error al escribir en el archivo de disparos");
        sem_post(semaforo); // Desbloquear el semáforo
        sem_close(semaforo);
        close(fd);
        return;
    }

    sem_post(semaforo); // Desbloquear el semáforo
    sem_close(semaforo);
    close(fd);
}


void atacante(int jugador, const Tablero* miTablero, const Tablero* tableroOponente) {
    srand(time(NULL) + jugador);  // Inicializar la semilla para generar números aleatorios

    int oponente = (jugador == 1) ? 2 : 1;  // Determinar el número del oponente

    int ultima_direccion = 0;
    Coordenada ultima_coordenada;
    ultima_coordenada.x = 0;
    ultima_coordenada.y = 0;
    int pid = getpid();
    
    while (1) {
        int x, y;
        int disparo_aleatorio = 1;
        int barco_hundido = 0;

        if (ultima_direccion == 0 || barco_hundido) {
            // Generar coordenadas de disparo aleatorias
            x = rand() % tableroOponente->dimensionX;
            y = rand() % tableroOponente->dimensionY;
        } else {
            // Disparo en la dirección anterior
            x = ultima_coordenada.x;
            y = ultima_coordenada.y;

            // Determinar la siguiente coordenada según la dirección
            if (ultima_direccion == 1 && x < tableroOponente->dimensionX - 1) {
                x++;
            } else if (ultima_direccion == 2 && y < tableroOponente->dimensionY - 1) {
                y++;
            } else if (ultima_direccion == 3 && x > 0) {
                x--;
            } else if (ultima_direccion == 4 && y > 0) {
                y--;
            } else {
                // Si no es posible seguir en la dirección, disparar aleatoriamente
                x = rand() % tableroOponente->dimensionX;
                y = rand() % tableroOponente->dimensionY;
            }

            disparo_aleatorio = 0;
        }

        Coordenada coordenada;
        coordenada.x = x;
        coordenada.y = y;

        int tipo_barco_hundido = -1;
        int tipo_barco_acertado = -1;

        for (int i = 0; i < tableroOponente->num_barcos; i++) {
            Barco barco = tableroOponente->barcos[i];
            for (int j = 0; j < barco.num_barcos; j++) {
                Coordenada barco_posicion;
                barco_posicion.x = barco.barcos[j]->x;
barco_posicion.y = barco.barcos[j]->y;


                // Comprobar si la coordenada coincide con la posición de un barco
                if (coordenada.x == barco_posicion.x && coordenada.y == barco_posicion.y) {
                    tipo_barco_acertado = i;
                    barco.num_barcos_hundidos++;
                    if (barco.num_barcos_hundidos == barco.num_barcos) {
                        barco_hundido = 1;
                        printf("¡El jugador %d ha hundido un barco del jugador %d!\n", jugador, oponente);
                    }
                    break;
                }
            }

            if (tipo_barco_acertado != -1) {
                break;
            }
        }

        if (tipo_barco_acertado != -1) {
            printf("¡El jugador %d ha acertado en un barco del jugador %d! Coordenadas: %d, %d\n", jugador, oponente, coordenada.x, coordenada.y);
            ultima_direccion = 0; // Reiniciar la dirección
            char mensaje[50];
            snprintf(mensaje, sizeof(mensaje), "%d, %d : fallado", coordenada.x, coordenada.y);
            escribirDisparo("disparos.txt", mensaje, pid, jugador);
        } else {
            printf("El jugador %d ha disparado al agua. Coordenadas: %d, %d\n", jugador, coordenada.x, coordenada.y);
            if (disparo_aleatorio) {
                ultima_direccion = 0; // Reiniciar la dirección si el disparo fue aleatorio
                char mensaje[50];
                sprintf(mensaje, "%d, %d : fallado", coordenada.x, coordenada.y);
                escribirDisparo("disparos.txt", mensaje, pid, jugador);
            }
        }

        ultima_coordenada = coordenada;

        // Esperar un tiempo aleatorio entre 0 y 10 segundos antes de realizar el siguiente disparo
        int tiempoEspera = rand() % 11;
        sleep(tiempoEspera);
    }
}





int main() {
    reiniciarArchivo("disparos.txt");
    Tablero tablero1;
    cargarTablero("tablero1.txt", &tablero1);

    Tablero tablero2;
    cargarTablero("tablero2.txt", &tablero2);

    printf("Tablero del jugador 1:\n");
    imprimirTablero(&tablero1);

    printf("Tablero del jugador 2:\n");
    imprimirTablero(&tablero2);

    pid_t pid1, pid2;
    int status1, status2;

    pid1 = fork();
    if (pid1 == 0) {
        // Proceso del jugador 1 (atacante)
        atacante(1, &tablero1, &tablero2);
        exit(0);
    } else if (pid1 > 0) {
        pid2 = fork();
        if (pid2 == 0) {
            // Proceso del jugador 2 (atacante)
            atacante(2, &tablero2, &tablero1);
            exit(0);
        } else if (pid2 > 0) {
            // Proceso del juego
            waitpid(pid1, &status1, 0);
            waitpid(pid2, &status2, 0);

            if (WIFEXITED(status1)) {
                printf("El jugador 1 ha finalizado\n");
            }
            if (WIFEXITED(status2)) {
                printf("El jugador 2 ha finalizado\n");
            }
        } else {
            perror("Error al crear el proceso del jugador 2");
        }
    } else {
        perror("Error al crear el proceso del jugador 1");
    }

    return 0;
}