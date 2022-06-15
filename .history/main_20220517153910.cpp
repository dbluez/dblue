#include <iostream>
#include <vector>
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
    vector<double> pr;

    pr.push_back(calcuPr(5,85,15,-43)); 
    pr.push_back(calcuPr(5,91,15,-49));
    pr.push_back(calcuPr(5,91.5,15,-49));
    pr.push_back(calcuPr(5,95.3,15,-53));
    pr.push_back(calcuPr(12,100,15,-52));
    pr.push_back(calcuPr(14,107.7,15,-60));

    for(auto it : pr)
    {
        cout<< *it <<endl;
    }
    return 0;

}