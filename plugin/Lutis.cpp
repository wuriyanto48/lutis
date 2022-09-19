#include <napi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <Magick++.h>
#include <webp/decode.h>
#include <jpeglib.h>
#include "Type.h"
#include "NJpeg.h"
#include "NWebp.h"
#include "NMagick.h"
#include "NOpenCv.h"
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

namespace lutis
{
    static Napi::Value GrayFilterWebp(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        if (info.Length() < 1)
        {
            Napi::TypeError::New(env, "wrong number of argument").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsBuffer())
        {
            Napi::TypeError::New(env, "second argument should be buffer").ThrowAsJavaScriptException();
            return env.Null();
        }

        auto buf = info[0].As<Napi::Buffer<lutis::type::Byte>>();

        lutis::nwebp::NWebp* nw = lutis::nwebp::NWebp::FromBuffer(buf, 80);
        if (nw == nullptr)
        {
            Napi::TypeError::New(env, "error decode webp from buffer").ThrowAsJavaScriptException();
            return env.Null();
        }

        printf("webp size %zu\n", nw->Length());
        printf("colorspace: %d\n", nw->ColorSpace());
        printf("stride: %d\n", nw->Stride());

        // auto p_data = nw->PixelData();
        
        // for (int i = 0; i < p_data.size(); i++)
        // {
        //     p_data[i].Debug();
        // }

        // test encode
        lutis::type::Byte* out_data = nullptr;

        // manual encode
        // size_t encode_res = WebPEncodeRGBA(nw->Data(), nw->Width(), nw->Height(), nw->Stride(), (float)60, &out_data);

        // printf("encode_res: %lu\n", encode_res);

        // with advance encode (On Progress)
        std::vector<lutis::type::Byte> gray_pixel = nw->ToGrayPixel();
        
        lutis::type::Byte* gray_data = new lutis::type::Byte[gray_pixel.size()];
        memcpy(gray_data, gray_pixel.data(), gray_pixel.size());

        nw->Read(&gray_data);
        size_t output_webp_size;

        int encode_ok = nw->ToBuffer(&out_data, &output_webp_size);
        if (encode_ok != 0)
        {
            Napi::TypeError::New(env, "error encode webp to buffer").ThrowAsJavaScriptException();
            return env.Null();
        }

        delete nw;
        delete[] out_data;

        return Napi::Buffer<lutis::type::Byte>::Copy(env, out_data, output_webp_size);
    }

    static Napi::Value RotateMagick(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        if (info.Length() < 2)
        {
            Napi::TypeError::New(env, "wrong number of argument").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsNumber())
        {
            Napi::TypeError::New(env, "first argument should be number").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[1].IsBuffer())
        {
            Napi::TypeError::New(env, "second argument should be buffer").ThrowAsJavaScriptException();
            return env.Null();
        }

        double angle = info[0].As<Napi::Number>().DoubleValue();
        auto buf = info[1].As<Napi::Buffer<lutis::type::Byte>>();

        lutis::nmagick::NMagick* nmagick = lutis::nmagick::NMagick::FromBuffer(buf, lutis::nmagick::PNG);
        if (nmagick == nullptr) {
            Napi::TypeError::New(env, "error initialize nmagick").ThrowAsJavaScriptException();
            return env.Null();
        }

        lutis::type::Byte* out_data = nullptr;
        size_t out_size;

        int resize_ok = nmagick->Rotate(angle, &out_data, &out_size);
        if (resize_ok != 0) {
            Napi::TypeError::New(env, "error rotate with nmagick").ThrowAsJavaScriptException();
            return env.Null();
        }

        delete[] out_data;
        delete nmagick;

        return Napi::Buffer<lutis::type::Byte>::Copy(env, out_data, out_size);
    }

