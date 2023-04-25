// Harrison Dehler
// COSC 360
// March 20, 2023
// In this lab we create the tar file

//Josh suggested this define below, can do it my other way though as well.
#define _GNU_SOURCE
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "dllist.h"
#include "fields.h"
#include "jrb.h"
#include "jval.h"

// adds the inode to the tree if it is not already in the tree
bool exists(JRB tree, unsigned long inode) {
    JRB Node = jrb_find_int(tree, inode);

    // if the inode is not in the tree then insert it
    if (Node == NULL) {
        jrb_insert_int(tree, inode, JNULL);
        return false;
    } else {
        return true;
    }
}

void read_directory(const char *dn, JRB inodes, int index) {
    // inodes = make_jrb(); //this is what was making my program wack and searched for it for too long... 
    //I want to keep this here for memories
    
    Dllist subDirs = new_dllist();
    DIR *d;             // return val of the open directory
    struct dirent *de;  // the return val of readdir call
    struct stat f, dirStat, fMode;  // info about each file returned by stat call
    int filenameSize;

    // open directory which is going to be the second argument in call
    d = opendir(dn);
    if (d == NULL) {
        fprintf(stderr, "No files in the directory\n");
        exit(1);
    }

    // print the stuff thats printed everytime
    if (stat(dn, &dirStat) == -1) {
        fprintf(stderr, "stat failed\n");
        exit(1);
    }

    // iterate over the directory
    for (de = readdir(d); de != NULL; de = readdir(d)) {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
            continue;
        }

        // get the length of the file
        //  long shortFileLength = strlen(de->d_name);
        int fileLength = strlen(dn) + strlen(de->d_name) - index + 1;
        // char *fName = (char *)malloc(fileLength + index + 1);
        char *fName;
        asprintf(&fName, "%s/%s", dn, de->d_name);
        lstat(fName, &f);

        // check if there is a sub directory
        if (S_ISDIR(f.st_mode)) {
            dll_append(subDirs, new_jval_s(strdup(fName)));
        }

        // print the filename size, file name, and the inode
        fwrite(&fileLength, 4, 1, stdout);
        fwrite(fName + index, 1, fileLength, stdout);
        fwrite(&f.st_ino, 8, 1, stdout);

        // check to see if it is the first time we are seeing the inode
        if (!(exists(inodes, f.st_ino))) {
            // If it's the first time seen, print mode and modification time
            fwrite(&f.st_mode, 4, 1, stdout);
            fwrite(&f.st_mtime, 8, 1, stdout);

            if (S_ISREG(f.st_mode)) {
                FILE *openFile;
                openFile = fopen(fName, "rb");
                unsigned char c[f.st_size];
                fwrite(&f.st_size, 8, 1, stdout);
                fread(c, f.st_size, 1, openFile);
                fwrite(&c, f.st_size, 1, stdout);
                fclose(openFile);
            }
        }
        free(fName);
    }
    // recursively call
    closedir(d);
    Dllist tmp;
    dll_traverse(tmp, subDirs) { read_directory(tmp->val.s, inodes, index); }
    
    //free my mem
    Dllist tmp2;
    dll_traverse(tmp2, subDirs){
        free(tmp2->val.s);
    }
    free_dllist(subDirs);
}

int main(int argc, char *argv[]) {
    //create an index for the last "/" in the file name/path
    int index = 0;
    char *last;
    for (int i = strlen(argv[1]) - 1; i >= 0; i--) {
        if (argv[1][i] == '/') {
            index = i + 1;
            break;
        }
    }
    if (index != 0) {
        last = &argv[1][index];
    }

    JRB inode;
    inode = make_jrb();

    // printing for the very first directory. Not the files insider just the
    // directory name
    int filename_size = strlen(argv[1]) - index;
    fwrite(&filename_size, 4, 1, stdout);
    printf("%s", argv[1] + index);

    struct stat s;
    lstat(argv[1], &s);
    fwrite(&s.st_ino, 8, 1, stdout);

    // If it's the first time seen, print mode and modification time
    fwrite(&s.st_mode, 4, 1, stdout);
    fwrite(&s.st_mtime, 8, 1, stdout);

    read_directory(argv[1], inode, index);

    jrb_free_tree(inode);

    
}