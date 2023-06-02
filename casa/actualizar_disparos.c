#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

void actualizarDisparo(const char* archivoDisparos, const char* pidPropio, int coordX, int coordY, const char* resultadoDisparo) {
    FILE* archivo = fopen(archivoDisparos, "a");
    if (archivo == NULL) {
        perror("Error al abrir el archivo de disparos");
        exit(EXIT_FAILURE);
    }

    // Bloquear el archivo para escritura
    flock(fileno(archivo), LOCK_EX);

    // Actualizar el archivo con el resultado del disparo
    fprintf(archivo, "%s:%d,%d:%s\n", pidPropio, coordX, coordY, resultadoDisparo);

    // Liberar el archivo
    flock(fileno(archivo), LOCK_UN);

    fclose(archivo);
}

int main() {
    // Obtener el PID del proceso actual
    pid_t pid = getpid();

    // Obtener el PID del contrincante
    pid_t pidContrincante = (pid == pidHijo1) ? pidHijo2 : pidHijo1;

    // Generar coordenadas aleatorias para el disparo (supongamos que las almacenamos en las variables 'coordX' y 'coordY')

    // Generar resultado aleatorio del disparo (supongamos que lo almacenamos en la variable 'resultadoDisparo')

    // Actualizar el estado del disparo en el archivo de intercambio
    actualizarDisparo("archivo_disparos.txt", pid, coordX, coordY, resultadoDisparo);

    return 0;
}
