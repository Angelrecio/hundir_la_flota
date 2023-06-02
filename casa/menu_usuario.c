#include <stdio.h>
#include <stdlib.h>

void mostrarMenuUsuario() {
    printf("----- Menú de Usuario -----\n");
    printf("1. Configurar juego\n");
    printf("2. Editar archivos de configuración\n");
    printf("3. Compilar código fuente\n");
    printf("4. Lanzar juego\n");
    printf("5. Salir\n");
    printf("---------------------------\n");
    printf("Ingrese su opción: ");
}

int main() {
    int opcion;
    
    do {
        mostrarMenuUsuario();
        scanf("%d", &opcion);
        fflush(stdin);
        
        switch (opcion) {
            case 1:
                // Configurar juego
                break;
                
            case 2:
                // Editar archivos de configuración
                break;
                
            case 3:
                // Compilar código fuente
                break;
                
            case 4:
                // Lanzar juego
                break;
                
            case 5:
                printf("¡Hasta luego!\n");
                break;
                
            default:
                printf("Opción inválida. Intente nuevamente.\n");
                break;
        }
        
        printf("\n");
        
    } while (opcion != 5);
    
    return 0;
}
