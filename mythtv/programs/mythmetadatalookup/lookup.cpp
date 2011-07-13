#include <vector>

#include <QList>

#include "programinfo.h"
#include "recordingrule.h"
#include "mythlogging.h"
#include "jobqueue.h"
#include "remoteutil.h"

#include "metadataimagehelper.h"

#include "lookup.h"

LookerUpper::LookerUpper() :
    m_busyRecList(QList<ProgramInfo*>()),
    m_updaterules(false), m_updateartwork(false)
{
    m_metadataFactory = new MetadataFactory(this);
}

LookerUpper::~LookerUpper()
{
}

bool LookerUpper::StillWorking()
{
    if (m_metadataFactory->IsRunning() ||
        m_busyRecList.count())
    {
        return true;
    }

    return false;
}

void LookerUpper::HandleSingleRecording(const uint chanid,
                                        const QDateTime starttime,
                                        bool updaterules)
{
    ProgramInfo *pginfo = new ProgramInfo(chanid, starttime);

    if (!pginfo)
    {
        LOG(VB_GENERAL, LOG_ERR,
            "No valid program info for supplied chanid/starttime");
        return;
    }

    m_updaterules = updaterules;

    m_busyRecList.append(pginfo);
    m_metadataFactory->Lookup(pginfo, false, false);
}

void LookerUpper::HandleAllRecordings(bool updaterules)
{
    QMap< QString, ProgramInfo* > recMap;
    QMap< QString, uint32_t > inUseMap = ProgramInfo::QueryInUseMap();
    QMap< QString, bool > isJobRunning = ProgramInfo::QueryJobsRunning(JOB_COMMFLAG);

    m_updaterules = updaterules;

    ProgramList progList;

    LoadFromRecorded( progList, false, inUseMap, isJobRunning, recMap, -1 );

    for( int n = 0; n < (int)progList.size(); n++)
    {
        ProgramInfo *pginfo = new ProgramInfo(*(progList[n]));
        if (pginfo->GetInetRef().isEmpty() ||
            (!pginfo->GetSubtitle().isEmpty() &&
              (pginfo->GetSeason() == 0) &&
              (pginfo->GetEpisode() == 0)))
        {
            QString msg = QString("Looking up: %1 %2").arg(pginfo->GetTitle())
                                           .arg(pginfo->GetSubtitle());
            LOG(VB_GENERAL, LOG_INFO, msg);

            m_busyRecList.append(pginfo);
            m_metadataFactory->Lookup(pginfo, false, false);
        }
    }
}

void LookerUpper::HandleAllRecordingRules()
{
    m_updaterules = true;

    vector<ProgramInfo *> recordingList;

    RemoteGetAllScheduledRecordings(recordingList);

    for( int n = 0; n < (int)recordingList.size(); n++)
    {
        ProgramInfo *pginfo = new ProgramInfo(*(recordingList[n]));
        if (pginfo->GetInetRef().isEmpty())
        {
            QString msg = QString("Looking up: %1 %2").arg(pginfo->GetTitle())
                                           .arg(pginfo->GetSubtitle());
            LOG(VB_GENERAL, LOG_INFO, msg);

            m_busyRecList.append(pginfo);
            m_metadataFactory->Lookup(pginfo, false, false);
        }
    }
}

void LookerUpper::HandleAllArtwork(bool aggressive)
{
    m_updateartwork = true;

    // First, handle all recording rules w/ inetrefs
    vector<ProgramInfo *> recordingList;

    RemoteGetAllScheduledRecordings(recordingList);

    for( int n = 0; n < (int)recordingList.size(); n++)
    {
        ProgramInfo *pginfo = new ProgramInfo(*(recordingList[n]));
        bool dolookup = true;
        if (!aggressive && pginfo->GetInetRef().isEmpty())
            dolookup = false;
        if (dolookup)
        {
            ArtworkMap map = GetArtwork(pginfo->GetInetRef(), pginfo->GetSeason(), true);
            if (map.isEmpty())
            {
                QString msg = QString("Looking up artwork for recording rule: %1 %2")
                                               .arg(pginfo->GetTitle())
                                               .arg(pginfo->GetSubtitle());
                LOG(VB_GENERAL, LOG_INFO, msg);

                m_busyRecList.append(pginfo);
                m_metadataFactory->Lookup(pginfo, false, true);
            }
        }
    }

    // Now, Attempt to fill in the gaps for recordings
    QMap< QString, ProgramInfo* > recMap;
    QMap< QString, uint32_t > inUseMap = ProgramInfo::QueryInUseMap();
    QMap< QString, bool > isJobRunning = ProgramInfo::QueryJobsRunning(JOB_COMMFLAG);

    ProgramList progList;

    LoadFromRecorded( progList, false, inUseMap, isJobRunning, recMap, -1 );

    for( int n = 0; n < (int)progList.size(); n++)
    {
        ProgramInfo *pginfo = new ProgramInfo(*(progList[n]));
        bool dolookup = true;

        RecordingRule *rule = new RecordingRule();
        rule->LoadByProgram(pginfo);
        int ruleepisode = 0;
        if (rule && rule->Load())
            ruleepisode = rule->m_episode;
        delete rule;

        if ((!aggressive && pginfo->GetInetRef().isEmpty()) ||
             pginfo->GetRecordingGroup() == "Deleted" ||
             pginfo->GetRecordingGroup() == "LiveTV" ||
             (!aggressive && ruleepisode > 0 && pginfo->GetEpisode() == 0))
            dolookup = false;
        if (dolookup)
        {
            ArtworkMap map = GetArtwork(pginfo->GetInetRef(), pginfo->GetSeason(), true);
            if (map.isEmpty())
            {
               QString msg = QString("Looking up artwork for recording: %1 %2")
                                           .arg(pginfo->GetTitle())
                                           .arg(pginfo->GetSubtitle());
                LOG(VB_GENERAL, LOG_INFO, msg);

                m_busyRecList.append(pginfo);
                m_metadataFactory->Lookup(pginfo, false, true);
            }
        }
    }

}

