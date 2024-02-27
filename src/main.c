#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct barCharecters {
    int charByteSize;
    int charByteOffset;
    char *charecters[50];
    int amountOfChars;
};
typedef enum{
    bottom = 0,
    left = 1,
    top = 2,
    right = 3
} outputDirections;

/// returns amount of bytes changed.
int snprintBlock(char *buffer, const struct barCharecters *charecters, size_t barHeight,const size_t level){
    size_t barLayers = charecters->amountOfChars -1;
    int index = barHeight % barLayers;
    if ((level + 1) * barLayers <= barHeight){
        index = barLayers;
    }else if (level * barLayers >= barHeight){
        index = 0;
    }
    snprintf(buffer, charecters->charByteSize, "%s", charecters->charecters[index]);
    return charecters->charByteOffset;
}

void snprintBarsTop(char *buffer, int *sizeTracker, size_t layers, size_t amount_of_bars, int *bars, const struct barCharecters *charecters, char lineDelimiter){
    for(int level = 0; level < layers; level++) {
        for (int i = 0; i < amount_of_bars; i++) {
            *sizeTracker += snprintBlock(buffer + *sizeTracker, charecters, bars[i], level);
        }
        //appends bar line delimiter
        if (level != layers - 1){
            sprintf(buffer + *sizeTracker , "%c", lineDelimiter);
            *sizeTracker += 1;
        }
    }
}

void snprintBarsBottom(char *buffer, int *sizeTracker, size_t layers, size_t amount_of_bars, int *bars, const struct barCharecters *charecters, char lineDelimiter){
    for(int level = layers - 1; level >= 0; level--) {
        for (int i = 0; i < amount_of_bars; i++) {
            *sizeTracker += snprintBlock(buffer + *sizeTracker, charecters, bars[i], level);
        }
        //appends bar line delimiter
        if (level != 0){
            sprintf(buffer + *sizeTracker , "%c", lineDelimiter);
            *sizeTracker += 1;
        }
    }
}
void snprintBarsLeft(char *buffer, int *sizeTracker, size_t layers, size_t amount_of_bars, int *bars, const struct barCharecters *charecters, char lineDelimiter){
    for(size_t i = 0; i < amount_of_bars; i++){
        for( size_t level = 0; level < layers; level++ ){
            *sizeTracker += snprintBlock(buffer + *sizeTracker, charecters, bars[i], level);
        }
        sprintf(buffer + *sizeTracker , "%c", lineDelimiter);
        *sizeTracker += 1;
    }
}
void snprintBarsRight(char *buffer, int *sizeTracker, size_t layers, size_t amount_of_bars, int *bars, const struct barCharecters *charecters, char lineDelimiter){
    for(size_t i = 0; i < amount_of_bars; i++){
        for( int level = layers - 1; level >= 0; level--){
            *sizeTracker += snprintBlock(buffer + *sizeTracker, charecters, bars[i], level);
        }
        sprintf(buffer + *sizeTracker , "%c", lineDelimiter);
        *sizeTracker += 1;
    }
}

void printFinalLine(int *bars, size_t amountOfBars, size_t maxHeight, const struct barCharecters* charecters, char lineDelimiter, char frameDelimiter, outputDirections direction){
    //byte data to draw no hidden charecters
    //amount of visible bars
    int barLayers = charecters->amountOfChars - 1;

    //buffer to write frame into
    int layers = maxHeight / 8 + (maxHeight % 8 == 0 ? 0 : 1);
    //the 2 is space for the delimiter
    char mainBuf[maxHeight * amountOfBars * charecters->charByteSize + amountOfBars * 2];
    int size_mainBuf = 0;
    //each line of bars
    
    if(direction == 0){ //bottom
        snprintBarsBottom(mainBuf + size_mainBuf, &size_mainBuf, layers, amountOfBars, bars, charecters, lineDelimiter);
    } else if (direction == 1) { //left
        snprintBarsLeft(mainBuf + size_mainBuf, &size_mainBuf, layers, amountOfBars, bars, charecters, lineDelimiter);
    } else if (direction == 2) { //top
        snprintBarsTop(mainBuf + size_mainBuf, &size_mainBuf, layers, amountOfBars, bars, charecters, lineDelimiter);
    } else {//right
        snprintBarsRight(mainBuf + size_mainBuf, &size_mainBuf, layers, amountOfBars, bars, charecters, lineDelimiter);
    }

    //append end of framw delimiter
    sprintf(mainBuf + size_mainBuf, "%c", frameDelimiter);
    size_mainBuf += 1;
    //writes to rawOutput path
    fwrite(&mainBuf, size_mainBuf, 1, stdout);
}


