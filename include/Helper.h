#ifndef BASEIS_HELPER_H
#define BASEIS_HELPER_H
#include <stdio.h>
#include <HashTable.h>

int getRecordSize();

Record* createRecord(int id, char* name, char* surname, char* address);

//Record* createEmptyRecord();

Block* createEmptyBlock();

unsigned char* recordToByteArray(Record* record);

unsigned char* blockToByteArray( Block* block);

Block* blockFromByteArray(void* byteArray);

int addBlockRecord(Block* block, Record* record);

int printBlock(Block* block);

#endif //BASEIS_HELPER_H
