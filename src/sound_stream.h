#ifndef __SOUND_STREAM_H__
#define __SOUND_STREAM_H__

#include "types.h"

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}

#include <string>
#include <mutex>
#include <map>

class SoundStream {
  public:
    virtual pos_t get_pos() = 0;
    virtual void set_pos(pos_t ms) = 0;
    virtual void set_loop(bool loop) = 0;
    virtual uint16_t *get_next_frame(uint& frame_size) = 0;
    virtual uint get_channels() = 0;
    virtual AVSampleFormat get_sample_fmt() = 0;
    virtual uint get_sample_rate() = 0;
    virtual pos_t get_duration() = 0;
    virtual metadata_t get_metadata() = 0;
    virtual ~SoundStream() { };
};

class SoundFile : public SoundStream {
  private:
    std::string filename;

    AVDictionary *m_metadata;
    AVFormatContext *m_container;
    AVCodecContext *m_ctx;
    AVCodec *m_codec;
    AVFrame *frame;
    AVPacket packet;
    int m_stream_id;

    bool m_loop;

    pos_t m_pos;

    std::recursive_mutex m_mtx;
    std::recursive_mutex m_meta_mtx;

  public:
    SoundFile(std::string filename);
    virtual ~SoundFile();

    virtual pos_t get_pos();
    virtual void set_pos(pos_t ms);
    virtual void set_loop(bool loop);

    virtual uint16_t *get_next_frame(uint& frame_size);

    virtual uint get_channels();
    virtual AVSampleFormat get_sample_fmt();
    virtual uint get_sample_rate();
    virtual pos_t get_duration(); 

    virtual metadata_t get_metadata();
};
    
#endif
