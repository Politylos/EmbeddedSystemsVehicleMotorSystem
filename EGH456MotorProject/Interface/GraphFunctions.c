/*
 * GraphFunctions.c
 *
 *  Created on: 19 May 2022
 *      Author: polit
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include"GraphFunctions.h"

int CalcNoRemainder(int num, int den){
    if (den<num){
    int rem;
    rem = num%den;
    while (rem > 0){
        den=den+1;
        rem = num%den;
    }
    }
    return den;
}

double CalcAvg(double *data, int size){
    int i;
    double Avg=0;
    for (i=0;i<size;i++){
        Avg=Avg+data[i];
    }
    Avg=Avg/size;
    return Avg;
}

double* ArrayUpdate(double *arr, double newdata){
    //double data = CalcAvg(newdata,size);
    int i;
    //double temp = arr[0];
    double temparr[50];

    for (i=0;i<49;i++){
        temparr[i+1] = arr[i];
    }
    temparr[0]=newdata;
    for (i=0;i<50;i++){
        arr[i] = temparr[i];
    }
    //dd=temparr;
    //arr=temparr;


    //return temparr;
}
double max_element(double arr[]){
    int i;
    double maxEle=0;
    for (i=0;i<50;i++){
        if (arr[i] > maxEle){
            maxEle =arr[i];
        }
    }
    return maxEle;
}

double min_element(double arr[]){
    int i;
    double ele;
    double maxEle=0;
    for (i=0;i<50;i++){
        ele = arr[i];
        if ((ele < maxEle) ){
            maxEle =ele;
        }
    }
    return maxEle;
}
