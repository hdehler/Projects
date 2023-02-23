// Harrison Dehler
// COSC 360
/*In this program we are healing those around with a spell. This spell can only heal to max and those
  nearby may also be healed. However, there is a reduction after each jump. Our job is to find the best path
  for healing */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h> /* w/ math.h included, must compile with -lm */


typedef struct node{
	char *playerName;
	int x, y;
	int cur_PP, max_PP;
	struct node *prev;
	bool visited;
	int adj_size;	   //holds size
	struct node **adj; //array
} Node;

typedef struct globalInfo
{
	int num_jumps;
	double power_reduction;
	int best_healing;
	int initial_healing;
    
    
    Node **cur_path;
	int *cur_inc;
    int cur_path_size;
	int *opt_inc;
    Node **opt_path;
    int opt_path_size;    
    int opt_healing;
} info;

void DFS(Node *n, info *pInfo, int depth, int healing, double h){

	//rounding the val
    int inc = n->max_PP - n->cur_PP;
    if( inc > rint(h) ) inc = rint(h);
    
    healing += inc;
    
	//curent path increment per DFS
    pInfo->cur_path[pInfo->cur_path_size] = n;
    pInfo->cur_inc[pInfo->cur_path_size] = inc;
    pInfo->cur_path_size++;
	
	
    if(depth >= pInfo->num_jumps-1){
		//update path the path if healing is greater than optimal healing
        if(healing > pInfo->opt_healing){
            memcpy(pInfo->opt_path,pInfo->cur_path,pInfo->cur_path_size*sizeof(pInfo->cur_path));
            memcpy(pInfo->opt_inc,pInfo->cur_inc,pInfo->cur_path_size*sizeof(int));
            pInfo->opt_path_size = pInfo->cur_path_size;
            pInfo->opt_healing = healing;
        }
		//mark as not visited initially, then again after dfs has been called
        pInfo->cur_path_size--;
        n->visited = false;
        return;
    }

    n->visited = true;
    h *= (1.0 - pInfo->power_reduction);
    bool recurse = false;
	//loop through adj if node is already been visitede continue, otherwise we call DFS 
    for(int i=0;i < n->adj_size;i++){
        if(n->adj[i]->visited) continue;
        recurse = true;
        DFS(n->adj[i], pInfo, depth+1, healing,h);
    }
	// If no more nodes need to be visited check if the current healing is greater than the optimum healing
    if(!recurse){
        if(healing > pInfo->opt_healing){
			//copy the path to the optimal path and then copy the incremental vals and update
            memcpy(pInfo->opt_path,pInfo->cur_path,pInfo->cur_path_size*sizeof(pInfo->cur_path));
            memcpy(pInfo->opt_inc,pInfo->cur_inc,pInfo->cur_path_size*sizeof(int));
            pInfo->opt_path_size = pInfo->cur_path_size;
            pInfo->opt_healing = healing;
        }
        pInfo->cur_path_size--;
        n->visited = false;
        return;        
    }
    
    n->visited = false;
    pInfo->cur_path_size--;
    
}

double dist(double x1, double x2, double y1, double y2){
	double totalDistX = x2-x1;
	double totalDistY = y1 - y2;
	return sqrt((totalDistX * totalDistX) + (totalDistY*totalDistY)); 
}


int main(int argc, char *argv[])
{
	// convert argument to an integer
	info f;
	f.power_reduction = atof(argv[5]);
	f.initial_healing = atoi(argv[4]);
	int jump_range = atoi(argv[2]);
	int initial_range = atoi(argv[1]);

	f.num_jumps = atoi(argv[3]);

	Node tmpNode;
	tmpNode.playerName = malloc(100 * sizeof(char));

	Node *nArr = NULL;
	int pCount = 0;

	// read player data
	while (scanf("%d %d %d %d %s", &tmpNode.x, &tmpNode.y, &tmpNode.cur_PP, &tmpNode.max_PP, tmpNode.playerName) != EOF)
	{
		nArr = realloc(nArr, (pCount + 1) * sizeof(Node));
		// copy the data from the temporary node to the newly added node in the node array
		nArr[pCount].x = tmpNode.x;
		nArr[pCount].y = tmpNode.y;
		nArr[pCount].cur_PP = tmpNode.cur_PP;
		nArr[pCount].max_PP = tmpNode.max_PP;
		nArr[pCount].playerName = malloc(100 * sizeof(char));
		strcpy(nArr[pCount].playerName, tmpNode.playerName);

		pCount++;
	}

	// setting initial node to prev
	nArr[0].prev = NULL;
	// essentially setting each new node nArr[count] to prev as the nArr[count-1]
	for (int i = pCount - 1; i < 1; i--)
	{
		nArr[i].prev = &nArr[i - 1];
	}

	// creating and linking the graph
	for (int i = 0; i < pCount; i++)
	{
		// count the number of connections for each node
		int connectCount = 0;
		for (int j = 0; j < pCount; j++)
		{
			if (i == j)
			{
				continue;
			}
			if (dist(nArr[i].x, nArr[j].x, nArr[i].y, nArr[j].y) <= jump_range)
			{
				connectCount++;
			}
		}
		// allocating memory for the adj list
		nArr[i].adj_size = connectCount;
		if (connectCount > 0)
		{
			nArr[i].adj = malloc(connectCount * sizeof(Node *));
		}
		else
		{
			nArr[i].adj = NULL;
		}

		connectCount = 0;
		// populating adj list
		for (int j = 0; j < pCount; j++)
		{
			if (i == j)
			{
				continue;
			}
			if (dist(nArr[i].x, nArr[j].x, nArr[i].y, nArr[j].y) <= jump_range)
			{
				nArr[i].adj[connectCount++] = &nArr[j];
			}
		}
	}

	// making a copy of the array into an array of node pointers
	Node **copyNode;
	copyNode = malloc(pCount * sizeof(Node *));
	for (int i = 0; i < pCount; i++)
	{
		copyNode[i] = &nArr[i];
	}

	info tmpInfo;
	tmpInfo.num_jumps = f.num_jumps;

	for (int i = 0; i < pCount; i++)
	{
		copyNode[i]->visited = false;
	}

	 int storeUrgosa = 0;
	 for(int i = 0; i < pCount; i++){
	 	if(strcmp(nArr[i].playerName, "Urgosa_the_Healing_Shaman") == 0){
	 		storeUrgosa = i;
	 	}
	 }

	f.opt_healing = 0; // flag that we don't want to keep the path
	f.opt_path_size = 0;
    f.cur_path_size=0;
	f.cur_path = malloc(pCount*sizeof(Node*));
	f.opt_path = malloc(pCount*sizeof(Node*));
	f.cur_inc = malloc(pCount*sizeof(int));
	f.opt_inc = malloc(pCount*sizeof(int));
	
	for (int i = 0; i < pCount; i++)
	{
		if (dist(nArr[i].x, nArr[storeUrgosa].x, nArr[i].y, nArr[storeUrgosa].y) <= initial_range)
		{
			DFS(copyNode[i], &f, 0, 0, f.initial_healing);
		}
	
	}

	for (int i = 0; i < f.opt_path_size; i++)
	{
		printf("%s %d\n", f.opt_path[i]->playerName, f.opt_inc[i]);
	}
	printf("Total_Healing %d\n", f.opt_healing);

    
free(tmpNode.playerName);
for(int i = 0; i < pCount; i++){
    free(nArr[i].adj);
    free(nArr[i].playerName);
}
free(nArr);
free(copyNode);
free(f.cur_path);
free(f.opt_path);
free(f.cur_inc);
free(f.opt_inc);
    
return 0;
}