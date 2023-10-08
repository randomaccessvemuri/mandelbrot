#include <gmpxx.h>
#include <iostream>

class complexNumber{
    mpf_class real;
    mpf_class imag;

    public:
        complexNumber(mpf_class real, mpf_class imag){
            this->real = real;
            this->imag = imag;
        }
        complexNumber operator+(complexNumber other){
            return complexNumber(this->real + other.real, this->imag + other.imag);
        }
        complexNumber operator*(complexNumber other){
            return complexNumber(this->real * other.real - this->imag * other.imag, this->real * other.imag + this->imag * other.real);
        }

        mpf_class getReal(){
            return this->real;
        }
        mpf_class getImag(){
            return this->imag;
        }
        mpf_class getMagnitude(){
            return sqrt(this->real * this->real + this->imag * this->imag);
        }
        void print(){
            std::cout << this->real << " + " << this->imag << "i" << std::endl;
        }
};