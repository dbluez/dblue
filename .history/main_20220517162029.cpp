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

    // pr.push_back(calcuPr(5,85,15,-43)); 
    // pr.push_back(calcuPr(5,91,15,-49));
    // pr.push_back(calcuPr(5,91.5,15,-49));
    // pr.push_back(calcuPr(5,95.3,15,-53));
    // pr.push_back(calcuPr(12,100,15,-52));
    // pr.push_back(calcuPr(14,107.7,15,-60));

    pr.push_back(calcuPr(5,94.6,15,-43)); 
    pr.push_back(calcuPr(5,100.6,15,-49));
    pr.push_back(calcuPr(5,101.1,15,-49));
    pr.push_back(calcuPr(5,104.9,15,-53));
    pr.push_back(calcuPr(12,109.6,15,-52));
    pr.push_back(calcuPr(14,117.3,15,-60));

    // pr.push_back(calcuPr(4,94,43,-150)); 
    // pr.push_back(calcuPr(4,96,43,-150));
    // pr.push_back(calcuPr(4,103.6,43,-150));
    // pr.push_back(calcuPr(4,105.6,43,-150));
    // pr.push_back(calcuPr(4,108,43,-150));
    // pr.push_back(calcuPr(4,110,43,-150));


    vector<double>::iterator it = pr.begin();
    for(; it != pr.end(); ++it)
	{
		cout<<(*it)<<endl;
	}
    return 0;

}