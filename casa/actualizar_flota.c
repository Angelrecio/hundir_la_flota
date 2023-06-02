#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void actualizarFlotaPropia(const char* archivoDisparosPropios, const char* resultadoDisparo) {
    FILE* archivo = fopen(archivoDisparosPropios, "a");
    if (archivo == NULL) {
        perror("Error al abrir el archivo de disparos propios");
        exit(EXIT_FAILURE);
    }

    // Bloquear el archivo para escritura
    flock(fileno(archivo), LOCK_EX);

    // Obtener la última línea del archivo
    fseek(archivo, 0, SEEK_END);
    long posicion = ftell(archivo);
    char ultimaLinea[100];
    do {
        if (posicion <= 0) {
            // No se encontró una línea anterior
            break;
        }
        fseek(archivo, --posicion, SEEK_SET);
        if (fgetc(archivo) == '\n') {
            fgets(ultimaLinea, sizeof(ultimaLinea), archivo);
            break;
        }
    } while (posicion > 0);
    
    // Actualizar el resultado del último disparo en el archivo
    if (strlen(ultimaLinea) > 0) {
        char* token = strtok(ultimaLinea, ":");
        fprintf(archivo, "%s:%s\n", token, resultadoDisparo);
    }

    // Liberar el archivo
    flock(fileno(archivo), LOCK_UN);

    fclose(archivo);
}

void actualizarFlotaContrincante(const char* archivoDisparosContrincante) {
    FILE* archivo = fopen(archivoDisparosContrincante, "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo de disparos del contrincante");
        exit(EXIT_FAILURE);
    }

    // Leer el archivo línea por línea
    char linea[100];
    while (fgets(linea, sizeof(linea), archivo)) {
        // Procesar cada línea para obtener el resultado del disparo
        char* token = strtok(linea, ":");
        char* coordenadas = strtok(NULL, ":");
        char* resultado = strtok(NULL, ":");
        
        // Actualizar el estado de la flota propia según el resultado del disparo
        // ...
    }

    fclose(archivo);
}

int main() {
    // Obtener el PID del proceso actual
    pid_t pid = getpid();

    // Obtener el PID del contrincante
    pid_t pidContrincante = (pid == pidHijo1) ? pidHijo2 : pidHijo1;

    // Obtener el resultado del último disparo propio (supongamos que lo almacenamos en la variable 'resultadoDisparo')

    // Actualizar el estado de la flota propia según el resultado del disparo
    actualizarFlotaPropia("archivo_disparos_propios.txt", resultadoDisparo);

    // Actualizar el estado de la flota contrincante según los disparos registrados en el archivo
    actualizarFlotaContrincante("archivo_disparos_contrincante.txt");

    return 0;
}
