struct test{
    int a;
    int b;
    int c;
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

int main(){
    //cin>>an;cin>>am;
    read(an);
    read(am);
    i=0;
    j=0;
    for(i=0;i<an;i=i+1){
        for(j=0;j<am;j=j+1){
            //cin>>b[i][j];
            read(m[i][j].a);
        }
    }

    //cin>>bn;cin>>bm;
    read(bn);
    read(bm);
    for(i=0;i<bn;i=i+1){
        for(j=0;j<bm;j=j+1){
            //cin>>b[i][j];
            read(m[i][j].b);
        }
    }

    cn = an;
    cm = bm;
    for(i=0;i<cn;i=i+1){
        for(j=0;j<cm;j=j+1){
            t=0;
            for(x=0;x<am;x=x+1){
                t = t + m[i][x].a*m[x][j].b;
            }
            m[i][j].c=t;
        }
    }

    i=0;
    for(i=0;i<cn;i=i+1){
        for(j=0;j<cm;j=j+1){
            //cout<<c[i][j]<<' ';
            write(m[i][j].c);
            write(" ");
        }
        //cout<<'\n';
        write("\n");
    }
    return 0;
}