void proccess(size_t amount_of_bytes, const char *str, const struct barCharecters* charecters,
        int minimumHeight, int addedBarHeight, char inputBarDelimiter, char lineDelimiter, char frameDelimiter, outputDirections direction){
    int biggest = minimumHeight;
    int barSizes[amount_of_bytes/2];
    int barSizesHead = 0;
    int numStorageHead = 0;
    char numStorage[6];
    for(int head = 0; head < amount_of_bytes; head++){
        if(str[head] != inputBarDelimiter){
            numStorage[numStorageHead++] = str[head];
            continue;
        }
        int num = ((int) strtol(numStorage, NULL, 10)) + addedBarHeight;
        barSizes[barSizesHead++] = num;
        if (num > biggest) {
            biggest = num;
        }
        for(int i=0;i<numStorageHead;i++){numStorage[i]=' ';};
        numStorageHead = 0;
    }
    printFinalLine(barSizes, barSizesHead, biggest, charecters, lineDelimiter, frameDelimiter, direction);
}

uint findChar(const char *buffer, const uint bufferSize, const char charecter, int *positionOut){
    for(int i = 0; i < bufferSize; i++){
        if(buffer[i] == charecter) {
            *positionOut = i;
            return 1;
        }
    }
    return 0;
}
const char *getArgString(const int argc, const char **argv, const char *argumentName, const char *defaultValue){
    for(int i = 1; i < argc - 1; i++){
        //only runs if previous argument name was a match
        if(strcmp(argumentName, argv[i]) == 0){
            return argv[i+1];
        }
    }
    return defaultValue;
}

int getArgInt(const int argc, const char **argv,const char *argumentName, int defaultValue){
    size_t foundArgument = 0;
    char *lastChar;
    for(int i = 1; i < argc; i++){
        //only runs if previous argument name was a match
        if(foundArgument) {
            int value = (int) strtol(argv[i], &lastChar, 10);
            return *lastChar == '\0' ? value : defaultValue;
        }
        if(strcmp(argumentName, argv[i]) == 0){
            foundArgument = 1;
        }
    }
    return defaultValue;
}

size_t containsArg(const int argc, const char **argv,const char *argumentName){
    char *lastChar;
    for(int i = 1; i < argc; i++){
        //only runs if previous argument name was a match
        if(strcmp(argumentName, argv[i]) == 0){
            return 1;
        }
    }
    return 0;
}

