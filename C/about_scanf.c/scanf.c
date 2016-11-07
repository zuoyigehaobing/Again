#include <stdio.h>

int main(){
  double latitude;
  double longitude;
  char info[80];
  int result;

  int started = 0;
  printf("data = [\n");
  while(scanf("%lf,%lf,%79[^\n]s",&latitude,&longitude,info) == 3){
    if(latitude < - 90 || latitude > 90)
      fprintf(stderr,"Invalid latitude \n");

    else if(longitude < -180 || longitude > 180)
      fprintf(stderr,"Invalid longitude\n");

    else{
      if(started)
        printf(",\n");
      else
        started = 1;

      printf("{latitude:%lf,longtitude:%lf,info:'%s'}",latitude,longitude,info);
    }





  }



  printf("\n];");
  return 0;
}
