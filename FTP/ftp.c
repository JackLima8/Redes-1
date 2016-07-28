#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lib_ftp.h"

clrscr();
int main(int argc, char *argv[]){
   
    
   
   if (!strcmp(argv[1],"-l")){
	   printf("####### CLIENTE #########\n");
	   a_local();
  }
  else if (!strcmp(argv[1],"-r")){
       printf("####### SERVIDOR #########\n");
       a_remoto();
 
  }
  else {
	  printf("falta parametro!!!\n"); return 0;
  }	  
 // manda_fonte();





}
