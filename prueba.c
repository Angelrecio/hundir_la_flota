#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>

#define MAX_DIMENSION 10
#define MAX_SHOTS 100
#define MAX_PID_LEN 10
#define MAX_COORD_LEN 10
#define MAX_RESULT_LEN 10

typedef struct {
    int x;
    int y;
} Coordenada;

typedef enum {
    AGUA,
    IMPACTO,
    HUNDIDO
} ResultadoDisparo;

typedef struct {
    Coordenada posicion;
    ResultadoDisparo resultado;
} Disparo;

sem_t* semaforo;
int archivoCompartido;

void bloquearArchivoCompartido() {
    sem_wait(semaforo);
}

void desbloquearArchivoCompartido() {
    sem_post(semaforo);
}

void escribirDisparoEnArchivo(const char* jugador, const Coordenada* coordenada, const ResultadoDisparo resultado) {
    bloquearArchivoCompartido();
    char buffer[MAX_PID_LEN + MAX_COORD_LEN + MAX_RESULT_LEN + 6];  // +6 por los separadores y el terminador nulo
    sprintf(buffer, "%s:%d,%d:%d\n", jugador, coordenada->x, coordenada->y, resultado);
    write(archivoCompartido, buffer, strlen(buffer));
    desbloquearArchivoCompartido();
}

Disparo* leerDisparosDelArchivo(const char* jugador, int* contadorDisparos) {
    bloquearArchivoCompartido();

    // Obtener el tamaño del archivo
    off_t tamanoArchivo = lseek(archivoCompartido, 0, SEEK_END);
    lseek(archivoCompartido, 0, SEEK_SET);

    // Asignar memoria para los disparos
    Disparo* disparos = (Disparo*)malloc(MAX_SHOTS * sizeof(Disparo));

    // Leer los disparos del archivo
    char* buffer = (char*)malloc(tamanoArchivo + 1);
    read(archivoCompartido, buffer, tamanoArchivo);
    buffer[tamanoArchivo] = '\0';

    // Analizar los disparos
    char* token = strtok(buffer, "\n");
    while (token != NULL) {
        if (strncmp(token, jugador, strlen(jugador)) == 0) {
            char* tokenCoordenada = strtok(token, ":");
            tokenCoordenada = strtok(NULL, ":");
            sscanf(tokenCoordenada, "%d,%d", &(disparos[*contadorDisparos].posicion.x), &(disparos[*contadorDisparos].posicion.y));

            char* tokenResultado = strtok(NULL, ":");
            sscanf(tokenResultado, "%d", &(disparos[*contadorDisparos].resultado));

            (*contadorDisparos)++;
        }
        token = strtok(NULL, "\n");
    }

    free(buffer);
    desbloquearArchivoCompartido();
    return disparos;
}


void imprimirTablero(const char* nombreJugador, const Disparo* disparos, int contadorDisparos) {
    printf("=== Tablero para el Jugador %s ===\n", nombreJugador);
    char tablero[MAX_DIMENSION][MAX_DIMENSION];

    // Inicializar el tablero con agua
    for (int i = 0; i < MAX_DIMENSION; i++) {
        for (int j = 0; j < MAX_DIMENSION; j++) {
            tablero[i][j] = '~';
        }
    }

    // Actualizar el tablero con los disparos
    for (int i = 0; i < contadorDisparos; i++) {
        const Disparo* disparo = &(disparos[i]);
        if (disparo->resultado == IMPACTO) {
            tablero[disparo->posicion.x][disparo->posicion.y] = 'X';
        } else if (disparo->resultado == HUNDIDO) {
            tablero[disparo->posicion.x][disparo->posicion.y] = 'S';
        }
    }

    // Imprimir el tablero
    printf("   ");
    for (int i = 0; i < MAX_DIMENSION; i++) {
        printf("%c ", 'A' + i);
    }
    printf("\n");
    for (int i = 0; i < MAX_DIMENSION; i++) {
        printf("%2d ", i);
        for (int j = 0; j < MAX_DIMENSION; j++) {
            printf("%c ", tablero[i][j]);
        }
        printf("\n");
    }
}

Coordenada obtenerCoordenadaAleatoria() {
    Coordenada coord;
    coord.x = rand() % MAX_DIMENSION;
    coord.y = rand() % MAX_DIMENSION;
    return coord;
}

ResultadoDisparo verificarResultadoDisparo(const Disparo* disparos, int contadorDisparos, const Coordenada* coordenada) {
    for (int i = 0; i < contadorDisparos; i++) {
        const Disparo* disparo = &(disparos[i]);
        if (disparo->posicion.x == coordenada->x && disparo->posicion.y == coordenada->y) {
            return disparo->resultado;
        }
    }
    return AGUA;
}

