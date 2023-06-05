/* Definiciones externas para el sistema de colas simple */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lcgrand.h" /* Encabezado para el generador de numeros aleatorios */

#define LIMITE_COLA 100 /* Capacidad maxima de la cola */
#define OCUPADO 1       /* Indicador de Servidor Ocupado */
#define LIBRE 0         /* Indicador de Servidor Libre */

int sig_tipo_evento, num_clientes_espera, num_esperas_requerido, num_eventos,
    num_entra_cola, num_servidores, num_servidores_ocupados;
float area_num_entra_cola, area_estado_servidor, media_entre_llegadas, media_atencion,
    tiempo_simulacion, tiempo_llegada[LIMITE_COLA + 1], tiempo_ultimo_evento,
    total_de_esperas, tiempo_cola_activa;
FILE *parametros, *resultados, *impresion;
float *tiempo_sig_evento;

void inicializar(void);
void control_tiempo(void);
void llegada(void);
void salida(void);
void reportes(void);
void actualizar_estad_prom_tiempo(void);
float expon(float mean);
int get_estado_servidor(void);
int get_servidor_libre(void);
int get_servidor_libre2(void);

int main(void) /* Funcion Principal */
{
    /* Abre los archivos de entrada y salida */
    parametros = fopen("param.txt", "r");
    resultados = fopen("result.txt", "w");
    impresion = fopen("impresion.txt", "w");

    /* Lee los parametros de enrtrada. */
    fscanf(parametros, "%f %f %d %d", &media_entre_llegadas, &media_atencion,
           &num_esperas_requerido, &num_servidores);

    /* Especifica el numero de eventos para la funcion controltiempo. */
    num_eventos = num_servidores + 1;
    tiempo_sig_evento = new float[num_eventos + 1];
    /* Escribe en el archivo de salida los encabezados del reporte y los parametros iniciales */
    fprintf(resultados, "Sistema de Colas Simple\n\n");
    fprintf(resultados, "Tiempo promedio de llegada%11.3f minutos\n\n",
            media_entre_llegadas);
    fprintf(resultados, "Tiempo promedio de atencion%16.3f minutos\n\n", media_atencion);
    fprintf(resultados, "Numero de clientes%14d\n\n", num_esperas_requerido);

    /* iInicializa la simulacion. */
    inicializar();

    /* Corre la simulacion mientras no se llegue al numero de clientes especificaco en el archivo de entrada*/
    while (num_clientes_espera < num_esperas_requerido)
    {
        /* Determina el siguiente evento */
        control_tiempo();

        /* Actualiza los acumuladores estadisticos de tiempo promedio */
        actualizar_estad_prom_tiempo();
        /* Invoca la funcion del evento adecuado. */
        switch (sig_tipo_evento)
        {
        case 1:
            llegada();
            break;
        default:
            salida();
            break;
        }
    }

    /* Invoca el generador de reportes y termina la simulacion. */
    reportes();

    fclose(parametros);
    fclose(resultados);

    return 0;
}

void inicializar(void) /* Funcion de inicializacion. */
{
    /* Inicializa el reloj de la simulacion. */
    tiempo_simulacion = 0.0;

    /* Inicializa las variables de estado */
    num_entra_cola = 0;
    tiempo_ultimo_evento = 0.0;
    num_servidores_ocupados = 0;

    /* Inicializa los contadores estadisticos. */
    num_clientes_espera = 0;
    total_de_esperas = 0.0;
    tiempo_cola_activa = 0.0;
    area_num_entra_cola = 0.0;
    area_estado_servidor = 0.0;

    /* Inicializa la lista de eventos. Ya que no hay clientes, el evento salida
       (terminacion del servicio) no se tiene en cuenta */
    for (int i = 1; i <= num_eventos; i++)
    {
        tiempo_sig_evento[i] = 1.0e+30;
    }
    tiempo_sig_evento[1] = tiempo_simulacion + expon(media_entre_llegadas);
}

void control_tiempo(void) /* Funcion controltiempo */
{
    int i;
    float min_tiempo_sig_evento = 1.0e+29;

    sig_tipo_evento = 0;

    /*  Determina el tipo de evento que debe ocurrir. */
    for (i = 1; i <= num_eventos; ++i)
    {
        if (tiempo_sig_evento[i] < min_tiempo_sig_evento)
        {
            min_tiempo_sig_evento = tiempo_sig_evento[i];
            sig_tipo_evento = i;
        }
    }
    // printf("\n sig_tipo_evento: %d", sig_tipo_evento);

    /* Revisa si la lista de eventos esta vacia. */
    if (sig_tipo_evento == 0)
    {
        /* La lista de eventos esta vacia, se detiene la simulacion. */
        fprintf(resultados, "\nLa lista de eventos esta vacia %f", tiempo_simulacion);
        exit(1);
    }

    /* TLa lista de eventos no esta vacia, adelanta el reloj de la simulacion. */
    tiempo_simulacion = min_tiempo_sig_evento;
}

