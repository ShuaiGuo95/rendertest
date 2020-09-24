#include <iostream>
#include <algorithm>

#include "modelrend.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"

#include <ctime>

using namespace std;

int main(int argc, char** argv)
{
    // cv::VideoCapture cap("./test.mp4");
    cv::VideoCapture cap(0);

    clock_t startTime, endTime;

    if (!cap.isOpened())
    {
        std::cout << "Webcam cannot open!\n";
        return 0;
    }
    int frameH = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    int frameW = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    // int fps = cap.get(CV_CAP_PROP_FPS);
    // int numFrames = cap.get(CV_CAP_PROP_FRAME_COUNT);
    // printf("height=%d, width=%d, fps=%d, totalframes=%d\n", frameH, frameW, fps, numFrames);

    // Modelrender render(frameW, frameH);
    Modelrender * render = new Modelrender(frameW, frameH);
    cv::Mat frame;
    int index = 0;
    string dir= "./output/";

    while (cap.read(frame) && index<50)
    {
        cv::Mat dst1 = cv::Mat(frame.size(), CV_8UC3, cv::Scalar(255, 255, 255));
        // cout << index << endl;

        startTime = clock();
        dst1.data = render->rend(frame.data);
        // cv::imwrite(dir+to_string(index++)+".png", dst1);
        endTime = clock();
        cout << "Timeconsume = " << (double)(endTime-startTime)/CLOCKS_PER_SEC << "s" << endl;

        cv::imshow("test", dst1);
        cvWaitKey(5);
    }
    return 0;
}