void LookerUpper::CopyRuleInetrefsToRecordings()
{
    QMap< QString, ProgramInfo* > recMap;
    QMap< QString, uint32_t > inUseMap = ProgramInfo::QueryInUseMap();
    QMap< QString, bool > isJobRunning = ProgramInfo::QueryJobsRunning(JOB_COMMFLAG);

    ProgramList progList;

    LoadFromRecorded( progList, false, inUseMap, isJobRunning, recMap, -1 );

    for( int n = 0; n < (int)progList.size(); n++)
    {
        ProgramInfo *pginfo = new ProgramInfo(*(progList[n]));
        if (pginfo && pginfo->GetInetRef().isEmpty())
        {
            RecordingRule *rule = new RecordingRule();
            rule->LoadByProgram(pginfo);
            if (rule && rule->Load() && !rule->m_inetref.isEmpty())
            {
                QString msg = QString("%1").arg(pginfo->GetTitle());
                if (!pginfo->GetSubtitle().isEmpty())
                    msg += QString(": %1").arg(pginfo->GetSubtitle());
                msg += " has no inetref, but its recording rule does. Copying...";
                LOG(VB_GENERAL, LOG_INFO, msg);
                pginfo->SaveInetRef(rule->m_inetref);
                delete rule;
            }
            delete pginfo;
        }
    }
}

void LookerUpper::customEvent(QEvent *levent)
{
    if (levent->type() == MetadataFactoryMultiResult::kEventType)
    {
        LOG(VB_GENERAL, LOG_INFO, "Unable to match this title, too many possible matches. "
                                  "You may wish to manually set the season, episode, and "
                                  "inetref in the 'Watch Recordings' screen.");

        MetadataFactoryMultiResult *mfmr = dynamic_cast<MetadataFactoryMultiResult*>(levent);

        if (!mfmr)
            return;

        MetadataLookupList list = mfmr->results;

        if (list.count() >= 1)
        {
            ProgramInfo *pginfo = qVariantValue<ProgramInfo *>(list.takeFirst()->GetData());

            if (pginfo)
                m_busyRecList.removeAll(pginfo);
        }
    }
    else if (levent->type() == MetadataFactorySingleResult::kEventType)
    {
        MetadataFactorySingleResult *mfsr =
            dynamic_cast<MetadataFactorySingleResult*>(levent);

        if (!mfsr)
            return;

        MetadataLookup *lookup = mfsr->result;

        if (!lookup)
            return;

        ProgramInfo *pginfo = qVariantValue<ProgramInfo *>(lookup->GetData());

        // This null check could hang us as this pginfo would then never be
        // removed
        if (!pginfo)
            return;

        LOG(VB_GENERAL, LOG_DEBUG, "I found the following data:");
        LOG(VB_GENERAL, LOG_DEBUG,
            QString("        Input Title: %1").arg(pginfo->GetTitle()));
        LOG(VB_GENERAL, LOG_DEBUG,
            QString("        Input Sub:   %1").arg(pginfo->GetSubtitle()));
        LOG(VB_GENERAL, LOG_DEBUG,
            QString("        Title:       %1").arg(lookup->GetTitle()));
        LOG(VB_GENERAL, LOG_DEBUG,
            QString("        Subtitle:    %1").arg(lookup->GetSubtitle()));
        LOG(VB_GENERAL, LOG_DEBUG,
            QString("        Season:      %1").arg(lookup->GetSeason()));
        LOG(VB_GENERAL, LOG_DEBUG,
            QString("        Episode:     %1").arg(lookup->GetEpisode()));
        LOG(VB_GENERAL, LOG_DEBUG,
            QString("        Inetref:     %1").arg(lookup->GetInetref()));
        LOG(VB_GENERAL, LOG_DEBUG,
            QString("        User Rating: %1").arg(lookup->GetUserRating()));

        pginfo->SaveSeasonEpisode(lookup->GetSeason(), lookup->GetEpisode());
        pginfo->SaveInetRef(lookup->GetInetref());

        if (m_updaterules)
        {
            RecordingRule *rule = new RecordingRule();
            rule->LoadByProgram(pginfo);
            rule->m_inetref = lookup->GetInetref();
            rule->Save();

            delete rule;
        }

        if (m_updateartwork)
        {
            ArtworkMap map = lookup->GetDownloads();
            SetArtwork(lookup->GetInetref(), lookup->GetSeason(),
                       gCoreContext->GetMasterHostName(), map);
        }

        m_busyRecList.removeAll(pginfo);
    }
    else if (levent->type() == MetadataFactoryNoResult::kEventType)
    {
        MetadataFactoryNoResult *mfnr = dynamic_cast<MetadataFactoryNoResult*>(levent);

        if (!mfnr)
            return;

        MetadataLookup *lookup = mfnr->result;

        if (!lookup)
            return;

        ProgramInfo *pginfo = qVariantValue<ProgramInfo *>(lookup->GetData());

        // This null check could hang us as this pginfo would then never be removed
        if (!pginfo)
            return;

        m_busyRecList.removeAll(pginfo);
    }
}
