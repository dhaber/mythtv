// -*- Mode: c++ -*-
// Copyright (c) 2003-2004, Daniel Thor Kristjansson
#ifndef _MPEG_TABLES_H_
#define _MPEG_TABLES_H_

#include <cassert>
#include "pespacket.h"
#include "mpegdescriptors.h"

/** \file mpegtables.h
 *  \code
 *   special ATSC restrictions on PMT,PAT tables
 *   max 400ms,100ms between PMTs,PATs, resp.
 *   no adaptation headers in PMT,PAT or PSIP packets
 *     (?unless version number is discontinuous?)
 *   PES Payload must not be scrambled (1999 Sarnoff)
 *   No Clock in PES, no MPEG-1 System Fields, no CRC or priv. data
 *   no more than one frame per PES packet
 *   program 0 is network information table in DVB streams
 *  \endcode
 */

/** \class PESStreamID
 *  \brief Contains a listing of PES Stream ID's for various PES Packet types.
 */
class PESStreamID
{
  public:
    enum
    {
        PictureStartCode        = 0x00,
        SliceStartCodeBegin     = 0x01,
        SliceStartCodeEnd       = 0xaf,
        DVBECMData              = 0xb0,
        DVBEMMData              = 0xb1,
        UserData                = 0xb2,
        /// Sequence (SEQ) start code contains frame size, aspect ratio and fps
        SequenceStartCode       = 0xb3,
        SequenceError           = 0xb4,
        /// Followed by an extension byte, not documented here
        MPEG2ExtensionStartCode = 0xb5,
        MPEGReservedB6          = 0xb6,
        SEQEndCode              = 0xb7,
        /// Group of Pictures (GOP) start code. Tells us how to
        /// reorder frames from transmitted order to display order.
        /// Required in MPEG-1, but optional in MPEG-2.
        GOPStartCode            = 0xb8,
        ProgramEndCode          = 0xb9,
        PackHeader              = 0xba,
        SystemHeader            = 0xbb,
        ProgramStreamMap        = 0xbc,
        /// Non-MPEG audio & subpictures (w/ext hdr)
        NonMPEGAudioVideo       = 0xbd,
        PaddingStream           = 0xbe,
        /// DVD navigation data
        DVDNavigation           = 0xbf,
        /// First MPEG-1/2 audio stream (w/ext hdr)
        MPEGAudioStreamBegin    = 0xc0,
        /// Last MPEG-1/2 audio stream (w/ext hdr)
        MPEGAudioStreamEnd      = 0xdf,
        /// First MPEG-1/2 video stream (w/ext hdr)
        MPEGVideoStreamBegin    = 0xe0,
        /// Last MPEG-1/2 video stream (w/ext hdr)
        MPEGVideoStreamEnd      = 0xef,
        ECMData                 = 0xf0,
        EMMData                 = 0xf1,
        DSMCCData               = 0xf2,
        /// Data as defined in ISO and IEC standard number 13522
        Data13522               = 0xf3,
        /// First Data as defined in ITU-T recomendation H.222.1
        DataH2221Begin          = 0xf4,
        /// Last Data as defined in ITU-T recomendation H.222.1
        DataH2221End            = 0xf8,
        AncillaryData           = 0xf9,
        MPEGReservedFA          = 0xfa,
        FlexMux                 = 0xfb, // MPEG-2 13818-1
        MPEGReservedFC          = 0xfc,
        MPEGReservedFD          = 0xfd,
        MPEGReservedFE          = 0xfe,
        ProgramStreamDirectory  = 0xff, // MPEG-2 13818-1
    };
};


/** \class StreamID
 *  \brief Contains listing of PMT Stream ID's for various A/V Stream types.
 *
 *   This is used by the Program Map Table (PMT), and often used by other
 *   tables, such as the DVB SDT table.
 */
