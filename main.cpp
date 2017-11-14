//Nicolas Stoian
//CS780 Image Processing
//Project 8.2 - Hough Transform

//This program needs 3 command line arguments
//argv[1] "input" for text file representing the binary input image
//argv[2] "output1" for the result of the houghAry with header information
//argv[3] "output2" for pretty print of the houghAry

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif
using namespace std;

class ImageProcessing{ friend class HoughTransform;
private:
    int** imgAry;
    int numRows;
    int numCols;
    int minVal;
    int maxVal;

public:
    ImageProcessing();
    ImageProcessing(ifstream& inFile);
    ~ImageProcessing();
    void loadImage(ifstream& inFile);
};

class HoughTransform{
private:
    int** houghAry;
    ImageProcessing* image;
    int numRows;
    int numCols;
    int minVal;
    int maxVal;
    struct XYCoord{
        int x;
        int y;
    };
    XYCoord point;
    int angleInDegree;
    double angleInRadians;

public:
    HoughTransform();
    HoughTransform(ImageProcessing* image);
    ~HoughTransform();
    void setPoint(int x, int y);
    void computeDistance();
    void determineHeader();
    static string mapInt2Char(int theInt);
    void prettyPrint(ofstream& outFile);
    void printImage(ofstream& outFile);
};

int main(int argc, char* argv[]){
    ifstream inFile1;
    inFile1.open(argv[1]);
    ImageProcessing* image = new ImageProcessing(inFile1);
    inFile1.close();
    HoughTransform* hough = new HoughTransform(image);
    hough->computeDistance();
    ofstream outFile1;
    outFile1.open(argv[2]);
    hough->printImage(outFile1);
    outFile1.close();
    ofstream outFile2;
    outFile2.open(argv[3]);
    hough->prettyPrint(outFile2);
    outFile2.close();
}

ImageProcessing::ImageProcessing(): imgAry(NULL), numRows(0), numCols(0), minVal(0), maxVal(0){
}

ImageProcessing::ImageProcessing(ifstream& inFile){
    loadImage(inFile);
}

ImageProcessing::~ImageProcessing(){
    if(imgAry != NULL){
        for(int i = 0; i < numRows; i++){
            delete [] imgAry[i];
        }
    }
    delete [] imgAry;
}

void ImageProcessing::loadImage(ifstream& inFile){
    inFile >> numRows;
    inFile >> numCols;
    inFile >> minVal;
    inFile >> maxVal;
    imgAry = new int* [numRows];
    for(int i = 0; i < numRows; i++){
        imgAry[i] = new int [numCols];
    }
    for(int row = 0; row < numRows; row++){
        for(int col = 0; col < numCols; col++){
            imgAry[row][col] = 0;
        }
    }
    for(int row = 0; row < numRows; row++){
        for(int col = 0; col < numCols; col++){
            int value;
            inFile >> value;
            imgAry[row][col] = value;
        }
    }
}

HoughTransform::HoughTransform(): houghAry(NULL), numRows(0), numCols(0), minVal(0), maxVal(0), angleInDegree(0), angleInRadians(0.0){
    point.x = 0;
    point.y = 0;
}

HoughTransform::HoughTransform(ImageProcessing* image): numCols(180), minVal(0), maxVal(0), angleInDegree(0.0){
    this->image = image;
    point.x = 0;
    point.y = 0;
    angleInRadians = angleInDegree * M_PI / 180.0;
    numRows = (int)ceil(sqrt(pow(image->numRows, 2.0) + pow(image->numCols, 2.0)));
    houghAry = new int* [numRows];
    for(int i = 0; i < numRows; i++){
        houghAry[i] = new int [numCols];
    }
    for(int row = 0; row < numRows; row++){
        for(int col = 0; col < numCols; col++){
            houghAry[row][col] = 0;
        }
    }
}

HoughTransform::~HoughTransform(){
    if(houghAry != NULL){
        for(int i = 0; i < numRows; i++){
            delete [] houghAry[i];
        }
    }
    delete [] houghAry;
    image = NULL;
    delete image;
}

void HoughTransform::setPoint(int x, int y){
    point.x = x;
    point.y = y;
}

void HoughTransform::computeDistance(){
    for(int row = 0; row < image->numRows; row++){
        for(int col = 0; col < image->numCols; col++){
            if(image->imgAry[row][col] == 1){
                setPoint(col, row);
                for(angleInDegree = 0; angleInDegree < 180; angleInDegree++){
                    angleInRadians = angleInDegree * M_PI / 180.0;
                    double t = angleInRadians - atan((double)point.y / (double)point.x) - (M_PI / 2.0);
                    //double d = (sqrt(pow(point.x, 2.0) + pow(point.y, 2.0))) * cos(t);
                    int dist = abs((int)round((sqrt(pow(point.x, 2.0) + pow(point.y, 2.0))) * cos(t)));
                    houghAry[dist][angleInDegree]++;
                    //cout << setw(2) << point.x << " " << setw(2) << point.y << " " << setw(3) << angleInDegree << " " << setw(10)
                    //<< angleInRadians << " " << setw(10)<< t << " " << setw(10)<< d << " " << setw(2) << dist << endl;
                }
            }
        }
    }
}

string HoughTransform::mapInt2Char(int theInt){
    char toReturn [33];
    sprintf(toReturn, "%d", theInt);
    return toReturn;
}

void HoughTransform::prettyPrint(ofstream& outFile){
    for(int row = numRows - 1; row >= 0; row--){
        for(int col = 0; col < numCols; col++){
            if(houghAry[row][col] <= 0){
                outFile << " " << " ";
            }
            else{
                outFile << mapInt2Char(houghAry[row][col]) << " ";
            }
        }
        outFile << endl;
    }
}

void HoughTransform::determineHeader(){
    for(int row = 0; row < numRows; row++){
        for(int col = 0; col < numCols; col++){
            if(houghAry[row][col] < minVal){
                minVal = houghAry[row][col];
            }
            if(houghAry[row][col] > maxVal){
                maxVal = houghAry[row][col];
            }
        }
    }
}

void HoughTransform::printImage(ofstream& outFile){
    determineHeader();
    outFile << numRows << " " << numCols << " " << minVal << " " << maxVal << endl;
    for(int row = numRows - 1; row >= 0 ; row--){
        for(int col = 0; col < numCols; col++){
            outFile << mapInt2Char(houghAry[row][col]) << " ";
        }
        outFile << endl;
    }
}
