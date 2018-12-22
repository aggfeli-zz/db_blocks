#ifndef BASEIS_HELPER_H
#define BASEIS_HELPER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 20
#define ADDRESS_SIZE 40

typedef struct{
    int id;
    char name[SIZE];
    char surname[SIZE];
    char address[ADDRESS_SIZE];
} Record;

typedef struct{
    int recordsCounter;
    Record** records;
    int maxRecords;
    int overflowBucket;
} Block;

int getRecordSize();

Record* createRecord(int id, char* name, char* surname, char* address);

//Record* createEmptyRecord();

Block* createEmptyBlock();

unsigned char* recordToByteArray(Record* record);

unsigned char* blockToByteArray( Block* block);

Block* blockFromByteArray(void* byteArray);

int addBlockRecord(Block* block, Record* record, int numBlocks);

int printRecord(Record* record);

int printBlock(Block* block);

int printBucket(Block* bucket,char * attrName, char attrType, void * value);

#endif //BASEIS_HELPER_H
