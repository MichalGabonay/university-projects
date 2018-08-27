/*
 * algorithm: priradenie poradia preorder vrcholom
 * author: Michal Gabonay (xgabon00@stud.fit.vutbr.cz)
 * date: 17.4.2018
 */

#include <mpi.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <math.h>
#include <stdio.h>

using namespace std;

#define TAG 0

typedef struct {
    int id;
    int left_node;
    int right_node;
} tree_edge_t;

typedef struct route_t{
    int node;
    tree_edge_t first_in_node;
    tree_edge_t edge;
    tree_edge_t reverse_edge;
    struct route_t *next;
    int e_toure;
    int pred;   //predecessor
    int weight;
} route_t;

tree_edge_t *createEdges(int num_of_edges, int num_of_nodes){

    tree_edge_t *edges;
    edges = (tree_edge_t *) malloc(sizeof(tree_edge_t) * num_of_edges);

    int j = 0;
    for (int i = 0; i < num_of_nodes; ++i)
    {
        if (j<(2*num_of_nodes-2))
        {
            edges[j].left_node = i;
            edges[j].right_node = (2*i)+1;
            edges[j].id = j;
            j++;
        }
        if (j<(2*num_of_nodes-2))
        {
            edges[j].left_node = (2*i)+1;
            edges[j].right_node = i;
            edges[j].id = j;
            j++;
        }
        if (j<(2*num_of_nodes-2))
        {
            edges[j].left_node = i;
            edges[j].right_node = (2*i)+2;
            edges[j].id = j;
            j++;
        }
        if (j<(2*num_of_nodes-2))
        {
            edges[j].left_node = (2*i)+2;
            edges[j].right_node = i;
            edges[j].id = j;
            j++;
        }
    }
    return edges;
}

route_t *createAdjacencyList(int num_of_edges, int num_of_nodes, tree_edge_t *edges){
    route_t *adjacency_l;
    adjacency_l = (route_t *) malloc(sizeof(route_t) * num_of_edges);

    int j = 0;
    for (int i = 0; i < num_of_nodes; ++i)
    {

        if (j<(2*num_of_nodes-2))
        {
            adjacency_l[j].node = i;            
            adjacency_l[j].edge = edges[j];
            adjacency_l[j].reverse_edge = edges[j+1];
            if (i==0 && num_of_nodes>2)
            {
                adjacency_l[j].first_in_node = edges[j];
                adjacency_l[j].next = &adjacency_l[2];
            } else if (j+2<(2*num_of_nodes-2))
            {
                adjacency_l[j].first_in_node = edges[(j/2)-1];
                adjacency_l[j].next = &adjacency_l[j+2];
            } else {
                adjacency_l[j].first_in_node = edges[(j/2)-1];
                adjacency_l[j].next = NULL;
            }
            j++;
        }
        if (j<(2*num_of_nodes-2))
        {
            adjacency_l[j].node = 2*i+1;
            adjacency_l[j].first_in_node = edges[j];
            adjacency_l[j].edge = edges[j];
            adjacency_l[j].reverse_edge = edges[j-1];
            if (j<(num_of_nodes-2))
            {
                adjacency_l[j].next = &adjacency_l[2*j+2];
            } else {
                adjacency_l[j].next = NULL;
            }
            j++;
        }
        if (j<(2*num_of_nodes-2))
        {
            adjacency_l[j].node = i;
            adjacency_l[j].edge = edges[j];
            adjacency_l[j].reverse_edge = edges[j+1];
            adjacency_l[j].next = NULL;
            if (i==0 && num_of_nodes>2)
            {
                adjacency_l[j].first_in_node = edges[0];
            } else {
                adjacency_l[j].first_in_node = edges[(j/2)-2];
            }
            j++;
        }
        if (j<(2*num_of_nodes-2))
        {
            adjacency_l[j].node = 2*i+2;
            adjacency_l[j].edge = edges[j];
            adjacency_l[j].reverse_edge = edges[j-1];
            adjacency_l[j].first_in_node = edges[j];
            if (j<(num_of_nodes-2))
            {
                adjacency_l[j].next = &adjacency_l[2*j+2];
            } else {
                adjacency_l[j].next = NULL;
            }
            j++;
        }
    }
    return adjacency_l;
}

