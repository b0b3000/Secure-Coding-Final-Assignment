#define _XOPEN_SOURCE 600

#include "p_and_p.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <pwd.h>
#include <sys/stat.h>

/** Validates and serialises a given array of ItemDetails structs to a file specified by the caller. 
* The written file will contain a header, with the number of ItemDetails struct serialised,
* followed by one or more blocks, containing information about each struct.
* The function will return 0 upon success, and 1 otherwise.
 * \param arr Array of ItemDetails structs
 * \param nmemb Number of member ItemDetails structs in array 'arr'
 * \param fd File Descriptor of file to be serialise to
 * \return 0 if succesfully serialised, 1 if not succesfully serialised
 */
int saveItemDetails(const struct ItemDetails* arr, size_t nmemb, int fd) {

  if(fd<=2){
    return 1;
  }

  int fdDup = dup(fd);

  if(fdDup<=2){
    return 1;
  }

  FILE* fp = fdopen(fdDup, "w");
  if (!fp){
    fflush(fp);
    return 1;
  }
  fflush(fp);

  uint64_t numItems = nmemb;
  if(fwrite(&numItems, sizeof(uint64_t), 1, fp) != 1){
    fflush(fp);
    fclose(fp);
    return 1;
  }

  for (size_t index = 0; index < nmemb; index++){
    if (!isValidItemDetails(&arr[index])){
      fflush(fp);
      fclose(fp);
      return 1;
    }
    if(fwrite(&arr[index], sizeof(struct ItemDetails), 1, fp) != 1){
      fflush(fp);
      fclose(fp);
      return 1;
    }
  }

  if (fclose(fp) != 0){
    return 1;
  }
  return 0;
}

/** If succesful, this function will deserialise details of items from a file into a pointer 
* array of ItemDetails structs. The header of the file contains the number of itemDetails 
* elements to be deserialised, and is written into 'nmemb'
* This number of ItemDetails elements is deserialised from the file specified by the inputted 
* file descriptor, each validated and written into 'ptr'.
* The memory of ptr is dynamically allocated, and should be freed by the caller
* The function returns 0 on succesful deserialisation, and 1 otherwise
 *
 * \param ptr Pointer to ItemDetails struct pointer array to be deserialised into
 * \param numItems Pointer to variable to have the number of ItemDetails elements written into
 * \param fd File Descriptor to deserialise from
 * \return 0 if successfully deserialised, 1 otherwise
 */
int loadItemDetails(struct ItemDetails** ptr, size_t* nmemb, int fd) {

  if(fd<=2){

    return 1;
  }

  int fdDup = dup(fd);

  if(fdDup<=2){

    return 1;
  }

  FILE* fp = fdopen(fdDup, "r");
  if (!fp){
    return 1;
  }
  fflush(fp);

  uint64_t headerBuf;
  
  if(fread(&headerBuf, sizeof(uint64_t), 1, fp) <= 0){
    fflush(fp);
    fclose(fp);
    return 1;
  }

  *nmemb = headerBuf;

  struct ItemDetails *structArrayBuffer = calloc(*nmemb, sizeof(struct ItemDetails));

  if (!structArrayBuffer){
    return 1;
  }
  
  for (size_t i = 0; i < *nmemb; i++){

    struct ItemDetails structBuffer;

    if(fread(&structBuffer, sizeof(struct ItemDetails), 1, fp) <= 0){
      free(structArrayBuffer);
      fflush(fp);
      fclose(fp);
      return 1;
    }

    if(!isValidItemDetails(&structBuffer)){
      free(structArrayBuffer);
      fflush(fp);
      fclose(fp);
      return 1;
    }

    structArrayBuffer[i] = structBuffer;

  }

  *ptr = structArrayBuffer;

  if (fclose(fp) != 0){
    return 1;
  }

  return 0;

}

/** Ensures a character array conforms to the 'name' format
 * All characters in a valid 'name' field should have a graphical representation 
 * i.e. no control characters, whitespace, etc.
 * A name field must also not be empty/NULL, and be null terminated, 
 * and not eclipse DEFAULT_BUFFER_SIZE bytes.
 *
 * \param str 'name' string to be validated
 * \return 1 if valid, 0 otherwise
 */
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


