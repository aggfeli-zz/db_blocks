#include <BF.h>
#include "Helper.h"

int getRecordSize() {
    return sizeof(int) + SIZE * 2 + ADDRESS_SIZE;
}


Record* createRecord(int id, char* name, char* surname, char* address) {
    Record* record = (Record*) malloc(sizeof(Record));
    record->id = id;
    strcpy(record->name, name);
    strcpy(record->surname, surname);
    strcpy(record->address, address);
    return record;
}

Block* createEmptyBlock() {
    Block* block = malloc(sizeof(Block));
    int maxRecords = BLOCK_SIZE / getRecordSize() - sizeof(int); // we need to keep the index of the overflow bucket
    block->maxRecords = maxRecords;
    block->records = malloc(sizeof(Record*) * maxRecords);
    block->recordsCounter = 0;
    block->overflowBucket = 0;
    return block;
}

unsigned char* recordToByteArray(Record* record) {
    unsigned char* byteArray;
    byteArray = malloc((size_t) getRecordSize());
    memcpy(byteArray, &(record->id), sizeof(int));
    memcpy(&byteArray[sizeof(int)], (record->name), SIZE);
    memcpy(&byteArray[sizeof(int) + SIZE], (record->surname), SIZE);
    memcpy(&byteArray[sizeof(int) + (SIZE * 2)], (record->address), ADDRESS_SIZE);

    return byteArray;
}

Record* recordFromByteArray(void *byteArray) {
    Record* record = (Record*) malloc(sizeof(Record));
    memcpy(&record->id, byteArray, sizeof(int));
    memcpy(record->name, (char *) byteArray + sizeof(int), SIZE);
    memcpy(record->surname, (char *) byteArray + sizeof(int) + SIZE, SIZE);
    memcpy(record->address, (char *) byteArray + sizeof(int) + (SIZE * 2), ADDRESS_SIZE);
    return record;
}

unsigned char* blockToByteArray( Block* block) {
    unsigned char* byteArray = malloc(BLOCK_SIZE);
    byteArray[0] = (unsigned char) block->recordsCounter;
    byteArray[1] = (unsigned char) block->maxRecords;
    byteArray[2] = (unsigned char) block->overflowBucket;
    for (int i = 0; i < block->recordsCounter; ++i) {
        memcpy(&(byteArray[3 + i * getRecordSize()]), recordToByteArray(block->records[i]), sizeof(Record));
    }
    return byteArray;
}

Block* blockFromByteArray(void* byteArray) {
    Block* block = createEmptyBlock();
    unsigned char* charArray = (unsigned char*) byteArray;
    block->recordsCounter = charArray[0];
    block->maxRecords = charArray[1];
    block->overflowBucket = charArray[2];
//    if (charArray[0] == 0) {
//        printf("Empty\n");
//        return NULL;
//    }
    for (int i = 0; i < block->recordsCounter; ++i) {
        unsigned char* recordByteArray = &(charArray[3 + i * getRecordSize()]);
        Record* record = recordFromByteArray(recordByteArray);
        block->records[i] = record;
    }
    return block;
}

int addBlockRecord(Block* block, Record* record, int numBlocks) {
    if (block->recordsCounter == block->maxRecords) {
        if (block->overflowBucket == 0) {
            block->overflowBucket = numBlocks; //the first bucket contains the metadata
            return -1;  //overflow bucket does not exist
        } else {
            return block->overflowBucket; // return the overflow bucket
        }
    }
    block->records[block->recordsCounter] = record;
    block->recordsCounter++;
    return 0;
}

int printRecord(Record* record) {
    if (record == NULL) {
        return -1;
    }
    printf("Record data: %d, %s, %s,%s \n", record->id, record->name, record->surname, record->address);
    return 0;
}

int printBlock(Block* block) {
    if (block == NULL) {
        return -1;
    }
    for (int i = 0; i < block->recordsCounter; ++i) {
        printRecord(block->records[i]);
    }
    return 0;
}

int printBucket(Block bucket, char * attrName, char attrType, void * value) {
    int numOfPrintedRecords = 0;
    for (int j = 0; j < bucket.recordsCounter; ++j) {
        if (attrType == 'i') {
            if (bucket.records[j]->id == *(int *) value){
                printRecord(bucket.records[j]);
                numOfPrintedRecords++;
            }

        } else {
            if (strcmp(attrName, "name") == 0) {
                if (strcmp(bucket.records[j]->name, (char*) value) == 0 ) {
                    printRecord(bucket.records[j]);
                    numOfPrintedRecords++;
                }
            } else if (strcmp(attrName, "surname") == 0) {
                if (strcmp(bucket.records[j]->surname, (char*)  value) == 0 ) {
                    printRecord(bucket.records[j]);
                    numOfPrintedRecords++;
                }
            } else {
                if (strcmp(bucket.records[j]->address, (char*) value) == 0 ) {
                    printRecord(bucket.records[j]);
                    numOfPrintedRecords++;
                }
            }
        }
    }

    return numOfPrintedRecords;
}
