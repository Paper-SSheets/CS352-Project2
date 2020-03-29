#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>


/*
  READ ME
  
  This file contains all methods for project 2 in 352. In the main method is considered the main thread.
  From the main thread it branches off into 5 other threads, reader, writer, encryption, input counter and
  output counter. Over all the project reads from a file counts all alphabetic characters, encrypts them,
  writes those to an output file, and counts the alphabetic characters in the output file. For input and output
  the buffers are handled in arrays.

*/
sem_t full, empty, mutex, enc, unenc, mutexOut, fullOut, emptyOut, counted, uncounted;

int consumPos = 0;
int producePos = 0;
int consumPosOut = 0;
int producePosOut = 0;

int countOut = 0;
int encryptPos = 0;

int bufferSize = 0;

char *inBuffer;
char *outBuffer;

//Reader method writes characters from input file into the inBuffer
void *reader(void *in)
{

    FILE *f = fopen(in, "r");

    char c = 1;

    do
    {

        c = getc(f);

        sem_wait(&enc);
        sem_wait(&empty);
        sem_wait(&mutex);

        inBuffer[producePos] = c;
        producePos = (producePos + 1) % bufferSize;

        sem_post(&mutex);
        sem_post(&full);
        sem_post(&unenc);


    } while (c != EOF);

    pthread_exit(NULL);

}

//Writer method writes characters from outBuffer into the out file
void *writer(void *in)
{


    FILE *f = fopen(in, "w");

    char c = 1;

    while (c != EOF)
    {

        sem_wait(&fullOut);
        sem_wait(&counted);
        //sem_wait(&mutexOut);

        c = outBuffer[consumPosOut];
        consumPosOut = (consumPosOut + 1) % bufferSize;

        //sem_post(&mutexOut);
        sem_post(&uncounted);
        sem_post(&emptyOut);

        if (c != -1)
        {
            fputc(c, f);
        }

    }

    pthread_exit(NULL);
}

//Encrypt reads from the inBuffer. Then with out locking anything in a non critcal section encrypts a the character.
//Then it will write that encrypted character into the Outbuffer. 
void *encrypt(void *in)
{

    char c = 1;
    int s = 1;

    do
    {

        //Read from inBuffer
        sem_wait(&unenc);
        sem_wait(&mutex);

        c = inBuffer[encryptPos];
        encryptPos = (encryptPos + 1) % bufferSize;

        sem_post(&mutex);
        sem_post(&enc);

        //Encrypt
        if (c > 64 && c < 91)
        {
            if (s == 1)
            {
                if (c == 90)
                {
                    c = 65;
                } else
                {
                    c++;
                }
                s = -1;
            } else if (s == -1)
            {
                if (c == 65)
                {
                    c = 90;
                } else
                {
                    c--;
                }
                s = 0;
            } else
            {
                s = 1;
            }
        } else if (c > 96 && c < 123)
        {
            if (s == 1)
            {
                if (c == 122)
                {
                    c = 97;
                } else
                {
                    c++;
                }
                s = -1;
            } else if (s == -1)
            {
                if (c == 97)
                {
                    c = 122;
                } else
                {
                    c--;
                }
                s = 0;
            } else
            {
                s = 1;
            }
        }



        //Write to outBuffer
        sem_wait(&emptyOut);
        sem_wait(&mutexOut);

        outBuffer[producePosOut] = c;
        producePosOut = (producePosOut + 1) % bufferSize;

        sem_post(&mutexOut);
        sem_post(&fullOut);


    } while (c != EOF);

    pthread_exit(NULL);

}

//Counts all alphabetic characters in rhw inBuffer.
void *inputCounter(void *in)
{

    char c = 1;
    char alphabet[26][1];

    int i = 0;

    for (i = 0; i < 26; i++)
    {
        alphabet[i][0] = 0;
    }

    do
    {

        sem_wait(&full);
        sem_wait(&mutex);

        c = inBuffer[consumPos];
        consumPos = (consumPos + 1) % bufferSize;

        sem_post(&mutex);
        sem_post(&empty);

        if (c > 96 && c < 123)
        {
            c -= 32;
        }

        if (c > 64 && c < 91)
        {
            alphabet[c - 65][0]++;
        }


    } while (c != EOF);

    printf("Input file contains\n");

    for (i = 0; i < 26; i++)
    {
        if (alphabet[i][0] != 0)
        {
            printf("%c: %d\n", i + 65, alphabet[i][0]);
        }
    }

    pthread_exit(NULL);

}

//Counts all alphabetic characters in rhw outBuffer.
void *outputCounter(void *in)
{

    char c = 1;

    char alphabet[26][1];

    int i = 0;

    for (i = 0; i < 26; i++)
    {
        alphabet[i][0] = 0;
    }

    do
    {

        sem_wait(&uncounted);
        sem_wait(&mutexOut);

        c = outBuffer[countOut];
        countOut = (countOut + 1) % bufferSize;

        sem_post(&mutexOut);
        sem_post(&counted);

        if (c > 96 && c < 123)
        {
            c = c - 32;
        }

        if (c > 64 && c < 91)
        {
            alphabet[c - 65][0]++;
        }


    } while (c != EOF);

    printf("Output file contains\n");

    for (i = 0; i < 26; i++)
    {
        if (alphabet[i][0] != 0)
        {
            printf("%c: %d\n", i + 65, alphabet[i][0]);
        }
    }
    pthread_exit(NULL);
}

//Main thread that inits all semaphores and gets the inFile, outFile, and bufferSize from the user. Then it will create all threads.
int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        printf("USAGE ./encrytp [infile] [outfile]\n");
        return 0;
    }

    printf("Enter buffer size: ");
    scanf("%d", &bufferSize);

    inBuffer = malloc(bufferSize);
    outBuffer = malloc(bufferSize);

    sem_init(&mutex, 0, 1);
    sem_init(&full, 0, 0);
    sem_init(&unenc, 0, 0);
    sem_init(&empty, 0, bufferSize);
    sem_init(&enc, 0, bufferSize);
    sem_init(&mutexOut, 0, 1);
    sem_init(&fullOut, 0, 0);
    sem_init(&emptyOut, 0, bufferSize);
    sem_init(&counted, 0, 0);
    sem_init(&uncounted, 0, bufferSize);

    pthread_t t1, t2, t3, t4, t5;

    pthread_create(&t1, NULL, reader, argv[1]);
    pthread_create(&t2, NULL, encrypt, NULL);
    pthread_create(&t3, NULL, inputCounter, NULL);
    pthread_create(&t4, NULL, writer, argv[2]);
    pthread_create(&t5, NULL, outputCounter, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    pthread_join(t5, NULL);


}


