/*---------------------------------------------------------------------------
-					   COM S 352: Project 2 - Multi-Threading               -
-	Name: Steven Marshall Sheets                                            -
- 	Section: A-2                                                            -
-	NetID: smsheets@iastate.edu                                             -
-	Date: 3/10/2020                                                         -
----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
-								Includes								    -
----------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

/*----------------------------------------------------------------------------
-	                                Notes                                    -
-----------------------------------------------------------------------------*/
/* This file parses the input string inside of the main loop by taking in
 * each command. Once these have been parsed, a child process is created to
 * be executed each command. These commands also have the functionality to
 * be inputted and outputted into files, and pipeline the commands into
 * other commands. View README.md for sample usages.                         */

/*----------------------------------------------------------------------------
-								 Implementation                              -
-----------------------------------------------------------------------------*/
int main()
{
    char *args[80 / 2 + 1]; /* Command line arguments */
    args[0] = NULL;
    char *args2[80 / 2 + 1];

    int word_position = 0;
    int argc = 0;
    int argc2 = 0;

    bool is_running = true; /* Exit shell if "exit" is inputted by the user */
    bool input = false;     /* True if '<', False otherwise */
    bool output = false;    /* True if '>', False otherwise */
    bool piped = false;     /* True if '|', False otherwise */
    bool first_or_second;   /* True if first, False if second */

    getpid();

    while (is_running)
    {
        printf("osh>");
        fflush(stdout);

        char tmp[100];
        char tmp2[50];
        char history[3];

        fgets(tmp, 50, stdin);

        if (strlen(tmp) == 3)
        {
            history[0] = tmp[0];
            history[1] = tmp[1];
            history[2] = 0;
        }

        if (strcmp(history, "!!") != 0)
        {
            input = output = piped = first_or_second = false;
            if (strstr(tmp, "<"))
            {
                input = true;
            } else if (strstr(tmp, "|"))
            {
                piped = true;
            } else if (strstr(tmp, ">"))
            {
                output = true;
            }

            word_position = argc = argc2 = 0;
            int i = 0;
            for (i = 0; i < strlen(tmp) - 1; i++)
            {
                if (i == strlen(tmp) - 2)
                {
                    tmp2[word_position] = tmp[i];
                    word_position++;
                    tmp2[word_position] = 0;
                    if (first_or_second)
                    {
                        args2[argc2] = (char *) malloc(sizeof(word_position));
                        strcpy(args2[argc2], tmp2);
                    } else
                    {
                        args[argc] = (char *) malloc(sizeof(word_position));
                        strcpy(args[argc], tmp2);
                    }
                } else if (tmp[i] == '<' || tmp[i] == '|' || tmp[i] == '>')
                {
                    word_position = 0;
                    first_or_second = true;
                    i++;
                    argc--;
                } else if (tmp[i] == ' ')
                {
                    tmp2[word_position] = 0;
                    if (first_or_second)
                    {
                        args2[argc] = (char *) malloc(sizeof(word_position));
                        strcpy(args2[argc], tmp2);
                        argc2++;
                    } else
                    {
                        args[argc] = (char *) malloc(sizeof(word_position));
                        strcpy(args[argc], tmp2);
                        argc++;
                    }
                    word_position = 0;
                } else
                {
                    tmp2[word_position] = tmp[i];
                    word_position++;
                }
            }

            args[argc + 1] = (char *) NULL;
            args2[argc2 + 1] = (char *) NULL;
            if (!strcmp(args[0], "exit"))
            {
                is_running = false;
            } else if (output || input)
            {
                pid_t pid = fork();
                if (pid)
                {
                    wait(NULL);
                } else
                {
                    int file_descriptor = open(args2[0], O_RDWR | O_CREAT, S_IROTH | S_IRWXU);
                    if (output)
                    {
                        dup2(file_descriptor, 1);
                    } else
                    {
                        dup2(file_descriptor, 0);
                    }
                    execvp(args[0], args);
                    printf("An error has occurred.\n");
                    exit(1);
                }
            } else if (piped)
            {
                int pipe_file_descriptor[2];
                if (pipe(pipe_file_descriptor) < 0)
                {
                    printf("A pipe error has occurred.\n");
                    return 1;
                }

                int pid2 = fork();
                if (pid2 == 0) // Child 2
                {
                    close(pipe_file_descriptor[1]);
                    dup2(pipe_file_descriptor[0], 0);
                    execvp(args2[0], args2);
                    printf("An error has occurred.\n");
                    exit(1);
                }

                int pid1 = fork();
                if (pid1 == 0) // Child 1
                {
                    close(pipe_file_descriptor[0]);
                    dup2(pipe_file_descriptor[1], 1);
                    execvp(args[0], args);
                    printf("An error has occurred.\n");
                    exit(1);
                }

                // Parent
                close(pipe_file_descriptor[0]);
                close(pipe_file_descriptor[1]);

                waitpid(pid1, NULL, 0);
                waitpid(pid2, NULL, 0);
            } else
            {
                pid_t pid = fork();
                if (pid)
                {
                    wait(NULL);
                } else
                {
                    execvp(args[0], args);
                    printf("An error has occurred.\n");
                    exit(1);
                }
            }
        } else
        {
            history[0] = 0;
            if (!args[0])
            {
                printf("No commands in history.\n");
            } else if (output || input)
            {
                pid_t pid = fork();
                if (pid)
                {
                    wait(NULL);
                } else
                {
                    int file_descriptor = open(args2[0], O_RDWR | O_CREAT, S_IROTH | S_IRWXU);
                    if (output)
                    {
                        dup2(file_descriptor, 1);
                    } else
                    {
                        dup2(file_descriptor, 0);
                    }
                    execvp(args[0], args);
                    printf("An error has occurred.\n");
                    exit(1);
                }
            } else if (piped)
            {
                int pipe_file_descriptor[2];
                if (pipe(pipe_file_descriptor) < 0)
                {
                    printf("A pipe error has occurred.\n");
                    return 1;
                }

                int pid2 = fork();
                if (pid2 == 0) // Child 2
                {
                    close(pipe_file_descriptor[1]);
                    dup2(pipe_file_descriptor[0], 0);
                    execvp(args2[0], args2);
                    printf("A pipe error has occurred.\n");
                    exit(1);
                }

                int pid1 = fork();
                if (pid1 == 0) // Child 1
                {
                    close(pipe_file_descriptor[0]);
                    dup2(pipe_file_descriptor[1], 1);
                    execvp(args[0], args);
                    printf("An error has occurred.\n");
                    exit(1);
                }

                // Parent
                close(pipe_file_descriptor[0]);
                close(pipe_file_descriptor[1]);

                waitpid(pid1, NULL, 0);
                waitpid(pid2, NULL, 0);
            } else
            {
                pid_t pid = fork();
                if (pid)
                {
                    wait(NULL);
                } else
                {
                    execvp(args[0], args);
                    printf("An error has occurred.\n");
                    exit(1);
                }
            }
        }
    }

    return 0;
}
