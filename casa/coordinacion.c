#include <stdio.h>

#define MAX_BARCOS 10  // Número máximo de barcos en el tablero
#define MAX_DIMENSION 10  // Tamaño máximo del tablero

typedef struct {
    int x;
    int y;
} Coordenada;

typedef struct {
    char tipo[20];
    Coordenada posiciones[MAX_BARCOS];
    int num_barcos;
} Tablero;

int cargarTableros(Tablero* jugador1, Tablero* jugador2, const char* archivo1, const char* archivo2) {
    FILE* file1 = fopen(archivo1, "r");
    FILE* file2 = fopen(archivo2, "r");

    if (file1 == NULL || file2 == NULL) {
        perror("Error al abrir los archivos de configuración");
        return -1;
    }

    // Cargar tablero del jugador 1
    fscanf(file1, "%d", &(jugador1->num_barcos));
    for (int i = 0; i < jugador1->num_barcos; i++) {
        fscanf(file1, "%s %d %d", jugador1->tipo, &(jugador1->posiciones[i].x), &(jugador1->posiciones[i].y));
    }

    // Cargar tablero del jugador 2
    fscanf(file2, "%d", &(jugador2->num_barcos));
    for (int i = 0; i < jugador2->num_barcos; i++) {
        fscanf(file2, "%s %d %d", jugador2->tipo, &(jugador2->posiciones[i].x), &(jugador2->posiciones[i].y));
    }

    fclose(file1);
    fclose(file2);

    return 0;
}

int coordinacion_main() {
    Tablero jugador1, jugador2;

    if (cargarTableros(&jugador1, &jugador2, "tablero1.txt", "tablero2.txt") == -1) {
        printf("Error al cargar los tableros.\n");
        return 1;
    }

    // Aquí puedes realizar las acciones necesarias con los tableros cargados
    // Por ejemplo, imprimir las posiciones de los barcos para verificar que se hayan cargado correctamente
    printf("Tablero del jugador 1:\n");
    for (int i = 0; i < jugador1.num_barcos; i++) {
        printf("Barco %d: Tipo: %s - Posición: (%d, %d)\n", i + 1, jugador1.tipo, jugador1.posiciones[i].x, jugador1.posiciones[i].y);
    }

    printf("\nTablero del jugador 2:\n");
    for (int i = 0; i < jugador2.num_barcos; i++) {
        printf("Barco %d: Tipo: %s - Posición: (%d, %d)\n", i + 1, jugador2.tipo, jugador2.posiciones[i].x, jugador2.posiciones[i].y);
    }

    return 0;
}
