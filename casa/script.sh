#!/bin/bash


JUEGO_EJECUTABLE="juego"


ARCHIVO_TABLERO="tablero1.txt"
ARCHIVO_TABLERO2="tablero2.txt"


function mostrar_menu {
    clear
    echo "-------- Menú de Usuario --------"
    echo "1. Configurar juego"
    echo "2. Editar archivos de configuración"
    echo "3. Compilar código fuente y verificar archivos"
    echo "4. Lanzar juego"
    echo "5. Monitorizar partida"
    echo "6. Salir"
    echo "---------------------------------"
}

function configurar_juego {
    echo "Configurando el juego..."
    gcc config.c -o configuracion
    ./configuracion
    echo "Juego configurado con éxito."
    read -p "Presiona enter para continuar..."
}

function editar_configuracion {
    echo "Editando archivos de configuración..."
    nano $ARCHIVO_TABLERO
    nano $ARCHIVO_TABLERO2
    echo "Archivos de configuración editados con éxito."
    read -p "Presiona enter para continuar..."
}


function compilar_codigo {
    echo "Compilando código fuente..."
    gcc juego.c -o $JUEGO_EJECUTABLE
    clear
    if [ $? -eq 0 ]; then
        echo "Código fuente compilado con éxito."
        echo "Verificando archivos..."
        if [ -f $JUEGO_EJECUTABLE ] && [ -f $ARCHIVO_TABLERO ] && [ -f $ARCHIVO_BARCOS ]; then
            echo "Archivos verificados correctamente."
        else
            echo "ERROR: Alguno de los archivos no existe."
        fi
    else
        echo "ERROR: Falló la compilación del código fuente."
    fi
    read -p "Presiona enter para continuar..."
}

function lanzar_juego {
    echo "Lanzando el juego..."
    ./$JUEGO_EJECUTABLE
    echo "Juego lanzado."
    read -p "Presiona enter para continuar..."
}

function monitorizar_partida {
    echo "Monitorizando la partida..."
    cat disparos.txt
    read -p "Presiona enter para continuar..."
}

while true; do
    mostrar_menu
    read -p "Selecciona una opción: " opcion
    case $opcion in
        1) configurar_juego;;
        2) editar_configuracion;;
        3) compilar_codigo;;
        4) lanzar_juego;;
        5) monitorizar_partida;;
        6) break;;
        *) echo "Opción inválida. Por favor, selecciona una opción válida.";;
    esac
done

echo "Saliendo del programa."
