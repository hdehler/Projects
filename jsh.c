//Harrison Dehler
//COSC 360
//Lab-8-Jsh
//This program I make my own shell
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "fields.h"
#include "jrb.h"

//setup the command and pipe arrays
void createArr(struct inputstruct *is, int *pipeArr, int *cmds, int pipeCount) {
    cmds[0] = 0;
    int cmdCount = 0;
    //iterate through fields and identify pipe positions
    for (int i = 0; i < is->NF; i++) {
        if (strcmp("|", is->fields[i]) == 0) {
            cmds[++cmdCount] = i + 1;
            is->fields[i] = NULL;
        }
    }
    cmdCount++;

    for (int i = 0; i < pipeCount; i++) {
        pipe(pipeArr + i * 2);
    }
}

//process file redirection operators in the input
void processFiles(struct inputstruct *is, char **iFile, char **oFile,
                  bool *needsAppend) {
    for (int i = 0; i < is->NF; i++) {
        if (is->fields[i] == NULL) {
            continue;
        }

        //identify the file redirection operator that has been used in line
        if (strcmp(is->fields[i], "<") == 0) {
            *iFile = is->fields[i + 1];
            is->fields[i++] = NULL;
        } else if (strcmp(is->fields[i], ">") == 0) {
            *oFile = is->fields[i + 1];
            is->fields[i++] = NULL;
        } else if (strcmp(is->fields[i], ">>") == 0) {
            *oFile = is->fields[i + 1];
            *needsAppend = true;
            is->fields[i++] = NULL;
        }
    }
}

//setup file descriptors and execute commands in the child process
void child_process(struct inputstruct *is, int *pipeArr, int *cmds,
                   int pipeCount, int i, char *iFile, char *oFile,
                   bool needsAppend) {
    int fileDes1, fileDes2;

    // Redirect input if the current command is the first and an input file is provided
    if (i == 0 && iFile != NULL) {
        fileDes1 = open(iFile, O_RDONLY);

        if (fileDes1 < 0) {
            perror(iFile);
            exit(1);
        }
        if (dup2(fileDes1, 0) != 0) {
            perror(iFile);
            exit(1);
        }
        close(fileDes1);
    }

    // Redirect output if the current command is the last and an output file is provided
    if (i == pipeCount && oFile != NULL) {
        int openFlag;

        if (needsAppend) {
            openFlag = O_WRONLY | O_CREAT | O_APPEND;
        } else {
            openFlag = O_WRONLY | O_TRUNC | O_CREAT;
        }

        fileDes2 = open(oFile, openFlag, 0666);

        if (fileDes2 < 0) {
            perror(oFile);
            exit(1);
        }
        if (dup2(fileDes2, 1) != 1) {
            perror(oFile);
            exit(1);
        }
        close(fileDes2);
    } else if (i == pipeCount) {
        dup2(pipeArr[i * 2 - 2], 0);
    }

    if (i != 0) {
        dup2(pipeArr[i * 2 - 2], 0);
    }

    if (i != pipeCount) {
        dup2(pipeArr[i * 2 + 1], 1);
    }

    for (int i = 0; i < pipeCount * 2; i++) {
        close(pipeArr[i]);
    }

    is->fields[is->NF] = NULL;

    execvp(is->fields[cmds[i]], &(is->fields[cmds[i]]));
    perror(is->fields[cmds[i]]);
    exit(EXIT_FAILURE);
}

//parent process handles closing the pipe file descriptors and waiting for child processes
void parent_process(JRB childids, int *pipeArr, int pipeCount, bool toWait) {
    for (int i = 0; i < pipeCount * 2; i++) {
        close(pipeArr[i]);
    }

    int wstatus;
    //wait for all child processes to finish if true
    if (toWait) {
        while (!jrb_empty(childids)) {
            int pid = wait(&wstatus);
            //if process is found in the tree then delete
            if (jrb_find_int(childids, pid) != NULL) {
                jrb_delete_node(jrb_find_int(childids, pid));
            }
        }
    }
}

int main(int argc, char **argv) {
    struct inputstruct *is;
    is = new_inputstruct(NULL);

    int *pipeArr;
    int *cmds;
    int pipeCount;
    char *oFile;
    char *iFile;

    //determine print prompt or not
    const bool printPrompt = argc == 1 || argv[1][0] != '-';

    if (printPrompt) {
        printf("jsh: ");
    }

    bool needsAppend = false;
    //read input lines
    while (get_line(is) != -1) {
        pipeCount = 0;

        //counting number of pipes in the command
        for (int i = 0; i < is->NF; i++) {
            if (strcmp("|", is->fields[i]) == 0) {
                pipeCount++;
            }
        }

        //allocate memory for pipe and command arrays
        pipeArr = malloc(sizeof(int) * (pipeCount)*2);
        cmds = malloc(sizeof(int) * (pipeCount + 1));

        createArr(is, pipeArr, cmds, pipeCount);

        //when the command is not empty then we need to process it
        if (is->fields[0] != NULL && strlen(is->fields[0]) > 0) {
            bool toWait = strcmp("&", is->fields[is->NF - 1]) != 0;
            is->NF -= !toWait;
            is->fields[is->NF] = NULL;

            oFile = NULL;
            iFile = NULL;
            needsAppend = false;

            processFiles(is, &iFile, &oFile, &needsAppend);

            JRB childids = make_jrb();

            //fork and execute the child processes
            for (int i = 0; i < pipeCount + 1; i++) {
                fflush(stdin);
                fflush(stdout);
                fflush(stderr);
                int fork_ret = fork();

                if (fork_ret == 0) {
                    child_process(is, pipeArr, cmds, pipeCount, i, iFile, oFile,
                                  needsAppend);
                } else {
                    jrb_insert_int(childids, fork_ret, JNULL);
                }
            }

            parent_process(childids, pipeArr, pipeCount, toWait);

            jrb_free_tree(childids);
        }

        if (printPrompt) {
            printf("jsh: ");
        }
        free(pipeArr);
        free(cmds);
    }

    jettison_inputstruct(is);
    exit(1);
}