int main(int argc, char *argv[])
{
    int numprocs;               //pocet procesoru
    int myid;                   //muj rank
    MPI_Status stat;            //struct- obsahuje kod- source, tag, error

    int num_of_nodes = strlen(argv[1]);
    int num_of_edges = (2*strlen(argv[1]))-2;

    double starttime, endtime;

    tree_edge_t *edges = createEdges(num_of_edges, num_of_nodes);
    route_t * adjacency_l = createAdjacencyList(num_of_edges, num_of_nodes, edges);

    //MPI INIT
    MPI_Init(&argc,&argv);                          // inicializace MPI 
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);       // zjistíme, kolik procesů běží 
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);           // zjistíme id svého procesu 

    // vstup len 1 uzol
    if (num_of_nodes == 1)
    {
        if (myid==0)
        {
            printf("%c\n", argv[1][0]);
        }
        MPI_Finalize();
        return 0;
    }

    int e_toure[num_of_edges];

    starttime = MPI_Wtime();    // začatok počítania času samotného algoritmu

    // vytvorenie e_toure (nasledovnikov)
    for(int i=0; i<numprocs; i++){
        if(myid == i){
            if (adjacency_l[adjacency_l[i].reverse_edge.id].next != NULL)
                e_toure[i] = adjacency_l[adjacency_l[i].reverse_edge.id].next->edge.id;
            else
                e_toure[i] = adjacency_l[adjacency_l[i].reverse_edge.id].first_in_node.id;
            
            // posledna cesta
            if (e_toure[i] == 0)
                e_toure[i] = -1;

            adjacency_l[i].e_toure = e_toure[i];

            // dopredné hrany majú hodnotu 1, ostatné 0
            if (adjacency_l[i].edge.id < adjacency_l[i].reverse_edge.id)
                adjacency_l[i].weight = 1;
            else
                adjacency_l[i].weight = 0;
        }
    }//for

    int successors[numprocs];
    int predccessors[numprocs];
    int predc;

    // spristupnenie kazdemu procesoru cele pole nasledovnikov
    MPI_Allgather(&adjacency_l[myid].e_toure, 1, MPI_INT, successors, 1, MPI_INT, MPI_COMM_WORLD);

    for(int i=1; i<numprocs; i++){
        if(myid == i){
            predccessors[successors[i]] = i;
            MPI_Send(&predccessors[successors[i]], 1, MPI_INT, 0, TAG,  MPI_COMM_WORLD);
        }
        if (myid == 0)
        {
            MPI_Recv(&predc, 1, MPI_INT, i, TAG, MPI_COMM_WORLD, &stat); //som 0 a prijmam
            // printf("%d: %d\n", successors[i], predc);
            predccessors[successors[i]] = predc;
            predccessors[0]=-1;
            predccessors[successors[0]]=0;
        }
    }

    // procesor s rankom 0 odosle broadcastom pole predchodcov vsetkym procesorom
    MPI_Bcast(&predccessors[0], numprocs, MPI_INT, 0, MPI_COMM_WORLD);
    adjacency_l[myid].pred = predccessors[myid];


    // SUFFIX SUM - start
    int value[num_of_edges];
    for(int i=0; i<numprocs; i++){
        if(myid == i ){
            if (adjacency_l[i].e_toure == -1)
            {
                value[i] = 0;
            } else {
                value[i] = adjacency_l[i].weight;
            }
            // printf("value of %d: %d\n", myid, value[myid]);

            int succ_val, succ_succ, pred_pred;
            for (int k = 0; k < log(numprocs)+1; ++k)
            {

                if (adjacency_l[myid].pred == -1 && adjacency_l[myid].e_toure == -1) //procesor nemá následníka ani predchodcu
                {
                    continue;
                } else if (adjacency_l[myid].pred == -1) // procesor nemá predchodcu
                {
                    // printf("k: %d, id:%d, value:%d, succ:%d, pred:%d\n", k, myid, value[myid], adjacency_l[myid].e_toure, adjacency_l[myid].pred);
                    // printf("k:%d, id:%d, value:%d, succ:%d\n", k, myid, value[myid], adjacency_l[myid].e_toure);
                    MPI_Recv(&succ_val, 1, MPI_INT, adjacency_l[myid].e_toure, TAG, MPI_COMM_WORLD, &stat);
                    // printf("prijal-val: %d, %d:%d\n", succ_val, k, myid);
                    MPI_Recv(&succ_succ, 1, MPI_INT, adjacency_l[myid].e_toure, TAG, MPI_COMM_WORLD, &stat);
                    // printf("prijal-succ: %d, %d:%d\n", succ_succ, k, myid);
                    MPI_Send(&adjacency_l[myid].pred, 1, MPI_INT, adjacency_l[myid].e_toure, TAG,  MPI_COMM_WORLD);
                    // printf("odoslal-pred: %d, %d:%d\n", adjacency_l[myid].pred, k, myid);
                    value[myid] = value[myid]+succ_val;
                    adjacency_l[myid].e_toure = succ_succ;
                } else if (adjacency_l[myid].e_toure == -1) //procesor nemá následníka
                {
                    // printf("k:%d, id:%d, value:%d, succ:%d\n", k, myid, value[myid], adjacency_l[myid].e_toure);
                    MPI_Send(&value[myid], 1, MPI_INT, adjacency_l[myid].pred, TAG,  MPI_COMM_WORLD);
                    // printf("odoslal-val: %d, %d:%d\n", adjacency_l[myid].weight, k, myid);
                    MPI_Send(&adjacency_l[myid].e_toure, 1, MPI_INT, adjacency_l[myid].pred, TAG,  MPI_COMM_WORLD);
                    // printf("odoslal-succ: %d, %d:%d\n", adjacency_l[myid].e_toure, k, myid);
                    MPI_Recv(&pred_pred, 1, MPI_INT, adjacency_l[myid].pred, TAG, MPI_COMM_WORLD, &stat);
                    // printf("prijal-pred: %d, %d:%d\n", pred_pred, k, myid);
                    adjacency_l[myid].pred = pred_pred;

                } else { // procesor má následníka aj predchodcu
                    // printf("k:%d, id:%d, value:%d, succ:%d\n", k, myid, value[myid], adjacency_l[myid].e_toure);
                    MPI_Send(&value[myid], 1, MPI_INT, adjacency_l[myid].pred, TAG,  MPI_COMM_WORLD);
                    // printf("odoslal-val: %d, %d:%d\n", adjacency_l[myid].weight, k, myid);
                    MPI_Send(&adjacency_l[myid].e_toure, 1, MPI_INT, adjacency_l[myid].pred, TAG,  MPI_COMM_WORLD);
                    // printf("odoslal-succ: %d, %d:%d\n", adjacency_l[myid].e_toure, k, myid);
                    MPI_Recv(&succ_val, 1, MPI_INT, adjacency_l[myid].e_toure, TAG, MPI_COMM_WORLD, &stat);
                    // printf("prijal-val: %d, %d:%d\n", succ_val, k, myid);
                    MPI_Recv(&succ_succ, 1, MPI_INT, adjacency_l[myid].e_toure, TAG, MPI_COMM_WORLD, &stat);
                    // printf("prijal-succ: %d, %d:%d\n", succ_succ, k, myid);
                    MPI_Recv(&pred_pred, 1, MPI_INT, adjacency_l[myid].pred, TAG, MPI_COMM_WORLD, &stat);
                    // printf("prijal-pred: %d, %d:%d\n", pred_pred, k, myid);
                    MPI_Send(&adjacency_l[myid].pred, 1, MPI_INT, adjacency_l[myid].e_toure, TAG,  MPI_COMM_WORLD);
                    // printf("odoslal-pred: %d, %d:%d\n", adjacency_l[myid].pred, k, myid);
                    value[i] = value[i]+succ_val;
                    adjacency_l[myid].e_toure = succ_succ;
                    adjacency_l[myid].pred = pred_pred;
                    
                }
            }
            if (adjacency_l[myid].weight == 1)
            {
                value[myid] = num_of_nodes-value[myid]-1;
            } else {
                value[myid] = -1;
            }
            // printf("value of %d: %d\n", myid, value[myid]);
        }
    }
    // SUFFIX SUM - end

    //kazdy procesor odosle svoju poziciu
    int preorder[num_of_nodes];
    int position;
    for(int i=0; i<numprocs; i++){
        if(myid == i) MPI_Send(&value[myid], 1, MPI_INT, 0, TAG,  MPI_COMM_WORLD);
        if(myid == 0){
            
            MPI_Recv(&position, 1, MPI_INT, i, TAG, MPI_COMM_WORLD, &stat); //somm 0 a prijmam
            if (position>=0)
            {
                // printf("preorder[%d]=%d\n",position,i );
                preorder[position] = i;
            }
        }//if som master
    }//for

    endtime = MPI_Wtime();

    // procesor s id 0 vypíše na výstup výslednú preorder postupnost
    if (myid == 0)
    {
        printf("%c", argv[1][0]);
        for (int i = 0; i < num_of_nodes-1; ++i)
        {
            printf("%c", argv[1][adjacency_l[preorder[i]].edge.right_node]);
        }
        printf("\n");
        cout<<num_of_nodes<<"   "<<endtime-starttime<<endl;
    }

    //DEBUG vypis
    if (false)
    {
        printf("node:%d, edge:%d, reverse:%d, first:%d, ",adjacency_l[myid].node,adjacency_l[myid].edge.id,adjacency_l[myid].reverse_edge.id,adjacency_l[myid].first_in_node.id);
        if (adjacency_l[myid].next != NULL)
        {
            printf("next:%d, ", adjacency_l[myid].next->edge.id);
        } else {
            printf("next:NULL, ");
        }
        printf("weight: %d, succ: %d, pred:%d\n", adjacency_l[myid].weight, adjacency_l[myid].e_toure, adjacency_l[myid].pred);
    }

    MPI_Finalize(); 
    return 0;

}//main