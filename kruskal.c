#include "kruskal.h"

//Funcion de inicializacion del conjunto combina-encuentra
void inicial(conjunto_CE ** C){

    //Se inicializa el conjunto con el primer item en el que es el mismo que su nombre
    //Por lo tanto, la cuenta va a ser 1 en principio y sin siguiente elemento
    for (int i = 0; i < VERTICES; i++)
    {
        (*C)->nombres[i].nombre_conjunto = i + 1;
        (*C)->nombres[i].siguiente_elemento = 0;
        (*C)->encabezamientos_conjunto[i].cuenta = 1;
        (*C)->encabezamientos_conjunto[i].primer_elemento = i + 1;
    }
}

// Función para combinar dos conjuntos en la estructura de Conjunto Combinar-Encontrar (CC)
void combina(tipo_nombre A, tipo_elemento B, conjunto_CE *C) {
    int i;

    //Este bloque evalúa cuál de los dos conjuntos (A o B) tiene más elementos
    //Se absorbera el conjunto más pequeño dentro del más grande
    if (C->encabezamientos_conjunto[A-1].cuenta >= C->encabezamientos_conjunto[B-1].cuenta) {
        //Actualiza los elementos del conjunto mas pequeño para que pertenezcan al conjunto mas grande (A)
        i = C->encabezamientos_conjunto[B-1].primer_elemento;
        do {
            C->nombres[i].nombre_conjunto = A;
            i = C->nombres[i].siguiente_elemento;
        } while (i != 0);

        //Actualiza la lista enlazada del conjunto mas grande (A)
        C->nombres[i].nombre_conjunto = A;
        C->nombres[i].siguiente_elemento = C->encabezamientos_conjunto[A-1].primer_elemento;

        //Actualiza el encabezado del conjunto mas grande (A) con la informacion del conjunto absorbido (B)
        C->encabezamientos_conjunto[A-1].primer_elemento = C->encabezamientos_conjunto[B-1].primer_elemento;
        C->encabezamientos_conjunto[A-1].cuenta += C->encabezamientos_conjunto[B-1].cuenta;
    } else {
        //Similar al bloque anterior, pero intercambia A y B en la logica.
        i = C->encabezamientos_conjunto[A-1].primer_elemento;
        do {
            C->nombres[i].nombre_conjunto = B;
            i = C->nombres[i].siguiente_elemento;
        } while (C->nombres[i].siguiente_elemento != 0);

        C->nombres[i].nombre_conjunto = B;
        C->nombres[i].siguiente_elemento = C->encabezamientos_conjunto[B-1].primer_elemento;

        C->encabezamientos_conjunto[B-1].primer_elemento = C->encabezamientos_conjunto[A-1].primer_elemento;
        C->encabezamientos_conjunto[B-1].cuenta += C->encabezamientos_conjunto[A-1].cuenta;
    }
}

//Encuentra el nombre del conjunto de un vertice dado
tipo_nombre encuentra(int x, conjunto_CE * C){
    return C->nombres[x-1].nombre_conjunto;
}

//Funcion para listar un arbol
void lista(rama * r){
    printf("u: %d | v: %d | c: %d\n", (*r).a.u, (*r).a.v, (*r).a.costo);
    if ((*r).sig)
        lista((*r).sig);
}

//Funcion para listar el conjunto combina-encuentra
void listaCE(conjunto_CE ** c){
    for (int i = 0; i < VERTICES; i++)
        printf("nom: %d sig: %d cuenta: %d primer: %d\n",(*c)->nombres[i].nombre_conjunto,(*c)->nombres[i].siguiente_elemento, (*c)->encabezamientos_conjunto[i].cuenta, (*c)->encabezamientos_conjunto[i].primer_elemento);
}

//Funcion del algoritmo de Kruskal
void kruskal(rama ** r, rama ** rn){

    conjunto_CE * c = (conjunto_CE *)malloc(sizeof(conjunto_CE));   //Reserva de memoria para el conjunto combina-encuentra
    int comp_u, comp_v;     //Declaracion de componentes de u y v

    inicial(&c);            //Inicializacion del conjunto combina-encuentra
    
    //listaCE(&c);          //Print del estado actual del conjunto CE

    // Bucle principal para ejecutar el algoritmo de Kruskal
    for (int i = VERTICES; i > 1;) {
        // Se reserva memoria para almacenar la arista mínima
        arista *a = (arista *)malloc(sizeof(arista));
        sacar_min(r, &a);

        // Se encuentra el componente al que pertenecen los vértices u y v
        comp_u = encuentra(a->u, c);
        comp_v = encuentra(a->v, c);

        // Si los vértices pertenecen a diferentes componentes, se combinan
        if (comp_u != comp_v) {
            combina(comp_u, comp_v, c);
            // Se inserta la arista en el árbol de expansión mínima resultante
            inserta(a->u, a->v, a->costo, rn);
            i--;
        }
        // Se libera la memoria de la arista
        free(a);
    }
}

//Funcion para insertar una arista en un arbol binario de busqueda ordenado por costo
void inserta(int u, int v, int c, rama **r) {
    rama *nodo = (rama *)malloc(sizeof(rama));

    //Si el arbol esta vacio, se crea el primer nodo con la informacion de la arista
    if ((*r) == NULL) {
        nodo->a.u = u;
        nodo->a.v = v;
        nodo->a.costo = c;
        nodo->sig = NULL;
        (*r) = nodo;
    } else {
        //Si el costo de la arista es menor o igual al costo de la raiz actual, se inserta a la izquierda
        if (c <= (*r)->a.costo) {
            nodo->a.u = u;
            nodo->a.v = v;
            nodo->a.costo = c;
            nodo->sig = (*r);
            (*r) = nodo;
        } else {
            //Si el costo es mayor, se busca la posicion correcta en el subarbol derecho
            inserta(u, v, c, &(*r)->sig);
        }
    }
}

// Función para sacar la arista con el costo mínimo del árbol y eliminar el nodo correspondiente
void sacar_min(rama **r, arista **arista) {
    (*(*arista)) = (*r)->a;  // Se copia la información de la arista
    rama *delete = (*r);     // Se guarda el nodo a eliminar
    (*r) = (*r)->sig;        // Se actualiza la raíz del árbol
    free(delete);            // Se libera la memoria del nodo eliminado
}


//Funcion main, comienzo del programa.
int main(){

    int M_Costos[VERTICES][VERTICES];   //Se declara un array bidimensional para guardar los costos de las aristas
    rama * arbol = NULL;                //Arbol para insertar las aristas
    rama * arbolNuevo = NULL;           //Variable donde se guardara el arbol minimo recubridor resultante

    //Se ingresan los costos
    for(int i = 0; i <= VERTICES - 1; i++)
        for(int j = i+1; j <= VERTICES - 1; j++){
            printf("Ingrese costo de lado entre vertices %d y %d: ", i, j);
            scanf("%d", &M_Costos[i][j]);
        }

    //Se insertan las aristas
    for(int i = 0; i <= VERTICES - 1; i++)
        for(int j = i+1; j <= VERTICES - 1; j++)
            if(M_Costos[i][j] != 0)
                inserta(i, j, M_Costos[i][j], &arbol); //inserto en cola prior.

    kruskal(&arbol,&arbolNuevo);    //Ejecucion del algoritmo de K-ruskal
    printf("\nArbol minimo recubridor:\n-----------------------\n");
    lista(arbolNuevo);
    return 0;
}