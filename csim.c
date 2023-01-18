// Name: Eugene Tye
// Email: weishun.tye@wmich.edu

#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

// used for timestamp
struct timespec tv;

// struct for line
typedef struct{
    int valid;
    unsigned long long tag;
    double timestamp;   // for FIFO implementation
}Line;

// struct for set
typedef struct{
    Line *lines;
}Set;

// struct for cache
typedef struct{
    Set *sets;
}Cache;

int hitCount = 0;
int missCount = 0;
int evictionCount = 0;

int main(int argc, char *argv[]){

    // check for number of arguments
    if (argc != 9){
        printf("Usage: ./csim -s <s> -E <E> -b <b> -t <tracefile>\n");
        exit(1);
    }

    int setIndexBits;
    int linesPerSet;
    int blockBits;
    char* tracefile;

    for (int i = 0; i < argc; i++){
        if (strcmp(argv[i], "-s") == 0){
            setIndexBits = atoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "-E") == 0){
            linesPerSet = atoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "-b") == 0){
            blockBits = atoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "-t") == 0){
            tracefile = argv[i + 1];
        }
    }

    // set up cache
    Cache cache;

    // find number of sets
    unsigned long long numberOfSets = 1 << setIndexBits;

    cache.sets = calloc(numberOfSets, sizeof(Set));
    for (int i = 0; i < numberOfSets; i++){
        cache.sets[i].lines = calloc(linesPerSet, sizeof(Line));
    }

    FILE *inputfile = fopen(tracefile, "r");
    char operation;
    unsigned long long address;
    int size;

    while (fscanf(inputfile, "%c %llx %d", &operation, &address, &size) != EOF){

        // M will always be a hit
        if (operation == 'M'){
            hitCount++;
        }

        // ignore I
        if (operation == 'L' || operation == 'S' || operation == 'M'){
            int tagBits = 64 - setIndexBits - blockBits;
            unsigned long long tag = address >> (64 - tagBits);
            unsigned long long setIndex = (address << tagBits) >> (tagBits + blockBits);

            bool hit = false;
            bool emptySpot = false;
            int emptySpotIndex = 0;

            for (int i = 0; i < linesPerSet; i++){

                // it's a hit if it's valid and the tag matches
                if (cache.sets[setIndex].lines[i].valid){
                    if(cache.sets[setIndex].lines[i].tag == tag){
                        hitCount++;
                        hit = true;
                    }
                }

                // if it's not valid, it would be an empty spot
                else{
                    emptySpotIndex = i;
                    emptySpot = true;
                }
            }

            // if it's a miss,
            if (!hit){
                missCount++;

                // use the spot if it's empty
                if(emptySpot){
                    cache.sets[setIndex].lines[emptySpotIndex].tag = tag;
                    cache.sets[setIndex].lines[emptySpotIndex].valid = 1;

                    // update the timestamp
                    char time_str[32];
                    clock_gettime(CLOCK_REALTIME, &tv);
                    sprintf(time_str, "%ld.%.9ld\n", tv.tv_sec, tv.tv_nsec);
                    cache.sets[setIndex].lines[emptySpotIndex].timestamp = atof(time_str);

                }

                // otherwise, do the eviction
                else{
                    evictionCount++;

                    // within the same setIndex, search for the smallest timestamp, and that index will be evicted
                    double minTime = cache.sets[setIndex].lines[0].timestamp;
                    int evictionIndex = 0;

                    for (int i = 1; i < linesPerSet; i++){
                        if (cache.sets[setIndex].lines[i].timestamp < minTime){
                            minTime = cache.sets[setIndex].lines[i].timestamp;
                            evictionIndex = i;
                        }
                    }

                    cache.sets[setIndex].lines[evictionIndex].tag = tag;

                    // update the timestamp
                    char time_str[32];
                    clock_gettime(CLOCK_REALTIME, &tv);
                    sprintf(time_str, "%ld.%.9ld\n", tv.tv_sec, tv.tv_nsec);
                    cache.sets[setIndex].lines[evictionIndex].timestamp = atof(time_str);
                }
            }
        }
    }

    // print results
    printf("hits:%d misses:%d evictions:%d\n", hitCount, missCount, evictionCount);

    // free memory
    fclose(inputfile);
    for (int i = 0; i < numberOfSets; i++){
        free(cache.sets[i].lines);
    }   
    free(cache.sets);

    return 0;
}    


