#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    int num_barcos;
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
            for (int j = 0; j < num_barcos; j++) {
                int x, y;
                fscanf(file, "%d %d", &x, &y);
                tipoBarco.posiciones[j].x = x;
                tipoBarco.posiciones[j].y = y;
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
            printf("Posición: %d, %d\n", coordenada.x, coordenada.y);
        }
        printf("\n");
    }
}

int main() {
    Tablero tablero1, tablero2;

    cargarTablero("tablero1.txt", &tablero1);
    cargarTablero("tablero2.txt", &tablero2);

    imprimirTablero(&tablero1);
    imprimirTablero(&tablero2);

    return 0;
}
