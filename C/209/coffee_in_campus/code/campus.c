
#include <stdio.h>

int campus(double latitude,double longitude){
	int i = 0;
	i = (43.65 <= latitude);
	i = i && (latitude <= 43.67);
	i = i && (-79.41 <= longitude);
	i = i && (longitude <= -79.38);
	return i;
} 


int main(int argc, const char * argv[]) {
    // insert code here...
    double latitude;
    double longitude;
    char info[100];


    while(fscanf(stdin,"%lf,%lf,%98[^\n]s",&latitude,&longitude,info) == 3){

        if(latitude < - 90 || latitude > 90){
            fprintf(stderr,"Invalid latitude \n");
        }
        
        else if(longitude < -180 || longitude > 180){
            fprintf(stderr,"Invalid longitude \n");
        }
        
        else if (campus(latitude,longitude) == 1){
            fprintf(stdout,"%lf,%lf,%s\n",latitude,longitude,info);
        }
    }

    return 0;
}

