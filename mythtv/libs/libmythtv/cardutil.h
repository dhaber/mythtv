// -*- Mode: c++ -*-
#ifndef _CARDUTIL_H_
#define _CARDUTIL_H_

// ANSI C
#include <stdint.h>

// C++ headers
#include <vector>
using namespace std;

// Qt headers
#include <QStringList>
#include <QMap>

// MythTV headers
#include "settings.h"
#include "mythexp.h"

class InputInfo;
class CardInput;
typedef QMap<int,QString> InputNames;

MPUBLIC QString get_on_cardid(const QString&, uint);

bool set_on_source(const QString&, uint, uint, const QString);

typedef enum
{
    DVB_DEV_FRONTEND = 1,
    DVB_DEV_DVR,
    DVB_DEV_DEMUX,
    DVB_DEV_CA,
    DVB_DEV_AUDIO,
    DVB_DEV_VIDEO,
} dvb_dev_type_t;

/** \class CardUtil
 *  \brief Collection of helper utilities for capture card DB use
 */
class MPUBLIC CardUtil
{
  public:
    /// \brief all the different capture cards
    enum CARD_TYPES
    {
        ERROR_OPEN    = 0,
        ERROR_UNKNOWN = 1,
        ERROR_PROBE   = 2,
        QPSK      = 3,        DVBS      = 3,
        QAM       = 4,        DVBC      = 4,
        OFDM      = 5,        DVBT      = 5,
        ATSC      = 6,
        V4L       = 7,
        MPEG      = 8,
        FIREWIRE  = 9,
        HDHOMERUN = 10,
        FREEBOX   = 11,
        HDPVR     = 12,
        DVBS2     = 13,
        IMPORT    = 14,
        DEMO      = 15,
        ASI       = 16,
        OCUR      = 17,
    };

    static enum CARD_TYPES toCardType(const QString &name)
    {
        if ("ERROR_OPEN" == name)
            return ERROR_OPEN;
        if ("ERROR_UNKNOWN" == name)
            return ERROR_UNKNOWN;
        if ("ERROR_PROBE" == name)
            return ERROR_PROBE;
        if ("QPSK" == name)
            return QPSK;
        if ("QAM" == name)
            return QAM;
        if ("OFDM" == name)
            return OFDM;
        if ("ATSC" == name)
            return ATSC;
        if ("V4L" == name)
            return V4L;
        if ("MPEG" == name)
            return MPEG;
        if ("FIREWIRE" == name)
            return FIREWIRE;
        if ("HDHOMERUN" == name)
            return HDHOMERUN;
        if ("FREEBOX" == name)
            return FREEBOX;
        if ("HDPVR" == name)
            return HDPVR;
        if ("DVB_S2" == name)
            return DVBS2;
        if ("IMPORT" == name)
            return IMPORT;
        if ("DEMO" == name)
            return DEMO;
        if ("ASI" == name)
            return ASI;
        if ("OCUR" == name)
            return ASI;
        return ERROR_UNKNOWN;
    }

    static bool         IsEncoder(const QString &rawtype)
    {
        return
            (rawtype != "DVB")       && (rawtype != "FIREWIRE") &&
            (rawtype != "HDHOMERUN") && (rawtype != "FREEBOX")  &&
            (rawtype != "IMPORT")    && (rawtype != "DEMO")     &&
            (rawtype != "ASI")       && (rawtype != "OCUR");
    }

    static bool         IsV4L(const QString &rawtype)
    {
        return (rawtype == "V4L" || rawtype == "MPEG");
    }

    static bool         IsChannelChangeDiscontinuous(const QString &rawtype)
    {
        return !IsEncoder(rawtype) || (rawtype == "HDPVR");
    }

    static bool         IsUnscanable(const QString &rawtype)
    {
        return
            (rawtype == "FIREWIRE")  || (rawtype == "HDPVR") ||
            (rawtype == "IMPORT")    || (rawtype == "DEMO")  ||
            (rawtype == "ASI");
    }

