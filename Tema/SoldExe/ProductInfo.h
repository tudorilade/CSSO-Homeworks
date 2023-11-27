#pragma once
class ProductInfo
{
public:
    ProductInfo(int);

    int getShelveId() { return shelveId; }
    
private:
    int shelveId;
};

