// -*- Mode: c++ -*-
#ifndef AUDIOPROPS_GENERATOR_H_
#define AUDIOPROPS_GENERATOR_H_

#include <pthread.h>

#include <QMutex>
#include <QString>

#include "programinfo.h"

class MPUBLIC AudioPropsGenerator : public QObject
{
    Q_OBJECT

  public:
    AudioPropsGenerator(const ProgramInfo *pginfo);

    void Start(void);
    bool Run(void);

  signals:
    void audioPropsThreadDone(const ProgramInfo*);
    void audioPropsReady(const ProgramInfo*);

  protected:
    virtual ~AudioPropsGenerator();
    bool UpdateAudioProps(void);
    void disconnectSafe(void);
    void AttachSignals(QObject *);
    static void *AudioPropsRun(void*);

  protected:
    QMutex             audioPropsLock;
    pthread_t          audioPropsThread;
    ProgramInfo        programInfo;
};

#endif //AUDIOPROPS_GENERATOR_H_
