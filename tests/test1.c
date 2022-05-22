int a[100000];
int n;
int pivot; 
int t;
int I;
int tr;

int isSorted(int x,int y,int z,int w){
    tr = 1;
    for(z=0;z<y-1;z=z+1){
        if(a[x+z]>a[x+z+1])tr=0;
        if(tr==0)z=y;
    }
    return tr;
}

int qsort(int st, int arrN, int i, int j){
    pivot = a[st];
    i = 0;
    j = arrN-1;
    for(i=0;i<j;tr=0){
        t = a[st+j];
        for(tr=0;i<j && t>pivot;tr=0){
            j = j - 1;
            t = a[st+j];
        }
        t = a[st+i];
        a[st+i] = a[st+j];
        a[st+j] = t;
        t = a[st+i];
        for(tr=0;i<j && t<=pivot;tr=0){
            i = i + 1;
            t = a[st+i];
        }
        t = a[st+i];
        a[st+i] = a[st+j];
        a[st+j] = t;
    }
    if(isSorted(st,arrN,i,j)==0){
        if(i>1)qsort(st, i, 0, i-1);
        if(arrN-i-1>1)qsort(st+i+1, arrN-i-1, 0, arrN-i-2);
    }
    return 0;
}   

int main(){
    read(n,int);
    I=0;
    for(I=0;I<n;I=I+1){
        read(a[I],int);
    }
    if(n>1)qsort(0,n,0,n-1);
    I = 0;
    for(I=0;I<n;I=I+1){
        write(a[I],int);
        write("\n",char);
    }
    //if(I==0)write("\n",char);
    return 0;
}
