#ifndef HEADER_DRAW_H
#define HEADER_DRAW_H

#include <napi.h>
#include "Type.h"
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

namespace lutis 
{
    namespace draw 
    {
        static int DrawElipse(const std::string& format, 
            int w, int h, double angle, std::vector<lutis::type::Byte>& out)
        {
            int thickness = 2;
            int lineType = 8;

            cv::Mat img = cv::Mat::zeros(w, h, CV_8UC3);
            cv::ellipse(img,
                cv::Point( w/2, h/2 ),
                cv::Size( w/4, h/16 ),
                angle,
                0,
                360,
                cv::Scalar( 130, 24, 242 ),
                thickness,
                lineType );

            cv::imencode(format, img, out);

            return 0;
        }
    }
}

#endif