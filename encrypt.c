/*---------------------------------------------------------------------------
-		COM S 352: Project 2 - Multi-Threaded Text File Encryptor           -
-	Name: Steven Marshall Sheets                                            -
- 	Section: A-2                                                            -
-	NetID: smsheets@iastate.edu                                             -
-	Date: 3/20/2020                                                          -
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
-								Includes									 -
-----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <semaphore.h>

/*----------------------------------------------------------------------------
-	                                Notes                                    -
-----------------------------------------------------------------------------*/
/* This file contains all required methods for Project II of COM S 352:
 * Introduction to Operating Systems. The main method is to be considered
 * the main thread. From the main thread, it will branch off into 5 unique
 * threads - reader, writer, encryptor, input counter, and output counter.
 * The project will read from a file, count all alphabetic characters,
 * encryptor them, write that to an output file, and count the alphabetic
 * characters in the output file.
 */

/*---------------------------------------------------------------------------
-								Globals                                     -
----------------------------------------------------------------------------*/
sem_t full, empty, mutex, encrypted, unencrypted, mutex_out, full_out, empty_out, counted, uncounted;
int consume_pos_in, produce_pos_in, consume_pos_out, produce_pos_out, count_out, encrypt_pos, buffer_size;
char *in_buffer, *out_buffer;

/*---------------------------------------------------------------------------
-								Prototypes                                  -
----------------------------------------------------------------------------*/
void *reader(void *in);

void *writer(void *in);

void *encryptor();

void *input_counter();

void *output_counter();

/*---------------------------------------------------------------------------
-								Implementation                              -
----------------------------------------------------------------------------*/
/**
 * Main thread that initializes all semaphores, gets the input file, selects output file name,
 * and the size of the buffer from the user. Afterwards, all threads will be created.
 *
 * @param argc - Number of parameters given in the command line
 * @param argv - Parameters given on the command line. Should be in the following form:
 *               ./executable [infile] [outfile]
 * @return - 0 if no errors, error if error
 */
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("USAGE: ./encryptor [infile] [outfile]\n");
        return 0;
    }

    printf("Enter buffer size: ");
    scanf("%d", &buffer_size);

    in_buffer = malloc(buffer_size);
    out_buffer = malloc(buffer_size);

    sem_init(&mutex, 0, 1);
    sem_init(&full, 0, 0);
    sem_init(&unencrypted, 0, 0);
    sem_init(&empty, 0, buffer_size);
    sem_init(&encrypted, 0, buffer_size);
    sem_init(&mutex_out, 0, 1);
    sem_init(&full_out, 0, 0);
    sem_init(&empty_out, 0, buffer_size);
    sem_init(&counted, 0, 0);
    sem_init(&uncounted, 0, buffer_size);

    pthread_t thread1, thread2, thread3, thread4, thread5;

    pthread_create(&thread1, NULL, reader, argv[1]);
    pthread_create(&thread2, NULL, encryptor, NULL);
    pthread_create(&thread3, NULL, input_counter, NULL);
    pthread_create(&thread4, NULL, writer, argv[2]);
    pthread_create(&thread5, NULL, output_counter, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    pthread_join(thread4, NULL);
    pthread_join(thread5, NULL);

    return 0;
}

/**
 * The reader thread will write characters from the input file
 * into the input buffer, in_buffer.
 *
 * @param in - The input file
 */
void *reader(void *in)
{
    FILE *f = fopen(in, "r");
    char c;

    do
    {
        c = getc(f);

        sem_wait(&encrypted);
        sem_wait(&empty);
        sem_wait(&mutex);

        in_buffer[produce_pos_in] = c;
        produce_pos_in = (produce_pos_in + 1) % buffer_size;

        sem_post(&mutex);
        sem_post(&full);
        sem_post(&unencrypted);
    } while (c != EOF);

    pthread_exit(NULL);
}

/**
 * The writer thread will write characters from the out_buffer into
 * the output file given by the user.
 *
 * @param in - The input file.
 */
void *writer(void *in)
{
    FILE *f = fopen(in, "w");
    char c = 1;

    while (c != EOF)
    {
        sem_wait(&full_out);
        sem_wait(&counted);

        c = out_buffer[consume_pos_out];
        consume_pos_out = (consume_pos_out + 1) % buffer_size;

        sem_post(&uncounted);
        sem_post(&empty_out);

        if (c != -1)
        {
            fputc(c, f);
        }
    }

    pthread_exit(NULL);
}


/**
 * The encryptor thread will read from the in_buffer. Afterwards, without locking anything
 * within a non-critical section, will encrypt a character. Then, it will write that
 * encrypted character into the out_buffer.
 */
void *encryptor()
{
    char c;
    int s = 1;

    do
    {
        // Read from in_buffer
        sem_wait(&unencrypted);
        sem_wait(&mutex);

        c = in_buffer[encrypt_pos];
        encrypt_pos = (encrypt_pos + 1) % buffer_size;

        sem_post(&mutex);
        sem_post(&encrypted);

        // Encryption
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

        // Write to out_buffer
        sem_wait(&empty_out);
        sem_wait(&mutex_out);

        out_buffer[produce_pos_out] = c;
        produce_pos_out = (produce_pos_out + 1) % buffer_size;

        sem_post(&mutex_out);
        sem_post(&full_out);
    } while (c != EOF);

    pthread_exit(NULL);
}

/**
 * The input_counter thread simply counts all alphabetic characters
 * within the in_buffer.
 */
void *input_counter()
{
    char c;
    char alphabet[26][1];

    for (int i = 0; i < 26; i++)
    {
        alphabet[i][0] = 0;
    }

    do
    {
        sem_wait(&full);
        sem_wait(&mutex);

        c = in_buffer[consume_pos_in];
        consume_pos_in = (consume_pos_in + 1) % buffer_size;

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

    for (int i = 0; i < 26; i++)
    {
        if (alphabet[i][0] != 0)
        {
            printf("%c: %d\n", i + 65, alphabet[i][0]);
        }
    }

    pthread_exit(NULL);
}

/**
 * The output_counter thread simply counts all alphabetic characters
 * within the output_buffer.
 */
void *output_counter()
{
    char c;
    char alphabet[26][1];

    for (int i = 0; i < 26; i++)
    {
        alphabet[i][0] = 0;
    }

    do
    {
        sem_wait(&uncounted);
        sem_wait(&mutex_out);

        c = out_buffer[count_out];
        count_out = (count_out + 1) % buffer_size;

        sem_post(&mutex_out);
        sem_post(&counted);

        if (c > 96 && c < 123)
        {
            c -= 32;
        }
        if (c > 64 && c < 91)
        {
            alphabet[c - 65][0]++;
        }
    } while (c != EOF);

    printf("Output file contains\n");

    for (int i = 0; i < 26; i++)
    {
        if (alphabet[i][0] != 0)
        {
            printf("%c: %d\n", i + 65, alphabet[i][0]);
        }
    }
    pthread_exit(NULL);
}
