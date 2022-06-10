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
/*
 * this function is used to count up from a given number unitll the reminder is 0
 * takes num: set number to get 0 reminder from
 *       den: number to count up from to get 0 remender
 */
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
/*
 * this function is used to add up all the elemnets in an array to caclulate the avrage value in
 * that array
 * takes data: array of size n to avrage
 *       size: length of array
 *       returns double avrage of arrray's elements
 */
double CalcAvg(double *data, int size){
    int i;
    double Avg=0;
    for (i=0;i<size;i++){
        Avg=Avg+data[i];
    }
    Avg=Avg/size;
    return Avg;
}
/*
 * this function is used to insert a new value into the start of an array, moving all
 * other values up 1
 * takes arr: array to insert new elemnt ointo
 *       newdata: element to insert into the array
 */
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
/*
 * this function finds the max value in an array
 * take arr: array of legnth 50
 * returns: maax elemnt from arr
 */
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
/*
 * this function finds the min value in an array
 * take arr: array of legnth 50
 * returns: min elemnt from arr
 */
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
