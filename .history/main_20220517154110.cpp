#include <iostream>
#include <vector>
using namespace std;


double calcuPr(double zy, double sh, double yudu,double mx)
{
    double pr = 0;
    pr = mx + yudu+sh-zy;
    return pr;
}

int main()
{

    vector<double> pr;

    pr.push_back(calcuPr(5,85,15,-43)); 
    pr.push_back(calcuPr(5,91,15,-49));
    pr.push_back(calcuPr(5,91.5,15,-49));
    pr.push_back(calcuPr(5,95.3,15,-53));
    pr.push_back(calcuPr(12,100,15,-52));
    pr.push_back(calcuPr(14,107.7,15,-60));

vector<int>::iterator it = v.begin();
    for(; it != pr.end(); ++it)
	{
		cout<<(*it)<<endl;
	}
    return 0;

}