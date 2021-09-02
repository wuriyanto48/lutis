#include <napi.h>
#include <stdio.h>
#include <vector>
#include <Magick++.h>
#include "Core.h"
#include "Draw.h"
#include "Type.h"
#include "Filter.h"

namespace lutis
{
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

        Magick::Image image;

        int decodeRes = lutis::core::DecodeFromBufferToMagickImage(buf, image);
        if (decodeRes != 0) {
            Napi::TypeError::New(env, "error decoding buffer").ThrowAsJavaScriptException();
            return env.Null();
        }

        // operations
        try 
        {
            image.rotate(angle);
        } catch(Magick::Error& err)
        {
            Napi::TypeError::New(env, "error rotate buffer").ThrowAsJavaScriptException();
            return env.Null();
        }

        Magick::Blob blobOut;
        image.magick("PNG");
        image.write(&blobOut);

        lutis::type::Byte datas[blobOut.length()];
        memcpy(datas, blobOut.data(), blobOut.length());

        return Napi::Buffer<lutis::type::Byte>::Copy(env, datas, sizeof(datas));
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

        Magick::Image image;

        int decodeRes = lutis::core::DecodeFromBufferToMagickImage(buf, image);
        if (decodeRes != 0) {
            Napi::TypeError::New(env, "error decoding buffer").ThrowAsJavaScriptException();
            return env.Null();
        }

        // operations
        try 
        {
            Magick::Geometry size = Magick::Geometry(height*factor, width*factor);
            image.zoom(size);
        } catch(Magick::Error& err)
        {
            Napi::TypeError::New(env, "error resize buffer").ThrowAsJavaScriptException();
            return env.Null();
        }

        Magick::Blob blobOut;
        image.magick("PNG");
        image.write(&blobOut);

        lutis::type::Byte datas[blobOut.length()];
        memcpy(datas, blobOut.data(), blobOut.length());

        return Napi::Buffer<lutis::type::Byte>::Copy(env, datas, sizeof(datas));
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

    static Napi::Value Inspect(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        if (info.Length() < 1)
        {
            Napi::TypeError::New(env, "wrong number of argument").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsBuffer())
        {
            Napi::TypeError::New(env, "argument should be buffer").ThrowAsJavaScriptException();
            return env.Null();
        }

        auto buf = info[0].As<Napi::Buffer<lutis::type::Byte>>();

        lutis::type::InspectData inspectData;
        int inspectResult = lutis::core::Inspect(buf, inspectData);
        if (inspectResult != 0)
        {
            Napi::TypeError::New(env, "error inspecting data").ThrowAsJavaScriptException();
            return env.Null();
        }

        Napi::Object inspectObject = Napi::Object::New(env);
        inspectObject.Set("sizeKB", inspectData.sizeKB);
        inspectObject.Set("colorChannelSize", inspectData.colorChannelSize);
        inspectObject.Set("totalArrayElement", inspectData.totalArrayElement);

        return inspectObject;
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
        Napi::Buffer<lutis::type::Byte> buf = info[1].As<Napi::Buffer<lutis::type::Byte>>();
        std::vector<lutis::type::Byte> out;
        int gaussianBlurRes = lutis::filter::GaussianBlur(format, buf, out);
        if (gaussianBlurRes != 0)
            callback.Call(env.Global(), { Napi::String::New(env, "error execute gaussian blur"), env.Null() });

        // lutis::type::Byte dataOuts[buf.Length()];
        // for (size_t index = 0; index < buf.Length(); index++) {
        //     // printf("array[%lu] = %d\n", index, out[index]);
        //     dataOuts[index] = out[index];
        // }

        // // free memory
        // lutis::core::CleanUp(&out);

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

        int drawElipseRes = lutis::draw::DrawElipse(format, lutis::type::c_rgb, width, height, angle, out);
        if (drawElipseRes != 0)
            callback.Call(env.Global(), { Napi::String::New(env, "error execute draw elipse blur"), env.Null() });

        callback.Call(env.Global(), {
            env.Null(), 
            Napi::Buffer<lutis::type::Byte>::Copy(env, out.data(), out.size())
        });
    }

    Napi::Object Init(Napi::Env env, Napi::Object exports)
    {
        exports.Set(Napi::String::New(env, "inspect"), Napi::Function::New(env, Inspect));
        exports.Set(Napi::String::New(env, "gaussianBlur"), Napi::Function::New(env, GaussianBlur));
        exports.Set(Napi::String::New(env, "drawCircle"), Napi::Function::New(env, DrawCircle));
        exports.Set(Napi::String::New(env, "drawMagick"), Napi::Function::New(env, DrawMagick));
        exports.Set(Napi::String::New(env, "rotateMagick"), Napi::Function::New(env, RotateMagick));
        exports.Set(Napi::String::New(env, "resizeMagick"), Napi::Function::New(env, ResizeMagick));
        return exports;
    }

    NODE_API_MODULE(lutis, Init);
}