Coordenada obtenerSiguienteObjetivo(const Disparo* disparos, int contadorDisparos) {
    Coordenada ultimoDisparo = disparos[contadorDisparos - 1].posicion;
    Coordenada siguienteObjetivo = ultimoDisparo;

    if (verificarResultadoDisparo(disparos, contadorDisparos, &ultimoDisparo) == IMPACTO) {
        // Continuar disparando en la misma dirección
        siguienteObjetivo.x = ultimoDisparo.x + 1;
        siguienteObjetivo.y = ultimoDisparo.y;
        if (siguienteObjetivo.x >= MAX_DIMENSION || verificarResultadoDisparo(disparos, contadorDisparos, &siguienteObjetivo) != AGUA) {
            siguienteObjetivo.x = ultimoDisparo.x;
            siguienteObjetivo.y = ultimoDisparo.y + 1;
            if (siguienteObjetivo.y >= MAX_DIMENSION || verificarResultadoDisparo(disparos, contadorDisparos, &siguienteObjetivo) != AGUA) {
                siguienteObjetivo.x = ultimoDisparo.x - 1;
                siguienteObjetivo.y = ultimoDisparo.y;
                if (siguienteObjetivo.x < 0 || verificarResultadoDisparo(disparos, contadorDisparos, &siguienteObjetivo) != AGUA) {
                    siguienteObjetivo.x = ultimoDisparo.x;
                    siguienteObjetivo.y = ultimoDisparo.y - 1;
                }
            }
        }
    }

    return siguienteObjetivo;
}

void jugarPartida(const char* jugador) {
    srand(time(NULL));

    int contadorDisparos = 0;
    Disparo* disparos = leerDisparosDelArchivo(jugador == "Jugador1" ? "Jugador1" : "Jugador2", &contadorDisparos);

    while (1) {
        Coordenada objetivo;
        if (contadorDisparos > 0 && disparos[contadorDisparos - 1].resultado != AGUA) {
            // Continuar disparando en la misma dirección
            objetivo = obtenerSiguienteObjetivo(disparos, contadorDisparos);
        } else {
            // Disparo aleatorio
            objetivo = obtenerCoordenadaAleatoria();
        }

        // Realizar disparo
        usleep(rand() % 10000001);  // Retardo aleatorio entre 0 y 10 segundos
        ResultadoDisparo resultado = rand() % 2 == 0 ? IMPACTO : AGUA;
        escribirDisparoEnArchivo(jugador, &objetivo, resultado);
        if (resultado == HUNDIDO) {
            break;
        }

        // Verificar los disparos del oponente
        int contadorDisparosOponente = 0;
        Disparo* disparosOponente = leerDisparosDelArchivo(jugador == "Jugador1" ? "Jugador2" : "Jugador1", &contadorDisparosOponente);
        for (int i = 0; i < contadorDisparosOponente; i++) {
            const Disparo* disparoOponente = &(disparosOponente[i]);
            if (verificarResultadoDisparo(disparos, contadorDisparos, &(disparoOponente->posicion)) != AGUA) {
                continue;  // Saltar disparos ya procesados
            }

            // Procesar el disparo del oponente y actualizar el resultado
            if (disparoOponente->resultado == IMPACTO) {
                resultado = IMPACTO;
            } else if (disparoOponente->resultado == HUNDIDO) {
                resultado = HUNDIDO;
                break;
            }
        }
        free(disparosOponente);

        if (resultado == HUNDIDO) {
            break;
        }
    }

    free(disparos);
}

int main() {
    // Crear archivo compartido
    archivoCompartido = open("compartido.txt", O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    if (archivoCompartido == -1) {
        perror("Error al crear el archivo compartido");
        exit(1);
    }

    // Crear semáforo
    semaforo = sem_open("semaforo", O_CREAT, S_IRUSR | S_IWUSR, 1);
    if (semaforo == SEM_FAILED) {
        perror("Error al crear el semáforo");
        exit(1);
    }

    // Crear procesos de jugadores
    pid_t pid1 = fork();
    if (pid1 == 0) {
        jugarPartida("Jugador1");
        exit(0);
    } else if (pid1 > 0) {
        pid_t pid2 = fork();
        if (pid2 == 0) {
            jugarPartida("Jugador2");
            exit(0);
        } else if (pid2 > 0) {
            // Esperar a que los procesos de los jugadores terminen
            int status;
            waitpid(pid1, &status, 0);
            waitpid(pid2, &status, 0);
        } else {
            perror("Error al crear el proceso del Jugador2");
            exit(1);
        }
    } else {
        perror("Error al crear el proceso del Jugador1");
        exit(1);
    }

    // Cerrar archivo compartido y liberar semáforo
    close(archivoCompartido);
    sem_close(semaforo);

    return 0;
}