    static Napi::Value ResizeMagick(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        if (info.Length() < 4)
        {
            Napi::TypeError::New(env, "wrong number of argument").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsNumber())
        {
            Napi::TypeError::New(env, "first argument should be number").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[1].IsNumber())
        {
            Napi::TypeError::New(env, "second argument should be number").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[2].IsNumber())
        {
            Napi::TypeError::New(env, "third argument should be number").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[3].IsBuffer())
        {
            Napi::TypeError::New(env, "fourth argument should be buffer").ThrowAsJavaScriptException();
            return env.Null();
        }

        double height = info[0].As<Napi::Number>().DoubleValue();
        double width = info[1].As<Napi::Number>().DoubleValue();
        double factor = info[2].As<Napi::Number>().DoubleValue();
        auto buf = info[3].As<Napi::Buffer<lutis::type::Byte>>();

        lutis::nmagick::NMagick* nmagick = lutis::nmagick::NMagick::FromBuffer(buf, lutis::nmagick::PNG);
        if (nmagick == nullptr) {
            Napi::TypeError::New(env, "error initialize nmagick").ThrowAsJavaScriptException();
            return env.Null();
        }

        lutis::type::Byte* out_data = nullptr;
        size_t out_size;

        int resize_ok = nmagick->Resize(width, height, factor, &out_data, &out_size);
        if (resize_ok != 0) {
            Napi::TypeError::New(env, "error resize with nmagick").ThrowAsJavaScriptException();
            return env.Null();
        }

        delete[] out_data;
        delete nmagick;

        return Napi::Buffer<lutis::type::Byte>::Copy(env, out_data, out_size);
    }

    static Napi::Value DrawTextMagick(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        if (info.Length() < 9)
        {
            Napi::TypeError::New(env, "wrong number of argument").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsString())
        {
            Napi::TypeError::New(env, "first argument should be string").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[1].IsString())
        {
            Napi::TypeError::New(env, "second argument should be string").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[2].IsNumber())
        {
            Napi::TypeError::New(env, "third argument should be number").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[3].IsBuffer())
        {
            Napi::TypeError::New(env, "fourth argument should be buffer").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[4].IsNumber())
        {
            Napi::TypeError::New(env, "fifth argument should be number").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[5].IsNumber())
        {
            Napi::TypeError::New(env, "seventh argument should be number").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[6].IsObject())
        {
            Napi::TypeError::New(env, "eighth argument should be object of color").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[7].IsObject())
        {
            Napi::TypeError::New(env, "nineth argument should be object of color").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[8].IsString())
        {
            Napi::TypeError::New(env, "tenth argument should be string").ThrowAsJavaScriptException();
            return env.Null();
        }

        Napi::String text = info[0].As<Napi::String>();
        Napi::String font = info[1].As<Napi::String>();
        double text_size = info[2].As<Napi::Number>().DoubleValue();
        auto buf = info[3].As<Napi::Buffer<lutis::type::Byte>>();

        uint32_t text_width = info[4].As<Napi::Number>().Uint32Value();
        uint32_t text_height = info[5].As<Napi::Number>().Uint32Value();

        Napi::Object position_object = info[6].As<Napi::Object>();

        lutis::type::position_t.x = position_object.Get("x").As<Napi::Number>().Int64Value();
        lutis::type::position_t.y = position_object.Get("y").As<Napi::Number>().Int64Value();

        Napi::Object color_object = info[7].As<Napi::Object>();

        lutis::type::c_rgb.B = color_object.Get("B").As<Napi::Number>().DoubleValue();
        lutis::type::c_rgb.G = color_object.Get("G").As<Napi::Number>().DoubleValue();
        lutis::type::c_rgb.R = color_object.Get("R").As<Napi::Number>().DoubleValue();

        Napi::String picture_format = info[8].As<Napi::String>();

        lutis::nmagick::NMagick* nmagick = lutis::nmagick::NMagick::FromBuffer(buf, lutis::nmagick::GetFormat(picture_format));
        if (nmagick == nullptr) {
            Napi::TypeError::New(env, "error initialize nmagick").ThrowAsJavaScriptException();
            return env.Null();
        }

        lutis::type::Byte* out_data = nullptr;
        size_t out_size;

        int draw_ok = nmagick->DrawText(text, font, text_size, text_width, text_height, lutis::type::position_t, lutis::type::c_rgb, &out_data, &out_size);
        if (draw_ok != 0) {
            Napi::TypeError::New(env, "error draw text with nmagick").ThrowAsJavaScriptException();
            return env.Null();
        }

        delete[] out_data;
        delete nmagick;

        return Napi::Buffer<lutis::type::Byte>::Copy(env, out_data, out_size);
    }

