#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define URL_LENGTH 300
#define TABLE_SIZE 128

struct node{
	char* url;
	int depth;
	struct node* next_node;
};
struct bucket{
	int key;
	char* value;
	struct bucket* next_bucket;
};

void addPair(struct bucket* table,int key,char* value){
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
struct node* initiate( char* url ,int depth){
	struct node* head=(struct node*)malloc( sizeof(struct node) );
	head->url = (char*)malloc(sizeof(char)*URL_LENGTH);
	strcpy( head->url,url );
	head->depth=depth;
	return head;
}

int count=0;

//read links from links.txt and add them to linked list
void addLinks(struct node* head,struct bucket* table,int depth){
	if(depth<1){return;}
	struct node *temp=head,*new_node;
	while(temp->next_node!=NULL){
		printf("url in LL-%s\n",temp->url);
		temp=temp->next_node;
	}
	FILE* links=fopen("links.txt","r");
	char* str=(char*)malloc(sizeof(char)*URL_LENGTH);
	while(fgets(str,URL_LENGTH,links)){
		int hash=generateHash(str);
		if(getValue(table,hash)!=NULL){
		}else{
			addPair(table,hash,str);
		}
		
		//printf("adding %s\n",str);
		count++;
		*(str+strlen(str)-1)='\0'; //NULL append url string, overwrite new line character
		
		new_node=(struct node*)malloc( sizeof(struct node) );
		new_node->url=(char*)malloc(sizeof(char)*URL_LENGTH);
		strcpy( new_node->url,str );
		new_node->depth=depth;
		temp->next_node=new_node;
		temp=temp->next_node;
	}
}

//attempt to create a directory in case directory not found
int makeDir(char* dir){
	if( mkdir(dir,0777) == -1 ){
		fprintf(stderr,"--| failed to create directory |--\n");
		return -1;
	}else{
		fprintf(stdout,"--| directory created successfully |--\n");
	}
	return 0;
}

//validate directory
int testDir(char* dir){
	struct stat file_info;
	//check for valid path
	if( stat(dir,&file_info)== -1 ){
		fprintf(stderr,"--| directory not found, attempting to create it |--\n");
		//attempt to make the directory
		if( makeDir(dir) == -1){
			return -1;
		}
		//check again for the new directory created above
		stat(dir,&file_info);
	}
	//check if path is a directory
	else if( !S_ISDIR( file_info.st_mode ) ){
		fprintf(stderr,"--| given path is not a directory |--\n");
		return -1;
	}
	//check for read/write permissions on directory
	if( file_info.st_mode & S_IWUSR != S_IWUSR){
		fprintf(stderr,"--| access denied in \"%s\" |--\n",dir);
		return -1;
	}
	fprintf(stdout,"--| directory validated |--\n");
	return 0;
}

int testUrl(char* url){
	char wget_command[URL_LENGTH]="wget --spider ";
	strcat(wget_command,url);
	if( system(wget_command) ){
		fprintf(stderr,"--| url \"%s\" not reachable |--\n",url);
		return -1;
	}
	else{
		fprintf(stdout,"--| URL validated successfully |--\n");
		return 0;
	}
}

//download web page in temp.txt
void getWebPage(char* url){
	//construct system command
	char wget_command[URL_LENGTH+300]="wget -O temp.txt ";
	strcat(wget_command,url);
	system(wget_command);
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
	if( argc<4 ){
		fprintf(stderr,"--| not enough arguments supplied |--\n");
		exit(-1);
	}
	//validate directory
	if( testDir( argv[1] ) == -1 ){
		exit(-1);
	}
	//validate URL
	if( testUrl( argv[2] ) == -1){
		exit(-1);
	}
	
	
	int depth=atoi( argv[3] );
	int s_no=0;
	struct bucket* table=(struct bucket*)malloc( sizeof(struct bucket)*TABLE_SIZE );
	addPair(table,generateHash(argv[2]),argv[2]);
	struct node *head=initiate( argv[2] ,depth),*temp;
	temp=head;
	
	while(temp->depth && temp!=NULL){
			printf("downloading file %d\n",s_no);
			getWebPage( temp->url ); //download web page
			system("bash grephtml.sh"); //strip links
			char urls[URL_LENGTH]={0};
			addLinks(head,table,temp->depth-1); //add links to linked list
			copyTemp( &argv[1],s_no ); //copy temp.txt to destination
			s_no++;
			temp=temp->next_node;
			printf("total links- %d\n",count);
	}
	
	return 0;
}

