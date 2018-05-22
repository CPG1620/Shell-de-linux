#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/wait.h>
#include <stdio_ext.h>
#include <termios.h>
#include "lista.c"
#include "getch.c"


// Colores
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// Estos valores se usarán para los estados
#define OCUPADO -1
#define ESPERA 0
#define LISTO 1

// Booleanos
#define TRUE 1
#define true 1
#define FALSE 0
#define false 0

// Maximo de caracteres bytes admitidos por la terminal
// getconf ARG_MAX
#define tamComando 2097152

//Esta estrcutura permitirá que los procesos se comuniquen de una manera correcta

struct memoriaCompartida {
    int estadoPadre;
    int estadoHijo;
    char * comando;
   	char * argumentos[20];
};


// Funciones
void limpiaString (char *, char * []);
int ejecutar(char *, char * []);
void actualizar (char *,char *,char *);
char* ingresaLetra (char *, char, int);
void llegarAlFinal (int, int);
void acomodarCursor (int, int);

int main(int argc, char const *argv[])
{

    char * comando, * directorio_actual, * argumentos [20];
    int contador = 0;
    int estado;
    int c = 0;
    int x,y;
    int cursor = 0;
    char * salida;
    char * usuario;
    char * pc;

    // Ejecuciones
    comando = malloc (tamComando * sizeof(char));
    directorio_actual = malloc(tamComando * sizeof(char));
    usuario = malloc(tamComando * sizeof(char));
    pc = malloc(tamComando * sizeof(char));
    

    do{
        //Separa argumentos de programa
        limpiaString (comando, argumentos);
        
        // Ejecuto el comnando que hay
        ejecutar(comando, argumentos);
        // Limpiar comando
        comando[0] = 0;

        // Actualizar informacion
        actualizar (directorio_actual, usuario, pc);

         // Muestro usuario, pc y directorio actual
         /*
        printf(ANSI_COLOR_YELLOW"%s@%s"ANSI_COLOR_RESET, usuario, pc);
        printf("%s>",directorio_actual);
        */

        //printf ("%s@%s %s>", usuario, pc, directorio_actual);

        printf ("%s>",directorio_actual);

        // Limpiar stdin
        __fpurge(stdin);
        
        // Leer entrada hasta que den ent
        //scanf("%[^\n]s",comando);

        // Inicializar los valores
        c = -1;
        contador = -1;
        cursor = -1;
        // Esto es para cada comando
        while (c != 10)
        {
            
            // Tomar tecla actual
            c = detectaTecla ();
            
            switch(c){

                case BORRAR:

                    if (contador >= 0)
                    {
                        // Borrar, coloca espacio, borra
                        
                        // Simpliar el string
                        if (cursor != contador)
                        {
                            int actual = 0;
                            for (int i = 0; i <= contador; i++){
                                if (i == cursor)
                                {

                                }
                                else
                                {
                                    comando [actual] = comando [i];
                                    actual++;
                                }
                            }
                        }
                            comando [contador] = 0;
                            contador--;
                            cursor--;
                            fflush(stdout);
                        

                        printf ("\r%s>%s ",directorio_actual,comando);
                        for(int i = 0; i < contador - cursor + 1; i++)
                                printf ("\033[D");
                        
                    }

                    break;

                case ARRIBA:

                    for (int i = 0; i < contador; i++)
                            printf ("\b \b");
                    // Esto debe devolver el comando anterior
                    comando = subir();
                    
                    
                    // Se imprime el comando 
                    printf("%s",comando);
                    // Se conocan los contadores de manera correcta
                    contador = strlen (comando);
                    //contador++;
                    cursor = contador;
                    break;

                case ABAJO:
                    for (int i = 0; i < contador; i++)
                            printf ("\b \b");

                    comando = bajar();
                    printf("%s",comando);
                    contador = strlen (comando);
                    //contador++;
                    cursor = contador;
                    break;

                case DERECHA:

                    // Solo ocurre si no ha pasado el limite
                    if (contador > cursor)
                    {
                        printf ("\033[1C");
                        cursor++;
                    }
                    
                    break;

                case IZQUIERDA:

                    // Solo ocurre si no ha llegado al limite
                    if (cursor >= 0)
                    {
                        printf ("\033[1D");
                        cursor --;    
                    }
                    break;

                case INICIO:
                    for (int i = 0; i <= cursor; i++)
                        {
                            printf ("\033[D");
                            
                        }
                        cursor = -1;
                    break;

                
                case FIN:
                    for (int i = 0; i <     contador - cursor; i++)
                        {
                            printf ("\033[C");
                            
                        }
                        cursor = contador;
                    break;

                default:

                    
                    contador++;
                    cursor++;

                    // Terminar aqui
                    if (c == 10)
                    {
                        comando [contador] = 0; 
                        printf("\n");

                        //for (int i = 0; i < strlen(comando); i++)
                        //    printf("%d\n", comando[i])
                        //printf ("%s\n",comando);
                    }
                    else
                    { 
                        //llegarAlFinal(contador, cursor);
                        //printf("\n");
                        // Limpio el texto existene
                        /*
                        for (int i = 0; i < contador; i++)
                            printf ("\b");
                         */
                         
                        if (contador == cursor)
                        {   

                            // Colocar el que es
                            comando [contador] = c;
                            // El siguiente es el ultimo
                            comando [contador + 1] = 0;
                        }
                        else
                        {                            
                            comando = ingresaLetra (comando, c, cursor);   
                            //acomodarCursor(contador, cursor);
                        }

                        printf ("\r%s>%s",directorio_actual,comando);
                        for(int i = 0; i < contador - cursor; i++)
                                printf ("\033[D");
                    }

                    break;
            }
        }
        
        // Si el comando no esta vacio, se inserta en la lista de comandos    
        

        if (strcmp(comando, "") != 0)
            push (comando, usuario);
            
        if (strcmp(comando, "exit") == 0)
            exit (0);
    }while (strcmp(comando, "exit"));
    // Salir con Exit

    // Limpiar memoria
    free (comando);

    //Borrar lista
    borrarTodo ();

    printf("\n\n");
    return 0;
}


