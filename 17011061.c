//
//  main.c
//  hw3
//
//  Created by can ozer on 19.11.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <math.h>
#include <stdbool.h>


#define LOADFACTOR 0.9


typedef struct hash{
    struct NODE *head;
    int flag;
}HASH;

typedef struct node{
    int key;
    int count;
    char query[20];
    char webPage[60];
    struct node *next;
}NODE;

typedef struct result{
    int flag;
    char result[60];
    struct result *next;
}RESULT;

NODE* createNode(char* webPage, int key, char* query, int count){
    
    NODE* new_node = (NODE*)malloc(sizeof(NODE));
    strcpy(new_node->webPage,webPage); // ilgili webPage string ini yeni node a kopyaladik
    new_node->key = key;
    new_node->count = count;
    strcpy(new_node->query,query);  //en son yazdirirken goruntulemek icin match olan key degerini kopyaladik
    new_node->next = NULL;
    return new_node;
}

RESULT* createResultNode(char* webPage){
    RESULT* new_node = (RESULT*)malloc(sizeof(RESULT));
    strcpy(new_node->result, webPage);
    new_node->flag = 0;
    new_node->next = NULL;
    return new_node;
}


unsigned long long int hornerKey(char* string){
    
    int R=31;
    unsigned long L = strlen(string);
    unsigned long long int key = 0;
    int i;
    for(i=0; i<L; i++){
        key = key + pow(R,L-i-1) * (string[i] - 'a' + 1);    }
    return key;
}

void readFileAndHashIt(FILE *fp, HASH **hashTable, int tableSize){
    
    char buffer[200]; //line okumak icin buffer
    char webPage[40]; // webPage degerini aktarmak icin buffer
    
    int i=0;
    unsigned long long int key;
    int index=0; //hash index
    
    NODE* current;
    NODE* new_node;
    char* ptr1; // token islemi icin kullandigim pointerlar
    char* ptr2;
    
    
    while(fgets(buffer,201,fp)){ // while the end of the hash.txt
        
        strcpy(webPage, buffer);
        
        fgets(buffer,201,fp);
        ptr1 = strtok(buffer,"\r");
        ptr2 = strtok(ptr1," ");
        
        while(ptr2 != NULL ){
            
            key = hornerKey(ptr2);
            index = key % tableSize;
            current = ((*hashTable)[index].head);
            i = 0;
            
            while(((*hashTable)[index].head) != NULL && (strcmp(current->query, ptr2)) && (i<tableSize)){
                i++;
                index = (index + 1) % tableSize;
                current = ((*hashTable)[index].head);
            }
            new_node = createNode(webPage,index,ptr2,i);  //ilgili key ve webpage degerine sahip node u olustur
            
            new_node->next = (*hashTable)[index].head;  //olusturulan node u hashtable array inde
            (*hashTable)[index].head = new_node;        //ilgili indise yerlestir
            
            
            ptr2 = strtok(NULL," ");    //sonraki token a gec
            
        }
    }
}


void printHash(HASH** hashTable, int tableSize){
    int i;
    NODE* current;
    
    for(i=0; i<tableSize; i++){
        current = (*hashTable)[i].head;
        
        while(current != NULL){
            printf("%d. Indis %d. Denemede Yerlesti -> %s : %s \n",current->key,current->count,current->query,current->webPage);
            current = current->next;
        }
    }
}

