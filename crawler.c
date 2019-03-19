#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define URL_LENGTH 300

//create a directory
void makeDir(char* dir){
	char str[]="mkdir ";
	strcat(str,dir);
	if( system(str) ==-1){
		fprintf(stderr,"--|unable to create directory %s|--",dir);
	}
	else{
		fprintf(stdout,"created directory \"%s\" successfully\n",dir);
	}
}

//validate directory
void testDir(char* dir){
	struct stat file_info;
	//check for valid path
	if( stat(dir,&file_info)==-1 ){
		fprintf(stderr,"--| directory not found,attempting to create it |--\n");
		makeDir(dir); //try to make the directory
		stat(dir,&file_info); //check again for the directory created above
	}
	//check if path is a directory
	if( !S_ISDIR( file_info.st_mode ) ){
		fprintf(stderr,"--| given path is not a directory |--\n");
		exit(-1);
	}
	//check for read/write permissions on directory
	if( file_info.st_mode & S_IWUSR != S_IWUSR){
		fprintf(stderr,"--| access denied in \"%s\" |--\n",dir);
		exit(-1);
	}
	fprintf(stdout,"--| directory validated |--\n");
}

void testUrl(char* url){
	char command[URL_LENGTH]="wget --spider ";
	strcat(command,url);
	if( system(command) ){
		fprintf(stderr,"--| url \"%s\" not reachable |--\n",url);
		exit(-1);
	}
	else{
		fprintf(stdout,"--| URL validated successfully |--\n");
	}
}

void getWebPage(char* url){
	testUrl(url); //validate URL
	char url_buffer[URL_LENGTH+300]="wget -O temp.txt ";
	strcat(url_buffer,url);
	system(url_buffer);
}

//copy temp.txt from here to directory defined by usr
void copyTemp(char* dir){

}

int main(int argc,char* argv[]){
	if( argc<3 ){
		fprintf(stderr,"--| not enough arguments supplied |--\n");
		exit(-1);
	}
	testDir( argv[1] );
	getWebPage( argv[2] );
	
	/*
	char urls[URL_LENGTH]={0};
	FILE *f1=fopen("links.txt","r");
	while(fgets(urls,200,f1) != NULL){
		
		printf("%s",urls);
	}
	*/
	return 0;
}

