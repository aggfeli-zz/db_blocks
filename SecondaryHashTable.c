#include "SecondaryHashTable.h"

void initializeSecondaryBucket(void *bucket);

void initializeSecondaryBuckets(SHT_info *headerInfo);

void initializeSecondaryHashArray(SHT_info *headerInfo);

void initializeSecondaryMetadataBlock(SHT_info *headerInfo);

int SHT_CreateSecondaryIndex( char *sfileName, char* attrName, int attrLength, int buckets) {
    BF_Init();
    int fileDesc;

    if (BF_CreateFile(sfileName) < 0) {
        BF_PrintError("Error opening file");
    }

    if ((fileDesc = BF_OpenFile(sfileName)) < 0) {
        BF_PrintError("Error opening file");
    }

    SHT_info *headerInfo = malloc(sizeof(SHT_info));
    headerInfo->attrLength = attrLength;
    headerInfo->fileDesc = fileDesc;
    headerInfo->numBuckets = buckets;
    headerInfo->attrName = malloc(sizeof(char *));
    strcpy(headerInfo->attrName, attrName);

    initializeSecondaryMetadataBlock(headerInfo);

    initializeSecondaryHashArray(headerInfo);

    initializeSecondaryBuckets(headerInfo);

    if (BF_CloseFile(fileDesc) < 0) {
        BF_PrintError("Error closing file");
    }

    free(headerInfo->attrName);
    free(headerInfo);

    return 0;
}

SHT_info* SHT_OpenSecondaryIndex( char *sfileName) {
    void *block;

    SHT_info *headerInfo = malloc(sizeof(SHT_info));
    headerInfo->attrName = malloc(sizeof(char *));

    if ((headerInfo->fileDesc = BF_OpenFile(sfileName)) < 0) {
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

    memcpy((headerInfo->attrName), block + sizeof("Hashtable"), sizeof(char *));
    memcpy(&(headerInfo->attrLength), block + sizeof("Hashtable") + sizeof(char *), sizeof(int));
    memcpy(&(headerInfo->numBuckets), block + sizeof("Hashtable") + sizeof(char *) + sizeof(int), sizeof(int));

    return headerInfo;
}

int SHT_CloseSecondaryIndex(SHT_info* header_info) {
    if (BF_CloseFile(header_info->fileDesc) < 0) {
        BF_PrintError("Error closing secondary hash file");
        return -1;
    }
    free(header_info->attrName);
    free(header_info);
    return 0;
}

void initializeSecondaryBucket(void *bucket) {
    Block* block = createEmptyBlock();
    unsigned char* byteArray = blockToByteArray(block);
    memcpy(bucket, byteArray, BLOCK_SIZE);
}

void initializeSecondaryBuckets(SHT_info *headerInfo) {
    void *bucket;

    /* The first 2 blocks are allocated by the metadata and hashArray*/
    for (int i = 2; i < (headerInfo->numBuckets + 2); i++) {

        if (BF_AllocateBlock(headerInfo->fileDesc) < 0) {
            BF_PrintError("Error allocating block");
        }

        if (BF_ReadBlock(headerInfo->fileDesc, i, &bucket) < 0) {
            BF_PrintError("Error getting block");
        }

        initializeSecondaryBucket(bucket);

        if (BF_WriteBlock(headerInfo->fileDesc, i) < 0) {
            BF_PrintError("Error writing block back");
        }
    }
}

void initializeSecondaryHashArray(SHT_info *headerInfo) {
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

void initializeSecondaryMetadataBlock(SHT_info *headerInfo) {
    void *block;

    if (BF_AllocateBlock(headerInfo->fileDesc) < 0) {
        BF_PrintError("Error allocating block");
    }

    if (BF_ReadBlock(headerInfo->fileDesc, 0, &block) < 0) {
        BF_PrintError("Error getting block");
    }

    memcpy(block, "Hashtable", sizeof("Hashtable"));
    memcpy(block + sizeof("Hashtable"), headerInfo->attrName, sizeof(char *));
    memcpy(block + sizeof("Hashtable") + sizeof(char *), &headerInfo->attrLength, sizeof(int));
    memcpy(block + sizeof("Hashtable") + sizeof(char *) + sizeof(int), &headerInfo->numBuckets, sizeof(int));

    // for -> store the number of indexes ???????????????

    if (BF_WriteBlock(headerInfo->fileDesc, 0) < 0) {
        BF_PrintError("Error writing block back");
    }
}