/** Ensures a given character array is a valid 'Multiword'. 
 * A valid multiword should only contain characters with graphical
 * representations, and space characters.
 * However, the first and last characters should not be space characters
 * A multiword field must also not be empty/NULL, and be null terminated, 
 * and not eclipse DEFAULT_BUFFER_SIZE bytes.
 *
 * \param str 'multiword' string to be validated.
 * \return 1 if validated, 0 if invalid.
 */
int isValidMultiword(const char *str) {

  if(str == NULL){
    return 0;
  }

  if (!isgraph(str[0])){
    return 0;
  } 

  int validMulti = 0;

  for (int index = 1; index < DEFAULT_BUFFER_SIZE; index++){

    if (str[index] == '\0'){
      if (isspace(str[index - 1])){
        return 0;
      } else {
        return validMulti;
      }
    }
    
    if(isgraph(str[index]) || (isspace(str[index]))){
      validMulti = 1;
    } else {
      return 0;
    }
  }

  return 0;

}


/** Ensures a given itemDetails struct is of the correct format
 *  To be in the correct format, the 'name' field should be 
 * a valid Name, and the 'desc' field should be a valid
 * Multiword
 *
 * \param id ItemDetails struct to be validated
 * \return 1 if validated, 0 if invalid.
 */
int isValidItemDetails(const struct ItemDetails *id) {

  if (id == NULL){
    return 0;
  }

  if (!isValidName(id->name)){
    return 0;
  } 

  if (!isValidMultiword(id->desc)){
    return 0;
  } 
  
  return 1; 

}

/** Ensures a given Character struct is of the correct format
 * To be in the correct format, the 'name' field should be 
 * a valid Multiword, the 'profession' field should be a valid
 * name, the value of 'inventorySize' should not eclipse MAX_ITEMS,
 * and the total items carried by the character should not eclipse 
 * MAX_ITEMS.
 *
 * \param c Character struct to be validated
 * \return 1 if validated, 0 if invalid.
 */

int isValidCharacter(const struct Character * c) {
  if (c == NULL){
    return 0;
  }
  if (!isValidMultiword(c->name)){
    return 0;
  } 

  if (!isValidName(c->profession)){
    return 0;
  } 

  if(c->inventorySize > MAX_ITEMS){
    return 0;
  }
  
  long unsigned int itemsCarried = 0;
  for (long unsigned int i = 0; i < c->inventorySize; i++){
    itemsCarried = itemsCarried + c->inventory[i].quantity;
  }
  if(itemsCarried > MAX_ITEMS){
    return 0;
  }

  return 1;
}

/** Validates and serialises a given array of Character structs to a file specified by the caller. 
* The written file will contain a header, with the number of Character struct serialised,
* followed by one or more blocks, containing information about each struct.
* The function will return 0 upon success, and 1 otherwise.
 * \param arr Array of Character structs
 * \param nmemb Number of member Character structs in array 'arr'
 * \param fd File Descriptor of file to be serialised to
 * \return 0 if succesfully serialised, 1 if not succesfully serialised
 */
int saveCharacters(struct Character *arr, size_t nmemb, int fd) {

  if(fd<=2){
    return 1;
  }

  int fdDup = dup(fd);

  if(fdDup<=2){
    return 1;
  }

  FILE* fp = fdopen(fdDup, "w");
  if(!fp){
    return 1;
  }
  fflush(fp);

  uint64_t numItems = nmemb;

  if(fwrite(&numItems, sizeof(uint64_t), 1, fp) <= 0){
    fflush(fp);
    fclose(fp);
    return 1;
  }

  for (size_t index = 0; index < nmemb; index++){
    
    if (!isValidCharacter(&arr[index])){
      fflush(fp);
      fclose(fp);
      return 1;
    }

    if(fwrite(&arr[index], sizeof(struct Character), 1, fp) <=0){
      fflush(fp);
      fclose(fp);
      return 1;
    }

  }

  if (fclose(fp) != 0){
    return 1;
  }
  return 0;
}