class StreamID
{
  public:
    enum
    {
        // video
        MPEG1Video     = 0x01, ///< ISO 11172-2 (aka MPEG-1)
        MPEG2Video     = 0x02, ///< ISO 13818-2 & ITU H.262 (aka MPEG-2)
        MPEG4Video     = 0x10, ///< ISO 14492-2 (aka MPEG-4)
        H264Video      = 0x1b, ///< ISO 14492-10 & ITU H.264 (aka MPEG-4-AVC)
        OpenCableVideo = 0x80,
        VC1Video       = 0xea, ///< SMPTE 421M video codec (aka VC1) in Blu-Ray

        // audio
        MPEG1Audio     = 0x03, ///< ISO 11172-3
        MPEG2Audio     = 0x04, ///< ISO 13818-3
        MPEG2AACAudio  = 0x0f, ///< ISO 13818-7 Audio w/ADTS syntax
        MPEG2AudioAmd1 = 0x11, ///< ISO 13818-3/AMD-1 Audio using LATM syntax
        AC3Audio       = 0x81,
        DTSAudio       = 0x8a,

        // DSM-CC Object Carousel
        DSMCC          = 0x08, ///< ISO 13818-1 Annex A DSM-CC & ITU H.222.0
        DSMCC_A        = 0x0a, ///< ISO 13818-6 type A Multi-protocol Encap
        DSMCC_B        = 0x0b, ///< ISO 13818-6 type B Std DSMCC Data
        DSMCC_C        = 0x0c, ///< ISO 13818-6 type C NPT DSMCC Data
        DSMCC_D        = 0x0d, ///< ISO 13818-6 type D Any DSMCC Data
        DSMCC_DL       = 0x14, ///< ISO 13818-6 Download Protocol
        MetaDataPES    = 0x15, ///< Meta data in PES packets
        MetaDataSec    = 0x16, ///< Meta data in metadata_section's
        MetaDataDC     = 0x17, ///< ISO 13818-6 Metadata in Data Carousel
        MetaDataOC     = 0x18, ///< ISO 13818-6 Metadata in Object Carousel
        MetaDataDL     = 0x19, ///< ISO 13818-6 Metadata in Download Protocol

        // other
        PrivSec        = 0x05, ///< ISO 13818-1 private tables   & ITU H.222.0
        PrivData       = 0x06, ///< ISO 13818-1 PES private data & ITU H.222.0

        MHEG           = 0x07, ///< ISO 13522 MHEG
        H222_1         = 0x09, ///< ITU H.222.1

        MPEG2Aux       = 0x0e, ///< ISO 13818-1 auxiliary & ITU H.222.0

        FlexMuxPES     = 0x12, ///< ISO 14496-1 SL/FlexMux in PES packets
        FlexMuxSec     = 0x13, ///< ISO 14496-1 SL/FlexMux in 14496_sections

        MPEG2IPMP      = 0x1a, ///< ISO 13818-10 Digital Restrictions Mangment
        MPEG2IPMP2     = 0x7f, ///< ISO 13818-10 Digital Restrictions Mangment

        // special id's, not actually ID's but can be used in FindPIDs
        AnyMask        = 0xFFFF0000,
        AnyVideo       = 0xFFFF0001,
        AnyAudio       = 0xFFFF0002,
    };
    /// Returns true iff video is an MPEG1/2/3, H264 or open cable video stream.
    static bool IsVideo(uint type)
    {
        return ((StreamID::MPEG1Video == type) ||
                (StreamID::MPEG2Video == type) ||
                (StreamID::MPEG4Video == type) ||
                (StreamID::H264Video  == type) ||
                (StreamID::VC1Video   == type) ||
                (StreamID::OpenCableVideo == type));
    }
    /// Returns true iff audio is MPEG1/2, AAC, AC3 or DTS audio stream.
    static bool IsAudio(uint type)
    {
        return ((StreamID::MPEG1Audio     == type) ||
                (StreamID::MPEG2Audio     == type) ||
                (StreamID::MPEG2AudioAmd1 == type) ||
                (StreamID::MPEG2AACAudio  == type) ||
                (StreamID::AC3Audio       == type) ||
                (StreamID::DTSAudio       == type));
    }
    /// Returns true iff stream contains DSMCC Object Carousel
    static bool IsObjectCarousel(uint type)
    {
        return ((StreamID::DSMCC_A == type) ||
                (StreamID::DSMCC_B == type) ||
                (StreamID::DSMCC_C == type) ||
                (StreamID::DSMCC_D == type));
    }
    static uint Normalize(uint stream_id, const desc_list_t &desc,
                          const QString &sistandard);
    static const char* toString(uint streamID);
    static QString GetDescription(uint streamID);
};

