// MythTV headers
#include "ringbuffer.h"
#include "mythplayer.h"
#include "audiopropsgenerator.h"
#include "tv_rec.h"
#include "playercontext.h"
#include "mythlogging.h"
#include "programtypes.h"

#define LOC QString("Audio Props: ")

/** \class AudioPropsGenerator
 *  \brief This class updates the Audio Properties of a recording.
 *
 *   The usage is simple: First, pass a ProgramInfo whose pathname points
 *   to a local or remote recording to the constructor. Then call either
 *   Start(void) or Run(void) to update the audio props.
 *
 *   Start(void) will create a thread that processes the request.
 *
 *   Run(void) will process the request in the current thread, 
 *
 *   The AudioPropsGenerator will send Qt signals when the update is ready
 *   and when the thread finishes running if Start(void) was called.
 */

/**
 *  \brief Constructor
 *
 *  \param pginfo     ProgramInfo for the reording we want to generate audio props for
 */
AudioPropsGenerator::AudioPropsGenerator(const ProgramInfo *pginfo)
    : programInfo(*pginfo)
{
    LOG(VB_AUDIO, LOG_DEBUG, QString("Constructing Audio Props Generator"));
}

AudioPropsGenerator::~AudioPropsGenerator()
{
    LOG(VB_AUDIO, LOG_DEBUG, QString("Deconstructing Audio Props Generator"));

    audioPropsLock.lock();
    emit audioPropsThreadDone(&programInfo);
    audioPropsLock.unlock();
    disconnectSafe();
}

void AudioPropsGenerator::AttachSignals(QObject *obj)
{
    QMutexLocker locker(&audioPropsLock);
    qRegisterMetaType<bool>("bool &");
    connect(this, SIGNAL(audioPropsThreadDone(const QString&,bool&)),
            obj,  SLOT(  audioPropsThreadDone(const QString&,bool&)),
            Qt::DirectConnection);
    connect(this, SIGNAL(audioPropsReady(const ProgramInfo*)),
            obj,  SLOT(  audioPropsReady(const ProgramInfo*)),
            Qt::DirectConnection);
}

/** \fn AudioProps::disconnectSafe(void)
 *  \brief disconnects signals while holding audioPropsLock, ensuring that
 *         no one will receive a signal from this class after this call.
 */
void AudioPropsGenerator::disconnectSafe(void)
{
    QMutexLocker locker(&audioPropsLock);
    QObject::disconnect(this, NULL, NULL, NULL);
}

/** \fn AudioProps::Start(void)
 *  \brief This call starts a thread that will update the audio props
 */
void AudioPropsGenerator::Start(void)
{
    pthread_create(&audioPropsThread, NULL, AudioPropsRun, this);
    // detach, so we don't have to join thread to free thread local mem.
    pthread_detach(audioPropsThread);
}

bool AudioPropsGenerator::Run(void)
{
    bool ok = UpdateAudioProps();

    if (ok)
    {
        QMutexLocker locker(&audioPropsLock);
        emit audioPropsReady(&programInfo);
    }

    return ok;
}

void *AudioPropsGenerator::AudioPropsRun(void *param)
{
    // Lower scheduling priority, to avoid problems with recordings.
    if (setpriority(PRIO_PROCESS, 0, 9))
        LOG(VB_GENERAL, LOG_ERR, "Setting priority failed." + ENO);
    AudioPropsGenerator *gen = (AudioPropsGenerator*) param;
    gen->Run();
    gen->deleteLater();
    return NULL;
}

bool AudioPropsGenerator::UpdateAudioProps(void)
{

    LOG(VB_AUDIO, LOG_DEBUG, QString("Starting Audio Inspection"));
    PlayerContext *ctx = new PlayerContext(kAudioGeneratorInUseID);

    LOG(VB_AUDIO, LOG_DEBUG, QString("Using file: %1").arg(programInfo.GetPathname()));
    RingBuffer *rbuf = RingBuffer::Create(programInfo.GetPathname(), false, false, 0);

    LOG(VB_AUDIO, LOG_DEBUG, QString("Setting player"));
    ctx->SetRingBuffer(rbuf);
    ctx->SetPlayingInfo(&programInfo);
    ctx->SetPlayer(new MythPlayer((PlayerFlags)(kVideoIsNull)));
    ctx->player->SetPlayerInfo(NULL, NULL, ctx);


    LOG(VB_AUDIO, LOG_DEBUG, QString("Getting audio props"));
    int audioprops  = ctx->player->GetAudioProperties();

    LOG(VB_AUDIO, LOG_DEBUG, QString("Got audio props"));
    bool updated = audioprops >= 0;

    if (!updated)
        LOG(VB_GENERAL, LOG_DEBUG, QString("NOT Saving audio props for %1").arg(programInfo.GetTitle()));
    else
    {
        LOG(VB_GENERAL, LOG_DEBUG, QString("PSYCHE Saving audio props (%1) for %2").arg(audioprops).arg(programInfo.GetTitle()));
        //programInfo.SaveAudioProps(audioprops);
    }

    return updated;
}

/* vim: set expandtab tabstop=4 shiftwidth=4: */
