#include <thread>
#include <iostream>
#include <cstdlib>
#include <mutex>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <iostream>
#include <string>
#include <string.h>
#include <condition_variable>
#include <atomic>
#include <sys/time.h>


#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define BASE 0
#define OPTIMIZACION 1
#define LOGGER_G 0

//variables atomicas

std::atomic<int> gAtomic_x; //VARIABLE ATÓMICA

//variables globales
long double sumTot_g = 0;
long double revTot_g = 0;

//creamos el mutex
std::mutex mutex_g;

char revFunct[] = "rev";
char sumFunct[] = "sum";

int numThreads = 0;
char* funcionARealizar;

//booleanos
bool primeraVez = 1;
bool dameSuma = 0;

//variables para logger
long double sumaLogger_g = 0;
long double revLogger_g = 0;


//mutex logger con los hilos

std::condition_variable CVLogger;
bool despiertaLoger_g = 0;

//mutex logger con el main
bool terminaSuma = 0;

int numDeThreadsEscritos = 0;
int hiloLLegado = 0;

//hacemos la estructura
typedef struct estructHilos {

  double valorHilosUsados[12];

} estructHilos;

//declaramos la variable atomica
std::atomic<double> totSumaAt_g;
std::atomic<double> totRevAt_g;


void funcionGeneral(char* funcionARealizar, double* arrayElementos, int posInicial, int posFinal, int posInicial2, int posFinal2, estructHilos* estructHilos, int numHilo) {

  //protegemos la seccion con un mutex,
  //ya que trabaja con una variable global

  long double sumaParcialHilo = 0;
  long double revParcialHilo = 0;

  #if OPTIMIZACION
    std::lock_guard<std::mutex> guard(mutex_g);
  #endif

  for (int i = posInicial; i < posFinal; i++) {

    {
      if(strcmp(funcionARealizar, sumFunct) == 0) {

        //std::lock_guard<std::mutex> guard(mutex_g);

        //gAtomic_x.is_lock_free();

        //sumTot_g = sumTot_g + arrayElementos[i];

        #if !OPTIMIZACION
          std::lock_guard<std::mutex> guard(mutex_g);
          sumTot_g = sumTot_g + arrayElementos[i];
        #endif

        #if OPTIMIZACION
          totSumaAt_g.is_lock_free();
          totSumaAt_g = totSumaAt_g + arrayElementos[i];
        #endif

        sumaParcialHilo = sumaParcialHilo + arrayElementos[i];


      } else {

        #if !OPTIMIZACION
          std::lock_guard<std::mutex> guard(mutex_g);
          revTot_g = revTot_g - arrayElementos[i];
        #endif

        #if OPTIMIZACION
        totRevAt_g.is_lock_free();
        totRevAt_g = totRevAt_g - arrayElementos[i];
        #endif


        revParcialHilo = revParcialHilo - arrayElementos[i];

      }
    }

}

  hiloLLegado = hiloLLegado + 1;

  if(primeraVez == 1) {

    //std::lock_guard<std::mutex> guard(mutex_g);

    primeraVez = 0;

    for (int i = posInicial2; i < posFinal2; i++) {

      {
        if(strcmp(funcionARealizar, sumFunct) == 0) {

          #if !OPTIMIZACION
            std::lock_guard<std::mutex> guard(mutex_g);
            sumTot_g = sumTot_g + arrayElementos[i];
          #endif

          #if OPTIMIZACION
            totSumaAt_g.is_lock_free();
            totSumaAt_g = totSumaAt_g + arrayElementos[i];
          #endif

          sumaParcialHilo = sumaParcialHilo + arrayElementos[i];

        } else {

          #if !OPTIMIZACION
            std::lock_guard<std::mutex> guard(mutex_g);
            revTot_g = revTot_g - arrayElementos[i];
          #endif

          #if OPTIMIZACION
          totRevAt_g.is_lock_free();
          totRevAt_g = totRevAt_g - arrayElementos[i];
          #endif


          revParcialHilo = revParcialHilo - arrayElementos[i];

        }
      }

    }


  }


  #if !BASE

      //guardamos la solucion del hilo y en cual ha sido

      if(strcmp(funcionARealizar, sumFunct) == 0) {

        estructHilos->valorHilosUsados[numHilo] = sumaParcialHilo;

      } else {

        estructHilos->valorHilosUsados[numHilo] = revParcialHilo;

      }
      //despertamos a logger

      despiertaLoger_g = 1;
      CVLogger.notify_one();

  #endif

}

