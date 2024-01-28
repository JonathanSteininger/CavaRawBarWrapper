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

void printFinalLine(int *bars, size_t amountOfBars, size_t maxHeight, const struct barCharecters* charecters, char lineDelimiter, char frameDelimiter){
    //byte data to draw no hidden charecters
    //amount of visible bars
    int barLayers = charecters->amountOfChars - 1;

    //buffer to write frame into
    int layers = maxHeight / 8 + (maxHeight % 8 == 0 ? 0 : 1);
    //the 2 is space for the delimiter
    char mainBuf[maxHeight * amountOfBars * charecters->charByteSize + amountOfBars * 2];
    int size_mainBuf = 0;
    //each line of bars
    for(int level = layers - 1; level >= 0; level--) {
        for (int i = 0; i < amountOfBars; i++) {
            int value = bars[i];

            //gets the index for what bar to draw
            int index = value % barLayers;
            if ((level + 1) * barLayers <= value){
                index = barLayers;
            }else if (level * barLayers >= value){
                index = 0;
            }
            //appends char to buffer
            snprintf(mainBuf + size_mainBuf, charecters->charByteSize, "%s", charecters->charecters[index]);
            size_mainBuf += charecters->charByteOffset;
        }
        //appends bar line delimiter
        if (level != 0){
            sprintf(mainBuf + size_mainBuf, "%c", lineDelimiter);
            size_mainBuf += 1;
        }
    }
    //append end of framw delimiter
    sprintf(mainBuf + size_mainBuf, "%c", frameDelimiter);
    size_mainBuf += 1;
    //writes to rawOutput path
    fwrite(&mainBuf, size_mainBuf, 1, stdout);
}

void proccess(size_t amount_of_bytes, const char *str, const struct barCharecters* charecters,
        int minimumHeight, int addedBarHeight, char inputBarDelimiter, char lineDelimiter, char frameDelimiter){
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
    printFinalLine(barSizes, barSizesHead, biggest, charecters, lineDelimiter, frameDelimiter);
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
int getArgInt(const int argc, const char **argv,const char *argumentName, int defaultValue){
    size_t foundArgument = 0;
    char *lastChar;
    for(int i = 1; i < argc; i++){
        //only runs if previous argument name was a match
        if(foundArgument) {
            int value = (int) strtol(argv[i], &lastChar, 10);
            return *lastChar == '\0' ? value : defaultValue;
        }
        if(strcmp(argumentName, argv[i])){
            foundArgument = 1;
        }
    }
    return defaultValue;
}
size_t checkHelp(const int argc, const char **argv){
    for( int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-h") || strcmp(argv[i], "--help")) {
            return 1;
        }
    }
    return 0;
}
void printHelp(){
    printf("This program reads the raw output from cava in ascii mode.\nThe cava output will need to be piped into this program.\n\nOptional Arguments:\n");
}

int main(int argc, const char **argv){
    if(checkHelp(argc, argv)){
        printHelp();
        return 0;
    }
    freopen(NULL, "rs", stdin);
    const int BUFFER_SIZE = 1024;
    int head = 0;
    int bytesRead = 0;
    int newLinePosition = 0;
    char buffer[1024] = {0};
    char tempBuffer[1024] = {0};
    int minimumDrawnHeight = getArgInt(argc, argv, "--minimum-output-height", 80);
    int barAddedHeight = getArgInt(argc, argv, "--bar-added-height", 0);
    struct barCharecters chars;
    chars.charByteSize = 4;
    chars.charByteOffset = 3;
    chars.amountOfChars = 9;
    chars.charecters[0] = " ";
    chars.charecters[1] = "▁";
    chars.charecters[2] = "▂";
    chars.charecters[3] = "▃";
    chars.charecters[4] = "▄";
    chars.charecters[5] = "▅";
    chars.charecters[6] = "▆";
    chars.charecters[7] = "▇";
    chars.charecters[8] = "█";
    while (1) {
        bytesRead = read(0, &buffer[head], BUFFER_SIZE - head);

        if(findChar(&buffer[head], bytesRead, '\n', &newLinePosition)){
            int endOfLineBufferSize = head + newLinePosition + 1;

            proccess(endOfLineBufferSize, buffer, &chars, minimumDrawnHeight, barAddedHeight, ';', ';', '\n');

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