    static Napi::Value DrawMagick(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();
        Magick::Image image( Magick::Geometry(300,200), Magick::Color("white") );

        // Set draw options 
        image.strokeColor("red"); // Outline color 
        image.fillColor("green"); // Fill color 
        image.strokeWidth(5);

        // Draw a circle 
        image.draw( Magick::DrawableCircle(100,100, 50,100) );

        // Draw a rectangle 
        image.draw( Magick::DrawableRectangle(200,200, 270,170) );

        Magick::Blob blob;
        image.magick("JPEG");
        image.write(&blob);
        
        printf("blob length : %lu\n", blob.length());

        lutis::type::Byte datas[blob.length()];
        memcpy(datas, blob.data(), blob.length());

        return Napi::Buffer<lutis::type::Byte>::Copy(env, datas, sizeof(datas));
    }

    static void GaussianBlur(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        if (info.Length() < 2)
        {
            Napi::TypeError::New(env, "wrong number of argument").ThrowAsJavaScriptException();
            return;
        }

        if (!info[0].IsString())
        {
            Napi::TypeError::New(env, "first argument should be string").ThrowAsJavaScriptException();
            return;
        }

        if (!info[1].IsBuffer())
        {
            Napi::TypeError::New(env, "second argument should be buffer").ThrowAsJavaScriptException();
            return;
        }

        if (!info[2].IsFunction())
        {
            Napi::TypeError::New(env, "first argument should be function").ThrowAsJavaScriptException();
            return;
        }

        Napi::Function callback = info[2].As<Napi::Function>();

        Napi::String format = info[0].As<Napi::String>();
        auto buf = info[1].As<Napi::Buffer<lutis::type::Byte>>();
        std::vector<lutis::type::Byte> out;

        lutis::ncv::NCv* ncv = lutis::ncv::NCv::FromBuffer(buf);
        if (ncv == nullptr)
            callback.Call(env.Global(), { Napi::String::New(env, "error initialize ncv"), env.Null() });

        int blur_ok = ncv->GaussianBlur(format, out);
        if (blur_ok != 0)
            callback.Call(env.Global(), { Napi::String::New(env, "error execute gaussian blur"), env.Null() });

        // free memory
        delete ncv;

        callback.Call(env.Global(), {
            env.Null(), 
            Napi::Buffer<lutis::type::Byte>::Copy(env, out.data(), out.size())
        });
    }

