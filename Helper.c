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

//Record* createEmptyRecord() {
//    Record* record = (Record*) malloc(sizeof(Record));
//    record->id = -1;
//    record->name = malloc(sizeof(char) * SIZE);
//
//}

Block* createEmptyBlock() {
    Block* block = malloc(sizeof(Block));
    int maxRecords = BLOCK_SIZE / getRecordSize();
    block->maxRecords = maxRecords;
    block->records = malloc(sizeof(Record*) * maxRecords);
    block->recordsCounter = 0;
    return block;
}

unsigned char* recordToByteArray(Record* record) {
    unsigned char* byteArray;
    byteArray = malloc((size_t) getRecordSize());
    memcpy(byteArray, &(record->id), sizeof(int));
    memcpy(&byteArray[sizeof(int)], &(record->name), sizeof(SIZE));
    memcpy(&byteArray[sizeof(int) + SIZE], &(record->surname), sizeof(SIZE));
    memcpy(&byteArray[sizeof(int) + (SIZE * 2)], &(record->address), sizeof(SIZE));

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
    byteArray[0] = 1;
    byteArray[1] = block->recordsCounter;
    for (int i = 0; i < block->recordsCounter; ++i) {
        memcpy(&(byteArray[2 + i * getRecordSize()]), recordToByteArray(block->records[i]), sizeof(Record));
    }
    return byteArray;
}

Block* blockFromByteArray(void* byteArray) {
    Block* block = createEmptyBlock();
    unsigned char* charArray = (unsigned char*) byteArray;
    if (charArray[0] == 0) {
        printf("Empty\n");
        return NULL;
    }
    char numRecords = charArray[1];
    for (int i = 0; i < numRecords; ++i) {
        unsigned char* recordByteArray = &(charArray[2 + i * getRecordSize()]);
        Record* record = recordFromByteArray(recordByteArray);
        block->records[i] = record;
    }
    block->recordsCounter = numRecords;
    return block;
}

int addBlockRecord(Block* block, Record* record) {
    if (block->recordsCounter == block->maxRecords) {
        return -1;
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