enum
{
    MPEG_PAT_PID  = 0x0000,
    MPEG_CAT_PID  = 0x0001,
    MPEG_TSDT_PID = 0x0002,

    DVB_NIT_PID   = 0x0010,
    DVB_SDT_PID   = 0x0011,
    DVB_EIT_PID   = 0x0012,
    DVB_RST_PID   = 0x0013,
    DVB_TDT_PID   = 0x0014,

    // Dishnet longterm EIT is on pid 0x300
    DVB_DNLONG_EIT_PID = 0x0300,

    // Bell longterm EIT is on pid 0x441
    DVB_BVLONG_EIT_PID = 0x0441,

    // Premiere EIT for Direkt/Sport PPV
    PREMIERE_EIT_DIREKT_PID = 0x0b11,
    PREMIERE_EIT_SPORT_PID  = 0x0b12,

    ATSC_PSIP_PID = 0x1ffb,

    // UK Freesat PIDs: SDTo/BAT, longterm EIT, shortterm EIT
    FREESAT_SI_PID     = 0x0f01,
    FREESAT_EIT_PID    = 0x0f02,
    FREESAT_ST_EIT_PID = 0x0f03,
};

/** \class TableID
 *  \brief Contains listing of Table ID's for various tables (PAT=0,PMT=2,etc).
 */
class TableID
{
  public:
    enum
    {
        PAT      = 0x00, // always on pid 0x00
        CAT      = 0x01, // always on pid 0x01
        PMT      = 0x02,
        TSDT     = 0x03, // always on pid 0x02

        // DVB manditory
        NIT      = 0x40, // always on pid 0x10
        SDT      = 0x42, // always on pid 0x11
        PF_EIT   = 0x4e, // always on pid 0x12
        TDT      = 0x70, // always on pid 0x14

        // DVB optional
        NITo     = 0x41, // always on pid 0x10
        SDTo     = 0x46, // always on pid 0x11
        BAT      = 0x4a, // always on pid 0x11
        PF_EITo  = 0x4f, // always on pid 0x12
        SC_EITbeg  = 0x50, // always on pid 0x12
        SC_EITend  = 0x5f, // always on pid 0x12
        SC_EITbego = 0x60, // always on pid 0x12
        SC_EITendo = 0x6f, // always on pid 0x12
        RST      = 0x71, // always on pid 0x13
        ST       = 0x72, // any pid 0x10-0x14
        TOT      = 0x73, // always on pid 0x14
        RNT      = 0x74, // always on pid 0x16
        CT       = 0x75,
        RCT      = 0x76,
        CIT      = 0x77, // always on pid 0x12
        MPEFEC   = 0x78,
        DIT      = 0x7e, // always on pid 0x1e
        SIT      = 0x7f, // always on pid 0x1f

        // DVB Conditional Access
        DVBCAbeg = 0x80,
        DVBCAend = 0x8f,

        // Dishnet Longterm EIT data
        DN_EITbego = 0x80, // always on pid 0x300
        DN_EITendo = 0xfe, // always on pid 0x300

        // ARIB
        ARIBbeg  = 0x80,
        ARIBend  = 0x8f,

