#include "sound_stream.h"

extern "C" {
#include "libavutil/mathematics.h"
#include "libavutil/samplefmt.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}
#include <iostream>
#include <string>
#include <limits>

#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio

using std::cerr;
using std::endl;

SoundStream::SoundStream(std::string filename) : pos(0) {
  m_container = avformat_alloc_context();
  if (avformat_open_input(&m_container, filename.c_str(), NULL, NULL) < 0) {
    cerr<<"Could not open file"<<endl;
  }

  if (avformat_find_stream_info(m_container, NULL) < 0){
    cerr<<"Could not find file info"<<endl;
  }

  av_dump_format(m_container, 0, filename.c_str(), false);

  m_stream_id=-1;
  for (uint i = 0; i < m_container->nb_streams; i++) {
    if (m_container->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
      m_stream_id=i;
      break;
    }
  }
  if (m_stream_id == -1) {
    cerr<<"Could not find Audio Stream"<<endl;
  }

  m_metadata = m_container->metadata;
  m_ctx = m_container->streams[m_stream_id]->codec;
  m_codec = avcodec_find_decoder(m_ctx->codec_id);

  if (m_codec == NULL) {
    cerr<<("cannot find codec!")<<endl;
  }

  if (avcodec_open2(m_ctx,m_codec,NULL) < 0){
    cerr<<("Codec cannot be opended!")<<endl;
  }
  av_init_packet(&packet);
  frame = avcodec_alloc_frame();

}

uint SoundStream::get_sample_rate() {
  return m_ctx->sample_rate;  
}
AVSampleFormat SoundStream::get_sample_fmt() {
  return m_ctx->sample_fmt;
}
uint SoundStream::get_channels() {
  return m_ctx->channels;  
}

uint16_t *SoundStream::get_next_frame(uint& frame_size) {
  int buffer_size=AVCODEC_MAX_AUDIO_FRAME_SIZE + FF_INPUT_BUFFER_PADDING_SIZE;

  uint8_t *buffer = new uint8_t[buffer_size];
  packet.data = buffer;
  packet.size = buffer_size;

  uint8_t *samples= new uint8_t[buffer_size];
  int len;
  int frameFinished=0;

  int plane_size;
  
  if (av_read_frame(m_container, &packet) < 0) {
    if (m_loop) {
      set_pos(0);
      return get_next_frame(frame_size);
    } 
    else
      return NULL;
  }

  pos += (uint) (packet.duration * m_container->streams[m_stream_id]->time_base.num * 1000. / m_container->streams[m_stream_id]->time_base.den);

  if(packet.stream_index==m_stream_id){
    int len = avcodec_decode_audio4(m_ctx, frame, &frameFinished, &packet);
    if (len < 0) {
      cerr << "Error " << len << " during decode." << endl;  
    }
    int data_size = av_samples_get_buffer_size(&plane_size, m_ctx->channels,
        frame->nb_samples,
        m_ctx->sample_fmt, 1);
    uint16_t *out = (uint16_t *)samples;

    if(frameFinished){
      int write_p=0;

      switch (m_ctx->sample_fmt){
        case AV_SAMPLE_FMT_S16P:
          for (int nb=0; nb<plane_size/sizeof(uint16_t); nb++) {
            for (int ch = 0; ch < m_ctx->channels; ch++) {
              out[write_p] = ((uint16_t *) frame->extended_data[ch])[nb];
              write_p++;
            }
          }
          frame_size = (plane_size) * m_ctx->channels;
          break;
          
        case AV_SAMPLE_FMT_FLTP:
          for (int nb=0;nb<plane_size/sizeof(float);nb++){
            for (int ch = 0; ch < m_ctx->channels; ch++) {
              out[write_p] = ((float *) frame->extended_data[ch])[nb] * std::numeric_limits<short>::max() ;
              write_p++;
            }
          }
          frame_size = ( plane_size/sizeof(float) )  * sizeof(uint16_t) * m_ctx->channels;
          break;

        case AV_SAMPLE_FMT_S16:
          for (int nb=0; nb<plane_size/sizeof(uint16_t); nb++) {
            out[write_p] = ((uint16_t *) frame->extended_data[0])[nb];
            write_p++;
          }
          frame_size = plane_size;
          break;

        case AV_SAMPLE_FMT_FLT:
          for (int nb=0;nb<plane_size/sizeof(float);nb++){
            out[nb] = static_cast<short> ( ((float *) frame->extended_data[0])[nb] * std::numeric_limits<short>::max() );
          }
          frame_size = ( plane_size/sizeof(float) )  * sizeof(uint16_t);
          break;

        case AV_SAMPLE_FMT_U8P:
          for (int nb=0;nb<plane_size/sizeof(uint8_t);nb++){
            for (int ch = 0; ch < m_ctx->channels; ch++) {
              out[write_p] = ( ((uint8_t *) frame->extended_data[0])[nb] - 127) * std::numeric_limits<short>::max() / 127 ;
              write_p++;
            }
          }
          frame_size = ( plane_size/sizeof(uint8_t) )  * sizeof(uint16_t) * m_ctx->channels;
          break;

        case AV_SAMPLE_FMT_U8:
          for (int nb=0;nb<plane_size/sizeof(uint8_t);nb++){
            out[nb] = static_cast<short> ( ( ((uint8_t *) frame->extended_data[0])[nb] - 127) * std::numeric_limits<short>::max() / 127 );
          }
          frame_size = ( plane_size/sizeof(uint8_t) )  * sizeof(uint16_t);
          break;

        default:
          cerr<<"PCM type not supported"<<endl;
      }
      //delete buffer;
      //delete samples;
      return out;
    } 
    else {
      cerr<<"frame failed"<<endl;
      //delete buffer;
      //delete samples;
      return NULL;
    }
  }
}

SoundStream::~SoundStream() {
  av_free_packet(&packet);
  avcodec_close(m_ctx);
  av_close_input_file(m_container);
}

uint SoundStream::get_pos() {
  return pos;
}

void SoundStream::set_pos(uint ms) {
  int flag = 0;
  if (ms < pos) flag |= AVSEEK_FLAG_BACKWARD;

  AVStream *stream = m_container->streams[m_stream_id];
  double sec = ms / 1000.;
  int64_t ts = (int64_t) (sec / (stream->time_base.num / (double) stream->time_base.den));
  av_seek_frame(m_container, m_stream_id, ts, flag);
  pos = ms;
}
