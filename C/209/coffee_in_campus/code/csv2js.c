//
//  main.c
//  A1
//
//  Created by Bingzhao Shan on 2016-10-16.
//  Copyright © 2016 Bingzhao Shan. All rights reserved.
//

#include <stdio.h>

int main(int argc, const char * argv[]) {
    // insert code here...
    double latitude;
    double longitude;
    char info[100];

    
    int started = 0;
    fprintf(stdout,"data=[\n");

    while(fscanf(stdin,"%lf,%lf,%98[^\n]s",&latitude,&longitude,info) == 3){

        if(latitude < - 90 || latitude > 90){
            fprintf(stderr,"Invalid latitude \n");
        }
        
        else if(longitude < -180 || longitude > 180){
            fprintf(stderr,"Invalid longitude \n");
        }
        
        else{
            if(started)
                fprintf(stdout,",\n");
            else
                started = 1;
            
            fprintf(stdout,"{latitude:%lf,longitude:%lf,label:'%s'}",latitude,longitude,info);
        }
    }
    
    
    fprintf(stdout,"\n]");
    return 0;
}
