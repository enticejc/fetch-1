struct A1
{
    int a;
    struct A2
    {
        int mile;
        int mile_1[10];
    } b, c, d[10][20][60][2];
    struct
    {
        struct
        {
            int array_l[8];
        } mom[5];
    } dad[5];
    float mile_3[200][8];
};
struct A1 func_1(int para_a[4], float para_b[56][5], struct A2 para_c[4][5][6])
{
    struct A1 ddd;
    return ddd;
}
int main()
{
    struct A1 ddd1;
    struct
    {
        struct A1 uytre[8][8];
    } A23;
    A23.uytre[9][9];
    return func_1(ddd1.d[1][1][1][1].mile_1, ddd1.mile_3, ddd1.d[2]).dad[5].mom[7].array_l[0];
}