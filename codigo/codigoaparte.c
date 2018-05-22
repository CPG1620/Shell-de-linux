/*
	Me base de este codigo
	http://www.csl.mtu.edu/cs4411.ck/www/NOTES/process/shm/example-2.html
*/
#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>



struct estructuraCompartida {
    int estadoPadre;
    int estadoHijo;
    int cantidad;
    unsigned long long *datos;
};



unsigned long long fibonacci (unsigned long long val1,  unsigned long long val2, int veces, unsigned long long * array);

int main (int argc, char *argv[2]){

	key_t llaveUnica; // Esto es para que se use una memoria entre los dos
	key_t llaveArray; // Esto es para que se use una memoria entre los dos
    int idMemoria; // Para identificar la memoria
    struct estructuraCompartida *dato; // Informacion a compartir
    pid_t pid; // Pid de los procesos
    llaveUnica = ftok(".", 'I'); // Se genera la llave
    llaveArray = ftok(".", 'O'); // Se genera la llave para el array


    pid = fork(); // Se hace el proceso hijo
    if (pid == 0) // Proceso Hijo
    {
    	printf("Hijo Solicita Espacio para estructura\n");

    	idMemoria = shmget(llaveUnica, sizeof(struct estructuraCompartida *), 0666); // Se usa la memoria con permisos de lectura/escritura y se guarda su ID
    	if (idMemoria < 0)// Error
    	{
    		printf("Hijo tuvo problema para usar la memoria\n");
    		exit(0);
    	}
    	printf("Hijo Recibe correctamente espacio\n");
    	printf("Hijo Solicita infomracion compartida\n");
    	dato = (struct estructuraCompartida *) shmat(idMemoria, NULL, 0); // En dato se guarda la informacion compartida entre los dos procesos
    	dato->estadoHijo = 0;
    	printf("Hijo Espera a que el padre coloque infomacion\n");
    	while (dato->estadoPadre != 1) // Espera a que se coloque la informacion
          sleep(1);

     	printf("Hijo Solicita Espacio para array\n");
		idMemoria = shmget(llaveArray, sizeof(unsigned long long), 0666);
		
		dato->datos = (unsigned long long*)shmat(idMemoria, 0, 0);
		printf("Hijo Desarrolla fibonacci\n");
		fibonacci (0, 1, dato->cantidad, dato->datos);
		printf("Hijo termina su trabajo\n");
      	dato->estadoHijo = 1; // Cambio de estado
        
        shmdt(idMemoria);
        printf("Hijo separa la memoria de si\n");      	

    }
    else
    {
    	int tam = atoi (argv[1]);
    	printf("Padre crea memoria para estructura\n");
    	idMemoria = shmget(llaveUnica, sizeof(struct estructuraCompartida *), IPC_CREAT | 0666);// Se crea la memoria con permisos de lectura/escritura
    	if (idMemoria < 0)// Error
    	{
    		printf("Padre tuvo problema para usar la memoria\n");
    		exit(0);
    	}
    	printf("Padre da memoria a estructura\n");
    	dato = (struct estructuraCompartida *) shmat(idMemoria, NULL, 0); // En dato se van a almacenar los datos a compartir
    	printf("Padre inicia a darle infomacion a Hijo\n");
    	dato->estadoPadre  = 0; // El estado es que aun no esta listo

    	dato->cantidad = tam; // Se coloca informacion

    	printf("Padre crea memoria para array\n");
		idMemoria = shmget(llaveArray,sizeof(unsigned long long), IPC_CREAT | 0666);
		printf("Padre da memoria para array\n");
		dato->datos = (unsigned long long*) shmat(idMemoria, 0, 0);
		printf("Padre termina de dar infomacion a hijo\n");
   	    dato->estadoPadre = 1; // Se cambia estado

   	    wait(NULL);

   	    printf("Padre recibe informacion de hijo\n");

		for(int i = tam-1; i >= 0; i--)
			printf("%llu\n", dato->datos [i]);

        shmdt(idMemoria);
        printf("Padre separa la memoria de si\n");
        shmctl(idMemoria, IPC_RMID, NULL);
        printf("Padre Elimina memoria compartida\n");
        printf("Padre termina\n");      
        

    }
	return 0;
}

unsigned long long fibonacci (unsigned long long val1,  unsigned long long val2, int veces, unsigned long long * array)
{
	array [veces - 1] = val2;
	if (veces <= 1)
		return val2;
	return fibonacci (val2, val1 + val2, veces - 1, array);
}