    static bool         IsEITCapable(const QString &rawtype)
    {
        return
            (rawtype == "DVB")       || (rawtype == "HDHOMERUN");
    }

    static bool         IsTunerSharingCapable(const QString &rawtype)
    {
        return
            (rawtype == "DVB")       || (rawtype == "HDHOMERUN") ||
            (rawtype == "ASI")       || (rawtype == "OCUR");
    }

    static bool         IsTunerShared(uint cardidA, uint cardidB);

    static bool         IsTuningDigital(const QString &rawtype)
    {
        return
            (rawtype == "DVB")       || (rawtype == "HDHOMERUN") ||
            (rawtype == "ASI");
    }

    static bool         IsTuningAnalog(const QString &rawtype)
    {
        return
            (rawtype == "V4L")       || (rawtype == "MPEG");
    }

    static bool         IsTuningVirtual(const QString &rawtype)
    {
        return
            (rawtype == "FIREWIRE")  || (rawtype == "HDPVR")     ||
            (rawtype == "OCUR");
    }

    static bool         IsSingleInputCard(const QString &rawtype)
    {
        return
            (rawtype == "FIREWIRE")  || (rawtype == "HDHOMERUN") ||
            (rawtype == "FREEBOX")   || (rawtype == "ASI")       ||
            (rawtype == "IMPORT")    || (rawtype == "DEMO")      ||
            (rawtype == "OCUR");
    }

    /// Convenience function for GetCardIDs()
    static uint         GetFirstCardID(const QString &videodevice)
    {
        vector<uint> list = GetCardIDs(videodevice);
        if (list.empty())
            return 0;
        return list[0];
    }

    static vector<uint> GetCardIDs(QString videodevice = QString::null,
                                   QString rawtype     = QString::null,
                                   QString hostname    = QString::null);

    static bool         IsCardTypePresent(const QString &rawtype,
                                          QString hostname = QString::null);
    static QStringList  GetCardTypes(void); // card types on ALL hosts

    static QStringList  GetVideoDevices(const QString &rawtype,
                                        QString hostname = QString::null);

    static QString      GetRawCardType(uint cardid)
        { return get_on_cardid("cardtype", cardid).toUpper(); }
    static QString      GetVideoDevice(uint cardid)
        { return get_on_cardid("videodevice", cardid); }
    static QString      GetAudioDevice(uint cardid)
        { return get_on_cardid("audiodevice", cardid); }
    static QString      GetVBIDevice(uint cardid)
        { return get_on_cardid("vbidevice", cardid); }
    static uint         GetHDHRTuner(uint cardid)
        { return get_on_cardid("dbox2_port", cardid).toUInt(); }

    static int          GetValueInt(const QString &col, uint cid)
        { return get_on_cardid(col, cid).toInt(); }
    static bool         SetValue(const QString &col, uint cid,
                                 uint sid, int val)
        { return set_on_source(col, cid, sid, QString::number(val)); }
    static bool         SetValue(const QString &col, uint cid,
                                 uint sid, const QString &val)
        { return set_on_source(col, cid, sid, val); }

    static int          GetCardInputID(uint cardid, const QString &channum,
                                       QString &inputname);
    static bool         SetStartChannel(uint cardinputid,
                                        const QString &channum);
    static bool         SetStartInput(uint cardid,
                                      const QString &inputname);

    // Inputs
    static vector<uint> GetCardIDs(uint sourceid);
    static QString      GetDefaultInput(uint cardid);
    static QStringList  GetInputNames(uint cardid, uint sourceid = 0);
    static bool         GetInputInfo(InputInfo &info,
                                     vector<uint> *groupids = NULL);
    static uint         GetCardID(uint inputid);
    static QString      GetInputName(uint inputid);
    static QString      GetStartingChannel(uint inputid);
    static QString      GetDisplayName(uint inputid);
    static QString      GetDisplayName(uint cardid, const QString &inputname)
        { return GetDisplayName(GetInputID(cardid, inputname)); }
    static vector<uint> GetInputIDs(uint cardid);
    static uint         GetInputID(uint cardid, const QString &inputname);
    static uint         GetSourceID(uint inputid);
    static bool         DeleteInput(uint inputid);
    static bool         DeleteOrphanInputs(void);