/** If succesful, this function will deserialise details of items from a file into a pointer 
* array of Character structs. The header of the file contains the number of Character 
* elements to be deserialised, and is written into 'nmemb'
* This number of Character elements is deserialised from the file specified by the inputted 
* file descriptor, each validated and written into 'ptr'.
* The memory of ptr is dynamically allocated, and should be freed by the caller
* The function returns 0 on succesful deserialisation, and 1 otherwise
 *
 * \param ptr Pointer to Character struct pointer array to be deserialised into
 * \param numItems Pointer to variable to have the number of Character elements written into
 * \param fd File Descriptor to deserialised from
 * \return 0 if successfully deserialised, 1 otherwise
 */

int loadCharacters(struct Character** ptr, size_t* nmemb, int fd) {

  if(fd<=2){
    return 1;
  }

  int fdDup = dup(fd);

  if(fdDup<=2){
    return 1;
  }

  FILE* fp = fdopen(fdDup, "r");
  if(!fp){
    return 1;
  }
  fflush(fp);

  uint64_t headerBuf;

  if(fread(&headerBuf, sizeof(uint64_t), 1, fp) <= 0){
    fflush(fp);
    fclose(fp);
    return 1;
  }

  *nmemb = headerBuf;

  struct Character *structArrayBuffer = calloc(*nmemb, sizeof(struct Character));
  
  for (size_t i = 0; i < *nmemb; i++){

    struct Character structBuffer;

    if(fread(&structBuffer, sizeof(struct Character), 1, fp) <= 0){
      free(structArrayBuffer);
      fflush(fp);
      fclose(fp);
      return 1;
    }

    if(!isValidCharacter(&structBuffer)){
      free(structArrayBuffer);
      fflush(fp);
      fclose(fp);
      return 1;
    }

    structArrayBuffer[i] = structBuffer;
 
  }

  *ptr = structArrayBuffer;
  
  if (fclose(fp) != 0){
    return 1;
  }
  return 0;
}

/** 
 * This function checks that the executable is run by 'pitchpoladmin'
 * After validating the user, the eUID is set to the UID
 * of pitchpoltadmin), an ItemDetails pointer array, and the number of elements 
 * is deserialised from a file at the specified path. Following this, 
 * permissions are dropped and the playGame function is called with the 
 * deserialised data as parameters
 * 
 * \param filepath Path to file containing ItemDetails data
 * \return 0 if all operations succesful, 1 if an error occurs with serialisation, 
 * 2 if an error occurs with permissions.
 */

int secureLoad(const char *filepath) {
  
  struct passwd ppAdmin;
  struct passwd *result;

  long int bufferSize = sysconf(_SC_GETPW_R_SIZE_MAX);
  if (bufferSize < 0){        
    bufferSize = 16000;  
  }

  char* buf = malloc((size_t) bufferSize);
  if (buf == NULL) {
    return 1;
  }

  if(getpwnam_r("pitchpoladmin", &ppAdmin, buf, (size_t) bufferSize, &result) != 0){
    return 2;
  }

  if (result == NULL){
    return 2;
  }

  uid_t callerUid = getuid();

  if (callerUid != ppAdmin.pw_uid){
    return 2;
  }

  if(seteuid(ppAdmin.pw_uid) == -1){
    return 2;
  }

  int fd = open(filepath, O_RDONLY);

  if (fd <= 2){
    return 1;
  }

  struct stat fileStats;

  if(fstat(fd, &fileStats) == -1){
    return 1;
  }

  if(!((fileStats.st_uid == ppAdmin.pw_uid) && (fileStats.st_gid == ppAdmin.pw_gid))){
    return 1;
  }

  struct ItemDetails *ptr;
  size_t nmemb;

  if(loadItemDetails(&ptr, &nmemb, fd) == 1){
    return 1;
  }

  if(close(fd) != 0){
    return 1;
  }

  if(seteuid(callerUid) == -1){
    return 2;
  }

  playGame(ptr, nmemb);

  free(buf);
  free(ptr);
  return 0;
}
