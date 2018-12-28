#include <stdio.h>
#include "HashTable.h"

#define CALL_OR_DIE(call)     \
{                             \
    int code = call;     \
    char message[20];    \
    if (code != BFE_OK) {     \
      sprintf(message, "Error code is %d", code); \
      BF_PrintError(message);    \
      exit(EXIT_FAILURE);     \
    }                         \
}

void initializeMetadataBlock(HT_info *headerInfo);

void initializeHashArray(HT_info *headerInfo);

void initializeBuckets(HT_info *headerInfo);

void initializeBucket(void* bucket);

void* chooseHashingAttribute(HT_info ht_info, Record *record);

unsigned hashFunction(void *key, HT_info ht_info);

int HT_CreateIndex(char *fileName, char attrType, char *attrName, int attrLength, int buckets) {

    BF_Init();
    int fileDesc;

    CALL_OR_DIE(BF_CreateFile(fileName));

    if ((fileDesc = BF_OpenFile(fileName)) < 0) {
        BF_PrintError("Error opening file");
    }

    HT_info *headerInfo = malloc(sizeof(HT_info));
    headerInfo->attrLength = attrLength;
    headerInfo->fileDesc = fileDesc;
    headerInfo->numBuckets = buckets;
    headerInfo->attrType = attrType;
    headerInfo->attrName = malloc(sizeof(char *));
    strcpy(headerInfo->attrName, attrName);

    initializeMetadataBlock(headerInfo);

    initializeHashArray(headerInfo);

    initializeBuckets(headerInfo);

    if (BF_CloseFile(fileDesc) < 0) {
        BF_PrintError("Error closing file");
    }

    free(headerInfo->attrName);
    free(headerInfo);

    return 0;
}

HT_info *HT_OpenIndex(char *fileName) {
    void *block;

    HT_info *headerInfo = malloc(sizeof(HT_info));
    headerInfo->attrName = malloc(sizeof(char *));

    if ((headerInfo->fileDesc = BF_OpenFile(fileName)) < 0) {
        BF_PrintError("Error opening file");
        return NULL;
    }

    if (BF_ReadBlock(headerInfo->fileDesc, 0, &block) < 0) {
        BF_PrintError("Error reading file");
        return NULL;
    }

    if (memcmp("Hashtable", block, sizeof("Hashtable"))) {
        BF_PrintError("Wrong type of file");
        return NULL;
    }

    memcpy(&(headerInfo->attrType), block + sizeof("Hashtable"), sizeof(char));
    memcpy(headerInfo->attrName, block + sizeof("Hashtable") + sizeof(char), sizeof(char *));
    memcpy(&(headerInfo->attrLength), block + sizeof("Hashtable") + sizeof(char *) + sizeof(char), sizeof(int));
    memcpy(&(headerInfo->numBuckets), block + sizeof("Hashtable") + sizeof(char *) + sizeof(char) + sizeof(int),
           sizeof(int));

    return headerInfo;
}

int HT_CloseIndex(HT_info *header_info) {
    if (BF_CloseFile(header_info->fileDesc) < 0) {
        BF_PrintError("Error closing hash file");
        return -1;
    }
    free(header_info->attrName);
    free(header_info);
    return 0;
}

int HT_InsertEntry(HT_info header_info, Record record) {
    void *block;
    unsigned char* byteArray;

    int numBlocks = BF_GetBlockCounter(header_info.fileDesc);

    int hashIndex = hashFunction(chooseHashingAttribute(header_info, &record), header_info);

    while (1) {
        if (BF_ReadBlock(header_info.fileDesc, hashIndex, &block) < 0) {
            BF_PrintError("Error getting block");
        }

        Block* bucket = blockFromByteArray(block);

        int result = addBlockRecord(bucket, &record, numBlocks);

        if (result == 0) { //success
            byteArray = blockToByteArray(bucket);
            break;
        } else if (result == -1) { //create overflow bucket

            /*save the changes in current bucket*/
            byteArray = blockToByteArray(bucket);

            memcpy(block, byteArray, BLOCK_SIZE);

            if (BF_WriteBlock(header_info.fileDesc, hashIndex) < 0) {
                BF_PrintError("Error writing block back");
            }

            /*Allocate new bucket*/
            if (BF_AllocateBlock(header_info.fileDesc) < 0) {
                BF_PrintError("Error allocating block");
            }

            hashIndex = numBlocks;

            if (BF_ReadBlock(header_info.fileDesc, hashIndex, &block) < 0) {
                BF_PrintError("Error getting block");
            }

            Block* newBucket = createEmptyBlock();

            byteArray = blockToByteArray(newBucket);

            memcpy(block, byteArray, BLOCK_SIZE);

            if (BF_WriteBlock(header_info.fileDesc, hashIndex) < 0) {
                BF_PrintError("Error writing block back");
            }

        } else { //move to overflow buckets
            hashIndex = result;
        }
    }

    memcpy(block, byteArray, BLOCK_SIZE);

    if (BF_WriteBlock(header_info.fileDesc, hashIndex) < 0) {
        BF_PrintError("Error writing block back");
    }

    return hashIndex;
}

