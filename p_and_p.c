#include "p_and_p.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAXINTDIGITS 21

//CC=gcc CFLAGS="-std=c11 -pedantic -Wall -Wextra -Wshadow -Wconversion -O" make p_and_p.c p_and_p.o
//make p_and_p
// ./p_and_p

int saveItemDetails(const struct ItemDetails* arr, size_t numEls, int fd) {

  if(fd<=2){
    printf("File fd error\n");
    return 1;
  }

  

  //Header
  uint64_t numItems = numEls;
  if(write(fd, &numItems, sizeof(uint64_t)) <= 0){
    printf("ERROR writing header");
    return 1;
  }

  //Block
  for (size_t index = 0; index < numEls; index++){
    if (!isValidItemDetails(&arr[index])){
      printf("INVALID ITEM DETAILS\n\n");
      return 1;
    }
    if(write(fd, &arr[index], sizeof(struct ItemDetails)) <=0){
      printf("ERROR writing body");
    }
  }

  return 0;
}

int saveItemDetailsToPath(const struct ItemDetails* arr, size_t numEls, const char* filename) {
  int fd = open(filename,O_WRONLY);

  if(fd<=2){
        printf("File fd error %i\n", fd);
        return 1;
    }

  saveItemDetails(arr, numEls, fd);

  close(fd);
  return 0;
}

int loadItemDetails(struct ItemDetails** ptr, size_t* numEls, int fd) {

  if(fd<=2){
    printf("File fd error%i\n", fd);
    return 1;
  }

  
  
  //uint64_t* headerBuf = (uint64_t*)malloc(sizeof(uint64_t));
  uint64_t headerBuf;
  
  printf("FD %i\n", fd);
  if(read(fd, &headerBuf, sizeof(uint64_t)) <= 0){
    printf("ERROR reading header");
    //free(headerBuf);
    return 1;
    //FLESH OUT
  }
  printf("HEADER%ld\n", headerBuf);
  fflush(stdout);

  *numEls = headerBuf;

  //size_t numElValue = (size_t) atoi(headerBuf);
  //*numEls = numElValue;

  struct ItemDetails *structArrayBuffer = calloc(*numEls, sizeof(struct ItemDetails));
  
  for (size_t i = 0; i < *numEls; i++){

    struct ItemDetails structBuffer;

    if(read(fd, &structBuffer, sizeof(struct ItemDetails)) <= 0){
      printf("ERROR reading body on iteration %li", i);
      free(numEls);
      free(structArrayBuffer);
      //free(headerBuf);
      //DO you only need to free pointers? what about numElValue and structBuffer?
      return 1;
      //FLESH OUT
    }

    structArrayBuffer[i] = structBuffer;
 
  }

  *ptr = structArrayBuffer;
  return 0;

}

int isValidName(const char *str) {
  
  int validName = 0;

  if(str == NULL){
    return 0;
  }

  for (int index = 0; index < DEFAULT_BUFFER_SIZE; index++){
    

    if (str[index] == '\0'){
      return validName;
    }

    if (isgraph(str[index])){
      validName = 1;
    } else {
      return 0;
    }

  }
  return 0;
}

int isValidMultiword(const char *str) {
  if(str == NULL){
    return 0;
  }
  //Check first letter not space
  if (!isgraph(str[0])){
    return 0;
  } 

  int validMulti = 0;

  for (int index = 1; index < DEFAULT_BUFFER_SIZE; index++){
    
    //if string over, break the loop
    if (str[index] == '\0'){
      //check last letter not blank
      if (isblank(str[index - 1])){
        return 0;
      } else {
        return validMulti;
      }
    }
    
    if(isgraph(str[index]) || (isblank(str[index]))){
      validMulti = 1;
    } else {
      return 0;
    }

  }
  return 0;
}

int isValidItemDetails(const struct ItemDetails *id) {
  if (id == NULL){
    return 0;
  }

  if (!isValidName(id->name)){
    printf("ItemDetails struct has invalid name\n");
    return 0;
  } 

  if (!isValidMultiword(id->desc)){
    printf("ItemDetails struct has invalid desc\n");
    return 0;
  } 
  
  return 1; 

  
}

int isValidCharacter(const struct Character * c) {
  if (c == NULL){
    printf("NULL");
    return 0;
  }
  if (!isValidMultiword(c->name)){
    printf("Character struct has invalid name\n");
    return 0;
  } 

  if (!isValidName(c->profession)){
    printf("Character struct has invalid profession\n");
    return 0;
  } 

  if(c->inventorySize > MAX_ITEMS){
    printf("Character inventory too big\n");
    return 0;
  }
  
  long unsigned int itemsCarried = 0;
  for (long unsigned int i = 0; i < c->inventorySize; i++){
    itemsCarried = itemsCarried + c->inventory[i].quantity;
  }
  printf("%li\n", itemsCarried);
  if(itemsCarried > MAX_ITEMS){
    printf("Character has too many items\n");
    return 0;
  }

  return 1;
}