    static void DrawCircle(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        if (info.Length() < 4)
        {
            Napi::TypeError::New(env, "wrong number of argument").ThrowAsJavaScriptException();
            return;
        }

        if (!info[0].IsString())
        {
            Napi::TypeError::New(env, "first argument should be string").ThrowAsJavaScriptException();
            return;
        }

        if (!info[1].IsNumber())
        {
            Napi::TypeError::New(env, "second argument should be number").ThrowAsJavaScriptException();
            return;
        }

        if (!info[2].IsNumber())
        {
            Napi::TypeError::New(env, "third argument should be number").ThrowAsJavaScriptException();
            return;
        }

        if (!info[3].IsNumber())
        {
            Napi::TypeError::New(env, "fourth argument should be number").ThrowAsJavaScriptException();
            return;
        }

        if (!info[4].IsObject())
        {
            Napi::TypeError::New(env, "fourth argument should be object of color").ThrowAsJavaScriptException();
            return;
        }

        if (!info[5].IsFunction())
        {
            Napi::TypeError::New(env, "fifth argument should be function").ThrowAsJavaScriptException();
            return;
        }

        Napi::String format = info[0].As<Napi::String>();
        uint32_t width = info[1].As<Napi::Number>().Uint32Value();
        uint32_t height = info[2].As<Napi::Number>().Uint32Value();
        double angle = info[3].As<Napi::Number>().DoubleValue();

        Napi::Object colorObject = info[4].As<Napi::Object>();

        lutis::type::c_rgb.B = colorObject.Get("B").As<Napi::Number>().DoubleValue();
        lutis::type::c_rgb.G = colorObject.Get("G").As<Napi::Number>().DoubleValue();
        lutis::type::c_rgb.R = colorObject.Get("R").As<Napi::Number>().DoubleValue();
        
        Napi::Function callback = info[5].As<Napi::Function>();

        std::vector<lutis::type::Byte> out;

        lutis::ncv::NCv* ncv = new lutis::ncv::NCv();
        if (ncv == nullptr)
            callback.Call(env.Global(), { Napi::String::New(env, "error initialize ncv"), env.Null() });

        int draw_ok = ncv->DrawElipse(format, lutis::type::c_rgb, width, height, angle, out);
        if (draw_ok != 0)
            callback.Call(env.Global(), { Napi::String::New(env, "error execute draw elipse blur"), env.Null() });

        delete ncv;

        callback.Call(env.Global(), {
            env.Null(), 
            Napi::Buffer<lutis::type::Byte>::Copy(env, out.data(), out.size())
        });
    }

    static Napi::Value RandomPixelJpeg(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        if (info.Length() < 2)
        {
            Napi::TypeError::New(env, "wrong number of argument").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsNumber())
        {
            Napi::TypeError::New(env, "first argument should be number").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[1].IsNumber())
        {
            Napi::TypeError::New(env, "second argument should be number").ThrowAsJavaScriptException();
            return env.Null();
        }

        uint32_t image_width = info[0].As<Napi::Number>().Uint32Value();
        uint32_t image_height = info[1].As<Napi::Number>().Uint32Value();

        lutis::type::Byte* out_data = nullptr;

        // With a pattern
        lutis::type::Byte* random_pixel_buffer = nullptr;
        uint32_t channel = 3;
        random_pixel_buffer = new lutis::type::Byte[image_width * image_height * channel];

        /* initialize random seed: */
        srand (time(NULL));
        for (int j = 0; j != image_height; j++) 
        {
            for (int i = 0; i != image_width*channel; i++)
                if (i%2==0)
                {
                    random_pixel_buffer[(j * image_width * channel) + i] = 0; 
                } else
                {
                    random_pixel_buffer[(j * image_width * channel) + i] = rand() % 256; 
                }
        }

        lutis::njpeg::NJpeg* input = new lutis::njpeg::NJpeg(image_width, image_height, channel);
        input->Read(&random_pixel_buffer);

        int write_res = input->ToBuffer(J_COLOR_SPACE::JCS_RGB, &out_data);
        if (write_res != 0)
        {
            delete input;
            delete[] out_data;
            
            Napi::TypeError::New(env, "error writing jpeg data").ThrowAsJavaScriptException();
            return env.Null();
        }

        delete input;
        delete[] out_data;

        return Napi::Buffer<lutis::type::Byte>::Copy(env, out_data, input->Length());
    }

    static Napi::Value GrayFilterJpeg(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        if (info.Length() < 1)
        {
            Napi::TypeError::New(env, "wrong number of argument").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsBuffer())
        {
            Napi::TypeError::New(env, "second argument should be buffer").ThrowAsJavaScriptException();
            return env.Null();
        }

        lutis::type::Byte* out_data = nullptr;

        auto buf = info[0].As<Napi::Buffer<lutis::type::Byte>>();

        lutis::njpeg::NJpeg* input_jpeg = lutis::njpeg::NJpeg::FromBuffer(buf);
        if (input_jpeg == nullptr)
        {
            Napi::TypeError::New(env, "read jpeg data to buffer").ThrowAsJavaScriptException();
            return env.Null();
        }

        // auto p_data = input_jpeg->PixelData();
        
        // for (int i = 0; i < p_data.size(); i++)
        // {
        //     printf("i: %d\n", i);
        //     p_data[i].Debug();
        // }

        std::vector<lutis::type::Byte> gray_pixel = input_jpeg->ToGrayPixel();
        
        lutis::type::Byte* gray_data = new lutis::type::Byte[gray_pixel.size()];
        memcpy(gray_data, gray_pixel.data(), gray_pixel.size());

        input_jpeg->Read(&gray_data);

        int write_res = input_jpeg->ToBuffer(J_COLOR_SPACE::JCS_RGB, &out_data);
        if (write_res != 0)
        {
            delete[] out_data;
            delete input_jpeg;
            Napi::TypeError::New(env, "error writing jpeg data").ThrowAsJavaScriptException();
            return env.Null();
        }

        delete[] out_data;
        delete input_jpeg;

        return Napi::Buffer<lutis::type::Byte>::Copy(env, out_data, input_jpeg->Length());
    }

