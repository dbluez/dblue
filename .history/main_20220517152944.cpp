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
    pr = calcuPr(5,95.3,15,-53);
    cout<<pr<<endl;
    return 0;

}