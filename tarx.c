// Harrison Dehler
// COSC 360
// In this lab we extract the tar file
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include "dllist.h"
#include "fields.h"
#include "jrb.h"
#include "jval.h"

typedef struct F {
    int fileName_size;
    long fileBytes_size;
    char *fileName;
    long f_inode;
    int f_mode;
    int modTime;
    long fileByte;
} f;

bool exists(JRB tree, unsigned long inode, char *fname) {
    JRB Node = jrb_find_int(tree, inode);

    // if the inode is not in the tree then insert it
    if (Node == NULL) {
        // before was new_jval_s(strdup(fname))
        jrb_insert_int(tree, inode, new_jval_s(strdup(fname)));
        return false;
    } else {
        return true;
    }
}

// make a bool so it breaks when it returns false
bool read_initial_info(f *f, JRB inodes) {
    //  reading the filesize
    int bytes_read = fread(&f->fileName_size, 1, 4, stdin);
    if (bytes_read == 0) {
        return false;
    } else if (bytes_read < 4) {
        fprintf(stderr, "Bad tar file");
        exit(1);
    }

    f->fileName = malloc(f->fileName_size + 1);

    // reading file name
    if (fread(f->fileName, 1, f->fileName_size, stdin) != f->fileName_size) {
        fprintf(stderr, "Bad tar file\n");
        exit(1);
        return false;
    }

    f->fileName[f->fileName_size] = '\0';

    // reading inode
    if (fread(&f->f_inode, 1, 8, stdin) < 8) {
        fprintf(stderr, "Bad tar file\n");
        exit(1);
        return false;
    }
    return true;
}

// funciton for when it is the first time being seen
void read_additional(f *f, JRB inodes) {
    if (fread(&f->f_mode, 1, 4, stdin) < 4) {
        fprintf(stderr, "Bad tar file\n");
        exit(1);
        return;
    }
    // printf("the mode is %d \n", f->f_mode);

    if (fread(&f->modTime, 1, 8, stdin) < 8) {
        fprintf(stderr, "Bad tar file\n");
        exit(1);
        return;
    }
    
//checking if its a directory and making a directory, otherwise
// it is a file and we call create files
    if (S_ISDIR(f->f_mode)) {
        mkdir(f->fileName, 0777);
    } else {
        create_files(f);
    }
}

//creating the files from tar file, going to be called when it is a new file
void create_files(f *f) {
    if (fread(&f->fileBytes_size, 1, 8, stdin) == 0) {
        return;
    }

    FILE *fo = fopen(f->fileName, "w");
    // chmod(f->fileName, f->f_mode);
    // printf("trying to malloc %lu bytes\n", f->fileBytes_size);

    char *byteBuf = malloc(f->fileBytes_size);
    // printf("done!\n");

    if (fread(byteBuf, f->fileBytes_size, 1, stdin) < 1) {
        fprintf(stderr, "Bad tar file\n");
        exit(1);
        return;
    }

    fwrite(byteBuf, 1, f->fileBytes_size, fo);
    

    fclose(fo);
    free(byteBuf);
}

int main(int argc, char *argv[]) {
    // make a dllist to store eveything from the file struct
    Dllist fileDll = new_dllist();
    Dllist dirDll = new_dllist();
    JRB inode;
    inode = make_jrb();
    f *temp;
    while (1) {
        f *fino = malloc(sizeof(f));
        if (read_initial_info(fino, inode) == false) {
            free(fino);
            break;
        }
        // if the inode already exists, then we are going to print the extra
        if (!exists(inode, fino->f_inode, fino->fileName)) {
            read_additional(fino, inode);
        } else {
            // its already in the tree
            link(jrb_find_int(inode, fino->f_inode)->val.s, fino->fileName);
            continue;
        }

        if (S_ISDIR(fino->f_mode)) {
            dll_append(dirDll, new_jval_v(fino));
        } else {
            dll_append(fileDll, new_jval_v(fino));
        }
        printf("\n");
    }


    Dllist tmp;

    dll_traverse(tmp, fileDll) {
        f *file = ((f *)(tmp->val.v));

        struct timeval times[2];
        times[0].tv_sec = 0;
        times[0].tv_usec = 0;
        times[1].tv_sec = file->modTime;
        times[1].tv_usec = 0;
        
        utimes(file->fileName, times);

        chmod(file->fileName, file->f_mode);
        free(file->fileName);
        

    }

    dll_traverse(tmp, dirDll) {
        f *file = ((f *)(tmp->val.v));

        struct timeval times[2];
        times[0].tv_sec = 0;
        times[0].tv_usec = 0;
        times[1].tv_sec = file->modTime;
        times[1].tv_usec = 0;
        
        utimes(file->fileName, times);
        chmod(file->fileName, file->f_mode);
        free(file->fileName);
    }

//free memory needed
    Dllist tmp1;
    f *fptr;
    dll_traverse(tmp1, dirDll) {
        free(tmp1->val.v);
        }
    dll_traverse(tmp1, fileDll) {
        free(tmp1->val.v);
    }
    
    free_dllist(dirDll);
    free_dllist(fileDll);

    JRB tmp2;
    jrb_traverse(tmp2, inode){
        temp = (f *) tmp2->val.v;
        free(temp);
    }
    jrb_free_tree(inode);
    

    
}
