#pragma once
class ProductInfo
{
public:
    ProductInfo(int, int , int , int );

    // Getter methods for accessing the private members
    int getIdProduct() { return id_product; }
    int getExpiresIn()  { return expires_in; }
    int getShelveId() { return shelve_id; }
    int getProductPrice() { return product_price; }

private:
    int id_product;
    int expires_in;
    int shelve_id;
    int product_price;
};

