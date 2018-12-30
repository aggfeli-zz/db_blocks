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
    char name[SIZE];
    int blockId;
} SecondarySimpleRecord;

typedef struct{
    int recordsCounter;
    Record** records;
    int maxRecords;
    int overflowBucket;
} Block;

typedef struct{
    int recordsCounter;
    SecondarySimpleRecord** records;
    int maxRecords;
    int overflowBucket;
} SecondaryBlock;

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

int searchBlock(Block* bucket, char * attrName, char attrType, void * value);

int printBucket(Block bucket,char * attrName, char attrType, void * value);


SecondaryBlock* createEmptySecondaryBlock();

SecondarySimpleRecord* createSecondarySimpleRecord(char* name, int blockId);

unsigned char* secondaryRecordToByteArray(SecondarySimpleRecord* record);

SecondarySimpleRecord* secondaryRecordFromByteArray(void *byteArray);

unsigned char* secondaryBlockToByteArray( SecondaryBlock* block);

SecondaryBlock* secondaryBlockFromByteArray(void* byteArray);

int addSecondaryBlockRecord(SecondaryBlock* block, char* name, int blockId, int numBlocks);

int printSecondaryBucket(SecondaryBlock bucket, void * value);

#endif //BASEIS_HELPER_H
