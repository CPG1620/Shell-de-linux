#include<stdio.h>
#include<stdlib.h>

typedef struct nodo {
    // si no se pone struct aqui, no sirve
    struct nodo * siguiente;
    char dato [2097152  ];
} nodo;

nodo * primero = NULL;
nodo * ultimo = NULL;
nodo * actual = NULL;

int contar (){
    nodo * aux = primero;
    int cont = 0;
    while (aux != NULL)
    {
        cont++;
        aux = aux->siguiente;
    }
}

void pop (){
    if (!primero)
        return;
    nodo * aux = primero;
    primero = primero->siguiente;
    free(aux);
    actual = ultimo;
}

void push (char comando [2097152], char * usuario){
    

    if (contar() >= 10)
        pop();
    // Se crea el nodo a insertar
    nodo * aux = malloc (sizeof(nodo));

    // Se le dan sus valores
    aux->siguiente = NULL;


    strcpy (aux->dato, comando);
    

    if (primero == NULL)
    {

        // Si no hay lista, este sera el primer y ultimo elemento
        primero = aux;
        ultimo = primero;
    }
    else
    {
        // Si no se agrega al final
        ultimo->siguiente = aux;
        ultimo = ultimo->siguiente;
    }

    FILE * archivo;
    // el archivo se llama .shhistory
    // estará en /home/usuario/.shhistory

    char direccion [1000];
    // Para poner la direccion correcta
    strcpy (direccion, "/home/");
    strcat (direccion, usuario);
    strcat (direccion, "/.shhistory");
    archivo = fopen (direccion, "a+");
    // PC de daniel
    //archivo = fopen ("/home/daniel/.shhistory", "a+");


    // Se inserta en el archivo
    fputs (comando, archivo);
    fputs ("\n", archivo);
    // Cerrar archivo
    fclose (archivo);

    // Asignar actual
    actual = ultimo;
}

void mostrar (){
    // Se toma la referencia del primer elemento
    nodo * aux = primero;
    int cont = 1;
    while (aux)
    {
        // Se va avanzando mientras se muestran los datos
        printf ("%d: %s\n", cont++, aux->dato);
        aux = aux->siguiente;
    }
    printf("\n");
}

nodo * get (int pos){
    nodo * aux = primero;
    for (int i = 0; i < pos; i++)
    {
        if (aux == NULL)
            break;
        aux = aux->siguiente;
    }
    return aux;
}


void borrarTodo (){
    nodo * aux = primero;
    while (aux)
    {
        primero = primero->siguiente;
        free (aux);
        aux = primero;
    }
}

char * subir (){
    /*
    if (primero == NULL)
        return NULL;
    if (primero = actual)
        return primero->dato;
    if (actual == NULL)
        actual = primero;
    */

    if (actual == primero)
        return primero->dato;
    if (!actual)
    {
        actual = ultimo;
        return "";
    }

    char * txt = actual->dato;

    nodo * aux;
    aux = primero;
    while (aux->siguiente != actual)
        aux = aux->siguiente;
    actual = aux; 

    return txt;
}

char * bajar(){
    if (actual -> siguiente == NULL)
        return "";
    actual = actual->siguiente;
    return actual->dato;
}

char * verActual(){
    if (actual->dato)
        return actual->dato;
    else
        return " ";
}