int logger(estructHilos* estructHilos, std::condition_variable* CVLoggerMain) {

  while(numThreads != hiloLLegado) {

      std::unique_lock<std::mutex> ulk(mutex_g);
      CVLogger.wait(ulk, []{return despiertaLoger_g;});

      //volvemos a dormir al logger
      despiertaLoger_g = 0;

      //abrimos el fichero
      FILE* ptFichero;
      ptFichero = fopen("dumb.log", "wt");

      float valorSumadoParcial;

      numDeThreadsEscritos = numDeThreadsEscritos + 1;
      ulk.unlock();

  }

  FILE* ptFichero;
  ptFichero = fopen("dumb.log", "wt");
  char linea[100];

  //printf("\n");
  for(int i = 0; i< numThreads; i++) {

    if(estructHilos->valorHilosUsados[i] != 0,5 && estructHilos->valorHilosUsados[i] != 0) {

      //printf("El valor recogido del hilo %d es: %f\n",i, estructHilos->valorHilosUsados[i]);
      //fprintf("El valor recogido del hilo %d es: %f\n",i, estructHilos->valorHilosUsados[i]);

    }

  }

  std::unique_lock<std::mutex> ulk(mutex_g);



  for(int i = 0; i < numThreads; i++) {

    //char* valorLeidoString = fgets(linea, 100, ptFichero);

    if(strcmp(funcionARealizar, sumFunct) == 0 && estructHilos->valorHilosUsados[i] != 0.5) {

        sumaLogger_g = sumaLogger_g + estructHilos->valorHilosUsados[i];

    } else if(strcmp(funcionARealizar, revFunct) == 0 && estructHilos->valorHilosUsados[i] != 0.5){

        revLogger_g =  revLogger_g + estructHilos->valorHilosUsados[i];

    }

  }

  if(strcmp(funcionARealizar, sumFunct) == 0) {

    fprintf(ptFichero, "%Lf\n", sumaLogger_g);
    //printf("Total calculado en el logger: %Lf\n", sumaLogger_g );

  } else {

    fprintf(ptFichero, "%Lf\n", revLogger_g);
    //printf("Total calculado en el logger: %Lf\n", revLogger_g );

  }

  fclose(ptFichero);

  //informamos al main que hemos terminado de sumar por punteros
  terminaSuma = 1;

  CVLoggerMain->notify_one();

  ulk.unlock();

}


