#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER_SIZE 100

void finalizarJuego(const char* resultado) {
    FILE* archivoDisparos = fopen("disparos.txt", "a");
    if (archivoDisparos == NULL) {
        printf("Error al abrir el archivo de disparos.\n");
        exit(1);
    }
    
    fprintf(archivoDisparos, "%d:%s\n", getpid(), resultado);
    fclose(archivoDisparos);
    exit(0);
}

int main() {
    FILE* archivoDisparos = fopen("disparos.txt", "r");
    if (archivoDisparos == NULL) {
        printf("Error al abrir el archivo de disparos.\n");
        exit(1);
    }
    
    char buffer[MAX_BUFFER_SIZE];
    char resultado[MAX_BUFFER_SIZE];
    snprintf(resultado, MAX_BUFFER_SIZE, "%d:%s", getpid(), "WINS!");
    
    // Verificar si el contrincante ha perdido
    while (fgets(buffer, MAX_BUFFER_SIZE, archivoDisparos) != NULL) {
        if (strstr(buffer, "GAME OVER") != NULL) {
            fclose(archivoDisparos);
            finalizarJuego(resultado);
        }
    }
    
    fclose(archivoDisparos);
    
    // Verificar si la flota propia ha sido destruida
    FILE* archivoFlota = fopen("archivo_disparos.txt", "r");
    if (archivoFlota == NULL) {
        printf("Error al abrir el archivo de la flota.\n");
        exit(1);
    }
    
    while (fgets(buffer, MAX_BUFFER_SIZE, archivoFlota) != NULL) {
        if (strstr(buffer, "TOCADO") != NULL || strstr(buffer, "AGUA") == NULL) {
            fclose(archivoFlota);
            finalizarJuego("GAME OVER");
        }
    }
    
    fclose(archivoFlota);
    
    // Si no se cumple ninguna condición de finalización, continúa el juego
    printf("El juego continúa...\n");
    
    return 0;
}
