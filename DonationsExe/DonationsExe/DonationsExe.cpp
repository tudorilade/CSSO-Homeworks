

#include "Donations.h"
#include <iostream>

int main(int argc, char* argv[])
{
    Donations donations = Donations();

    donations.startProccessing(argv[2]);

    return 0;
}