    static Napi::Value JpegToWebp(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        if (info.Length() < 1)
        {
            Napi::TypeError::New(env, "wrong number of argument").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsBuffer())
        {
            Napi::TypeError::New(env, "second argument should be buffer").ThrowAsJavaScriptException();
            return env.Null();
        }

        lutis::type::Byte* out_data = nullptr;

        auto buf = info[0].As<Napi::Buffer<lutis::type::Byte>>();

        lutis::njpeg::NJpeg* input_jpeg = lutis::njpeg::NJpeg::FromBuffer(buf);
        if (input_jpeg == nullptr)
        {
            Napi::TypeError::New(env, "read jpeg data to buffer").ThrowAsJavaScriptException();
            return env.Null();
        }

        // std::vector<lutis::type::Byte> gray_pixel = input_jpeg->ToGrayPixel();
        
        // lutis::type::Byte* gray_data = new lutis::type::Byte[gray_pixel.size()];
        // memcpy(gray_data, gray_pixel.data(), gray_pixel.size());

        // input_jpeg->Read(&gray_data);

        // webp
        lutis::nwebp::NWebp* output_webp = new lutis::nwebp::NWebp(input_jpeg->Width(), 
            input_jpeg->Height(), input_jpeg->Channel(), input_jpeg->Width()*input_jpeg->Channel(), 40);

        lutis::type::Byte* jpg_rgb_data = input_jpeg->Data();

        lutis::type::Byte* rgb_data = new lutis::type::Byte[input_jpeg->Height()*input_jpeg->Width()*input_jpeg->Channel()];
        memcpy(rgb_data, jpg_rgb_data, input_jpeg->Height()*input_jpeg->Width()*input_jpeg->Channel());

        output_webp->Read(&rgb_data);
        size_t output_webp_size; 

        int write_res = output_webp->ToBuffer(&out_data, &output_webp_size);
        if (write_res != 0)
        {
            delete[] out_data;
            delete input_jpeg;
            Napi::TypeError::New(env, "error writing jpeg data").ThrowAsJavaScriptException();
            return env.Null();
        }

        delete[] out_data;
        delete input_jpeg;
        delete output_webp;

        return Napi::Buffer<lutis::type::Byte>::Copy(env, out_data, output_webp_size);
    }

    static Napi::Value OcrScan(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        if (info.Length() < 1)
        {
            Napi::TypeError::New(env, "wrong number of argument").ThrowAsJavaScriptException();
            return env.Null();
        }

         if (!info[0].IsString())
        {
            Napi::TypeError::New(env, "first argument should be string").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[1].IsBuffer())
        {
            Napi::TypeError::New(env, "second argument should be buffer").ThrowAsJavaScriptException();
            return env.Null();
        }

        char* out_data;

        Napi::String tessdata_path = info[0].As<Napi::String>();
        std::string tessdata_path_str = tessdata_path.Utf8Value();
        auto buf = info[1].As<Napi::Buffer<lutis::type::Byte>>();

        lutis::ncv::NCv* ncv = lutis::ncv::NCv::FromBuffer(buf);
        if (ncv == nullptr)
        {
            Napi::TypeError::New(env, "error init opencv").ThrowAsJavaScriptException();
            return env.Null();
        }

        tesseract::TessBaseAPI* tess_api = new tesseract::TessBaseAPI();
        if (tess_api->Init(tessdata_path_str.data(), "eng", tesseract::OEM_LSTM_ONLY))
        {
            Napi::TypeError::New(env, "error init tesseract base api").ThrowAsJavaScriptException();
            return env.Null();
        }
        tess_api->SetPageSegMode(tesseract::PSM_AUTO);

        tess_api->SetImage(ncv->Mat().data, ncv->Mat().cols, ncv->Mat().rows, 3, 3*ncv->Mat().cols);
        out_data = tess_api->GetUTF8Text();

        tess_api->End();
        
        delete ncv;
        delete tess_api;
        delete[] out_data;

        return Napi::String::New(env, out_data);
    }