        // SCTE
        PIM      = 0xC0, // Program Information Message (57 2003) PMT PID
        PNM      = 0xC1, // Program Name Message (57 2003) PMT PID
        NIM      = 0xC2, // Network Information Message (57 2003)
                         // on Network PID (per PAT)
        NITscte  = 0xC2, // Network Information Table (NIT) (65 2002) on 0x1FFC
        NTM      = 0xC3, // Network Text Message (57 2003) on 0x1FFC
                         // (terrestrial only, Map Name Table only) or
                         // Network PID (per PAT)
        NTT      = 0xC3, // Network Text Table (NTT) (65 2002) on 0x1FFC
        VCM      = 0xC4, // Virtual Channel Message (57 2003) on 0x1FFC
                         // (terrestrial only) or Network PID (per PAT)
        SVCTscte = 0xC4, // Short Virtual Channel Table (65 2002) on 0x1FFC
        STM      = 0xC5, // System Time Message (57 2003)
                         // on Network PID (per PAT)
        STTscte  = 0xC5, // System Time Table (STT) (65 2002) on 0x1FFC
        SM       = 0xC6, // subtitle_message (27 2003)
        MGTscte  = 0xC7, // Master Guide Table (57 2003) on 0x1ffc
        LVCT     = 0xC9, // Long-form Virtual Channel Table (57 2003) on 0x1ffc
        RRTscte  = 0xCA, // Region Rating Table (57 2003) on 0x1ffc

        CEA      = 0xD8, // Cable Emergency Alert (18 2002)
        ADET     = 0xD9, // Aggregate Data Event Table (80 2002)

        // ATSC Conditional Access (A/70)
        ECM0     = 0x80,
        ECM1     = 0x81,
        ECMbeg   = 0x82, // ECM begin private data
        ECMend   = 0x8f, // ECM end private data

        // ATSC main
        MGT      = 0xC7, // Master Guide Table A/65 on 0x1ffb
        TVCT     = 0xC8, // Terrestrial Virtual Channel Table A/65 on 0x1ffb
        CVCT     = 0xC9, // Cable Virtual Channel Table A/65 on 0x1ffb
        RRT      = 0xCA, // Region Rating Table A/65 on 0x1ffb
        EIT      = 0xCB, // Event Information Table A/65 (per MGT)
        ETT      = 0xCC, // Extended Text Table A/65 (per MGT)
        STT      = 0xCD, // System Time Table A/65
        DET      = 0xCE, // Data Event Table A/90 (per MGT)
        DST      = 0xCF, // Data Service Table A/90

        PIT      = 0xD0, // Program ID Table ???
        NRT      = 0xD1, // Network Resources Table A/90
        LTST     = 0xD2, // Long Term Service Table A/90
        DCCT     = 0xD3, // Directed Channel Change Table A/57 on 0x1ffb
        DCCSCT   = 0xD4, // DCC Selection Code Table A/57 on 0x1ffb
        SITatsc  = 0xD5, // Selection Information Table (EIA-775.2 2000)
        AEIT     = 0xD6, // Aggregate Event Information Table A/81
        AETT     = 0xD7, // Aggregate Extended Text Table A/81
        SVCT     = 0xDA, // Satellite VCT A/81

        SRM      = 0xE0, // System Renewability Message (ATSC TSG-717r0)

        // Unknown
        STUFFING = 0x80,
        CAPTION  = 0x86,
        CENSOR   = 0x87,

        // Private premiere.de
        PREMIERE_CIT = 0xA0,
        PREMIERE_CPT = 0xA1,

        ECN      = 0xA0,
        SRVLOC   = 0xA1,
        TSS      = 0xA2,
        CMPNAME  = 0xA3,
    };
};

/** \class PSIPTable
 *  \brief A PSIP table is a special type of PES packet containing an
 *         MPEG, ATSC, or DVB table.
 */
class PSIPTable : public PESPacket
{
  private:
    // creates non-clone version, for View
    PSIPTable(const PESPacket& pkt, bool)
        : PESPacket(reinterpret_cast<const TSPacket*>(pkt.tsheader()), false)
        { ; }
  public:
    PSIPTable(const PSIPTable& table) : PESPacket(table)
    {
        // section_syntax_ind   1       1.0       8   should always be 1
        // private_indicator    1       1.1       9   should always be 1
    }
    PSIPTable(const PESPacket& table) : PESPacket(table)
    {
        // section_syntax_ind   1       1.0       8   should always be 1
        // private_indicator    1       1.1       9   should always be 1
    }
    PSIPTable(const TSPacket& table) : PESPacket(table)
    {
        // section_syntax_ind   1       1.0       8   should always be 1
        // private_indicator    1       1.1       9   should always be 1
    }