int saveCharacters(struct Character *arr, size_t numEls, int fd) {

  if(fd<=2){
    printf("File fd error\n");
    return 1;
  }

  //Header
  uint64_t numItems = numEls;
  if(write(fd, &numItems, sizeof(uint64_t)) <= 0){
    printf("ERROR writing header");
    return 1;

  }

  //Block
  for (size_t index = 0; index < numEls; index++){
    if (!isValidCharacter(&arr[index])){
      printf("INVALID CHARACTER\n\n");
      return 1;
    }
    if(write(fd, &arr[index], sizeof(struct Character)) <=0){
      printf("ERROR writing body");
    }
  }

  return 0;
}

int loadCharacters(struct Character** ptr, size_t* numEls, int fd) {
  if(fd<=2){
    printf("File fd error%i\n", fd);
    return 1;
  }

  //uint64_t* headerBuf = (uint64_t*)malloc(sizeof(uint64_t));
  uint64_t headerBuf;
  
  printf("FD %i", fd);
  if(read(fd, &headerBuf, sizeof(uint64_t)) <= 0){
    printf("ERROR reading header");
    //free(headerBuf);
    return 1;
    //FLESH OUT
  }
  printf("%ld", headerBuf);
  fflush(stdout);

  *numEls = headerBuf;

  //size_t numElValue = (size_t) atoi(headerBuf);
  //*numEls = numElValue;

  struct Character *structArrayBuffer = calloc(*numEls, sizeof(struct Character));
  
  for (size_t i = 0; i < *numEls; i++){

    struct Character structBuffer;

    if(read(fd, &structBuffer, sizeof(struct Character)) <= 0){
      printf("ERROR reading body on iteration %li", i);
      free(numEls);
      free(structArrayBuffer);
      //free(headerBuf);
      //DO you only need to free pointers? what about numElValue and structBuffer?
      return 1;
      //FLESH OUT
    }

    structArrayBuffer[i] = structBuffer;
 
  }

  *ptr = structArrayBuffer;
  return 0;
}


int secureLoad(const char *filepath) {
  return 0;
}


void playGame(struct ItemDetails* ptr, size_t numEls);


/*
int main(int argc,char *argv[]){
    
    // use appropriate location if you are using MacOS or Linux

    const struct ItemDetails testItem1 = {
      .itemID = 0,
      .name = "testitem1",
      .desc = "description 1",
    } ;

    const struct ItemDetails testItem2 = {
      .itemID = 1,
      .name = "testitem2",
      .desc = " description of item 2",
    } ;

    const struct ItemDetails testItem3 = {
      .itemID = 133,
      .name = "test  item2",
      .desc = "description of item 2",
    } ;

    

    printf("Valid struct 1: %i\n", isValidItemDetails(&testItem1));
    printf("Valid struct 2: %i\n", isValidItemDetails(&testItem2));
    printf("Invalid struct 1: %i\n", isValidItemDetails(&testItem3));

    struct ItemDetails * items = malloc(sizeof(struct ItemDetails) * 2);
    items[0] = testItem1;
    items[1] = testItem2;

    int fd = open("/home/bob/Desktop/CITS3007/Project3007/test-code/test.txt",O_RDWR);
    if(fd==-1){
        printf("file not found.\n");
        return -1;
    }

    saveItemDetails(items, 2, fd);
    close(fd);

    saveItemDetailsToPath(items, 2, "/home/bob/Desktop/CITS3007/Project3007/test-code/test2.txt");
    
    int fd2 = open("/home/bob/Desktop/CITS3007/Project3007/test-code/test.txt",O_RDONLY);
    if(fd2<= 0){
        printf("Invalid file \n");
        return -1;
    }
    struct ItemDetails *ptr;
    size_t* numEls = malloc(sizeof(size_t));
    if (loadItemDetails(&ptr, numEls, fd2) == 0){
      //Testing functionality of loadItemDetails
      printf("Num els: %ld\n", *numEls);
      printf("DESC 1:  %s\n", ptr[0].desc);
      printf("NAME 1: %s\n", ptr[0].name);
      printf("ITEMID 1: %li\n", ptr[0].itemID);
      printf("DESC 2:  %s\n", ptr[1].desc);
      printf("NAME 2: %s\n", ptr[1].name);
      printf("ITEMID 2: %li\n", ptr[1].itemID);
      close(fd2);

      char* invalidName = "toktok EEE";
      printf("INVALID name: %i\n", isValidName(invalidName));
      char* validName = "toktokEEE";
      printf("VALID name: %i\n", isValidName(validName));
      fflush(stdout);

      char *invalidMulti1 = NULL;
      char *invalidMulti2 = " tr333";
      char *invalidMulti3 = "qqq 3 ";
      char *validMulti1 = "33   4 1";
      char *validMulti2 = "2        wweru    ewfuwfr        uruwfr  1";
      printf("INVALID multi 1: %i\n", isValidMultiword(invalidMulti1));
      printf("INVALID multi 2: %i\n", isValidMultiword(invalidMulti2));
      printf("INVALID multi 3: %i\n", isValidMultiword(invalidMulti3));
      printf("VALID multi 1: %i\n", isValidMultiword(validMulti1));
      printf("VALID multi 2: %i\n", isValidMultiword(validMulti2));


      free(items);
      printf("mickey");
     

    }
    

    
    
    
    
}
*/