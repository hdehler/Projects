//Harrison Dehler
//Netid - hdehler
//COSC 360 - Dr. Plank
//In this lab we read in from a description of people and set their respected traits/relationships and sort the family tree.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dllist.h"
#include "fields.h"
#include "jrb.h"
#include "jval.h"

typedef struct person {
    char *name;
    char *sex;
    struct person *mother;
    struct person *father;
    Dllist children;

    int visited; 
    int printed;
} Person;

// get person function that adds a name to the tree if it is not already in the tree
Person *get_person_by_name(JRB tree, char *name) {
    JRB Node = jrb_find_str(tree, name);
    Person *person;
    if (Node == NULL) {
        person = malloc(sizeof(Person));
        person->name = name;
        person->children = new_dllist();
        jrb_insert_str(tree, name, new_jval_v(person));
    } else {
        person = Node->val.v;
    }
    // If the person is in the tree, return that person
    // otherwise, create a new person with the name `name`, and insert this
    // person into the tree
    return person;
}

//DFS - cycle detection
int cycleDetect(Person *p, JRB people, Dllist node)
{
	if (p->visited == 1)
		return 0; /* I.e. we've processed this
					 person before and he/she's ok */
	if (p->visited == 2)
	{ /* I.e. the graph is messed up */
		fprintf(stderr, "Bad input -- cycle in specification\n");
		exit(-1);
	}
	p->visited = 2;
    //call dfs
	dll_traverse(node, p->children)
	{
		Person *child_per = (Person *)node->val.v;
		if (cycleDetect(child_per, people, node))
		{
			return 1;
		}
	}

	p->visited = 1;
	return 0;
}

void sexCheck(Person *p, Person *p2, char *name) {}

