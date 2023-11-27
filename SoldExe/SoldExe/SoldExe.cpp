#include "Sold.h"


int main(int argc, char* argv[])
{
    Sold soldExe = Sold();
    cout << "SOLD " << argv[2] << endl;
    soldExe.startProccessing(argv[2]);

  /*  soldExe.startProccessing("C:\\Users\\tudor\\CSSO-Homeworks\\sold\\*");*/
}
