#include <iostream>
using namespace std;


int calcuPr(double zy, double sh, double yudu,double mx)
{
    double pr = 0;
    pr = mx + yudu+sh-zy;
    return pr;
}

int main()
{

    double pr = 0;

    pr1 = calcuPr(5,85,15,-43);
    pr2 = calcuPr(5,91,15,-49);
    pr3 = calcuPr(5,91.5,15,-49);
    pr4 = calcuPr(5,95.3,15,-53);
    pr5 = calcuPr(12,100,15,-52);
    pr6 = calcuPr(14,107.7,15,-60);

    for(int i=0;i<6;i++)
    {
    cout<<pr<<endl;

    }
    return 0;

}