#include <napi.h>
#include <stdio.h>
#include "Image.h"
#include "Type.h"

namespace lutis
{

    static Napi::Value ReadFile(const Napi::CallbackInfo& info)
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

        Napi::Buffer<lutis::type::Byte> buf = info[0].As<Napi::Buffer<lutis::type::Byte>>();
        lutis::image::Inspect(buf);

        return info.Env().Undefined();
    }

    Napi::Object Init(Napi::Env env, Napi::Object exports)
    {
        exports.Set(Napi::String::New(env, "readFile"), Napi::Function::New(env, ReadFile));
        return exports;
    }

    NODE_API_MODULE(lutis, Init);
}