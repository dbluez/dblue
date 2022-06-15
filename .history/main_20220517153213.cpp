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

    pr = calcuPr(5,100.3,15,-52);
    pr = calcuPr(5,100.3,15,-52);
    pr = calcuPr(5,100.3,15,-52);
    pr = calcuPr(5,100.3,15,-52);
    pr = calcuPr(12,100.3,15,-52);
    pr = calcuPr(14,100.3,15,-52);

    cout<<pr<<endl;
    return 0;

}