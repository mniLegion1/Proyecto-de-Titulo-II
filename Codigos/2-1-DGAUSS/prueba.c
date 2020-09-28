#include <stdio.h>

int main(){
    int i,a = 0;

    for(i=0;i<1;i++){
        if(a == 0){
        printf("continue\n");
        }
        else if(a == 1){
            printf("1\n");
        }
        else{
            printf("else\n");
        }
        printf("Hola\n");
    }
    
}