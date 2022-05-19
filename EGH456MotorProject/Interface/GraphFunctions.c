/*
 * GraphFunctions.c
 *
 *  Created on: 19 May 2022
 *      Author: polit
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include"GraphFunctions.h"

int CalcNoRemainder(int num, int den){
    int rem;
    rem = num%den;
    while (rem > 0){
        den=den+1;
        rem = num%den;
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

double* ArrayUpdate(double *arr, double *newdata, int size){
    double data = CalcAvg(newdata,size);
    int i;
    //double temp = arr[0];
    double temparr[50];

    for (i=0;i<49;i++){
        temparr[i+1] = arr[i];
    }
    temparr[0]=data;
    for (i=0;i<50;i++){
        arr[i] = temparr[i];
    }
    //dd=temparr;
    //arr=temparr;


    //return temparr;
}

