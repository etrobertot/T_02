# TL/DR
0. Programa dise�ado en lenguaje de programaci�n C++ para simulaci�n de sistemas de l�neas de espera tipo M/M/1.  Apartir de �ste se pueden hacer varios tipos de experimentos y extender el c�digo para otros tipos de modelos de forma muy sencilla.

1. Se cambiaron los nombres de las variables de ingles a espa�ol.

2. El archivo de entrada "mm1.in" se cambi� por "param.txt" ( para mayor claridad y f�cil modificaci�n de los parametros de entrada )

3. El archivo de salida "mm2.out" se cambi� por "result.txt" ( para una m�s f�cil lectura de las estadisticas )

4. El archivo "lcgrand.cpp" se tom� del libro 

# Ejecucion
Para ejecutar el codigo usar el comando make esto generar el binario queue, modificar los parametros del archivoo param.txt, conlos valores de media entre llegadas, media de atencion, numero de esperas requerido, y numero de servidores. Finalmente ejecutar el binario ./queue para obtener el archivo de salida result.txt con la informacion de la simulacion
