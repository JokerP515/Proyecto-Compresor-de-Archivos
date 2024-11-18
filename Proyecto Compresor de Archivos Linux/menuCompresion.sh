#!/bin/bash

clear

#Funcion para mostrar el menu principal
mostrar_menu() {
    clear
    echo
    echo "-----------------------------------"
    echo "-      Menu principal compresor   -"
    echo "-                                 -"
    echo "-----------------------------------"
    echo "-                                 -"
    echo "-     1. Comprimir Archivo        -"
    echo "-     2. Descomprimir Archivo     -"
    echo "-     3. Salir                    -"
    echo "-                                 -"
    echo "-----------------------------------"
    echo
}

# Funcion para comprimir un archivo
comprimir_archivo() {
    clear
    echo "Ingrese la ruta del archivo a comprimir:"
    read ruta

    if [ -z "$ruta" ]; then
        echo "No se ha ingresado ninguna ruta. Volviendo al menu principal..."
        read -p "Presione Enter para continuar..."
        return
    fi

    ./CompresorDeArchivos 1 "$ruta"
    if [ $? -ne 0 ]; then
        echo "Error durante la compresion."
    else
        echo "Archivo comprimido exitosamente."
    fi
    echo
    read -p "Presione Enter para continuar..."
}

# Funcion para descomprimir un archivo
descomprimir_archivo() {
    clear
    echo "Ingrese la ruta del archivo a descomprimir:"
    read ruta

    if [ -z "$ruta" ]; then
        echo "No se ha ingresado ninguna ruta. Volviendo al menu principal..."
        read -p "Presione Enter para continuar..."
        return
    fi

    ./CompresorDeArchivos 2 "$ruta"
    if [ $? -ne 0 ]; then
        echo "Error durante la descompresion."
    else
        echo "Archivo descomprimido exitosamente."
    fi
    echo
    read -p "Presione Enter para continuar..."
}

# Funcion para manejar opciones inválidas
opcion_invalida() {
    echo "Opcion invalida o no seleccionada. Por favor, seleccione una opcion valida."
    read -p "Presione Enter para continuar..."
}

# Función para finalizar el programa
finalizar_programa() {
    echo "Fin del programa"
    exit 0
}

# Bucle principal del menu
while true; do
    mostrar_menu
    echo -n "Opcion= "
    read menu

    case $menu in
        1)
            comprimir_archivo
            ;;
        2)
            descomprimir_archivo
            ;;
        3)
            finalizar_programa
            ;;
        *)
            opcion_invalida
            ;;
    esac
done
