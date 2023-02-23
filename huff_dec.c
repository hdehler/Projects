//Harrison Dehler
//Feb 15, 2023
//COSC 360
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct huff_node
{
    struct huff_node *zero;
    struct huff_node *one;
    char *string;
} Huff_node;

Huff_node *new_huff_node()
{
    Huff_node *node = malloc(sizeof(Huff_node));
    node->zero = NULL;
    node->one = NULL;
    node->string = NULL;

    return node;
}

void dfs(Huff_node *curr)
{
    if (curr->zero)
    {
        dfs(curr->zero);
    }
    if (curr->one)
    {
        dfs(curr->one);
    }
    
    free(curr->string);
    free(curr);
}

int main(int argc, char *argv[])
{
    int read_one;
    char c;

    Huff_node *root = new_huff_node();
    Huff_node *node = root;

    char charString[10000];

    char *fcode = argv[1];
    char *finput = argv[2];

    // reading in the code file
    FILE *fco;
    fco = fopen(fcode, "rb");

    if (fco == NULL)
    {
        printf("Open error: %s\n", argv[1]);
        exit(1);
    }

    // reading in the input file
    FILE *fiopen;
    fiopen = fopen(finput, "rb");
    if (fiopen == NULL)
    {
        printf("Open error - input file not open %s\n", argv[2]);
        exit(1);
    }

    int length_count = 0;
    // bool for flipping when we read a null because the next is not going to be a string
    bool read_string = true;
    // printf("fiopen reads in --> %s\n\n", fiopen);

    while (fread(&c, 1, 1, fco) != 0)
    { // may need to be EOF
        if (c == '\0')
        { // may need '\0'
            if (!read_string)
            {
                node->string = strdup(charString);
                node = root;
            }
            else
            {
                charString[length_count] = '\0';
                // printf("encoded characters: %s\n", charString);
                length_count = 0;
            }
            read_string = !read_string; // flips from true to false, false to true
            continue;
        }

        if (read_string)
        {
            charString[length_count++] = c;
        }
        else
        {
            if (c == '0')
            {
                if (node->zero == NULL)
                {
                    node->zero = new_huff_node();
                }
                node = node->zero;
            }
            if (c == '1')
            {
                if (node->one == NULL)
                {
                    node->one = new_huff_node(); // allocating the memory
                }
                node = node->one;
            }
        }
    }

    // get the number of bits to read from the last 4 bytes of input file
    int num_bits;
    fseek(fiopen, -4, SEEK_END);
    fread(&num_bits, 4, 1, fiopen);

    // fseek(fiopen, 0, SEEK_END);
    long int file_size = ftell(fiopen);
    rewind(fiopen);

    if (file_size < 4)
    {
        fprintf(stderr, "Error: file is not the correct size.\n");
        exit(1);
    }

    if(num_bits > 8*file_size){ 
        fprintf(stderr, "Error: Total bits = %d, but file's size is %d\n", num_bits, file_size);
        exit(1);
    }

    // rewind back to the beginnig of the file after getting the bits
    rewind(fiopen);

    // read the bits from the input file and decode them using the Huffman tree
    int bits_read = 0;
    node = root;
    while (bits_read < num_bits && fread(&c, 1, 1, fiopen))
    {
        // process bits from left to right
        for (int i = 0; i < 8 && bits_read < num_bits; bits_read++, i++)
        {
            if (c & (1 << i))
            {
                //if one is null then it is unrecognized and we need to print an error
                if(node->one == NULL){
                    fprintf(stderr, "Unrecognized bits\n");
                    exit(1);
                }
                node = node->one;
            }
            else
            {
                //if zero is null then it is unrecognized and we need to print an error
                if(node->zero == NULL){
                    fprintf(stderr, "Unrecognized bits\n");
                    exit(1);
                }
                node = node->zero;
            }
            if (node->string != NULL)
            {
                printf("%s", node->string);
                node = root;
            }
        }
    }
    dfs(root);
    fclose(fco);
    fclose(fiopen);

    return 0;
}