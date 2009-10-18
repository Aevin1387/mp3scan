#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
//#include "/usr/local/include/taglib/taglib.h"
//#include "/usr/local/include/taglib/fileref.h"
#include <id3/misc_support.h>
#include <id3/tag.h>


typedef struct MP3NODE{
	char file_name[255];
	struct MP3NODE* next;
	ID3_Tag songTag;
	//TagLib::FileRef songFile;
	char title[511];
	char artist[511];
	char album[511];
} mp3_node;

typedef struct {
	mp3_node* head;
	mp3_node* tail;
	int length;
}mp3_list;

mp3_list* mp3list;

void fillMp3Data(mp3_node* dataNode){
	
}

void get_mp3s(char* directory){
	struct dirent** directory_list;
	int n;
	
	n = scandir(directory, &directory_list, 0, alphasort);
	if(n < 0){
		printf("Error in scandir for directory %s\n", directory);
	} else {
		while(n--){
			struct stat statBuf;
			char* path;
			path = (char*)malloc(sizeof(char)*strlen(directory_list[n]->d_name));
			strncpy(path, directory_list[n]->d_name, strlen(directory_list[n]->d_name));
			chdir(directory);
			if(stat(directory_list[n]->d_name, &statBuf) < 0){
				exit(errno);
			}
			if(S_ISDIR(statBuf.st_mode)){
				int p_length = strlen(directory_list[n]->d_name);
				if(p_length == 1 && directory_list[n]->d_name[0] == '.')
					break;
				if(p_length == 2 && !strcmp(directory_list[n]->d_name, ".."))
					break;
				char full_path[255];
				strncpy(full_path,directory,255);
				
				full_path[strlen(full_path)] = '/';
				strcat(full_path, directory_list[n]->d_name);
				get_mp3s(full_path);
			}else{
				int start_ext = strlen(directory_list[n]->d_name) - 4;
				if(!strcmp(directory_list[n]->d_name + start_ext, ".mp3")){
					mp3_node* mp3node = (mp3_node*)malloc(sizeof(mp3_node));
					strncpy(mp3node->file_name, directory_list[n]->d_name, 255);
					//TagLib::FileRef f(mp3node->file_name);
					mp3node->songTag.Link(mp3node->file_name);
					if(mp3list->head == NULL){
						mp3list->head = mp3node;
						mp3list->tail = mp3node;
						mp3list->length++;
					}else{
						mp3list->tail->next = mp3node;
						mp3list->tail = mp3node;
						mp3list->length++;
					}

				}
				free(directory_list[n]);
			}
		}
	}
}

void init_list(){
	mp3list = (mp3_list*)malloc(sizeof(mp3_list));
	mp3list->head = NULL;
	mp3list->tail = NULL;
	mp3list->length = 0;
}

int main() {
	char current_path[PATH_MAX];
	char answer;
	
	if(!getcwd(current_path, PATH_MAX)){
		printf("ERROR!");
		return 1;
	}
	
	init_list();
	
	printf("Current directory is %s do you want to use this as the starting scan directory (Y/N)? ", current_path);
	scanf("%c", &answer);
	
	if(answer != 'Y' && answer != 'y'){
		printf("Please run this application from the directory you want to use as the starting scan directory.");
		return -1;
	}
	
	get_mp3s(current_path);
	
	if(mp3list->head == NULL){
		printf("No mp3s found!");
	}else{
		char* artist;
		mp3_node* curr = mp3list->head;
		while(curr != NULL){
			printf("%s\n", curr->file_name);
			if(artist = ID3_GetArtist(&(curr->songTag)))
				printf("Artist: %s\n", artist);
			curr = curr->next;
		}
	}
	
	return 0;
}