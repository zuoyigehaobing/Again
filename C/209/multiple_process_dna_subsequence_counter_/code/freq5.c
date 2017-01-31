#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h>
#include<getopt.h>
#include<dirent.h>
#include<sys/time.h>

// ========================== Initialization of array


// This function is used to calculate a to the power of p
int power(int a, int p){
    if(a == 0){
        return 0;
    }
    int result = 1;
    while(p > 0){
        result = result * a;
        p--;
    }
    return result;
}


// Given a letters like "AAAAA", array_hash will hash this letter to index 0, and return that index
int array_hash(char * letters){
    int result = 0;
    if(strlen(letters) != 5){
        printf("strlen falt at function array_hash\n");
        return -1;
    }
    else{
        int size = strlen(letters);
        for(int i=0;i<size;i++){
            int p = size-i-1;
            if((char)letters[i] == 'A' || (char)letters[i] == 'a'){
                result += 0 * power(4,p);
            }

            else if((char)letters[i] == 'C' || (char)letters[i] == 'c'){
                result += 1* power(4,p);
            }

            else if((char)letters[i] == 'G' || (char)letters[i] == 'g'){
                result += 2 *power(4,p);
            }

            else if((char)letters[i] == 'T' || (char)letters[i] == 't'){
                result += 3 *power(4,p);
            }

            // not a valid char
            else{
                return -1;
            }

        }
    }
    return result;
}


// initialize the result array
int * num_arr(){
    int * result = (int*)calloc(1024,sizeof(int));
    for(int i=0;i<1024;i++){
        result[i] = 0;
    }
    return result;
}


// initialize an array of array (containing 1024 permutation of letters [AAAAA AAAAT AAAAC ....])
char ** string_arr(){
    char **result = (char**) calloc(1024,sizeof(char*));
    int p = 0;
    int remain;
    for(int i =0;i <1024;i++){
        result[i] = (char*)calloc(6,sizeof(char));
        p = 4;
        char item[6];
        item[0] = '\0';
        item[5] = '\0';
        int curr = i;
        while(p>= 0){
            remain = curr % 4;
            curr = curr /4;
            if(strlen(item) > 5){
                printf("strlen error at function string_at\n");
                return NULL;
            }
            else if(remain == 0){
                item[p] = 'a';
            }
            else if(remain == 1){
                item[p] = 'c';
            }
            else if(remain == 2){
                item[p] = 'g';
            }
            else if(remain == 3){
                item[p] = 't';
            }
            p --;
        }
        strncpy(result[i],item,5);
    }

    return result;
}


// free the str_list
void free_str(char ** str_arr){
    for(int i=0;i<1024;i++){
        free(str_arr[i]);
    }
    free(str_arr);
}

// ====================== Reading part =====================


// read a line and count each 5-length substring
void readline(char * line,int * count){
    int length = strlen(line);
    char five[6];
    for(int i=0;i<length-4;i++){
        strncpy(five,&line[i],5);
        five[5] = '\0';
        int index = array_hash(five);
        if(index >= 0 && index <1024){
            count[index] += 1;
        }
    }
}


// read a file and count each 5-length substring
void readfile(char * filename,int *count){
    char line[256];
	FILE* file = fopen(filename,"r");
	while( fgets(line,256,file) != NULL){
		line[strcspn(line,"\r\n")] = '\0';
		readline(line,count);
	}
	fclose(file);
}


// count the number of files in the directory
int count_files(char * dirname){
    DIR *d = NULL;
    struct dirent *dir;
    d = opendir(dirname);
    int counter = 0;

    // count the number of files
    if(d){
        while((dir = readdir(d)) != NULL){
            //printf("%s\n",dir->d_name);
            if((strcmp(dir->d_name,".") != 0) && (strcmp(dir->d_name,"..") != 0)){
                counter ++;
            }
        }
    }
    closedir(d);
    return counter;
}

// return a collection of files in the directory
char ** collection(char *dirname){
    DIR *d = NULL;
    struct dirent *dir;
    d = opendir(dirname);

    int counter = count_files(dirname);

    // collect files
    char ** result = (char**)calloc(counter,sizeof(char*));
    d = opendir(dirname);
    int index = 0;
    if(d){
        while((dir = readdir(d)) != NULL){
            //printf("%s\n",dir->d_name);
            if((strcmp(dir->d_name,".") != 0) && (strcmp(dir->d_name,"..") != 0)){
                char path[256];
                path[0] = '\0';
                strncpy(path,dirname,strlen(dirname));
                path[strlen(dirname)] = '\0';
                strncat(path,"/",1);
                strncat(path,dir->d_name,strlen(dir->d_name));
                path[strlen(path)] = '\0';
                result[index] = (char*)calloc(strlen(path)+1,sizeof(char));
                strncpy(result[index],path,strlen(path));
                index ++;
            }
        }
    }
    closedir(d);
    return result;
}


