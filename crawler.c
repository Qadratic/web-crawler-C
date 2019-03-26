#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define URL_LENGTH 300

struct node{
	char* url;
	struct node* next_node;
};

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

//create entry point node
struct node* initiate( char* url ){
	struct node* head=(struct node*)malloc( sizeof(struct node) );
	head->url = (char*)malloc(sizeof(char)*URL_LENGTH);
	strcpy( head->url,url );
	
	return head;
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

int testUrl(char* url){
	char command[URL_LENGTH]="wget --spider ";
	strcat(command,url);
	if( system(command) ){
		fprintf(stderr,"--| url \"%s\" not reachable |--\n",url);
		return 0;
	}
	else{
		fprintf(stdout,"--| URL validated successfully |--\n");
		return 1;
	}
}

void getWebPage(char* url){
	char url_buffer[URL_LENGTH+300]="wget -O temp.txt ";
	strcat(url_buffer,url);
	system(url_buffer);
}

//copy temp.txt from here to directory defined by usr
void copyTemp(char** dir,int file_no){
	char target[10];
	sprintf( target,"%d",file_no ); //convert file_no to string
	char dir_main[50];
	strcpy(dir_main,*dir);
	
	strcat(dir_main,"/");
	strcat( dir_main,target );
	strcat(dir_main,".html"); //creating directory to save file
	printf("%s",dir_main);
	//return;
	
	//copying temp.txt to destination
	FILE *src,*dest;
	src=fopen("temp.txt","r");
	dest=fopen(dir_main,"w");
	char *str=(char*)malloc(sizeof(char));
	char c;
	while(c=fgetc(src)){
		if(c==EOF){break;}
		str[0]=c;
		fprintf(dest,str);
		//printf(str);
	}
	free(str);
	fclose(src);
	fclose(dest);
}

int main(int argc,char* argv[]){
	if( argc<3 ){
		fprintf(stderr,"--| not enough arguments supplied |--\n");
		exit(-1);
	}
	testDir( argv[1] );
	
	//validate URL
	/*if(!testUrl( argv[2] )){
		fprintf(stderr,"--| seed url not reachable |--\n");
		exit(-1);
	}*/
	
	int depth=1,s_no=0;
	struct node *head=initiate( argv[2] ),*temp;
	temp=head;
	
	while(depth--){
		if(/*testUrl( temp->url )*/ 1){
			//getWebPage( temp->url );
			system("bash grephtml.sh");
			char urls[URL_LENGTH]={0};
			FILE *f1=fopen("links.txt","r");
			while(fgets(urls,200,f1) != NULL){
				printf("%s",urls);
			}
			//fclose(f1);
			
			//printf("%s",argv[1]);
			copyTemp( &argv[1],s_no );
			
			
			s_no++;
		}
	}
	
	return 0;
}

