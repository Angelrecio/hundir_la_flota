#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void actualizarDisparoContrincante(const char* archivoDisparos, const char* pidContrincante, const char* resultadoDisparo) {
    FILE* archivo = fopen(archivoDisparos, "r+");
    if (archivo == NULL) {
        perror("Error al abrir el archivo de disparos");
        exit(EXIT_FAILURE);
    }

    char linea[256];
    char lineaActualizada[256];
    size_t longitudPid = strlen(pidContrincante);

    // Leer cada línea del archivo de disparos
    while (fgets(linea, sizeof(linea), archivo)) {
        // Buscar líneas que contengan el PID del contrincante
        if (strncmp(linea, pidContrincante, longitudPid) == 0) {
            // Actualizar el resultado del disparo en la línea correspondiente
            strcpy(lineaActualizada, linea);
            char* resultadoPtr = strstr(lineaActualizada, ":");
            if (resultadoPtr != NULL) {
                resultadoPtr += 1; // Moverse después del ":"
                strncpy(resultadoPtr, resultadoDisparo, strlen(resultadoDisparo));
            }
            fseek(archivo, -strlen(linea), SEEK_CUR); // Retroceder al inicio de la línea
            fputs(lineaActualizada, archivo); // Escribir la línea actualizada
            fflush(archivo);
            break;
        }
    }

    fclose(archivo);
}

int main() {
    // Obtener el PID del proceso actual
    pid_t pid = getpid();

    // Obtener el PID del contrincante
    pid_t pidContrincante = (pid == pidHijo1) ? pidHijo2 : pidHijo1;

    // Obtener el resultado del último disparo del contrincante
    // (supongamos que lo almacenamos en la variable 'resultadoDisparoContrincante')

    // Actualizar el resultado del disparo del contrincante en el archivo de disparos
    actualizarDisparoContrincante("archivo_disparos.txt", pidContrincante, resultadoDisparoContrincante);

    return 0;
}