int main(int argc, char *argv[]) {
   
    Dllist tmp;

    IS is;
    Person *currentPeople = NULL;
    char *name;

    is = new_inputstruct(argv[1]);
    if (is == NULL) {
        perror(argv[1]);
        exit(1);
    }

    Dllist names = new_dllist();
    

    /* Read each line with get_line().  Print out each word. */
    JRB peopleTree = make_jrb();
    while (get_line(is) >= 0) {
        if (strlen(is->text1) <= 1) {
            continue;
        }
        int nameSize = 0;
        for (int i = 1; i < is->NF; i++) {  // was i=1
            nameSize += (1 + strlen(is->fields[i]));
        }
        // currentPeople = (Person *) malloc(sizeof(Person));
        name = (char *)malloc(sizeof(char) * nameSize);
        dll_append(names, new_jval_v(name));
        strcpy(name, is->fields[1]);
        nameSize = strlen(name);
        for (int i = 2; i < is->NF; i++) {
            name[nameSize] = ' ';
            strcpy(name + nameSize + 1, is->fields[i]);
            nameSize += strlen(name + nameSize);
            // printf("%d %s\n", nameSize, name);
        }

        // calling get person if first work is person
        if (strcmp(is->fields[0], "PERSON") == 0) {
            currentPeople = get_person_by_name(peopleTree, name);
            // printf("Person: %s\n", name);
        } else if (strcmp(is->fields[0], "FATHER_OF") == 0) {
            Person *child = get_person_by_name(peopleTree, name);
            
            //check for bad input when there are two fathers to a child, there are no g-... fams
            //do this in all else ifs as well (besides sex ofc)
            if (child->father != NULL &&
                strcmp(child->father->name, currentPeople->name) != 0) {
                fprintf(stderr, "Bad input -- child with two fathers on line %d\n", is->line);
                exit(1);
            } else {
                child->father = currentPeople;
                
                //flag if the current is already in the list -- do this in every else if ie. mother_of, mother, father, father_of
                int flag = 1;
                dll_traverse(tmp, child->father->children) {
                    char *childName = ((Person *)tmp->val.v)->name;
                    if (strcmp(currentPeople->name, childName) == 0) {
                        flag = 0;
                        break;
                    }
                }
                if (flag == 1) {
                    dll_append(currentPeople->children, new_jval_v(child));
                }

                //sex check
                if (child->father->sex == "F") {
                    fprintf(stderr, "Bad input - sex mismatch on line %d\n", is->line);
                    exit(-1);
                } else if (child->father->sex == NULL) {
                    child->father->sex = "M";
                }
            }
        }

        else if (strcmp(is->fields[0], "MOTHER_OF") == 0) {
            Person *child = get_person_by_name(peopleTree, name);
            if (child->mother != NULL &&
                strcmp(child->mother->name, currentPeople->name) != 0) {
                fprintf(stderr, "Bad input -- child with two mothers on line %d\n", is->line);
                exit(1);
            } else {
                child->mother = currentPeople;
                int flag = 1;
                dll_traverse(tmp, child->mother->children) {
                    char *childName = ((Person *)tmp->val.v)->name;
                    if (strcmp(currentPeople->name, childName) == 0) {
                        flag = 0;
                        break;
                    }
                }
                if (flag == 1) {
                    dll_append(currentPeople->children, new_jval_v(child));
                }

                if (child->mother->sex == "M") {
                    fprintf(stderr, "Bad input - sex mismatch on line %d\n", is->line);
                    exit(-1);
                } else if (child->mother->sex == NULL) {
                    child->mother->sex = "F";
                }
            }
        }

        //setting the current person sex - originally was Male and Female
        //TODO, if break, set back to male and female not m and f
        else if (strcmp(is->fields[0], "SEX") == 0) {
            // currentPeople->sex = is->fields[1];
            if (strcmp(is->fields[1], "M") == 0) {
                currentPeople->sex = "M";
            } else if (strcmp(is->fields[1], "F") == 0) {
                currentPeople->sex = "F";
            }

        
            
    }else if (strcmp(is->fields[0], "MOTHER") == 0) {
            Person *mother = get_person_by_name(peopleTree, name);
            if (currentPeople->mother != NULL &&strcmp(currentPeople->mother->name, mother->name) != 0) {
                fprintf(stderr, "Bad input -- child with two mothers on line %d\n", is->line);
                exit(1);
            } else {
                currentPeople->mother = mother;
                int flag = 1;
                dll_traverse(tmp, mother->children) {
                    char *childName = ((Person *)tmp->val.v)->name;
                    if (strcmp(currentPeople->name, childName) == 0) {
                        flag = 0;
                        break;
                    }
                }
                if (flag == 1) {
                    dll_append(mother->children, new_jval_v(currentPeople));
                }
            if(currentPeople->mother->sex != NULL){
                if (currentPeople->mother->sex == "M"){
                    fprintf(stderr, "Bad input - sex mismatch on line %d\n", is->line);
                    exit(-1);
                }
                } else{
                    currentPeople->mother->sex = "F";
                }
                
            }
        } else if (strcmp(is->fields[0], "FATHER") == 0) {
            Person *father = get_person_by_name(peopleTree, name);

            if (currentPeople->father != NULL &&
                strcmp(currentPeople->father->name, father->name) != 0) {
                fprintf(stderr, "Bad input -- child with two fathers on line %d\n", is->line);
                exit(-1);
            } else {
                currentPeople->father = father;
                int flag = 1;
                dll_traverse(tmp, father->children) {
                    char *childName = ((Person *)tmp->val.v)->name;
                    if (strcmp(currentPeople->name, childName) == 0) {
                        flag = 0;
                        break;
                    }
                }
                if (flag == 1) {
                    dll_append(father->children, new_jval_v(currentPeople));
                }

                if (currentPeople->father->sex == "F") {
                    fprintf(stderr, "Bad input - sex mismatch on line %d\n", is->line);
                    exit(-1);
                } else if (currentPeople->father->sex == NULL) {
                    currentPeople->father->sex = "M";
                }
            }
        }
    }

//call to the DFS/cycle detection
    JRB tmpNodeDFS;
	Dllist dllNode;
	jrb_traverse(tmpNodeDFS, peopleTree){
		currentPeople = ((Person *)tmpNodeDFS->val.v);
        cycleDetect(currentPeople, peopleTree, dllNode);
	}

    // copy people into topPrint
    Dllist topPrint;
    topPrint = new_dllist();
    jrb_traverse(tmpNodeDFS, peopleTree) {
        currentPeople = (Person *)tmpNodeDFS->val.v;
        dll_append(topPrint, new_jval_v(currentPeople));
    }

    Dllist tmpPrint;

    // this if statement is used to check each of the ifstatements for parents,
    // and then print Cannot print using a bunch of ifstatements becasue it will
    // not reach all of them if one is true.
    int ifCheck = 0;
    while (!dll_empty(topPrint)) {
        tmpPrint = dll_first(topPrint)->val.v;
        currentPeople = tmpPrint;

        // if people have not been printed yet
        ifCheck = 0;
        if (currentPeople->printed == 0) {

            // Check if there are no parents or if the parents have already been
            // printed
            if ((currentPeople->father == NULL ||
                 currentPeople->father->printed) &&
                (currentPeople->mother == NULL ||
                 currentPeople->mother->printed)) {
                ifCheck = 1;
            }

            // if the ifCheck is true, then we print the names and all of their
            // other stuff
            if (ifCheck == 1) {
                printf("%s\n", currentPeople->name);
                if (currentPeople->sex == "M") {
                    printf("  Sex: Male\n");
                } else if(currentPeople->sex == "F"){
                    printf("  Sex: Female\n");
                }
                else{
                    printf("  Sex: Unknown\n");
                }

                if (currentPeople->father == NULL) {
                    printf("  Father: Unknown\n");
                } else {
                    printf("  Father: %s\n", currentPeople->father->name);
                }

                if (currentPeople->mother == NULL) {
                    printf("  Mother: Unknown\n");
                } else {
                    printf("  Mother: %s\n", currentPeople->mother->name);
                }
                if (dll_empty(currentPeople->children)) {
                    printf("  Children: None\n\n");
                }

                Dllist temp_node;
                Dllist c = currentPeople->children;
                // traverse children, append to back of dlist topPrint
                if (!dll_empty(currentPeople->children)) {
                    Person *t;
                    printf("  Children: \n");
                    dll_traverse(temp_node, c) {
                        t = (Person *)temp_node->val.v;
                        printf("    %s\n", ((Person *)temp_node->val.v)->name);
                    }
                    printf("\n");

                }
                currentPeople->printed = 1;
            } else {
                dll_append(topPrint, new_jval_v(currentPeople));
            }
        }

        dll_delete_node(topPrint->flink);
    }


    //since people were made and then checked, we need to delete the ones that were not used
    //i had to create a dllist in order to free them, could not get it to work otherwise, kinda a jank way of doing it
    //but it frees it.
    jrb_traverse(tmpNodeDFS, peopleTree){
        currentPeople = (Person *) tmpNodeDFS->val.v;
        if(currentPeople->children != NULL){
            free_dllist(currentPeople->children);
        }
        free(currentPeople);
    }
    Dllist tmpFree;
    dll_traverse(tmpFree, names){
        free(tmpFree->val.v);
    }

    free_dllist(topPrint);
    jrb_free_tree(peopleTree);
    
    free_dllist(names);
    
    jettison_inputstruct(is);

    return 0;
}