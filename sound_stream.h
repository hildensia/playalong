#ifndef __SOUND_STREAM_H__
#define __SOUND_STREAM_H__

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}

#include <string>
#include <mutex>

class SoundStream {
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

    uint pos;

    std::recursive_mutex m_mtx;

  public:
    SoundStream(std::string filename);
    ~SoundStream();
    uint get_pos();
    void set_pos(uint ms);

    /** Should the stream loop, i.e. restart from the beginning after the last
     *  frame?
     **/
    void set_loop(bool loop);

    uint16_t *get_next_frame(uint& frame_size);

    uint get_channels();
    AVSampleFormat get_sample_fmt();
    uint get_sample_rate();
    uint get_frame_size();
};

#endif