int main(int argc, char *argv[]) {

  struct timeval tiempoInicial;

  gettimeofday(&tiempoInicial, NULL);

  //tratamos los argumentos y ponemos errores

  int numElementos = 0;

  printf("Arg 1: %d\n", atoi(argv[1]));
  printf("Arg 2: %s\n", argv[2]);
  printf("Arg 3: %s\n", argv[3]);

  if(argc == 5) {

    printf("Arg 4: %d\n", atoi(argv[4]));

  }


  char* numElementChar = argv[1];

  if(isdigit(numElementChar[0]) == 0) {

    printf("Introduzca un número para el tamaño del array\n");
    return(EXIT_FAILURE);

  }

  numElementos = atoi(argv[1]);

  if(strcmp(argv[2], sumFunct) != 0 && strcmp(argv[2], revFunct) != 0) {

    printf("Introduzca una funcion valida\n");
    return(EXIT_FAILURE);

  } else {

    funcionARealizar = argv[2];

  }

  char multiThread[] = "--multi-thread";

  if (argc == 3) {

    numThreads = 1;


  } else if(strcmp(argv[3], multiThread) == 0 && argc == 5) {

      char* numThreadsChar = argv[4];

      if(isdigit(numThreadsChar[0]) == 0) {

        printf("Introduzca un número para el numero de threads\n");
        return(EXIT_FAILURE);

      }

      numThreads = atoi(argv[4]);

      if(numThreads > 12 || numThreads < 0){

        printf("Introduzca un número de threads menor de 12\n");
        return(EXIT_FAILURE);

      }

  } else {

    printf("Introduza el numero de threads a utilizar\n");
    return(EXIT_FAILURE);

  }

  //creamos dinamicamamente el array de elementos


	double* arrayElementos = (double*) malloc(numElementos * sizeof(double));

	if (arrayElementos == NULL) {

		printf("No se ha podido reservar memoria correctamente");
		return(EXIT_FAILURE);

	}

  //lo rellenamos
  for(int i = 0; i < numElementos; i++) {

		arrayElementos[i] = i;

  }

  //creacion de tantos threads como numero de elementos
  //nos digan

  //analizamos el numero de bloques que tendremos

  float restoNumBloques = fmod(numElementos, numThreads);

  int numBloques = numThreads;
  int numElementosPorBloque;
  bool thread1MasTrabajo = 0;

  if(restoNumBloques == 0) {

    numElementosPorBloque = numElementos / numThreads;

  } else {

    numElementosPorBloque = numElementos / numThreads;

    thread1MasTrabajo = 1;

  }


  //creamos la estructura de hilos a pasar como parametro
  estructHilos estructuraHilos;

  //inicializamos el array de la estructura
  for(int i = 0; i < numThreads; i++) {

    estructuraHilos.valorHilosUsados[i] = 0,5;

  }

  #if !BASE

      //llamamos al logger y creamos la variable condicional

      std::condition_variable CVLoggerMain;

      std::thread loggerHilo(logger, &estructuraHilos, &CVLoggerMain);

  #endif


  //creamos el array de threads
  std::thread threads[numThreads];

  //asignamos a cada thread la funcion a realizar

  int posFinal = numElementosPorBloque;
  int posInicial = 0;

  int posInicial2 = numElementos - restoNumBloques;
  int posFinal2 = numElementos;
  int numHilo = 0;

  for(int i = 0; i < numThreads; i++) {

    numHilo = i;
    threads[i] = std::thread(funcionGeneral, funcionARealizar, arrayElementos, posInicial,
                                        posFinal, posInicial2, posFinal2, &estructuraHilos,numHilo);

    posInicial = posFinal;
    posFinal = posFinal + numElementosPorBloque;

  }

  //hacemos el join de todos los threads
  for(int i = 0; i < numThreads; i++) {


    threads[i].join();


  }


#if LOGGER_G

      loggerHilo.join();

      std::unique_lock<std::mutex> ulk(mutex_g);
      CVLoggerMain.wait(ulk, []{return terminaSuma;}); // freed on wait

      //comprobamos si lo que ha calculado logger es igual a lo que han calculado los
      //hilos, y si es asi damos el resultado por pantalla


      if(strcmp(funcionARealizar, sumFunct) == 0) {

        if(sumaLogger_g == sumTot_g) {

          //printf("total calculado sum: %Lf\n", sumTot_g);

          struct timeval tiempoFinal;

          gettimeofday(&tiempoFinal, NULL);

          time_t tiempoQueTardaSeg = tiempoFinal.tv_sec - tiempoInicial.tv_sec;
          suseconds_t tiempoQueTardaMlSeg = fabs(tiempoFinal.tv_usec - tiempoInicial.tv_usec);

          printf("\n");
          printf("Tarda en ejecutarse %f\n", tiempoQueTardaSeg + 1e-6 * tiempoQueTardaMlSeg);

          printf("\n");



          return EXIT_SUCCESS;

        } else {

          //printf("total calculado sum: %Lf\n", sumTot_g);
          //printf("Calculo de suma mal hecho\n");


          struct timeval tiempoFinal;

          gettimeofday(&tiempoFinal, NULL);

          time_t tiempoQueTardaSeg = tiempoFinal.tv_sec - tiempoInicial.tv_sec;
          suseconds_t tiempoQueTardaMlSeg = fabs(tiempoFinal.tv_usec - tiempoInicial.tv_usec);

          printf("\n");
          printf("Tarda en ejecutarse %f\n", tiempoQueTardaSeg + 1e-6 * tiempoQueTardaMlSeg);



          return EXIT_FAILURE;

        }
      } else if(strcmp(funcionARealizar, revFunct) == 0) {

        if(revLogger_g == revTot_g) {

          //printf("total calculado rev: %Lf\n", revTot_g);


          struct timeval tiempoFinal;

          gettimeofday(&tiempoFinal, NULL);

          time_t tiempoQueTardaSeg = tiempoFinal.tv_sec - tiempoInicial.tv_sec;
          suseconds_t tiempoQueTardaMlSeg = fabs(tiempoFinal.tv_usec - tiempoInicial.tv_usec);

          printf("\n");
          printf("Tarda en ejecutarse %f\n", tiempoQueTardaSeg + 1e-6 * tiempoQueTardaMlSeg);



          return EXIT_SUCCESS;

        } else {

          //printf("total calculado rev: %Lf\n", revTot_g);
          //printf("Calculo de rev mal hecho\n");


          struct timeval tiempoFinal;

          gettimeofday(&tiempoFinal, NULL);

          time_t tiempoQueTardaSeg = tiempoFinal.tv_sec - tiempoInicial.tv_sec;
          suseconds_t tiempoQueTardaMlSeg = fabs(tiempoFinal.tv_usec - tiempoInicial.tv_usec);

          printf("\n");
          printf("Tarda en ejecutarse %f\n", tiempoQueTardaSeg + 1e-6 * tiempoQueTardaMlSeg);


          return EXIT_FAILURE;

        }

      } else {

          printf("Error\n");

          struct timeval tiempoFinal;

          gettimeofday(&tiempoFinal, NULL);

          time_t tiempoQueTardaSeg = tiempoFinal.tv_sec - tiempoInicial.tv_sec;
          suseconds_t tiempoQueTardaMlSeg = fabs(tiempoFinal.tv_usec - tiempoInicial.tv_usec);

          printf("\n");
          printf("Tarda en ejecutarse %f\n", tiempoQueTardaSeg + 1e-6 * tiempoQueTardaMlSeg);



          return EXIT_FAILURE;

      }

  #elif BASE

    if(strcmp(funcionARealizar, revFunct) == 0) {

      printf("total calculado rev: %Lf\n", revTot_g);

    } else {

      printf("total calculado sum: %Lf\n", sumTot_g);

    }

    struct timeval tiempoFinal;

    gettimeofday(&tiempoFinal, NULL);

    time_t tiempoQueTardaSeg = tiempoFinal.tv_sec - tiempoInicial.tv_sec;
    suseconds_t tiempoQueTardaMlSeg = tiempoFinal.tv_usec - tiempoInicial.tv_usec;


    printf("Tarda en ejecutarse %f\n", tiempoQueTardaSeg + (1e-6 * tiempoQueTardaMlSeg));
    printf("\n");
    printf("\n");

    return 0;

  #elif OPTIMIZACION

        loggerHilo.join();

        std::unique_lock<std::mutex> ulk(mutex_g);
        CVLoggerMain.wait(ulk, []{return terminaSuma;}); // freed on wait

        //comprobamos si lo que ha calculado logger es igual a lo que han calculado los
        //hilos, y si es asi damos el resultado por pantalla


        if(strcmp(funcionARealizar, sumFunct) == 0) {


          if(sumaLogger_g == totSumaAt_g.load()) {

            //printf("total calculado sum: %f\n", totSumaAt_g.load());

            struct timeval tiempoFinal;

            gettimeofday(&tiempoFinal, NULL);

            time_t tiempoQueTardaSeg = tiempoFinal.tv_sec - tiempoInicial.tv_sec;
            suseconds_t tiempoQueTardaMlSeg = fabs(tiempoFinal.tv_usec - tiempoInicial.tv_usec);

            printf("Tarda en ejecutarse %f\n", tiempoQueTardaSeg + 1e-6 * tiempoQueTardaMlSeg);
            printf("\n");


            return EXIT_SUCCESS;

          } else {



            //printf("total calculado sum: %f\n", totSumaAt_g.load());
            //printf("Calculo de suma mal hecho\n");


            struct timeval tiempoFinal;

            gettimeofday(&tiempoFinal, NULL);

            time_t tiempoQueTardaSeg = tiempoFinal.tv_sec - tiempoInicial.tv_sec;
            suseconds_t tiempoQueTardaMlSeg = fabs(tiempoFinal.tv_usec - tiempoInicial.tv_usec);

            printf("\n");
            printf("Tarda en ejecutarse %f\n", tiempoQueTardaSeg + 1e-6 * tiempoQueTardaMlSeg);



            return EXIT_FAILURE;

          }
        } else if(strcmp(funcionARealizar, revFunct) == 0) {

          if(revLogger_g == totRevAt_g.load()) {

            printf("total calculado rev: %f\n", totRevAt_g.load());


            struct timeval tiempoFinal;

            gettimeofday(&tiempoFinal, NULL);

            time_t tiempoQueTardaSeg = tiempoFinal.tv_sec - tiempoInicial.tv_sec;
            suseconds_t tiempoQueTardaMlSeg = fabs(tiempoFinal.tv_usec - tiempoInicial.tv_usec);

            printf("\n");
            printf("Tarda en ejecutarse %f\n", tiempoQueTardaSeg + 1e-6 * tiempoQueTardaMlSeg);



            return EXIT_SUCCESS;

          } else {

            printf("total calculado rev: %f\n", totRevAt_g.load());
            printf("Calculo de rev mal hecho\n");


            struct timeval tiempoFinal;

            gettimeofday(&tiempoFinal, NULL);

            time_t tiempoQueTardaSeg = tiempoFinal.tv_sec - tiempoInicial.tv_sec;
            suseconds_t tiempoQueTardaMlSeg = fabs(tiempoFinal.tv_usec - tiempoInicial.tv_usec);

            printf("\n");
            printf("Tarda en ejecutarse %f\n", tiempoQueTardaSeg + 1e-6 * tiempoQueTardaMlSeg);


            return EXIT_FAILURE;

          }

        } else {

            printf("Error\n");

            struct timeval tiempoFinal;

            gettimeofday(&tiempoFinal, NULL);

            time_t tiempoQueTardaSeg = tiempoFinal.tv_sec - tiempoInicial.tv_sec;
            suseconds_t tiempoQueTardaMlSeg = fabs(tiempoFinal.tv_usec - tiempoInicial.tv_usec);

            printf("\n");
            printf("Tarda en ejecutarse %f\n", tiempoQueTardaSeg + 1e-6 * tiempoQueTardaMlSeg);



            return EXIT_FAILURE;

        }


  #endif





}