void limpiaString (char * comando, char * argumentos[]){

    char * aux = malloc (tamComando * sizeof (char));
    int pos = 0;
    int flag = 0;
    for (int i = 0; i < strlen(comando); i++)
    {
        if (!flag)
        {
            if (comando [i] != 32 && comando [i] != 9)
            {
                aux [pos] = comando[i];
                pos++;
                flag = 1;
            }
        }
        else
        {
            if (comando [i] == 9)
            {
                aux [pos] = 32;
                pos++;
            }
            else
            {
                aux [pos] = comando[i];
                pos++;
            }
        }


    }

    strcpy (comando, aux);
   argumentos [0] = strtok(comando, " ");
   int index = 1;
   while(argumentos[0] != NULL )
   {
      argumentos [index] = strtok(NULL, " ");
      if (argumentos [index] == NULL)
        break;
      index++;
   }
   comando = argumentos[0];
}


int ejecutar(char * comando, char * argumentos[]){
    if (strcmp(comando, "") == 0)
        return false;

    // Declaraciones
	key_t llaveUnica; // Esto es para que se use una memoria entre los dos
    int idMemoria; // Para identificar la memoria
    struct memoriaCompartida *dato; // Informacion a compartir
    pid_t pid; // Pid de los procesos

    llaveUnica = ftok(".", 521); // Se genera la llave


    pid = fork ();
    if (pid < 0)
        {
            printf("Error\n");
            exit(0);
        }
    if(pid == 0)
    {
    	idMemoria = shmget(llaveUnica, sizeof(struct memoriaCompartida *), 0666);
    	dato = shmat(idMemoria, NULL, 0);
    	if (idMemoria < 0)
    		printf("Error struct\n");

    	while (dato->estadoPadre != LISTO)
          sleep(1);

        if (strcmp (dato->comando, "cd") == 0) // Si el comando es cd
            chdir(argumentos [1]); // Cambiar al directorio
        else if (strcmp (dato->comando, "history") == 0) // Si el comando es cd
            mostrar();
        else// Sino

        {   
            
            if (execvp (dato->comando, dato->argumentos) == -1) // Ejecutar el comando
            {
                printf("Comando no valido\n");// Pero si falla dar este mensaje      
                exit (0);
            }

        }
           

    }
    else
    {
    	idMemoria = shmget(llaveUnica, sizeof(struct memoriaCompartida *), IPC_CREAT | 0666);
		if (idMemoria < 0)// Error
    		exit(0);

    	dato = shmat(idMemoria, NULL, 0);
    	//Poner al hijo en espera
    	dato->estadoPadre = OCUPADO;
    	// Colocar datos
    	dato->comando = comando;
        
    	//Copiar el array
    	for (int i = 0; i < 20; i++)
    		    dato->argumentos[i] = argumentos[i];
        
            


    	//dato->argumentos = argumentos;
    	dato->estadoPadre = LISTO;


        
          wait(NULL); // Esperar

        //Esto por que cuando el comando es cd el proceso sigue ejecutandose 
        // y aunque se escriba exit, simplemente mata a proceso hijo 
        // y sigue con el proceso padre anterior
        if (strcmp(comando, "cd") == 0)
            exit (0);

    }

    return true;
}


void actualizar (char * directorio_actual, char * usuario,char * pc){
    getcwd (directorio_actual, tamComando);
    getlogin_r(usuario,tamComando);
    gethostname (pc, tamComando);
  
    if (strcmp (usuario, "") == 0)
      strcpy (usuario,"daniel");
    

}


char * ingresaLetra (char * comando, char c, int cursor)
{
    if (strcmp (comando, "") == 0)
    {
        comando[0] = c;
    }

    char * aux = malloc(tamComando * sizeof(char));
    int pos = 0;
    for(int i = 0; pos < strlen (comando); i++)
    {
        if (i != cursor)
        {
            aux[i] = comando [pos];
            pos ++;
        }
        else
        {
            aux [i] = c;
        }
    }

    return aux;
}

void llegarAlFinal (int contador, int cursor){
    if (contador != cursor)
    {
        while (cursor < contador)
        {
            // Moverse a la derecha
            printf ("\033[1C");
            cursor++;
        }
    }
}

void acomodarCursor (int contador, int cursor){
    for (int i = contador; i > cursor; i--)
        printf ("\033[1D"); // Moverse a la izquierda

}
