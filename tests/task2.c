struct test{
    int AA;
    int A;
    int a[3];
};


struct test m[100][100];
int an;
int am;
int bn;
int bm;
int cn;
int cm;
int t;
int i; 
int j; 
int x;
int y;
int kgCnt;
int kgnm;
int trash;

int getKgCnt(int n){
    kgnm=0;
    if(n<0){
        kgnm=kgnm+1;
        n=0-n;
    }
    if(n==0){
        kgnm=1;
    }
    for(trash=0;n>0;trash=0){
        kgnm=kgnm+1;
        n=n/10;
    }
    return 10-kgnm;
}

int main(){
    //cin>>an;cin>>am;
    read(an,int);
    read(am,int);
    i=0;
    j=0;
    for(i=0;i<an;i++){
        for(j=0;j<am;j++){
            //cin>>b[i][j];
            read(m[i][j].a[0],int);
        }
    }

    //cin>>bn;cin>>bm;
    read(bn,int);
    read(bm,int);
    for(i=0;i<bn;i++){
        for(j=0;j<bm;j++){
            //cin>>b[i][j];
            read(m[i][j].a[1],int);
        }
    }

    if(am!=bn){
        write("Incompatible Dimensions\n",char);

    }else{

    cn = an;
    cm = bm;
    for(i=0;i<cn;i++){
        for(j=0;j<cm;j++){
            t=0;
            for(x=0;x<am;x++){
                t = t + m[i][x].a[0]*m[x][j].a[1];
            }
            m[i][j].a[2]=t;
        }
    }

    i=0;
    for(i=0;i<cn;i++){
        for(j=0;j<cm;j++){
            //cout<<c[i][j]<<' ';
            kgCnt = getKgCnt(m[i][j].a[2]);
            for(t=0;t<kgCnt;t++){
                write(" ",char);
            }
            write(m[i][j].a[2],int);
        }
        //cout<<'\n';
        write("\n",int);
    }}
    return 0;
}
