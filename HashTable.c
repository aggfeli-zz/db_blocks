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

unsigned hashFunction(void *key, HT_info ht_info);

int HT_CreateIndex(char *fileName, char attrType, char *attrName, int attrLength, int buckets) {

    BF_Init();
    void *block, *hashArray;
    int bfs[3];
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
    strcpy(headerInfo->attrName, attrName);

    initializeMetadataBlock(headerInfo);

    initializeHashArray(headerInfo);

    initializeBuckets(headerInfo);

//    for (int i = 0; i <= buckets; i++) {
//
//        if (BF_AllocateBlock(fileDesc) < 0) {
//            BF_PrintError("Error allocating block");
//        }
//
//        if (BF_ReadBlock(fileDesc, 0, &block) < 0) {
//            BF_PrintError("Error getting block");
//        }
//
//        if (i == 0) { // create the block which contains the metadata
//
//            memcpy(block, "Hashtable", sizeof("Hashtable"));
//            memcpy(block + sizeof("Hashtable"), &attrType, sizeof(char));
//            memcpy(block + sizeof("Hashtable") + sizeof(char), attrName, sizeof(char*));
//            memcpy(block + sizeof("Hashtable") + sizeof(char) + sizeof(char*), &attrLength, sizeof(int));
//            memcpy(block + sizeof("Hashtable") + sizeof(char) + sizeof(char*) + sizeof(int), &buckets, sizeof(int));
//
//            // for -> store the number of indexes ???????????????
//
//            if (BF_WriteBlock(fileDesc, 0) < 0){
//                BF_PrintError("Error writing block back");
//            }
//
//            if (BF_AllocateBlock(fileDesc) < 0) {
//                BF_PrintError("Error allocating block");
//            }
//
//            if (BF_ReadBlock(fileDesc, 0, &hashArray) < 0) {
//                BF_PrintError("Error getting block");
//            }
//
//            for(int i = 0; i < buckets; i++) {  // create the hash array and store the number of buckets
//                memcpy(&hashArray +(i * sizeof(int)), &i, sizeof(int));
//            }
//
//            if (BF_WriteBlock(fileDesc, 0) < 0){
//                BF_PrintError("Error writing block back");
//            }
//        }
//
//        if (BF_WriteBlock(fileDesc, 0) < 0){
//            BF_PrintError("Error writing block back");
//        }
//    }

    if (BF_CloseFile(fileDesc) < 0) {
        BF_PrintError("Error closing file");
    }

    free(headerInfo);

    return 0;
}

HT_info *HT_OpenIndex(char *fileName) {
    void *block;
    char *fileType;

    HT_info *headerInfo = malloc(sizeof(HT_info));

    if ((headerInfo->fileDesc = BF_OpenFile(fileName)) < 0) {
        BF_PrintError("Error opening file");
        return NULL;
    }

    if (BF_ReadBlock(headerInfo->fileDesc, 0, &block) < 0) {
        BF_PrintError("Error reading file");
        return NULL;
    }

    memcpy(&fileType, block, sizeof(int));

    if (strcmp(fileType, "HashTable") == 0) {
        BF_PrintError("Wrong type of file");
        return NULL;
    }

    memcpy(&(headerInfo->attrType), block + sizeof("Hashtable"), sizeof(char));
    memcpy(&(headerInfo->attrName), block + sizeof("Hashtable") + sizeof(char), sizeof(char *));
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
    free(header_info); //efoson kaname malloc, eleutherwsh mnhmhs
    return 0;
}

int HT_InsertEntry(HT_info header_info, Record record) {
    void *block;

    int numBlocks = BF_GetBlockCounter(header_info.fileDesc);

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
    }
        return hashIndex;
}