// free the collection list
void free_collection(char ** files, int size){
    for(int i=0;i<size;i++){
        free(files[i]);
    }
    free(files);
}


// child will perform this tast : reading some files and writing to pipe
void child_tast(char ** files,int filesize,int processes,int index, int*result, int *fd){
    // index start from zero
    int seg = (filesize/processes);
    int start = (int) seg * index;
    int end = (int) seg * (index+1);
    if (end >= filesize){
        end = filesize;
    }
    if(index == processes - 1){
        end = filesize;
    }
    for(int i=0;i < end-start;i++){
        readfile(files[start+i],result);
    }

    // close for reading
    if(close(fd[0]) == -1){
        perror("close");
    }

    // write to pipe
    for(int i=0;i<1024;i++){
        if(write(fd[1],&result[i],sizeof(int))==-1){
            perror("write userid to pipe");
      }
    }

    if(close(fd[1]) == -1){
        perror("close");
    }


}




// ==================================== MAIN ================================
int main(int argc,char ** argv){
    // ========== time counting start here
    struct timeval starttime, endtime;
	double timediff;

	if((gettimeofday(&starttime,NULL)) == -1){
		perror("gettimeofday");
		exit(1);
	}

    // ==========  Initialization
    char ** str_arr = string_arr();
    int * result = num_arr();



    // ============ code related to getopt
	int ch;
    int num_process = 1;
	FILE *outfp = NULL;
    char dirname[256], *outfile = NULL;
    char* current_dir = ".";
    char * slash = "/";
    strncpy(dirname,current_dir,2);

    while((ch = getopt(argc,argv,"d:n:o:")) != -1){
        int dir_len = strlen(optarg);
		switch(ch){
			case 'd':
                strncat(dirname,slash,1);
				strncat(dirname,optarg,dir_len);
                dirname[2+dir_len] = '\0';
                printf("dir: %s\n",dirname);
				break;
			case 'n':
				num_process = (int)strtol(optarg,NULL,10);
                printf("num_process : %d\n",num_process );
				break;
            case 'o':
                outfile = optarg;
                outfp = fopen(outfile,"w+");
                printf("output : %s\n", outfile );
                break;
			default:
				fprintf(stderr,"Usage: test -d <directory name> -n <num_process> -o <outputfile>\n");
                exit(1);
		}
	}

    // ============ test 2 starts here
    char ** collect_files = collection(dirname);
    int num_files = count_files(dirname);


    // ============ test 3 ends here
    // multiple processes
    if(num_process > 1 && num_process<= num_files){
        int n = 1;
        // initialize pipes
        int fd[num_process][2];
        for(int i = 0;i< num_process+1;i++){
            if(pipe(fd[i]) == -1){
                perror("pipe");
            }
        }

        for(int i=0;i<num_process;i++){
            n = fork();
            if(n == 0){
                    child_tast(collect_files,num_files,num_process,i,result,fd[i]);
                    exit(0);
                    break;
            }

            if(n != 0){
                    // do something for parent here
                    if(i == num_process-1){

                        // then you should collect from pipes here
                        int i = 0;
                        while(i<1024){
                            int num_kid = num_process;
                            for(int k = 0;k<num_kid;k++){
                                int know;
                                int success = read(fd[k][0],&know,sizeof(int));
                                if(success == -1 || success != sizeof(int)){
                                    perror("collect");
                                    exit(1);
                                }
                                result[i] += know;
                            }
                            i++;
                        }
                    }
            }
            if(n < 0) {
                    perror("fork");
                    exit(1);
            }

        }

    }

    // single process
    else{
        if(num_process > num_files){
            printf("process more than number of files\n");
            exit(1);
        }
        for(int i =0;i<num_files;i++){
            readfile(collect_files[i],result);
        }

    }


    if(outfp == NULL){
        for(int i=0;i<1024;i++){
            printf("%s,%d\n",str_arr[i],result[i]);
        }
    }
    else{
        for(int i=0;i<1024;i++){
            fprintf(outfp,"%s,%d\n",str_arr[i],result[i]);
        }
        fclose(outfp);
    }
    free(result);
    free_str(str_arr);
    free_collection(collect_files,num_files);

    // ================ time counting ends here
    if((gettimeofday(&endtime,NULL)) == -1){
		perror("gettimeofday");
		exit(1);
	}

	timediff = (endtime.tv_sec - starttime.tv_sec) + (endtime.tv_usec - starttime.tv_usec) / 1000000.0;
	fprintf(stderr,"\nTIME USED : %.4f\n",timediff);

    return 0;
}
