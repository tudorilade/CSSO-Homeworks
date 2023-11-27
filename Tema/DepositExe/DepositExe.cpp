using namespace std;
#include "Deposit.h"


int main(int argc, char* argv[]) {
    
    Deposit depositProc = Deposit(); //procesul deposit
    depositProc.startProccessing(argv[1]);
    //depositProc.startProccessing("C:\\Users\\teomi\\Desktop\\an3\\SEM1\\CSSO\\CSSO-Homeworks\\deposit\\*");
    //depositProc.startProccessing("C:\\Users\\tudor\\\CSSO-Homeworks\\deposit\\*");

    return 0;
}