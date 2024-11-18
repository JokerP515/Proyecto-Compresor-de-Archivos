@echo off
title Compresor de Archivos
color d
mode 120,50

:inicio
cls
echo.
echo -----------------------------------
echo -      Menu principal compresor   -
echo -                                 -
echo -----------------------------------
echo -                                 -
echo -     1. Comprimir Archivo        -
echo -     2. Descomprimir Archivo     -
echo -     3. Salir                    -
echo -                                 -
echo -----------------------------------
echo.
echo.
set /p menu=Opcion= 
if "%menu%"=="" (
    echo No se ha ingresado ninguna opcion.
    echo.
    pause
    goto inicio
)

if "%menu%" == "1" goto comprimir_archivo
if "%menu%" == "2" goto descomprimir_archivo
if "%menu%" == "3" goto salir
goto numero_invalido

:comprimir_archivo
cls
echo Ingrese la ruta del archivo a comprimir:
set /p ruta=

if "%ruta%"=="" (
    echo No se ha ingresado ninguna ruta. Volviendo al menu principal...
    echo.
    pause
    goto inicio
)

CompresorDeArchivos.exe 1 "%ruta%"
if errorlevel 1 (
    echo Error durante la compresion.
) else (
    echo Archivo comprimido exitosamente.
    goto fin
)
echo.
pause
goto inicio

:descomprimir_archivo
cls
echo Ingrese la ruta del archivo a descomprimir:
set /p ruta=

if "%ruta%"=="" (
    echo No se ha ingresado ninguna ruta. Volviendo al menu principal...
    echo.
    pause
    goto inicio
)

CompresorDeArchivos.exe 2 "%ruta%"
if errorlevel 1 (
    echo Error durante la descompresion.
) else (
    echo Archivo descomprimido exitosamente.
    goto fin
)
echo.
pause
goto inicio

:numero_invalido
echo Opcion invalida o no seleccionada. Por favor, seleccione una opcion valida.
pause>nul
goto inicio

:fin
echo.
echo Desea realizar otra operacion? (s/n)
set /p continuar= 
if /i "%continuar%" == "s" goto inicio
echo Fin del programa
pause>nul
cls&exit

:salir
echo Fin del programa
pause>nul
exit