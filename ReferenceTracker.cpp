// ReferenceTracker.cpp : définit le point d'entrée de l'application.
//

#include "ReferenceTracker.h"
#include <QApplication>
#include <QPushButton>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#define w 400
using namespace cv;

void MyEllipse(Mat img, double angle)
{
    int thickness = 2;
    int lineType = 8;
    ellipse(img,
        Point(w / 2, w / 2),
        Size(w / 4, w / 16),
        angle,
        0,
        360,
        Scalar(255, 0, 0),
        thickness,
        lineType);
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QPushButton button("Hello world!");
    button.show();

    constexpr char atom_window[] = "Drawing 1: Atom";
    const Mat atom_image = Mat::zeros(w, w, CV_8UC3);
    const Mat rook_image = Mat::zeros(w, w, CV_8UC3);
    MyEllipse(atom_image, 90);
    MyEllipse(atom_image, 0);
    MyEllipse(atom_image, 45);
    MyEllipse(atom_image, -45);
    imshow(atom_window, atom_image);
    moveWindow(atom_window, 0, 200);
    waitKey(0);

    return QApplication::exec(); // .exec starts QApplication and related GUI, this line starts 'event loop'    
}