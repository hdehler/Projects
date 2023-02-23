//Harrison Dehler
//COSC 360
//February 27, 2023
//In this lab I made a makefile
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "dllist.h"
#include "fields.h"
#include "jrb.h"
#include "jval.h"

typedef struct makeFiles {
    Dllist cFile;
    Dllist hFile;
    Dllist libList;
    Dllist flags;
} files;

int main(int argc, char *argv[]) {
    files *file;

    // allocate memory
    file = malloc(sizeof(files));
    file->cFile = new_dllist();
    file->hFile = new_dllist();
    // file->oFile = new_dllist();
    file->libList = new_dllist();
    file->flags = new_dllist();

    char *executable;

    IS is;
    if (argc == 1) {
        is = new_inputstruct("fmakefile");
    }

    if (argc == 2) {
        is = new_inputstruct(argv[1]);
    }
    // if the input file is empty
    if (is == NULL) {
        fprintf(stderr, argv[1]);
        exit(1);
    }

    // reading the file and print to stdout
    int exeCount = 0;
    while (get_line(is) >= 0) {
        if (is->NF == 0) {
            continue;
        }
        if (is->fields[0][0] == 'C') {
            for (int i = 1; i < is->NF; i++) {
                dll_append(file->cFile, new_jval_s(strdup(is->fields[i])));  // do this for C H and L
            }
        } else if (is->fields[0][0] == 'H') {
            for (int i = 1; i < is->NF; i++) {
                dll_append(file->hFile, new_jval_s(strdup(is->fields[i])));
            }
        } else if (is->fields[0][0] == 'L') {
            for (int i = 1; i < is->NF; i++) {
                dll_append(file->libList, new_jval_s(strdup(is->fields[i])));
            }
        } else if (is->fields[0][0] == 'E') {
            executable = strdup(is->fields[1]);
            exeCount++;
            if(exeCount > 1){
            fprintf(stderr, "fmakefile (%d) cannot have more than one E line\n", is->line);
            exit(1);
            }
        } else if (is->fields[0][0] == 'F') {
            for (int i = 1; i < is->NF; i++) {
                dll_append(file->flags, new_jval_s(strdup(is->fields[i])));
            }
        } else {
            fprintf(stderr,
                    "fakemake (%d): Lines must start with C, H, E, F, or L\n",
                    is->line);
            exit(1);
        }
    }
    if (exeCount == 0) {
        fprintf(stderr, "No executable specified\n");
        exit(1);
    }


    Dllist tmp;
    time_t max = -1;
    time_t time;
    //calling stat on all hfiles and getting the max time
    dll_traverse(tmp, file->hFile) {
        struct stat hf;
        if (stat(tmp->val.s, &hf) == 0) {
            time = hf.st_mtime;
            if (time > max) {
                max = time;
            }
        } else {
            fprintf(stderr, "No file found for H-File\n");
            exit(1);
        }
    }

    int need_compile = 0;
    Dllist of_names = new_dllist();

    struct stat cf, of, ef;
    int execRet = stat(executable, &ef);

    // traversing the dllist of C files and calling stat
    dll_traverse(tmp, file->cFile) {
        int ret;
        // Check if the C file exists
        if (stat(tmp->val.s, &cf) == -1) {
            fprintf(stderr, "fmakefile: %s: No such file or directory\n", tmp->val.s);
            exit(1);
        }

        // Create the .o filename
        int len = strlen(tmp->val.s);
        char of_name[len + 1];
        strncpy(of_name, tmp->val.s, len - 1);
        of_name[len - 1] = 'o';
        of_name[len] = '\0';
        dll_append(of_names, new_jval_s(strdup(of_name)));

        ret = stat(of_name, &of);

        // if there is no o file
        if (ret == -1) {
            // printf("Compiling %s\n", tmp->val.s);
            // Create the gcc -c command
            char *command = (char *)malloc(1000);
            strcpy(command, "gcc -c ");

            // Add flags, if any
            Dllist tmp2;
            dll_traverse(tmp2, file->flags) {
                if (tmp2->val.s) {
                    strcat(command, tmp2->val.s);
                    strcat(command, " ");
                }
            }

            strcat(command, tmp->val.s);

            // Execute the command
            printf("%s\n", command);
             if(system(command) != 0){
                fprintf(stderr, "Command failed.  Exiting\n");
                exit(1);
            }
            free(command);
            need_compile = 1;
        }

        // if there is an o file but the c file is more recent
        else if (cf.st_mtime > of.st_mtime) {
            // printf("Remaking %s\n", of_name);
            // Create the gcc -c command
            char *command = (char *)malloc(1000);
            strcpy(command, "gcc -c ");

            // Add flags, if any
            Dllist tmp2;
            dll_traverse(tmp2, file->flags) {
                if (tmp2->val.s) {
                    strcat(command, tmp2->val.s);
                    strcat(command, " ");
                }
            }
            strcat(command, tmp->val.s);

            // Execute the command
            printf("%s\n", command);
            if(system(command) != 0){
                fprintf(stderr, "Command failed.  Exiting\n");
                exit(1);
            }
            free(command);
            need_compile = 1;

        } else if (max > of.st_mtime) {
            char *command = (char *)malloc(1000);
            strcpy(command, "gcc -c ");

            // Add flags, if any
            Dllist tmp2;
            dll_traverse(tmp2, file->flags) {
                if (tmp2->val.s) {
                    strcat(command, tmp2->val.s);
                    strcat(command, " ");
                }
            }
            strcat(command, tmp->val.s);

            // Execute the command
             if(system(command) != 0){
                fprintf(stderr, "Command failed.  Exiting\n");
                exit(1);
                }
            printf("%s\n", command);
            free(command);
            need_compile = 1;
        }

        else if (execRet == -1 || of.st_mtime > ef.st_mtime) {
            need_compile = 1;
        }
    }

    if (need_compile) {
        char *command = (char *)malloc(1000);
        strcpy(command, "gcc -o ");
        strcat(command, executable);

        Dllist tmp2;
        dll_traverse(tmp2, file->flags) {
            if (tmp2->val.s) {
                strcat(command, " ");
                strcat(command, tmp2->val.s);
            }
        }
        //add o file names
        dll_traverse(tmp, of_names) {
            strcat(command, " ");
            strcat(command, tmp->val.s);
        }

        //add libraries if there are any
        Dllist tmpLib;
            dll_traverse(tmpLib, file->libList){
                if(tmpLib->val.s){
                    strcat(command, " ");
                    strcat(command, tmpLib->val.s);
                }
            }

        // Execute the command
        printf("%s\n", command);
        if(system(command) != 0){
            fprintf(stderr, "Command failed.  Fakemake exiting\n");
            exit(1);
        }
        free(command);
    } else {
        printf("%s up to date\n", executable);
    }

    //freeing all memory
    Dllist tmp6;
    dll_traverse(tmp6, file->cFile){
        free(tmp6->val.s);
    }
    dll_traverse(tmp6, file->hFile){
        free(tmp6->val.s);
    }
    dll_traverse(tmp6, file->libList){
        free(tmp6->val.s);
    }
    dll_traverse(tmp6, file->flags){
        free(tmp6->val.s);
    }
    dll_traverse(tmp6, of_names){
        free(tmp6->val.s);
    }
    

    free_dllist(file->cFile);
    free_dllist(file->hFile);
    free_dllist(file->libList);
    free_dllist(file->flags);
    free_dllist(of_names);

    free(executable);
    free(file);

    jettison_inputstruct(is);

    return 0;
}
