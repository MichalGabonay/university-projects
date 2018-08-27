/*
 * algorithm: merge-splitting sort
 * author: Michal Gabonay (xgabon00@stud.fit.vutbr.cz)
 * date: 5.4.2018
 */

#include <mpi.h>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

#define TAG 0

//veľkosť súboru (v našom prípade počet hodnôt v súbore, keďže jedna hodnota jeden bajt)
std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}

// spojí 2 zoranené postupnosti čísel do jednej zoradenej a tú dalej rodelí na 2
void mergeAndSplit(int *s1, int s1_len, int *s2, int s2_len, int *s3){
    int i=0;
    int j=0;
    int k=0;

    //postupne porovnám prvé prvky z pola, menšie uložím a posuniem sa v danom poli
    for (int l = 0; l < s1_len+s2_len; ++l)
    {
        if (s1[i] < s2[j])
        {
            s3[k] = s1[i];
            i++;
        } else {
            s3[k] = s2[j];
            j++;
        }
        k++;
        if (i == s1_len && j == s2_len)
        {
            break;
        }else if (i == s1_len)
        {
            for (int h = j; h < s2_len; ++h)
            {
                s3[k] = s2[h];
                k++;
            }
            break;
        }else if (j == s2_len)
        {
            for (int h = i; h < s1_len; ++h)
            {
                s3[k] = s1[h];
                k++;
            }
            break;
        }
    }

    //rozdelenie spojeného pola hodnôt do dvoch
    for (int h = 0; h < s1_len; ++h)
    {
        s1[h] = s3[h];
    }
    j = 0;
    for (int i = s1_len; i < (s2_len+s1_len); i++)
    {
        s2[j] = s3[i];
        j++;
    }
}

 int main(int argc, char *argv[])
 {
    int numprocs;               //pocet procesoru
    int myid;                   //muj rank
    int *lNeighNumbers;         //hodnoty ľavého suseda
    // int *rNeighNumbers;         //hodnoty pravého suseda
    int *myNumbers;             //moje hodnoty
    int *mergedNums;            //hodnoty po spojení
    MPI_Status stat;            //struct- obsahuje kod- source, tag, error

    double starttime, endtime;

    //MPI INIT
    MPI_Init(&argc,&argv);                          // inicializace MPI 
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);       // zjistíme, kolik procesů běží 
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);           // zjistíme id svého procesu 


    char input[]= "numbers";
    int numValues = filesize(input);
    int valuesOnProcs = numValues/numprocs;  //počet hodnôt DIV počet procesorov
    int restOfNums = numValues%numprocs;     //počet hodnôt MOD počet procesorov (zvyšok po delení)
    int myValuesOnProcs;   // počet hodnôt na mojom procesore
    int lNeighValuesOnProcs; // počet hodnôt na lavom susedovy
    // int rNeighValuesOnProcs; // počet hodnôt na pravom susedovy

    //ak je počet hôdnou delitelný počtom procesorov bez zvyšku
    if (restOfNums == 0)
    {
        myValuesOnProcs = valuesOnProcs;
        lNeighValuesOnProcs = valuesOnProcs;
        // rNeighValuesOnProcs = valuesOnProcs;
    } else { // počet hodnôt nieje delitelný počtom procesorov -> procesory s nižším id ako je zvyšok príjmu o jeden prvok naviac
        if (myid < restOfNums)
        {
            myValuesOnProcs = valuesOnProcs+1;
        }else{
            myValuesOnProcs = valuesOnProcs;
        }
        // if (myid+1 < restOfNums)
        // {
        //     rNeighValuesOnProcs = valuesOnProcs+1;
        // }else{
        //     rNeighValuesOnProcs = valuesOnProcs;
        // }
        if (myid-1 < restOfNums)
        {
            lNeighValuesOnProcs = valuesOnProcs+1;
        }else{
            lNeighValuesOnProcs = valuesOnProcs;
        }
    }

    // alokácia a vynulovanie pamäti
    myNumbers = (int *) malloc(sizeof(int) * myValuesOnProcs);
    lNeighNumbers = (int *) malloc(sizeof(int) * lNeighValuesOnProcs);
    // rNeighNumbers = (int *) malloc(sizeof(int) * rNeighValuesOnProcs);
    mergedNums = (int *) malloc(sizeof(int) * (myValuesOnProcs + lNeighValuesOnProcs));
    memset(myNumbers, 0, sizeof(int) * myValuesOnProcs);
    memset(lNeighNumbers, 0, sizeof(int) * lNeighValuesOnProcs);
    // memset(rNeighNumbers, 0, sizeof(int) * rNeighValuesOnProcs);
    memset(mergedNums, 0, sizeof(int) * (myValuesOnProcs + lNeighValuesOnProcs));

    //NACTENI SOUBORU
    /* -proc s rankem 0 nacita vsechny hodnoty
     * -postupne rozesle jednotlive hodnoty vsem i sobe
    */
    if(myid == 0){
        
        int number;                         //hodnota pri nacitani souboru
        int numId = 0;                      //poradové číslo hodnoty vrámci jednoho procesoru
        int *numberBuffer;                  //buffer pre akupulovanie hodnôt pre procesor
        int procsId= 0;                     //invariant- urcuje cislo proc, kteremu se bude posilat
        fstream fin;                        //cteni ze souboru
        fin.open(input, ios::in);  

        int numOnProc = procsId<restOfNums ? valuesOnProcs+1 : valuesOnProcs;  //počet čísel koľko pripadá na daný procesor

        numberBuffer = (int *) malloc(sizeof(int) * numOnProc);  
        memset(numberBuffer, 0, sizeof(int) * numOnProc);
                 
        while(fin.good()){
            number= fin.get();
            if(!fin.good()) break;                      //nacte i eof, takze vyskocim
            cout<<number<<' ';                      //výpis jednotlivých načítaných hodnôt
            
            numberBuffer[numId] = number;         //pridanie čísla do bufferu
            numId++;
            if (numId == numOnProc)
            {
                if (procsId == 0)               //ak je to procesr s id 0 rovno skopírujem hodnoty
                {
                    myNumbers = numberBuffer;
                }else{
                    MPI_Send(numberBuffer, numOnProc, MPI_INT, procsId, TAG, MPI_COMM_WORLD); //buffer,velikost,typ,rank prijemce,tag,komunikacni skupina
                }
                procsId++;  
                numId = 0;
                numOnProc = procsId<restOfNums ? valuesOnProcs+1 : valuesOnProcs;
                numberBuffer = (int *) malloc(sizeof(int) * numOnProc);
                memset(numberBuffer, 0, sizeof(int) * numOnProc);
            }           
        }//while
        cout<<endl;
        free(numberBuffer);
        fin.close();
    }//nacteni souboru
    else{
        //PRIJATIE HODNOTY CISLA
        //vsetky procesory prijmu hodnotu
        MPI_Recv(myNumbers, myValuesOnProcs, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat); //buffer,velikost,typ,rank odesilatele,tag, skupina, stat
    }
    
    starttime = MPI_Wtime();    // začatok počítania času samotného algoritmu
    sort(myNumbers, myNumbers+myValuesOnProcs);  //sekvenčné zoradenie hodnôt priradených procesoru

    //LIMIT PRO INDEXY
    int oddlimit= 2*(numprocs/2)-1;                 //limity pro párne
    int evenlimit= 2*((numprocs-1)/2);              //nepárne
    int halfcycles= numprocs/2;

    //RAZENI-----------
    //cyklus pro linearitu
    for(int j=1; j<=halfcycles; j++){

        //párne proc 
        if((!(myid%2) || myid==0) && (myid<oddlimit)){
            MPI_Send(myNumbers, myValuesOnProcs, MPI_INT, myid+1, TAG, MPI_COMM_WORLD);          //posle susedovi svoje hodnoty
            MPI_Recv(myNumbers, myValuesOnProcs, MPI_INT, myid+1, TAG, MPI_COMM_WORLD, &stat);   //a cekam na nové hodnoty
        }//if párne
        else if(myid<=oddlimit){//nepárne prijmajú spravu a vracaju prvú polovisu spojených hodnôt
            MPI_Recv(lNeighNumbers, lNeighValuesOnProcs, MPI_INT, myid-1, TAG, MPI_COMM_WORLD, &stat);
            mergeAndSplit(lNeighNumbers, lNeighValuesOnProcs, myNumbers, myValuesOnProcs, mergedNums);
            MPI_Send(lNeighNumbers, lNeighValuesOnProcs, MPI_INT, myid-1, TAG, MPI_COMM_WORLD);
        }//else if (nepárne)

        //nepárne proc 
        if((myid%2) && (myid<evenlimit)){
            MPI_Send(myNumbers, myValuesOnProcs, MPI_INT, myid+1, TAG, MPI_COMM_WORLD);          //posle susedovi svoje hodnoty
            MPI_Recv(myNumbers, myValuesOnProcs, MPI_INT, myid+1, TAG, MPI_COMM_WORLD, &stat);   //a cekam na nové hodnoty
        }//if nepárne
        else if(myid<=evenlimit && myid!=0){//párne prijmajú spravu a vracaju prvú polovisu spojených hodnôt
            MPI_Recv(lNeighNumbers, lNeighValuesOnProcs, MPI_INT, myid-1, TAG, MPI_COMM_WORLD, &stat);
            mergeAndSplit(lNeighNumbers, lNeighValuesOnProcs, myNumbers, myValuesOnProcs, mergedNums);
            MPI_Send(lNeighNumbers, lNeighValuesOnProcs, MPI_INT, myid-1, TAG, MPI_COMM_WORLD);
        }
    }//for pro linearitu
    //RAZENI--------------------------------------------------------------------

    //FINALNI DISTRIBUCE VYSLEDKU K MASTEROVI-----------------------------------
    int* final;
    int* neighNumbers;
    int k = myValuesOnProcs;
    int countNum = 0;
    final=(int*) malloc(numValues*sizeof(int));
    memset(final, 0, sizeof(int) * numValues);
    for(int i=1; i<numprocs; i++){
        if(myid == i) MPI_Send(myNumbers, myValuesOnProcs, MPI_INT, 0, TAG,  MPI_COMM_WORLD);
        if(myid == 0){
            int numOnProc = i<restOfNums ? valuesOnProcs+1 : valuesOnProcs;

            neighNumbers = (int *) malloc(sizeof(int) * numOnProc);
            memset(neighNumbers, 0, sizeof(int) * numOnProc);

            MPI_Recv(neighNumbers, numOnProc, MPI_INT, i, TAG, MPI_COMM_WORLD, &stat); //somm 0 a prijmam
            for (int j = 0; j < numOnProc; ++j)
            {
                final[k]=neighNumbers[j];
                k++;
            }
        }//if som master
    }//for

    endtime = MPI_Wtime();

    if(myid == 0){
        for (int j = 0; j < myValuesOnProcs; ++j)
            {
                final[j]=myNumbers[j];
                k++;
            }
        for(int i=0; i<numValues; i++){
            cout<<final[i]<<endl;
        }//for
    }//if vypis
    //VYSLEDKY------------------------------------------------------------------

    // if (myid == 0) {
    //     cout<<numValues<<"; "<<numprocs<<"; "<<endtime-starttime<<"s; "<<endl;
    // }

    //uvolnenie pamäti
    free(final);
    free(myNumbers);
    free(lNeighNumbers);
    // free(rNeighNumbers);
    free(mergedNums);

    MPI_Finalize(); 
    return 0;
    
 }//main