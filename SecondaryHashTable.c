#include "SecondaryHashTable.h"

void initializeSecondaryBucket(void *bucket);

void initializeSecondaryBuckets(SHT_info *headerInfo);

void initializeSecondaryHashArray(SHT_info *headerInfo);

void initializeSecondaryMetadataBlock(SHT_info *headerInfo);

int secondaryHashFunction(void *key, SHT_info ht_info);

void sortBlockIds(int *pInt, int size);

int SHT_CreateSecondaryIndex(char *sfileName, char* attrName, int attrLength, int buckets, char* fileName) {
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

    if (memcmp("SHashtable", block, sizeof("SHashtable"))) {
        BF_PrintError("Wrong type of file");
        return NULL;
    }

    memcpy((headerInfo->attrName), block + sizeof("SHashtable"), sizeof(char *));
    memcpy(&(headerInfo->attrLength), block + sizeof("SHashtable") + sizeof(char *), sizeof(int));
    memcpy(&(headerInfo->numBuckets), block + sizeof("SHashtable") + sizeof(char *) + sizeof(int), sizeof(int));

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

int SHT_SecondaryInsertEntry(SHT_info header_info, SecondaryRecord record) {
    void *block;
    unsigned char* byteArray;

    int numBlocks = BF_GetBlockCounter(header_info.fileDesc);

    int hashIndex = secondaryHashFunction(record.record.name, header_info);

    while (1) {
        if (BF_ReadBlock(header_info.fileDesc, hashIndex, &block) < 0) {
            BF_PrintError("Error getting block");
        }

        SecondaryBlock* bucket = secondaryBlockFromByteArray(block);

        int result = addSecondaryBlockRecord(bucket, record.record.name, record.blockId, numBlocks);

        if (result == 0) { //success
            byteArray = secondaryBlockToByteArray(bucket);
            break;
        } else if (result == -1) { //create overflow bucket

            /*save the changes in current bucket*/
            byteArray = secondaryBlockToByteArray(bucket);

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

            SecondaryBlock* newBucket = createEmptySecondaryBlock();

            byteArray = secondaryBlockToByteArray(newBucket);

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

int SHT_SecondaryGetAllEntries(SHT_info header_info_sht, HT_info header_info_ht, void *value) {
    void *block;
    int numOfPrintedRecords = 0, numOfReadBlocks = 0;

    int hashIndex = secondaryHashFunction(value, header_info_sht);

    while (1) {
        if (BF_ReadBlock(header_info_sht.fileDesc, hashIndex, &block) < 0) {
            BF_PrintError("Error getting block");
        }
        numOfReadBlocks++;

        SecondaryBlock* bucket = secondaryBlockFromByteArray(block);
        int *blockIds = malloc(bucket->recordsCounter * sizeof(int));
        numOfPrintedRecords += printSecondaryBucket(*bucket, value, blockIds);

        sortBlockIds(blockIds, bucket->recordsCounter);
        HT_GetAllEntry(header_info_ht, value, blockIds, bucket->recordsCounter);

        free(blockIds);

        if (bucket->overflowBucket != 0) {
            hashIndex = bucket->overflowBucket;
        } else {
            break;
        }
    }

//    printf("Number of records printed: %d\n", numOfPrintedRecords);
//    printf("Number of blocks read: %d\n", numOfReadBlocks);
    if (numOfPrintedRecords > 0) {
        return numOfReadBlocks;
    } else {
        return -1;
    }
}

int SHT_Statistics(SHT_info ht_info) {
    void *block;
    int numOfRecords = 0, hashIndex, min = -1, max = 0, numOfBlocks = 0, numOfBucketsThatHaveOverflow = 0, arrayWithNumOfOverflow[ht_info.numBuckets];

    memset(arrayWithNumOfOverflow, 0 , ht_info.numBuckets * sizeof(int));

    for (int i = 2; i < ht_info.numBuckets + 2; ++i) {
        hashIndex = i;

        while (1) {
            if (BF_ReadBlock(ht_info.fileDesc, hashIndex, &block) < 0) {
                BF_PrintError("Error getting block");
            }

            Block* bucket = blockFromByteArray(block);
            numOfBlocks++;
            numOfRecords += bucket->recordsCounter;

            if(min > bucket->recordsCounter || i == 2) {
                min = bucket->recordsCounter;
            }

            if (max < bucket->recordsCounter) {
                max = bucket->recordsCounter;
            }

            if (bucket->overflowBucket != 0) {
                hashIndex = bucket->overflowBucket;
                numOfBucketsThatHaveOverflow++;
                arrayWithNumOfOverflow[i - 2]++;
            } else {
                break;
            }
        }
    }

    printf("**************************************************************************\n");

    printf("Stat 1: Number of blocks %d\n",BF_GetBlockCounter(ht_info.fileDesc));

    printf("Stat 2: Min number of records %d\n",min);
    printf("Stat 2: Mean number of records %.2f\n",numOfRecords / (float) numOfBlocks);
    printf("Stat 2: Max number of records %d\n", max);

    printf("Stat 3: Mean number of blocks %.2f\n",numOfBlocks / (float) ht_info.numBuckets);

    printf("Stat 4: Number of buckets that have overflow %d\n",numOfBucketsThatHaveOverflow);
    for (int i = 0; i < ht_info.numBuckets; ++i) {
        printf("Stat 4: Number of overflow buckets in bucket %d:  %d\n", (i + 1), arrayWithNumOfOverflow[i]);
    }

}

void sortBlockIds(int *pInt, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (pInt[j] < pInt[i]) {
                int tmp = pInt[i];
                pInt[i] = pInt[j];
                pInt[j] = tmp;
            }
        }
    }
}

int secondaryHashFunction(void *key, SHT_info ht_info) {
    unsigned hashIndex = 0;

    if (strcmp(ht_info.attrName, "id")) {
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

    memcpy(block, "SHashtable", sizeof("SHashtable"));
    memcpy(block + sizeof("SHashtable"), headerInfo->attrName, sizeof(char *));
    memcpy(block + sizeof("SHashtable") + sizeof(char *), &headerInfo->attrLength, sizeof(int));
    memcpy(block + sizeof("SHashtable") + sizeof(char *) + sizeof(int), &headerInfo->numBuckets, sizeof(int));

    // for -> store the number of indexes ???????????????

    if (BF_WriteBlock(headerInfo->fileDesc, 0) < 0) {
        BF_PrintError("Error writing block back");
    }
}