    static const PSIPTable View(const TSPacket& tspacket)
        { return PSIPTable(PESPacket::View(tspacket), false); }

    static PSIPTable View(TSPacket& tspacket)
        { return PSIPTable(PESPacket::View(tspacket), false); }

    // Section            Bits   Start Byte sbit
    // -----------------------------------------
    // table_id             8       0.0       0
    uint TableID(void) const { return StreamID(); }

    // section_syntax_ind   1       1.0       8   should always be 1
    bool SectionSyntaxIndicator(void) const { return pesdata()[1] & 0x80; }
    // private_indicator    1       1.1       9
    bool PrivateIndicator(void) const { return pesdata()[1] & 0x40; }
    // reserved             2       1.2      10

    // section_length      12       1.4      12   always less than 0x3fd
    // adds 3 to the total section length to account for 3 bytes
    // before the end of the section length field.
    uint SectionLength(void) const { return Length() + 3; }

    // table_id_extension  16       3.0      24   table dependent
    uint TableIDExtension(void) const
        { return (pesdata()[3]<<8) | pesdata()[4]; }

    // reserved             2       5.0      40

    // version_number       5       5.2      42
    // incremented modulo 32 when table info changes
    uint Version(void) const { return (pesdata()[5]>>1) & 0x1f; }

    // current_next_ind     1       5.7      47
    // if 0 this table is not yet valid, but will be the next psip
    // table with the same sectionNumber(), tableIDExtension() and
    // tableID() to become valid.
    bool IsCurrent(void) const { return bool(pesdata()[5]&1); }

    // section_number       8       6.0      48
    uint Section(void) const { return pesdata()[6]; }

    // last_section_number  8       7.0      56
    uint LastSection(void) const { return pesdata()[7]; }

    // this is only for real ATSC PSIP tables, not similar MPEG2 tables
    // protocol_version     8       8.0      64   should always be 0 for now
    uint ProtocolVersion(void) const { return pesdata()[8]; }

    // PSIP_table_data      x       8.0      72 (incl. protocolVersion)
    const unsigned char* psipdata(void) const
        { return pesdata() + PSIP_OFFSET; }
    unsigned char* psipdata(void)
        { return pesdata() + PSIP_OFFSET; }

    // sets
    void SetTableID(uint id) { SetStreamID(id); }
    // subtracts 3 from total section length to account for 3 bytes
    // before the end of the section length field.
    void SetSectionLength(uint length) { SetLength(length-3); }
    void SetTableIDExtension(uint len)
    {
        pesdata()[3] = (len>>8) & 0xff;
        pesdata()[4] = len & 0xff;
    }
    void SetVersionNumber(uint ver)
        { pesdata()[5] = (pesdata()[5] & 0xc1) | ((ver & 0x1f)<<1); }
    void SetCurrent(bool cur)
        { pesdata()[5] = (pesdata()[5] & 0xfe) | (cur ? 1 : 0); }
    void SetSection(uint num) { pesdata()[6] = num; }
    void SetLastSection(uint num) { pesdata()[7] = num; }

    // only for real ATSC PSIP tables, not similar MPEG2 tables
    void SetProtocolVersion(int ver) { pesdata()[8] = ver; }

    bool HasCRC(void) const;

    bool VerifyPSIP(bool verify_crc) const;

    const QString toString(void) const;

    static const uint PSIP_OFFSET = 8; // general PSIP header offset
};

