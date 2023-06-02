#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_DIMENSION 10
#define MIN_DIMENSION 5
#define MAX_BARCOS 5

typedef struct {
    int x;
    int y;
} Coordenada;

typedef struct {
    char tipo[20];
    Coordenada posiciones[MAX_BARCOS];
    Coordenada posiciones_finales[MAX_BARCOS];
    int num_barcos;
    int num_barcos_hundidos; // Número de barcos hundidos
} TipoBarco;

typedef struct {
    TipoBarco tipos[MAX_BARCOS];
    int num_tipos;
    int dimensionX;
    int dimensionY;
} Tablero;

void cargarTablero(const char* archivo, Tablero* tablero) {
    FILE* file = fopen(archivo, "r");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return;
    }

    int dimensionX, dimensionY;
    fscanf(file, "%d %d", &dimensionX, &dimensionY);
    tablero->num_tipos = 0;
    tablero->dimensionX = dimensionX;
    tablero->dimensionY = dimensionY;

    while (!feof(file)) {
        TipoBarco tipoBarco;
        int num_barcos;
        if (fscanf(file, "%s %d", tipoBarco.tipo, &num_barcos) != 2)
            break;

        if (num_barcos > 0) {
            tipoBarco.num_barcos = num_barcos;
            tipoBarco.num_barcos_hundidos = 0; // Inicializar el contador de barcos hundidos
            for (int j = 0; j < num_barcos; j++) {
                int x1, y1, x2, y2;
                fscanf(file, "%d %d, %d %d", &x1, &y1, &x2, &y2);
                tipoBarco.posiciones[j].x = x1;
                tipoBarco.posiciones[j].y = y1;
                tipoBarco.posiciones_finales[j].x = x2;
                tipoBarco.posiciones_finales[j].y = y2;
            }
            tablero->tipos[tablero->num_tipos] = tipoBarco;
            tablero->num_tipos++;
        }
    }

    fclose(file);
}

void imprimirTablero(const Tablero* tablero) {
    printf("Dimensiones del tablero: %d x %d\n", tablero->dimensionX, tablero->dimensionY);

    for (int i = 0; i < tablero->num_tipos; i++) {
        TipoBarco tipoBarco = tablero->tipos[i];
        printf("Barco: %s\n", tipoBarco.tipo);
        for (int j = 0; j < tipoBarco.num_barcos; j++) {
            Coordenada coordenada = tipoBarco.posiciones[j];
            Coordenada coordenada_final = tipoBarco.posiciones_finales[j];
            printf("Posición inicial: %d, %d\n", coordenada.x, coordenada.y);
            printf("Posición final: %d, %d\n", coordenada_final.x, coordenada_final.y);
        }
        printf("\n");
    }
}

void atacante(int jugador, const Tablero* miTablero, const Tablero* tableroOponente) {
    srand(time(NULL) + jugador);  // Inicializar la semilla para generar números aleatorios

    int oponente = (jugador == 1) ? 2 : 1;  // Determinar el número del oponente

    int ultima_direccion = 0;
    Coordenada ultima_coordenada;
    ultima_coordenada.x = 0;
    ultima_coordenada.y = 0;

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

        TipoBarco* tipoBarco = NULL;

        for (int i = 0; i < tableroOponente->num_tipos; i++) {
            TipoBarco* tipo = &tableroOponente->tipos[i];
            for (int j = 0; j < tipo->num_barcos; j++) {
                Coordenada coordenada_inicial = tipo->posiciones[j];
                Coordenada coordenada_final = tipo->posiciones_finales[j];

                if ((coordenada.x >= coordenada_inicial.x && coordenada.x <= coordenada_final.x) &&
                    (coordenada.y >= coordenada_inicial.y && coordenada.y <= coordenada_final.y)) {
                    tipoBarco = tipo;
                    tipoBarco->num_barcos_hundidos++;
                    if (tipoBarco->num_barcos_hundidos == tipoBarco->num_barcos) {
                        barco_hundido = 1;
                        printf("¡El jugador %d ha hundido un barco del jugador %d!\n", jugador, oponente);
                    }
                    break;
                }
            }
            if (tipoBarco) {
                break;
            }
        }

        if (tipoBarco) {
            printf("¡El jugador %d ha acertado en un barco del jugador %d! Coordenadas: %d, %d\n", jugador, oponente, coordenada.x, coordenada.y);
            ultima_direccion = 0; // Reiniciar la dirección
        } else {
            printf("El jugador %d ha disparado al agua. Coordenadas: %d, %d\n", jugador, coordenada.x, coordenada.y);
            if (disparo_aleatorio) {
                ultima_direccion = 0; // Reiniciar la dirección si el disparo fue aleatorio
            }
        }

        ultima_coordenada = coordenada;

        // Esperar un tiempo aleatorio entre 0 y 10 segundos antes de realizar el siguiente disparo
        int tiempoEspera = rand() % 11;
        sleep(tiempoEspera);
    }
}

int main() {
    Tablero tablero1, tablero2;

    cargarTablero("tablero1.txt", &tablero1);
    cargarTablero("tablero2.txt", &tablero2);

    imprimirTablero(&tablero1);
    imprimirTablero(&tablero2);

    // Crear un proceso hijo para el jugador 1
    pid_t pidtablero1 = fork();

    if (pidtablero1 == 0) {
        // Proceso hijo para el jugador 1
        atacante(1, &tablero1, &tablero2);
        exit(0);
    }

    // Crear un proceso hijo para el jugador 2
    pid_t pidJugador2 = fork();

    if (pidJugador2 == 0) {
        // Proceso hijo para el jugador 2
        atacante(2, &tablero2, &tablero1);
        exit(0);
    }

    // Esperar a que ambos procesos hijos terminen
    waitpid(pidtablero1, NULL, 0);
    waitpid(pidJugador2, NULL, 0);

    return 0;
}
