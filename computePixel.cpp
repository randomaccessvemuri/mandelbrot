#include <gmpxx.h>
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

#include "fundamentals/numbers/complexNumber.cpp"

#define MAX_BOUNCES 1000


class cvImage{
    private:
        int width;
        int height;
        cv::Mat imageInternal;

    public:
        cvImage(int width, int height){
            imageInternal = cv::Mat(height, width, CV_8UC3, cv::Scalar(0,0,0));
            std::cout << "Initialized to black image!";
        }

        void setPixel(int x, int y, int R, int G, int B){
            if (R > 255 || G > 255 || B > 255){
                std::cout<<"\nInvalid Color!\n";
                throw -1;
            }

            imageInternal.at<cv::Vec3b>(x,y)[0] = B;
            imageInternal.at<cv::Vec3b>(x,y)[1] = G;
            imageInternal.at<cv::Vec3b>(x,y)[2] = R;
            
        }

        void displayImage(std::string name){
            cv::imshow(name, imageInternal);
            cv::waitKey(1);
        }

        void saveImage(std::string name){
            cv::imwrite(name, imageInternal);
        }

};



//FIXME: If the whole screen turns out white, it's probably because I messed up the numBounces logic!
int computeBouncesMandelBrot(int maxBounces, complexNumber x, mpf_class maxDistanceX, mpf_class maxDistanceY){
    int numBounces = 0;
    complexNumber currentCoordinate = x;
    
    //Check if number of bounces has exceeded max bounces and check if the path has gone out of bounds (in this case, off the screen)
    while (numBounces < maxBounces && currentCoordinate.getImag() < maxDistanceY && currentCoordinate.getReal() < maxDistanceX){
        //These are the bounces that are required to generate the mandelbrot set
        // z_n+1 = (z_n)^2 + C; C = z_0

        currentCoordinate = currentCoordinate*currentCoordinate + x;
        numBounces++;
    }

    if (numBounces==maxBounces){
        return -1;
    }

    return numBounces;
}


int computeBouncesBurningShip(int maxBounces, complexNumber x, mpf_class &maxDistanceX, mpf_class &maxDistanceY){
    int numBounces = 0;
    complexNumber currentCoordinate = x;
    
    //Check if number of bounces has exceeded max bounces and check if the path has gone out of bounds (in this case, off the screen)
    while (numBounces < maxBounces && currentCoordinate.getImag() < maxDistanceY && currentCoordinate.getReal() < maxDistanceX){
        //These are the bounces that are required to generate the burning ship set
        // z_n+1 = (|Re(z_n)| + i|Im(z_n)|)^2 + C; C = z_0
        mpf_class real = currentCoordinate.getReal();
        mpf_class imag = currentCoordinate.getImag();

        //Modulo because mpf_class doesn't have a modulus
        real = real >= 0 ? real : -real;
        imag = imag >= 0 ? imag : -imag;
        currentCoordinate = complexNumber(real, imag)*complexNumber(real, imag) + x;
        numBounces++;
    }

    if (numBounces==maxBounces){
        return -1;
    }

    return numBounces;
}




void iteration(mpf_class* x, mpf_class* y, mpf_class* coordinateOfBottomOfImage, mpf_class* coordinateOfRightOfImage, cvImage* fractalImage, int xCounter, int yCounter){
    complexNumber i = complexNumber(*x,*y);
    int numBounces = computeBouncesMandelBrot(MAX_BOUNCES, i, *coordinateOfBottomOfImage, *coordinateOfRightOfImage);


    //TODO: Color instead of grayscale
    int R = 0,G = 0,B = 0;

    if (numBounces != -1) {
        R = (4*numBounces)%256;
        G = (8*numBounces)%256;
        B = (4*numBounces)%256;
    }

    fractalImage->setPixel(yCounter, xCounter, R,G,B);
    
}




int main(){
    //These are used for the infinite zoom as opposed to just the static zoomed out image.
    std::cout<<"Starting up..";   


    //TODO:SET DEFAULT PRECISION
    mpf_set_default_prec(1024);

    //Set this 
    mpf_class scalingFactor = 150;
    mpf_class uFactor = -0.77568377;
    mpf_class vFactor = 0.13646737;

    

    //Set image resolution
    int imageWidth = 1024;
    int imageHeight = 512 ;

    
    
    cvImage fractalWindow = cvImage(imageWidth, imageHeight);


    std::cout<<"\nGenerating video..\n";   
    
    for (int k = 0; k < 1000; k+=1)
    {
        
        mpf_class currentScalingFactor = scalingFactor + (k*5);

        

        //How to determine the width of each pixel in coordinate space?:
        //Once we have that, we can multiply it with resolution dimensions to determine max and min X and Y        

        //FIXME:
        
        mpf_class maxXCartesian = imageWidth/currentScalingFactor;
        mpf_class maxYCartesian = imageHeight/currentScalingFactor;


        
        

        mpf_class pixelWidthX = maxXCartesian/imageWidth;
        mpf_class pixelWidthY = maxYCartesian/imageHeight;

        // /2 because origin is considered in centre
        std::pair<mpf_class, mpf_class> origin(uFactor, vFactor);

        std::cout<<"Origin: ("<<origin.first<<", "<<origin.second<<")\n";


        mpf_class coordinateOfLeftOfImage = origin.first - (imageWidth/2)*pixelWidthX;
        mpf_class coordinateOfRightOfImage = origin.first + (imageWidth/2)*pixelWidthX;
        mpf_class coordinateOfTopOfImage = origin.second - (imageHeight/2)*pixelWidthY;
        mpf_class coordinateOfBottomOfImage = origin.second + (imageHeight/2)*pixelWidthY;
        

        std::cout<<"TOP LEFT: ("<<coordinateOfLeftOfImage<<", "<<coordinateOfTopOfImage<<")\n";
        std::cout<<"BOTTOM RIGHT: ("<<coordinateOfRightOfImage<<", "<<coordinateOfBottomOfImage<<")\n";


        int xCounter = 0;
        int yCounter = 0;
        int pixelCounter = 0;

         
        //To get (x,y) for x+yi
        for (mpf_class x = coordinateOfLeftOfImage; x <= coordinateOfRightOfImage; x+=pixelWidthX){
            for (mpf_class y = coordinateOfTopOfImage; y <= coordinateOfBottomOfImage; y += pixelWidthY){

                std::cout<< "(" << pixelCounter << "/" << imageHeight*imageWidth << ")" << "(" << xCounter << ", "<< yCounter << ")" << "Scaling Factor: "<<currentScalingFactor << "\r";
                iteration(&x, &y, &maxYCartesian, &maxXCartesian, &fractalWindow, xCounter, yCounter);

                fractalWindow.displayImage("Mandelbrot");

                pixelCounter++;
                yCounter++;
                

            }

            xCounter++;
            yCounter = 0;

        }
        std::cout<<"Saving image..";
        std::string fileName = "Mandelbrot" + std::to_string(k) + ".png";
        fractalWindow.saveImage(fileName);


        
    }

    return EXIT_SUCCESS;
}
