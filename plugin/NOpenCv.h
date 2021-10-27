#ifndef HEADER_CV_H
#define HEADER_CV_H

#include <napi.h>
#include <vector>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "Type.h"
#include "Base.h"

namespace lutis 
{
    namespace ncv
    {

        static const int MAX_KERNEL_LENGTH = 31;

        class NCv
        {
            public:
            NCv()
            {
                
            }
            NCv(const cv::Mat& _mat)
            {
                this->mat = _mat;
            }
            NCv(lutis::type::Byte* buffer, size_t buffer_size)
            {
                cv::Mat raw_mat(1, buffer_size, CV_8UC1, (void*) buffer);
                cv::Mat _mat = cv::imdecode(raw_mat, cv::IMREAD_COLOR|cv::IMREAD_ANYDEPTH);
                this->mat = _mat;
            }

            NCv(const std::vector<lutis::type::Byte>& buffer)
            {
                cv::Mat raw_mat(1, buffer.size(), CV_8UC1, (void*) buffer.data());
                cv::Mat _mat = cv::imdecode(raw_mat, cv::IMREAD_COLOR|cv::IMREAD_ANYDEPTH);
                this->mat = _mat;
            }

            ~NCv()
            {
                
            }

            public:
            cv::Mat Mat() const
            {
                return mat;
            }

            int GaussianBlur(const std::string& format, std::vector<lutis::type::Byte>& out)
            {
                printf("performing GaussianBlur\n");
                
                cv::Mat dst = mat.clone();

                for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
                    cv::GaussianBlur(mat, dst, cv::Size( i, i ), 0, 0 );
                
                // cv::imwrite("dst.jpg", dst);

                // int size = dst.total() * dst.elemSize();
                // *out = new lutis::type::Byte[size];
                // memcpy(*out, dst.data, size * sizeof(lutis::type::Byte));

                // *out = new lutis::type::Byte[data.Length()];
                // memcpy(*out, datas, data.Length() * sizeof(lutis::type::Byte));

                // encode Mat to memory buffer
                // opencv supported format https://docs.opencv.org/3.4/d4/da8/group__imgcodecs.html#ga288b8b3da0892bd651fce07b3bbd3a56
                cv::imencode(format, dst, out);

                return 0;
            }

            int DrawElipse(const std::string& format, const lutis::type::Color& color, 
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
                    cv::Scalar( color.B, color.G, color.R ),
                    thickness,
                    lineType );

                cv::imencode(format, img, out);

                return 0;
            }

            public:
            static NCv* FromBuffer(const Napi::Buffer<lutis::type::Byte>& input)
            {
                lutis::type::Byte* raw_data = reinterpret_cast<lutis::type::Byte*>(input.Data());
                const size_t size_data = input.Length();

                NCv* out = new NCv(raw_data, size_data);

                return out;
                
            }

            private:
            cv::Mat mat;
            
        };
    }
}

#endif