    // Input Groups
    static uint         CreateInputGroup(const QString &name);
    static bool         CreateInputGroupIfNeeded(uint cardid);
    static bool         LinkInputGroup(uint inputid, uint inputgroupid);
    static bool         UnlinkInputGroup(uint inputid, uint inputgroupid);
    static vector<uint> GetInputGroups(uint inputid);
    static vector<uint> GetSharedInputGroups(uint cardid);
    static vector<uint> GetGroupCardIDs(uint inputgroupid);
    static vector<uint> GetConflictingCards(uint inputid, uint exclude_cardid);

    static QString      GetDeviceLabel(const QString &cardtype,
                                       const QString &videodevice);
    static QString      GetDeviceLabel(uint cardid);

    static QString      ProbeSubTypeName(uint cardid);

    static QStringList  ProbeVideoInputs(QString device,
                                         QString cardtype = QString::null);
    static QStringList  ProbeAudioInputs(QString device,
                                         QString cardtype = QString::null);
    static void         GetCardInputs(uint                cardid,
                                      const QString      &device,
                                      const QString      &cardtype,
                                      QStringList        &inputLabels,
                                      vector<CardInput*> &cardInputs);

    static bool         DeleteCard(uint cardid);
    static bool         DeleteAllCards(void);
    static vector<uint> GetCardList(void);

    // General info from OS
    static QStringList  ProbeVideoDevices(const QString &rawtype);

    // Other
    static bool         CloneCard(uint src_cardid, uint dst_cardid);
    static vector<uint> GetCloneCardIDs(uint cardid);

    // DTV info
    static bool         GetTimeouts(uint cardid,
                                    uint &signal_timeout,
                                    uint &channel_timeout);
    static bool         IgnoreEncrypted(uint cardid, const QString &inputname);
    static bool         TVOnly(uint cardid, const QString &inputname);
    static bool         IsInNeedOfExternalInputConf(uint cardid);
    static uint         GetQuickTuning(uint cardid, const QString &inputname);

    // DVB info
    /// \brief Returns true if the card is a DVB card
    static bool         IsDVB(uint cardid)
        { return "DVB" == GetRawCardType(cardid); }
    static bool         IsDVBCardType(const QString card_type);
    static QString      ProbeDVBFrontendName(const QString &device);
    static QString      ProbeDVBType(const QString &device);
    static bool         HasDVBCRCBug(const QString &device);
    static uint         GetMinSignalMonitoringDelay(const QString &device);
    static QString      GetDeviceName(dvb_dev_type_t, const QString &device);
    static InputNames   GetConfiguredDVBInputs(uint cardid);

    // V4L info
    static bool         hasV4L2(int videofd);
    static bool         GetV4LInfo(int videofd, QString &card, QString &driver,
                                   uint32_t &version);
    static bool         GetV4LInfo(int videofd, QString &card, QString &driver)
        { uint32_t dummy; return GetV4LInfo(videofd, card, driver, dummy); }
    static InputNames   ProbeV4LVideoInputs(int videofd, bool &ok);
    static InputNames   ProbeV4LAudioInputs(int videofd, bool &ok);

    // HDHomeRun info
    static bool         HDHRdoesDVB(const QString &device);
    static QString      GetHDHRdesc(const QString &device);

    // ASI info
    static int          GetASIDeviceNumber(const QString &device,
                                           QString *error = NULL);

    static uint         GetASIBufferSize(uint device_num,
                                         QString *error = NULL);

  private:
    static QStringList  ProbeV4LVideoInputs(QString device);
    static QStringList  ProbeV4LAudioInputs(QString device);
    static QStringList  ProbeDVBInputs(QString device);
};

#endif //_CARDUTIL_H_