int HT_DeleteEntry(HT_info header_info, void *value) {
    void *block;
    unsigned char *byteArray;

    int hashIndex = hashFunction(value, header_info);

    while (1) {
        if (BF_ReadBlock(header_info.fileDesc, hashIndex, &block) < 0) {
            BF_PrintError("Error getting block");
        }

        Block *bucket = blockFromByteArray(block);

        int result = searchBlock(bucket, header_info.attrName, header_info.attrType, value);

        /*save the changes in current bucket*/
        byteArray = blockToByteArray(bucket);

        memcpy(block, byteArray, BLOCK_SIZE);

        if (BF_WriteBlock(header_info.fileDesc, hashIndex) < 0) {
            BF_PrintError("Error writing block back");
        }

        if (bucket->overflowBucket != 0) {
            hashIndex = bucket->overflowBucket;
        } else {
            break;
        }
    }
    return 0;
}

int HT_GetAllEntries(HT_info header_info, void *value) {
    void *block;
    int numOfPrintedRecords = 0;

    int hashIndex = hashFunction(value, header_info);

    while (1) {
        if (BF_ReadBlock(header_info.fileDesc, hashIndex, &block) < 0) {
            BF_PrintError("Error getting block");
        }

        Block* bucket = blockFromByteArray(block);
        numOfPrintedRecords += printBucket(*bucket, header_info.attrName, header_info.attrType, value);

        if (bucket->overflowBucket != 0) {
            hashIndex = bucket->overflowBucket;
        } else {
            break;
        }
    }

    printf("Number of records printed: %d\n", numOfPrintedRecords);
    return numOfPrintedRecords;
}

void initializeBucket(void* bucket) {
    Block* block = createEmptyBlock();
    unsigned char* byteArray = blockToByteArray(block);
    memcpy(bucket, byteArray, BLOCK_SIZE);
}

void initializeBuckets(HT_info *headerInfo) {
    void *bucket;

    /* The first 2 blocks are allocated by the metadata and hashArray*/
    for (int i = 2; i < (headerInfo->numBuckets + 2); i++) {

        if (BF_AllocateBlock(headerInfo->fileDesc) < 0) {
            BF_PrintError("Error allocating block");
        }

        if (BF_ReadBlock(headerInfo->fileDesc, i, &bucket) < 0) {
            BF_PrintError("Error getting block");
        }

        initializeBucket(bucket);

        if (BF_WriteBlock(headerInfo->fileDesc, i) < 0) {
            BF_PrintError("Error writing block back");
        }
    }
}

void initializeHashArray(HT_info *headerInfo) {
    void *hashArray;

    if (BF_AllocateBlock(headerInfo->fileDesc) < 0) {
        BF_PrintError("Error allocating block");
    }

    if (BF_ReadBlock(headerInfo->fileDesc, 1, &hashArray) < 0) {
        BF_PrintError("Error getting block");
    }

    for (int i = 0; i < headerInfo->numBuckets; i++) {  //store the number of buckets
        memcpy(&hashArray + (i * sizeof(int)), &i, sizeof(int));
    }

    if (BF_WriteBlock(headerInfo->fileDesc, 1) < 0) {
        BF_PrintError("Error writing block back");
    }

}

void initializeMetadataBlock(HT_info *headerInfo) {
    void *block;

    if (BF_AllocateBlock(headerInfo->fileDesc) < 0) {
        BF_PrintError("Error allocating block");
    }

    if (BF_ReadBlock(headerInfo->fileDesc, 0, &block) < 0) {
        BF_PrintError("Error getting block");
    }

    memcpy(block, "Hashtable", sizeof("Hashtable"));
    memcpy(block + sizeof("Hashtable"), &headerInfo->attrType, sizeof(char));
    memcpy(block + sizeof("Hashtable") + sizeof(char), headerInfo->attrName, sizeof(char *));
    memcpy(block + sizeof("Hashtable") + sizeof(char) + sizeof(char *), &headerInfo->attrLength, sizeof(int));
    memcpy(block + sizeof("Hashtable") + sizeof(char) + sizeof(char *) + sizeof(int), &headerInfo->numBuckets,
           sizeof(int));

    // for -> store the number of indexes ???????????????

    if (BF_WriteBlock(headerInfo->fileDesc, 0) < 0) {
        BF_PrintError("Error writing block back");
    }
}

void* chooseHashingAttribute(HT_info ht_info, Record *record) {
    if (strcmp(ht_info.attrName, "id") == 0) {
        return &record->id;
    } else if (strcmp(ht_info.attrName, "name") == 0) {
        return &record->name;
    } else if (strcmp(ht_info.attrName, "surname") == 0) {
        return &record->surname;
    } else {
        return &record->address;
    }

}

unsigned hashFunction(void *key, HT_info ht_info) {
    unsigned hashIndex = 0;

    if (ht_info.attrType == 'i') {
        int integerKey = *(int *)key;
        hashIndex = (unsigned int) (integerKey % ht_info.numBuckets);
    } else {
        char *k = key;

        for (int i = 0; i < ht_info.attrLength; i++) {
            hashIndex += (int) k[i];
        }
        hashIndex %= ht_info.numBuckets;
    }
    return hashIndex + 2;
}