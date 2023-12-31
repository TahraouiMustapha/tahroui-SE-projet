#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>


#define N 10// places dans le buffer

//les matrices
int B[100][100];
int C[100][100];
int A[100][100];

//the buffer
int T[N];
int indexTampon = 0;

typedef struct argument{
    int ligne;
    int colonne;
}argument;


//pour la synchronisation 
pthread_mutex_t mutex;
sem_t empty;
sem_t full;

//the fonctions:
void remplir(int tab[][100],int n,int m);
int estValide(int m1,int n2);
void afficheMatrice(int tab[][100],int n,int m);



// Producer
void* producer(void* arg)
{
    argument a = *(argument*)arg;
    int i,k;
    //pour chaque ligne
    for(i = 0 ; i < a.colonne ; i++)
    {
        int sum = 0;
        for(k = 0 ; k<a.colonne ; k++)
        {
            sum += B[a.ligne][k] * C[k][i];
        }
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

            T[indexTampon] = sum;
            indexTampon++;

        pthread_mutex_unlock(&mutex);
        sem_post(&full); 
    }
}

//Consumer
void* consumer(void* arg)
{
    argument a = *(argument*)arg;
    int i,j;
    for(i=0;i<a.colonne;i++)
    {         
        //remove the value from the buffer:
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
            A[a.ligne][i] = T[indexTampon-1];
            indexTampon--; 
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
}

int main ()
{
    int n1,m1,n2,m2;
    printf("entrer le nombre des lignes de la matrice B:");
    scanf("%d",&n1);
    printf("entrer le nombre des colonnes de la matrice B:");
    scanf("%d",&m1);
    remplir(B,n1,m1);

    printf("entrer le nombre des lignes de la matrice C:");
    scanf("%d",&n2);
    printf("entrer le nombre des colonnes de la matrice C:");
    scanf("%d",&m2);
    remplir(C,n2,m2);

    if(!estValide(m1,n2))
    {
        printf("\nle produit est impossible!\n");
        exit(1);
    }
    
    //affiche les matrices:
    printf("affiche le matrice B:\n");
    afficheMatrice(B,n1,m1);
    printf("affiche le matrice C:\n");
    afficheMatrice(C,n2,m2);

    // Initialisation
    pthread_mutex_init(&mutex,NULL);
    sem_init(&empty, 0 , N);  // buffer vide
    sem_init(&full, 0 , 0);   // buffer plein

    //creation des threads
    pthread_t threads[10];
    int i,j;
    argument arg;
    arg.colonne = m2;// the number the column (A)

    for(i=0 ; i<10; i++)
    {
        arg.ligne = i;
        pthread_create(&threads[i],NULL,&producer,&arg);
        pthread_create(&threads[i],NULL,&consumer,&arg);
        //attente des threads
        pthread_join(threads[i], NULL);
    }

    
   //Affiche A:
    printf("Matrice A:\n");
    afficheMatrice(A,n1,m2);

    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);
    
    return 0;
}

void remplir(int tab[][100],int n,int m) {
    int i,j;
    for(i=0;i<n;i++)
    {
        for(j=0;j<m;j++)
        {
            tab[i][j]=rand()%10;
        }
    }
}

int estValide(int m1,int n2) {
    return m1 == n2;
}

void afficheMatrice(int tab[][100],int n,int m)
{
    int i,j;
    for(i=0;i<n;i++)
    {
        for(j=0;j<m;j++)
        {
            printf("%d ",tab[i][j]);
        }
        printf("\n");
    }
}