void llegada(void) /* Funcion de llegada */
{
    float espera;
    int servidor = get_servidor_libre();
    /* Programa la siguiente llegada. */
    tiempo_sig_evento[1] = tiempo_simulacion + expon(media_entre_llegadas);

    /* Revisa si el servidor esta OCUPADO. */
    if (get_estado_servidor() == OCUPADO)
    {
        /* Servidor OCUPADO, aumenta el numero de clientes en cola */
        ++num_entra_cola;

        /* Verifica si hay condici�n de desbordamiento */
        if (num_entra_cola > LIMITE_COLA)
        {
            /* Se ha desbordado la cola, detiene la simulacion */
            fprintf(resultados, "\nDesbordamiento del arreglo tiempo_llegada a la hora ");
            fprintf(resultados, "%f", tiempo_simulacion);
            exit(2);
        }

        /* Todavia hay espacio en la cola, se almacena el tiempo de llegada del
            cliente en el ( nuevo ) fin de tiempo_llegada */
        tiempo_llegada[num_entra_cola] = tiempo_simulacion;
    }
    else
    {
        /*  El servidor esta LIBRE, por lo tanto el cliente que llega tiene tiempo de espera=0
           (Las siguientes dos lineas del programa son para claridad, y no afectan
           el reultado de la simulacion ) */
        espera = 0.0;
        total_de_esperas += espera;

        /* Incrementa el numero de clientes en espera, y pasa el servidor a ocupado */
        ++num_clientes_espera;
        ++num_servidores_ocupados;

        /* Programa una salida ( servicio terminado ). */
        tiempo_sig_evento[servidor] = tiempo_simulacion + expon(media_atencion);
    }
}

void salida(void) /* Funcion de Salida. */
{
    float espera;
    /* Revisa si la cola esta vacia */
    if (num_entra_cola == 0)
    {
        /* La cola esta vacia, pasa el servidor a LIBRE y
        no considera el evento de salida*/
        --num_servidores_ocupados;
        tiempo_sig_evento[sig_tipo_evento] = 1.0e+30;
    }
    else
    {
        /* La cola no esta vacia, disminuye el numero de clientes en cola. */
        --num_entra_cola;

        /*Calcula la espera del cliente que esta siendo atendido y
        actualiza el acumulador de espera */
        espera = tiempo_simulacion - tiempo_llegada[1];
        total_de_esperas += espera;

        /*Incrementa el numero de clientes en espera, y programa la salida. */
        ++num_clientes_espera;
        tiempo_sig_evento[sig_tipo_evento] = tiempo_simulacion + expon(media_atencion);

        /* Mueve cada cliente en la cola ( si los hay ) una posicion hacia adelante */
        for (int i = 1; i <= num_entra_cola; ++i)
            tiempo_llegada[i] = tiempo_llegada[i + 1];
    }
}

void reportes(void) /* Funcion generadora de reportes. */
{
    /* Calcula y estima los estimados de las medidas deseadas de desempe�o */
    fprintf(resultados, "\n\nEspera promedio en la cola%11.3f minutos\n\n",
            total_de_esperas / num_clientes_espera);
    fprintf(resultados, "Numero promedio en cola%10.3f\n\n",
            area_num_entra_cola / tiempo_simulacion);
    fprintf(resultados, "Uso del servidor%15.3f\n\n",
            area_estado_servidor / tiempo_simulacion);
    fprintf(resultados, "Proporción de tiempo cola activa%15.3f\n\n",
            tiempo_cola_activa / tiempo_simulacion);
    fprintf(resultados, "Tiempo de terminacion de la simulacion%12.3f minutos", tiempo_simulacion);
}

void actualizar_estad_prom_tiempo(void) /* Actualiza los acumuladores de
                                                       area para las estadisticas de tiempo promedio. */
{
    float time_since_last_event;

    /* Calcula el tiempo desde el ultimo evento, y actualiza el marcador
        del ultimo evento */
    time_since_last_event = tiempo_simulacion - tiempo_ultimo_evento;
    tiempo_ultimo_evento = tiempo_simulacion;

    /* Actualiza el area bajo la funcion de numero_en_cola */
    area_num_entra_cola += num_entra_cola * time_since_last_event;
    tiempo_cola_activa += num_entra_cola ? time_since_last_event : 0;

    /*Actualiza el area bajo la funcion indicadora de servidor ocupado*/
    area_estado_servidor += get_estado_servidor() * time_since_last_event;
}

float expon(float media) /* Funcion generadora de la exponencias */
{
    /* Retorna una variable aleatoria exponencial con media "media"*/
    return -media * log(lcgrand(2));
}

int get_estado_servidor()
{
    if (num_servidores_ocupados == num_servidores)
        return OCUPADO;
    else
        return LIBRE;
}

int get_servidor_libre()
{
    int idx = 0;
    float val = 0;
    for (int i = 2; i <= num_eventos; i++)
    {
        if (tiempo_sig_evento[i] > val)
        {
            val = tiempo_sig_evento[i];
            idx = i;
        }
    }
    return idx;
}
