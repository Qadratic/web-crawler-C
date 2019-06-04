#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define URL_LENGTH 300
#define TABLE_SIZE 128

struct node{
	char* url;
	struct node* next_node;
};
struct bucket{
	int key;
	char* value;
	struct bucket* next_bucket;
};
void addPair(struct bucket* table,int key,char* value){
	//printf("adding %s to table\n",value);
	int hash=key%TABLE_SIZE;
	struct bucket* temp;
	temp=(table+hash);
	while(temp->next_bucket!=NULL ){
		temp=temp->next_bucket;
	}
	temp->key=key;
	temp->value=(char*)malloc( sizeof(char)*URL_LENGTH );
	strcpy(temp->value,value);
	temp->next_bucket=(struct bucket*)malloc( sizeof(struct bucket) );
}
char* getValue(struct bucket* table,int key){
	int hash=key%TABLE_SIZE;
	struct bucket* temp;
	temp=(table+hash);
	while(temp!=NULL){
		if(temp->key==key){
			return temp->value;
		}
		temp=temp->next_bucket;
	}
	return NULL;
}

int generateHash(char* str){
	int sum=0,length=strlen(str);
	for(int i=0;i<length;i++){
		sum+=(str[i]*(i+1));
	}
	return sum;
}

//create and initiate head node
struct node* initiate( char* url ){
	struct node* head=(struct node*)malloc( sizeof(struct node) );
	head->url = (char*)malloc(sizeof(char)*URL_LENGTH);
	strcpy( head->url,url );
	
	return head;
}

int count=0;
//read links from links.txt and add them to linked list
//TODO: check if link is already visited before adding to LL
void addLinks(struct node* head,struct bucket* table){
	//printf("adding links");
	struct node* temp=head;
	while(temp->next_node!=NULL){ temp=temp->next_node; }
	FILE* links=fopen("links.txt","r");
	char* str=(char*)malloc(sizeof(char)*URL_LENGTH);
	while(fgets(str,URL_LENGTH,links)){
		
		//printf("%s",str);
		int hash=generateHash(str);
		if(getValue(table,hash)!=NULL){
			//printf("found\n");
		}else{
			//printf("not found\n");
			addPair(table,hash,str);
		}
		
		count++;
		*(str+strlen(str)-1)='\0'; //NULL append url string, overwrite new line character
		temp->next_node=(struct node*)malloc( sizeof(struct node) );
		temp=temp->next_node;
		temp->url=(char*)malloc(sizeof(char)*URL_LENGTH);
		strcpy( temp->url,str );
	}
	printf("total links- %d\n",count);
}

//attempt to create a directory in case directory not found
void makeDir(char* dir){
	char str[]="mkdir ";
	strcat(str,dir);
	if( system(str) ==-1){ //TODO: fake test reconsider this
		fprintf(stderr,"--|unable to create directory %s|--",dir);
	}
	else{
		fprintf(stdout,"created directory \"%s\" successfully\n",dir);
	}
}

//validate directory
//TODO: refactor this funct
void testDir(char* dir){
	struct stat file_info;
	//check for valid path
	if( stat(dir,&file_info)==-1 ){
		fprintf(stderr,"--| directory not found,attempting to create it |--\n");
		makeDir(dir); //attempt to make the directory
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

//download web page in temp.txt
void getWebPage(char* url){
	//construct system command
	char url_buffer[URL_LENGTH+300]="wget -O temp.txt ";
	strcat(url_buffer,url);
	system(url_buffer);
}

//copy temp.txt from here to directory defined by usr
void copyTemp(char** dir,int file_no){
	char target[10];
	sprintf( target,"%d",file_no ); //convert file_no to string
	char dir_main[50]; //main destination file path
	strcpy(dir_main,*dir);
	
	strcat(dir_main,"/");
	strcat( dir_main,target );
	strcat(dir_main,".html"); //construct directory to save file
	
	//copying temp.txt to destination file character by character
	FILE *src,*dest;
	src=fopen("temp.txt","r");
	dest=fopen(dir_main,"w");
	char *str=(char*)malloc(sizeof(char)); //create string of length=1, str[1] not working
	char c;
	while(c=fgetc(src)){
		if(c==EOF){break;}
		str[0]=c;
		fprintf(dest,"%s",str);
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
	//validate directory
	//testDir( argv[1] );
	
	//validate URL
	if(!testUrl( argv[2] )){
		fprintf(stderr,"--| seed url not reachable |--\n");
		exit(-1);
	}
	
	
	int depth=1,s_no=0;
	struct bucket* table=(struct bucket*)malloc( sizeof(struct bucket)*TABLE_SIZE );
	addPair(table,generateHash(argv[2]),argv[2]);
	struct node *head=initiate( argv[2] ),*temp;
	temp=head;
	
	while(1){
			printf("downloading file %d\n",s_no);
			getWebPage( temp->url ); //download web page
			system("bash grephtml.sh"); //strip links
			char urls[URL_LENGTH]={0};
			addLinks(head,table); //add links to linked list
			printf("done adding urls\n");
			//exit(0);
			copyTemp( &argv[1],s_no ); //copy temp.txt to destination
			s_no++;
			temp=temp->next_node;
	}
	
	return 0;
}