/** \class ProgramAssociationTable
 *  \brief The Program Association Table lists all the programs
 *         in a stream, and is alwyas found on PID 0.
 *
 *   Based on info in this table and the ProgramMapTable
 *   for the program we are interested in we should
 *   be able determine which PID to write to the ringbuffer
 *   when given the program stream to record.
 *
 *   NOTE: Broadcasters are encouraged to keep the
 *   subprogram:PID mapping constant.  If we store this data
 *   in the channel database, we can branch-predict which
 *   PIDs we are looking for, and can thus "tune" the
 *   subprogram more quickly.
 *
 *  \sa ProgramMapTable
 */

class ProgramAssociationTable : public PSIPTable
{
  public:
    ProgramAssociationTable(const ProgramAssociationTable& table)
        : PSIPTable(table)
    {
        assert(TableID::PAT == TableID());
    }

    ProgramAssociationTable(const PSIPTable &table) : PSIPTable(table)
    {
        assert(TableID::PAT == TableID());
    }

    // transport stream ID, program ID, count
    static ProgramAssociationTable* Create(uint tsid, uint version,
                                           const vector<uint>& pnum,
                                           const vector<uint>& pid);

    uint TransportStreamID(void) const { return TableIDExtension(); }

    uint ProgramCount(void) const
    {
        if (SectionLength() > (PSIP_OFFSET+2))
            return (SectionLength()-PSIP_OFFSET-2)>>2;
        return 0;
    }

    uint ProgramNumber(uint i) const
        { return (psipdata()[(i<<2)] << 8) | psipdata()[(i<<2) + 1]; }

    uint ProgramPID(uint i) const
    {
        return (((psipdata()[(i<<2) + 2] & 0x1f) << 8) |
                psipdata()[(i<<2) + 3]);
    }

    void SetTranportStreamID(uint gtsid) { SetTableIDExtension(gtsid); }

    // helper function
    uint FindPID(uint progNum) const
    {
        for (uint i = 0; i < ProgramCount(); i++)
            if (progNum==ProgramNumber(i))
                return ProgramPID(i);
        return 0;
    }
    uint FindAnyPID(void) const
    {
        for (uint i = 0; i < ProgramCount(); i++)
            if (0!=ProgramNumber(i))
                return ProgramPID(i);
        return 0;
    }
    uint FindProgram(uint pid) const
    {
        for (uint i = 0; i < ProgramCount(); i++)
            if (pid==ProgramPID(i))
                return ProgramNumber(i);
        return 0;
    }

    const QString toString(void) const;

  private:
    static ProgramAssociationTable* CreateBlank(bool smallPacket = true);
};

/** \class ProgramMapTable
 *  \brief A PMT table maps a program described in the ProgramAssociationTable
 *         to various PID's which describe the multimedia contents of the
 *         program.
 */
class ProgramMapTable : public PSIPTable
{
  public:

    ProgramMapTable(const ProgramMapTable& table) : PSIPTable(table)
    {
        assert(TableID::PMT == TableID());
        Parse();
    }

    ProgramMapTable(const PSIPTable& table) : PSIPTable(table)
    {
        assert(TableID::PMT == TableID());
        Parse();
    }

    static ProgramMapTable* Create(uint programNumber, uint basepid,
                                   uint pcrpid, uint version,
                                   vector<uint> pids, vector<uint> types);

    static ProgramMapTable* Create(uint programNumber, uint basepid,
                                   uint pcrpid, uint version,
                                   const desc_list_t         &global_desc,
                                   const vector<uint>        &pids,
                                   const vector<uint>        &types,
                                   const vector<desc_list_t> &prog_desc);

    /// stream that contrains program clock reference.
    uint PCRPID(void) const
        { return ((psipdata()[0] << 8) | psipdata()[1]) & 0x1fff; }

    uint ProgramNumber(void) const
        { return TableIDExtension(); }

    uint ProgramInfoLength(void) const
        { return ((psipdata()[2]<<8) | psipdata()[3]) & 0x0fff; }

    const unsigned char* ProgramInfo(void) const
        { return psipdata() + 4; }

    uint StreamType(uint i) const
        { return _ptrs[i][0]; }

