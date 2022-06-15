#include <iostream>
using namespace std;


int calcuPr(int zy, int sh, int yudu,int mx)
{
    int pr = 0;
    pr = mx + yudu+sh-zy;
    return pr;
}

int main()
{

    int pr = 0;
    int sh = 0;
    int yudu = 15;
    pr = calcuPr(5,91.5,15,-49);
    cout<<pr<<endl;
    return 0;

}