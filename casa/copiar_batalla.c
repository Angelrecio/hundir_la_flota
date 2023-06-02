#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define MAX_FILENAME_SIZE 100

void crearCopiaBatalla(int pidHijo1, int pidHijo2) {
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    char filename[MAX_FILENAME_SIZE];
    
    snprintf(filename, MAX_FILENAME_SIZE, "batalla_%d_vs_%d.%04d%02d%02d_%02d%02d.txt",
             pidHijo1, pidHijo2, timeinfo->tm_year + 1900, timeinfo->tm_mon + 1,
             timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min);
    
    FILE* archivoBatalla = fopen(filename, "w");
    if (archivoBatalla == NULL) {
        printf("Error al crear el archivo de la batalla.\n");
        exit(1);
    }
    
    // Copiar el contenido del archivo de disparos a la nueva batalla
    FILE* archivoDisparos = fopen("disparos.txt", "r");
    if (archivoDisparos == NULL) {
        printf("Error al abrir el archivo de disparos.\n");
        fclose(archivoBatalla);
        exit(1);
    }
    
    char buffer[MAX_BUFFER_SIZE];
    while (fgets(buffer, MAX_BUFFER_SIZE, archivoDisparos) != NULL) {
        fputs(buffer, archivoBatalla);
    }
    
    fclose(archivoDisparos);
    fclose(archivoBatalla);
}

int main() {
    int pidHijo1 = /* PID del primer hijo */;
    int pidHijo2 = /* PID del segundo hijo */;
    
    // Esperar a que ambos hijos terminen su ejecución
    wait(NULL);
    wait(NULL);
    
    // Crear una copia del archivo de la batalla
    crearCopiaBatalla(pidHijo1, pidHijo2);
    
    return 0;
}