    uint StreamPID(uint i) const
        { return ((_ptrs[i][1] << 8) | _ptrs[i][2]) & 0x1fff; }

    uint StreamInfoLength(uint i) const
        { return ((_ptrs[i][3] << 8) | _ptrs[i][4]) & 0x0fff; }

    const unsigned char* StreamInfo(uint i) const
        { return _ptrs[i] + 5; }

    uint StreamCount(void) const
        { return (_ptrs.size()) ? _ptrs.size()-1 : 0; }

    // sets
    void SetPCRPID(uint pid)
    {
        psipdata()[0] = ((pid >> 8) & 0x1F) | (psipdata()[0] & 0xE0);
        psipdata()[1] = (pid & 0xFF);
    }

    void SetProgramNumber(uint num) { SetTableIDExtension(num); }

    void SetStreamPID(uint i, uint pid)
    {
        _ptrs[i][1] = ((pid>>8) & 0x1f) | (_ptrs[i][1] & 0xe0);
        _ptrs[i][2] = pid & 0xff;
    }

    void SetStreamType(uint i, uint type)
        { _ptrs[i][0] = type; }

    // helper methods
    bool IsVideo(uint i, QString sistandard) const;
    bool IsAudio(uint i, QString sistandard) const;
    bool IsEncrypted(void) const;
    bool IsProgramEncrypted(void) const;
    bool IsStreamEncrypted(uint pid) const;
    /// Returns true iff PMT contains a still-picture video stream
    bool IsStillPicture(QString sistandard) const;
    /// Returns a string representation of type at stream index i
    QString StreamTypeString(uint i) const
        { return StreamID::toString(StreamType(i)); }
    /// Returns a better (and more expensive) string representation
    /// of type at stream index i than StreamTypeString(uint)
    QString StreamDescription(uint i, QString sistandard) const;
    /// Returns the cannonical language if we find the iso639 descriptor
    QString GetLanguage(uint i) const;

    uint FindPIDs(uint type, vector<uint> &pids,
                  const QString &sistandard) const;
    uint FindPIDs(uint type, vector<uint> &pids, vector<uint> &types,
                  const QString &sistandard, bool normalize) const;

    /// \brief Locates stream index of pid.
    /// \return stream index if successful, -1 otherwise
    int FindPID(uint pid) const
    {
        for (uint i = 0; i < StreamCount(); i++)
            if (pid == StreamPID(i))
                return i;
        return -1;
    }
    uint FindUnusedPID(uint desired_pid = 0x20);

    void RemoveAllStreams(void)
    {
        memset(psipdata(), 0xff, pmt_header);
        SetProgramInfoLength(0);
        _ptrs.clear();
    }
    void AppendStream(uint pid, uint type, unsigned char* si = 0, uint il = 0);

    void Parse(void) const;
    const QString toString(void) const;
    // unsafe sets
  private:
    void SetStreamInfoLength(uint i, uint length)
    {
        _ptrs[i][3] = ((length>>8) & 0x0f) | (_ptrs[i][3] & 0xf0);
        _ptrs[i][4] = length & 0xff;
    }

    void SetStreamProgramInfo(uint i, unsigned char* streamInfo,
                              uint infoLength)
    {
        SetStreamInfoLength(i, infoLength);
        memcpy(_ptrs[i] + 5, streamInfo, infoLength);
    }

    void SetProgramInfoLength(uint length)
    {
        psipdata()[2] = ((length>>8) & 0x0f) | (psipdata()[2] & 0xf0);
        psipdata()[3] = length & 0xff;
    }

    void SetProgramInfo(unsigned char *streamInfo, uint infoLength)
    {
        SetProgramInfoLength(infoLength);
        memcpy(psipdata() + 4, streamInfo, infoLength);
    }

    static ProgramMapTable* CreateBlank(bool smallPacket = true);

    static const uint pmt_header = 4; // minimum PMT header offset
    mutable vector<unsigned char*> _ptrs; // used to parse
};

