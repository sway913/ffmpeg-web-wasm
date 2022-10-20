#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include <libavutil/log.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavformat/url.h>
#include <libavutil/opt.h>
#ifdef __cplusplus
}
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define WASM_HTTP_TAG "[wasmhttp]"
#define MAX_FLIENAME_LEN 512

typedef struct WasmHttpContext
{
  const AVClass *av_class;
  char filename[MAX_FLIENAME_LEN];
  int64_t filesize;
  int64_t pos;
} WasmHttpContext;

static const AVOption wasm_options[] = {
    {NULL}};

static const AVClass wasmhttp_class = {
    .class_name = "wasmhttp",
    .item_name = av_default_item_name,
    .option = wasm_options,
    .version = LIBAVUTIL_VERSION_INT,
};

static int wasm_http_read(URLContext *h, unsigned char *buf, int size)
{
  printf("%s:wasm_http_read \n", WASM_HTTP_TAG);
  WasmHttpContext *c = (WasmHttpContext *)h->priv_data;
  if (c->pos == c->filesize)
  {
    av_log(NULL, AV_LOG_INFO, "%s eof %s\n", WASM_HTTP_TAG, c->filename);
    return AVERROR_EOF;
  }

  int readSize = 0;
  int64_t nextPos = c->pos + size - 1;
  if (c->pos + size >= c->filesize)
  {
    nextPos = c->filesize;
    av_log(NULL, AV_LOG_INFO, "%s read end %s\n", WASM_HTTP_TAG, c->filename);
  }
  av_log(NULL, AV_LOG_INFO, "wasmhttp read (%" PRId64 "-%" PRId64 ")\n", c->pos, nextPos);
  // readSize = wasm_fech(c->filename + 11, c->pos, nextPos, buf);
  if (readSize <= 0)
  {
    av_log(NULL, AV_LOG_ERROR, "%s fech failed %s\n", WASM_HTTP_TAG, c->filename);
    return AVERROR_EXTERNAL;
  }
  c->pos = c->pos + readSize;
  av_log(NULL, AV_LOG_INFO, "wasmhttp read (pos=%" PRId64 " readed=%d) (%x %x %x %x)\n", c->pos, readSize, buf[0], buf[1], buf[2], buf[3]);

  return readSize;
}

static int wasm_http_open(URLContext *h, const char *filename, int flags)
{
  printf("%s:wasm_http_open \n", WASM_HTTP_TAG);
  WasmHttpContext *c = (WasmHttpContext *)h->priv_data;

  if (filename == NULL)
  {
    av_log(NULL, AV_LOG_ERROR, "%s open failed\n", WASM_HTTP_TAG);
    return AVERROR_EXTERNAL;
  }

  memcpy(c->filename, filename, strlen(filename) + 1);
  // c->filesize = wasm_fetch_get_filesize(c->filename + 11);
  if (c->filesize <= 0)
  {
    av_log(NULL, AV_LOG_ERROR, "%s can not get filesize \n", WASM_HTTP_TAG);
    return AVERROR_EXTERNAL;
  }
  h->max_packet_size = 2 * 1024 * 1024;

  av_log(NULL, AV_LOG_INFO, "%s open success filesize = %" PRId64 " filename = %s \n", WASM_HTTP_TAG, c->filesize, c->filename);
  return 0;
}

static int64_t wasm_http_seek(URLContext *h, int64_t pos, int whence)
{
  printf("%s:wasm_http_seek \n", WASM_HTTP_TAG);
  WasmHttpContext *c = (WasmHttpContext *)h->priv_data;
  if (whence == AVSEEK_SIZE)
  {
    long long file_size = c->filesize;
    av_log(NULL, AV_LOG_INFO, "%s AVSEEK_SIZE %" PRId64 " \n", WASM_HTTP_TAG, c->filesize);
    return file_size;
  }
  else if (whence == 0)
  {
    c->pos = pos;
  }
  else if (whence == 1)
  {
    c->pos += pos;
  }
  else
  {
    av_log(NULL, AV_LOG_WARNING, "%s wasmhttp seek not support %d \n", WASM_HTTP_TAG, whence);
  }
  av_log(NULL, AV_LOG_INFO, "%s wasmhttp seek pos = %" PRId64 ",c->pos = %" PRId64 ",whence = %d \n", WASM_HTTP_TAG, pos, c->pos, whence);
  if (c->pos > c->filesize)
  {
    av_log(NULL, AV_LOG_ERROR, "%s wasmhttp seek failed \n", WASM_HTTP_TAG);
    return AVERROR_EXTERNAL;
  }

  return c->pos;
}

static int wasm_http_close(URLContext *h)
{
  printf("%s:wasm_http_close \n", WASM_HTTP_TAG);
  WasmHttpContext *c = (WasmHttpContext *)h->priv_data;
  av_log(NULL, AV_LOG_INFO, "%s close %s\n", WASM_HTTP_TAG, c->filename);
  return 0;
}

static int wasm_http_write(URLContext *h, const unsigned char *buf, int size)
{
  printf("%s:wasm_http_write \n", WASM_HTTP_TAG);
  return 0;
}

const URLProtocol ff_wasm_http_protocol = {
    .name = "wasmhttp",
    .url_open = wasm_http_open,
    .url_read = wasm_http_read,
    .url_write = wasm_http_write,
    .url_seek = wasm_http_seek,
    .url_close = wasm_http_close,
    .priv_data_size = sizeof(WasmHttpContext),
    .priv_data_class = &wasmhttp_class};