    static Napi::Value GenerateJuliaSetJpeg(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        if (info.Length() < 2)
        {
            Napi::TypeError::New(env, "wrong number of argument").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsNumber())
        {
            Napi::TypeError::New(env, "first argument should be number").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[1].IsNumber())
        {
            Napi::TypeError::New(env, "second argument should be number").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[2].IsNumber())
        {
            Napi::TypeError::New(env, "third argument should be number").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[3].IsNumber())
        {
            Napi::TypeError::New(env, "fourth argument should be number").ThrowAsJavaScriptException();
            return env.Null();
        }

        uint32_t image_width = info[0].As<Napi::Number>().Uint32Value();
        uint32_t image_height = info[1].As<Napi::Number>().Uint32Value();
        float c_real = info[2].As<Napi::Number>().FloatValue();
        float c_imag = info[3].As<Napi::Number>().FloatValue();

        lutis::type::Byte* out_data = nullptr;

        uint32_t channel = 3;

        lutis::njpeg::NJpeg* input = new lutis::njpeg::NJpeg(image_width, image_height, channel);
        input->GenerateJuliaSet(c_real, c_imag);

        int write_res = input->ToBuffer(J_COLOR_SPACE::JCS_RGB, &out_data);
        if (write_res != 0)
        {
            delete input;
            delete[] out_data;
            
            Napi::TypeError::New(env, "error writing jpeg data").ThrowAsJavaScriptException();
            return env.Null();
        }

        delete input;
        delete[] out_data;

        return Napi::Buffer<lutis::type::Byte>::Copy(env, out_data, input->Length());
    }

    Napi::Object Init(Napi::Env env, Napi::Object exports)
    {
        exports.Set(Napi::String::New(env, "gaussianBlur"), Napi::Function::New(env, GaussianBlur));
        exports.Set(Napi::String::New(env, "drawCircle"), Napi::Function::New(env, DrawCircle));
        exports.Set(Napi::String::New(env, "drawMagick"), Napi::Function::New(env, DrawMagick));
        exports.Set(Napi::String::New(env, "rotateMagick"), Napi::Function::New(env, RotateMagick));
        exports.Set(Napi::String::New(env, "resizeMagick"), Napi::Function::New(env, ResizeMagick));
        exports.Set(Napi::String::New(env, "drawTextMagick"), Napi::Function::New(env, DrawTextMagick));
        exports.Set(Napi::String::New(env, "grayFilterWebp"), Napi::Function::New(env, GrayFilterWebp));
        exports.Set(Napi::String::New(env, "randomPixelJpeg"), Napi::Function::New(env, RandomPixelJpeg));
        exports.Set(Napi::String::New(env, "grayFilterJpeg"), Napi::Function::New(env, GrayFilterJpeg));
        exports.Set(Napi::String::New(env, "jpegToWebp"), Napi::Function::New(env, JpegToWebp));
        exports.Set(Napi::String::New(env, "ocrScan"), Napi::Function::New(env, OcrScan));
        exports.Set(Napi::String::New(env, "generateJuliaSet"), Napi::Function::New(env, GenerateJuliaSetJpeg));
        
        return exports;
    }

    NODE_API_MODULE(lutis, Init);
}