void searchOnHashTable(HASH** hashTable,char* query, RESULT *resultHead, int tableSize){
    
    int i = 0;
    int andFlag=0;
    int orFlag=0;
    unsigned long long int key;
    NODE* current = NULL;
    RESULT* currentResult = NULL;
    
    char *ptr1;
    char *ptr2;
    ptr1 = strtok(query,"\n");
    ptr2 = strtok(ptr1," ");
    
    
    while(ptr2 != NULL){
        key = hornerKey(ptr2);
        key = key % tableSize;
        current = (*hashTable)[key].head;
        
        if(!strcmp(ptr2, "and")){
            andFlag = 1;
            ptr2 = strtok(NULL," ");
            key = hornerKey(ptr2);
            key = key % tableSize;
            current = (*hashTable)[key].head;
        }
        if(!strcmp(ptr2, "or")){
            orFlag = 1;
            ptr2 = strtok(NULL," ");
            key = hornerKey(ptr2);
            key = key % tableSize;
            current = (*hashTable)[key].head;
        }
        
        
        //NULL  gorene kadar ya da basladigin noktaya donene kadar aramaya devam et
        i=0;
        while((current != NULL) && (i<tableSize) && strcmp(current->query,ptr2)){
            i++;
            current = (*hashTable)[key + i].head; // yerlesebilecegi indisleri ara i=0 i=1 ... i=tableSize-1
        }
        if(i<tableSize){
            while(current != NULL){
                currentResult = createResultNode(current->webPage);
                currentResult->next = resultHead;
                resultHead = currentResult;
                current = current->next;
            }
        }else{
            printf("Page could not found");
        }
        
        ptr2 = strtok(NULL," ");
    }
    
    if(andFlag == 1){
        while(resultHead->next != NULL){
            currentResult = resultHead->next;
            while(currentResult != NULL){
                if(!strcmp(resultHead->result, currentResult->result)){
                    printf("%s", resultHead->result);
                }
                currentResult = currentResult->next;
            }
            resultHead = resultHead->next;
        }
    }
    
    else if (orFlag == 1){
        while(resultHead->next != NULL){
            currentResult = resultHead->next;
            while(currentResult != NULL){
                if(!strcmp(resultHead->result, currentResult->result)){
                    currentResult->next->flag =1;
                }
                currentResult = currentResult->next;
            }
            if(resultHead->flag != 1){
                printf("%s", resultHead->result);
            }
            resultHead = resultHead->next;
        }
    }else{
        while(resultHead != NULL){
            printf("%s", resultHead->result);
            resultHead = resultHead->next;
        }
    }
   // if(currentResult == NULL){
    //    printf("Sonuc Bulunamadi \n\n");
   // }
}

bool isPrime(int n)
{
    if (n <= 1)  return false;
    if (n <= 3)  return true;
    if (n%2 == 0 || n%3 == 0) return false;
   
    for (int i=5; i*i<=n; i=i+6)
        if (n%i == 0 || n%(i+2) == 0)
           return false;
   
    return true;
}

int nextPrime(int N)
{

    if (N <= 1)
        return 2;
 
    int prime = N;
    bool found = false;

    while (!found) {
        prime++;
        
        if (isPrime(prime))
            found = true;
    }
 
    return prime;
}

int calculateTableSize(FILE *fp){
    
    char buffer[200];
    int count =0;
    char* ptr1;
    char* ptr2;
    
    while(fgets(buffer,201,fp)){
        
        fgets(buffer,201,fp);
        ptr1 = strtok(buffer,"\r");
        ptr2 = strtok(ptr1," ");

        while(ptr2 != NULL ){
            count++;
            ptr2 = strtok(NULL," ");
        }
    }
    return count;
}



int main(int argc, const char * argv[]) {
    
    char query[30];
    FILE* fp;
    int tableSize=0;
    int keyCount;
    int mode;
    
    fp = fopen("/Users/canozer/Downloads/hw3/Sample.txt","r");
    if(fp != NULL){
        keyCount = calculateTableSize(fp);
        tableSize = keyCount/LOADFACTOR;
        tableSize = nextPrime(tableSize);
    }
    fclose(fp);
    
   
    HASH* hashTable = NULL;
    hashTable = (HASH *)calloc(tableSize,sizeof(HASH));
    
    RESULT* resultHead = NULL;
    resultHead = (RESULT*)malloc(sizeof(RESULT));
    resultHead->next = NULL;
    
    fp = fopen("/Users/canozer/Downloads/hw3/Sample.txt","r");

    if(fp != NULL){
        readFileAndHashIt(fp,&hashTable,tableSize); // if there is hash.txt then load it
    }
    fclose(fp);

    printf("Normal Mode: 1, Detailed Mode: 2\n");
    printf("-------------------------------------------------------------------------------------\n\n");
    printf("Calistirmak istediginiz modu giriniz : ");
    scanf("%d",&mode);
    
    if(mode == 1){
        
        printf("Sorguyu giriniz: ");
        fflush (stdin);
        fgets(query,sizeof(query), stdin);
        printf("\nBulunan Sonuclar: \n\n");
        searchOnHashTable(&hashTable, query, resultHead,tableSize);

    }else if(mode == 2){
        
        printf("Load Factor : %f \n",LOADFACTOR);
        printf("Hash tablosunun uzunlugu : %d \n",tableSize);
        printHash(&hashTable, tableSize);
        printf("\n");
        printf("Sorguyu giriniz: ");
        fflush (stdin);
        fgets(query,sizeof(query), stdin);
        printf("\nBulunan Sonuclar: \n\n");
        searchOnHashTable(&hashTable, query, resultHead,tableSize);
        
    }else{
        printf("\n Gecersiz Deger Girildi \n\n");
    }
    return 0;
}
