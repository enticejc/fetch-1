int a,b;
int c[3];
int f1(){}
int f2(int x, int y){}
int f3(int f1, int a){}

struct s0{
    int z;
}s00;

struct s1{
    int w;
};

int main(){
    struct s1 s2, s3;
    if (s2.w == 1){
        s2 = s3;
    }
    return 0;
}