#ifndef __FFMPEG_CONFIG_H__
#define __FFMPEG_CONFIG_H__
#include <string>

typedef void (*registerModuleHandle)();

class FFConfig
{
public:
    static void Initialize();

private:
    static void FFInitialize(registerModuleHandle = nullptr);

private:
    FFConfig();
    ~FFConfig();
};

#endif //__FFMPEG_CONFIG_H__
