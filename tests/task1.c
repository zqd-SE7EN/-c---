int a[1000];
int n;
int pivot; 
int t;
int I;
int tr;

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
    if(i>1)qsort(st, i, 0, i-1);
    if(arrN-i-1>1)qsort(st+i+1, arrN-i-1, 0, arrN-i-2);
    return 0;
}

int main(){
    read(n);
    I=0;
    for(I=0;I<n;I=I+1){
        read(a[I]);
    }
    qsort(0,n,0,n-1);
    I = 0;
    for(I=0;I<n;I=I+1){
        write(a[I]);
        write(" ");
    }
    write("\n");
    return 0;
}
