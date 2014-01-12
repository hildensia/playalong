#ifndef __FILTER_H__
#define __FILTER_H__

#include "sound_stream.h"

class Filter : public SoundStream {
  protected:
    SoundStream& input;
  public:
    Filter(SoundStream& input) : input(input) {};
    virtual pos_t get_pos() { return input.get_pos(); }
    virtual void set_pos(pos_t ms) { input.set_pos(ms); }
    virtual void set_loop(bool loop) { input.set_loop(loop); }
    virtual uint16_t *get_next_frame(uint& frame_size) { return input.get_next_frame(frame_size); }
    virtual uint get_channels() { return input.get_channels(); }
    virtual AVSampleFormat get_sample_fmt() { return input.get_sample_fmt(); }
    virtual uint get_sample_rate() { return input.get_sample_rate(); }
    virtual pos_t get_duration() { return input.get_duration(); }
    virtual metadata_t get_metadata() { return input.get_metadata(); }
    virtual ~Filter() { };
};

#endif

