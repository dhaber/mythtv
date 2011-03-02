#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <stdint.h>

class MythPlayer;
class AudioOutput;

class MTV_PUBLIC AudioPlayer
{
  public:
    AudioPlayer(MythPlayer *parent, bool muted);
   ~AudioPlayer();

    void  Reset(void);
    void  DeleteOutput(void);
    QString ReinitAudio(void);
    void  SetAudioOutput(AudioOutput *ao);
    void  SetAudioInfo(const QString &main_device,
                       const QString &passthru_device,
                       uint           samplerate,
                       int            bitrate = -1);
    void  SetAudioParams(AudioFormat format, int orig_channels, int channels,
                         int codec, int samplerate, bool passthru,
                         int bitrate = -1);
    void  SetEffDsp(int dsprate);

    void  CheckFormat(void);
    void  SetNoAudio(void)        { m_no_audio_out = true;  }
    bool  HasAudioIn(void) const  { return !m_no_audio_in;  }
    bool  HasAudioOut(void) const { return !m_no_audio_out; }

    bool  Pause(bool pause);
    bool  IsPaused(void);
    void  PauseAudioUntilBuffered(void);
    int   GetCodec(void)        { return m_codec;         }
    int   GetNumChannels(void)  { return m_channels;      }
    int   GetOrigChannels(void) { return m_orig_channels; }
    uint  GetVolume(void);
    uint  AdjustVolume(int change);
    float GetStretchFactor(void) { return m_stretchfactor;   }
    void  SetStretchFactor(float factor);
    bool  ToggleUpmix(void);
    bool  CanPassthrough(int samplerate, int channels, int codec = 0);
    bool  CanAC3(void);
    bool  CanDTS(void);
    bool  CanEAC3(void);
    bool  CanTrueHD(void);
    bool  CanDTSHD(void);
    uint  GetMaxChannels(void);
    int   GetMaxHDRate(void);
    int64_t GetAudioTime(void);

    bool      IsMuted(void) { return GetMuteState() == kMuteAll; }
    bool      SetMuted(bool mute);
    MuteState GetMuteState(void);
    MuteState SetMuteState(MuteState);
    MuteState IncrMuteState(void);

    void AddAudioData(char *buffer, int len, int64_t timecode);
    int64_t LengthLastData(void);
    bool GetBufferStatus(uint &fill, uint &total);
    bool IsBufferAlmostFull(void);

  private:
    MythPlayer  *m_parent;
    AudioOutput *m_audioOutput;
    int          m_channels;
    int          m_orig_channels;
    int          m_codec;
    AudioFormat  m_format;
    int          m_samplerate;
    int          m_codec_profile;
    float        m_stretchfactor;
    bool         m_passthru;
    QMutex       m_lock;
    bool         m_muted_on_creation;
    QString      m_main_device;
    QString      m_passthru_device;
    bool         m_no_audio_in;
    bool         m_no_audio_out;
};

#endif // AUDIOPLAYER_H
