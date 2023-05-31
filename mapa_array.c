#include<stdio.h>

void mostrar_mapas(char *mapas) {
    FILE *archivo = fopen("mapas.txt", "r");
    
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
    
    char caracter;
    int i = 0;
    while ((caracter = fgetc(archivo)) != EOF) {
        mapas[i] = caracter;
        i++;
    }
    mapas[i] = '\0';  // Agregar el caracter nulo al final del array para indicar su final
    
    fclose(archivo);
}

int main() {
    char mapas[1000];  // Definir el array para almacenar el contenido del archivo
    
    mostrar_mapas(mapas);
    
    printf("%s\n", mapas);  // Imprimir el contenido del archivo
    
    return 0;
}
