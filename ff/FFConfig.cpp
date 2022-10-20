#include "FFConfig.h"
extern "C"
{
#include <libavformat/avformat.h>
#include <libavformat/url.h>
}

#define OS_MAXSTR 1024 * 30
extern "C" void av_register_protocol(const URLProtocol *p); // use ffmpeg ext api
extern "C" const URLProtocol ff_wasm_http_protocol;

static bool FFLogEnable = true;
static bool inited_ = false;
static bool register_module_inited_ = false;

void FFRegisterProtocol(const URLProtocol *p)
{
    printf("Register Protocol name:%s\n", p->name);
    av_register_protocol(p);
}

void FFLogCallbackFfmpeg(void *ptr, int level, const char *fmt, va_list args)
{
    if (FFLogEnable)
    {
        char line[OS_MAXSTR] = {0};
        int rc = vsnprintf(line, OS_MAXSTR, fmt, args);
        if (rc <= 0)
        {
            // error
        }

        switch (level)
        {
        case AV_LOG_QUIET:
            break;
        case AV_LOG_DEBUG:
            printf("D %s\n", line);
            break;
        case AV_LOG_VERBOSE:
            printf("V %s\n", line);
            break;
        case AV_LOG_INFO:
            printf("I %s\n", line);
            break;
        case AV_LOG_WARNING:
            printf("W %s\n", line);
            break;
        case AV_LOG_ERROR:
            printf("E %s\n", line);
            break;
        case AV_LOG_FATAL:
            printf("F %s\n", line);
            break;
        case AV_LOG_PANIC:
            printf("P %s\n", line);
            break;
        default:
            break;
        }
    }
}

static void registerModule()
{
    FFRegisterProtocol(&ff_wasm_http_protocol);
}

void FFConfig::FFInitialize(registerModuleHandle handle)
{
    if (!inited_)
    {
        avcodec_register_all();
        av_register_all();
        avformat_network_init();
        printf("register init\n");
        inited_ = true;
    }
    if (!register_module_inited_ && handle)
    {
        handle();
        register_module_inited_ = true;
    }
}

void FFConfig::Initialize()
{
    FFInitialize(registerModule);
    av_log_set_callback(FFLogCallbackFfmpeg);
}