size_t checkHelp(const int argc, const char **argv){
    for( int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            return 1;
        }
    }
    return 0;
}
void printHelp(){
    printf("\ncavawrapper Help Screen\n\n");
    printf("This program reads the raw output from cava in ascii mode.\nThe cava output will need to be piped into this program.\n\nOptional Arguments:\n");
    printf("\n\t--bar-added-height [num]  -  (default: 0)  -  adds a set amount of height to every bar.\n\t\tUsefull if you want to permenantly see the bars even when theres little to no sound.\n");
    printf("\n\t--minimum-output-height [amount of layers]  -  (default: 10)  -  forces the program to output a minimum amount of layers.\n\t\tThis will not change the bars height, but will make the program draw alot more empty space.\n\t\tThis is usefull if another program struggels with the length of the output changing alot.\n\n");
    printf("\n\t--input-bar-delimiter [ascii code]  -  (default: 59)  -  Changes the expected charecter between inputed bar values.\n");
    printf("\n\t--input-frame-delimiter [ascii code]  -  (default: 10)  - Changes the expected charecter after all bar values are printed.\n");
    printf("\n\t--output-layer-delimiter [ascii code]  -  (default: 59)  - Changes the output charecter that goes in between each layer\n");
    printf("\n\t--output-frame-delimiter [ascii code]  -  (default: 10)  - Changes the output charecter that goes after evey layer has been printed\n");
    printf("\n\t--position-output [position]  -  (default: bottom)  -  Chooses the direction of the output. options: bottom, left, top, right\n");
    printf("\n\n");
}
outputDirections getDirectionFromString(const char *directionString, outputDirections defaultDirection){
    size_t amount = 4;
    outputDirections directions[] = {bottom, left, top, right};
    char *comparisons[] = {"bottom", "left", "top", "right"};
    for(size_t i= 0; i < amount; i++){
        if(strcmp(comparisons[i], directionString) == 0){
            return directions[i];
        }
    }
    return defaultDirection;
}

int main(int argc, const char **argv){
    if(checkHelp(argc, argv)){
        printHelp();
        return 0;
    }

    char inputFrameDelimiter = (char)getArgInt(argc, argv, "--input-frame-delimiter", 10);
    char inputBarDelimiter = (char)getArgInt(argc, argv, "--input-bar-delimiter", 59);
    char outputFrameDelimiter = (char)getArgInt(argc, argv, "--output-frame-delimiter", 10);
    char outputLayerDelimiter = (char)getArgInt(argc, argv, "--output-layer-delimiter", 59);
    int minimumDrawnHeight = getArgInt(argc, argv, "--minimum-output-height", 10);
    int barAddedHeight = getArgInt(argc, argv, "--bar-added-height", 0);

    const char* directionString = getArgString(argc, argv, "--position-output", "bottom");
    outputDirections defaultDirection = bottom;
    outputDirections direction = getDirectionFromString(directionString, defaultDirection);



    const int BUFFER_SIZE = 8192;
    int head = 0;
    int bytesRead = 0;
    int newLinePosition = 0;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(char) * BUFFER_SIZE);
    char tempBuffer[BUFFER_SIZE];

    struct barCharecters chars;
    chars.charByteSize = 4;
    chars.charByteOffset = 3;
    chars.amountOfChars = 9;


    size_t charCount = 9;

    char *charMap[] = {" ", "▁", "▂", "▃", "▄", "▅", "▆", "▇", "█",
                       " ", "▏", "▎", "▍", "▌", "▋", "▊", "▉", "█",
                       " ", " ", "▔", "▔", "▔", "▀", "▀", "▀", "█",
                       " ", " ", "▕", "▕", "▕", "▐", "▐", "▐", "█",
    };

    char *tempCharecters[9]; 
    //copies memory from the chunk depending to the direction value.
    memcpy(tempCharecters, &charMap[ direction * 9 ], sizeof(char*) * 9);

    for(int i = 0; i < charCount; i++){
        chars.charecters[i] = tempCharecters[i];
    }

    minimumDrawnHeight *= chars.amountOfChars - 1;


    freopen(NULL, "rs", stdin);
    while (1) {
        bytesRead = read(0, &buffer[head], BUFFER_SIZE - head);

        if(findChar(&buffer[head], bytesRead, inputFrameDelimiter, &newLinePosition)){
            int endOfLineBufferSize = head + newLinePosition + 1;

            proccess(endOfLineBufferSize, buffer, &chars, minimumDrawnHeight, barAddedHeight, inputBarDelimiter, outputLayerDelimiter, outputFrameDelimiter, direction);

            memcpy(tempBuffer, buffer, sizeof(char) * BUFFER_SIZE);
            memset(buffer, 0, sizeof(char) * BUFFER_SIZE);

            head = bytesRead - (newLinePosition + 1);
            memcpy(buffer, &tempBuffer[endOfLineBufferSize], head);
            continue;
        }
        head += bytesRead;
    }
    return 0;
}