/** \class ConditionalAccessTable
 *  \brief The CAT is used to transmit additional ConditionalAccessDescriptor
 *         instances, in addition to the ones in the PMTs.
 */
class ConditionalAccessTable : public PSIPTable
{
  public:
    ConditionalAccessTable(const PSIPTable &table) : PSIPTable(table)
    {
    // Section            Bits   Start Byte sbit
    // -----------------------------------------
    // table_id             8       0.0       0
        assert(TableID::CAT == TableID());
    // section_syntax_ind   1       1.0       8   should always be 1
    // private_indicator    1       1.1       9   should always be 0
    // reserved             2       1.2      10
    // section_length      12       1.4      12   always less than 0x3fd
    // table_id_extension  16       3.0      24   unused
    // reserved             2       5.0      40
    // version_number       5       5.2      42
    // current_next_ind     1       5.3      47
    // section_numben       8       6.0      48
    // last_section_number  8       7.0      56
    }

    // for (i = 0; i < N; i++)      8.0      64
    //   { descriptor() }
    uint DescriptorsLength(void) const
        { return SectionLength() - PSIP_OFFSET; }
    const unsigned char *Descriptors(void) const { return psipdata(); }

    // CRC_32 32 rpchof
};

/** \class AdaptationFieldControl
 *  \brief AdaptationFieldControl is used to transmit various important
 *         stream attributes.
 *
 *   These include such things as the PCR and flags discontiunities in the
 *   program, such as when a commercial or another program begins. This is
 *   currently just passed through the MythTV recorders to the recorded
 *   stream.
 */
class AdaptationFieldControl
{
  public:
    AdaptationFieldControl(const unsigned char* packet) : _data(packet) { ; }

    /** adaptation header length
     * (after which is payload data)         8   0.0
     */
    uint Length(void) const             { return _data[0]; }

    /** discontinuity_indicator
     *  (time base may change)               1   1.0
     */
    bool Discontinuity(void) const      { return _data[1] & 0x80; }
    // random_access_indicator (?)           1   1.1
    bool RandomAccess(void) const       { return bool(_data[1] & 0x40); }
    // elementary_stream_priority_indicator  1   1.2
    bool Priority(void) const           { return bool(_data[1] & 0x20); }

// Each of the following extends the adaptation header.  In order:

    /** PCR flag (we have PCR data)          1   1.3
     *  (adds 6 bytes after adaptation header)
     */
    bool PCR(void) const                { return bool(_data[1] & 0x10); }
    /** OPCR flag (we have OPCR data)        1   1.4
     *  (adds 6 bytes) ((Original) Program Clock Reference; used to time output)
     */
    bool OPCR(void) const               { return bool(_data[1] & 0x08); }
    /** splicing_point_flag                  1   1.5
     *  (adds 1 byte) (we have splice point data)
     *  Splice data is packets until a good splice point for
     *  e.g. commercial insertion -- if these are still set,
     *  might be a good way to recognize potential commercials
     *  for flagging.
     */
    bool SplicingPoint(void) const      { return bool(_data[1] & 0x04); }
    //  transport_private_data_flag          1   1.6
    // (adds 1 byte)
    bool PrivateTransportData(void) const { return bool(_data[1] & 0x02); }
    // adaptation_field_extension_flag       1   1.7
    bool FieldExtension(void) const     { return bool(_data[1] & 0x1); }
    // extension length                      8   2.0
    uint ExtensionLength(void) const    { return _data[2]; }
    // ltw flag                              1   3.0
    // (adds 2 bytes)
    bool LTW(void) const                { return bool(_data[3] & 0x80); }
    // piecewise_rate_flag (adds 3 bytes)    1   3.1
    bool PiecewiseRate(void) const      { return bool(_data[3] & 0x40); }
    // seamless_splice_flag (adds 5 bytes)   1   3.2
    bool SeamlessSplice(void) const     { return bool(_data[3] & 0x20); }
    // unused flags                          5   3.3

  private:
    const unsigned char* _data;
};

#endif
