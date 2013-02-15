/*
 *  Brooktree Bt8x8 card types list
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License Version 2 as
 *  published by the Free Software Foundation. You find a copy of this
 *  license in the file COPYRIGHT in the root directory of this release.
 *
 *  THIS PROGRAM IS DISTRIBUTED IN THE HOPE THAT IT WILL BE USEFUL,
 *  BUT WITHOUT ANY WARRANTY; WITHOUT EVEN THE IMPLIED WARRANTY OF
 *  MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *
 *  Description:
 *
 *    This module contains a list of TV cards based on the Brooktree Bt8x8
 *    chip family.  If offers functions to autodetect cards (based on their
 *    PCI subsystem ID which can be stored in the EEPROM on the card) and
 *    tuners (for certain vendors only, if type is stored in EEPROM),
 *    enumerate card and input lists and card-specific functions to switch
 *    the video input channel.
 *
 *
 *  Authors:
 *
 *      Copyright (c) 2001 John Adcock.  All rights reserved.
 *
 *  DScaler #Id: BT848Card_Types.cpp,v 1.41 2004/01/29 15:14:41 adcockj Exp #
 *
 *  $Id: bt8x8_typ.c,v 1.18 2007/12/29 23:21:55 tom Exp tom $
 */

#define DEBUG_SWITCH DEBUG_SWITCH_VBI
#define DPRINTF_OFF

#include <windows.h>

#include "mytypes.h"
#include "debug.h"

#include "dsdrvlib.h"
#include "tvcard.h"
#include "bt8x8_reg.h"
#include "bt8x8_typ.h"

#define true TRUE
#define false FALSE


typedef enum
{
    TVCARD_UNKNOWN = 0,
    TVCARD_MIRO,
    TVCARD_HAUPPAUGE,
    TVCARD_STB,           
    TVCARD_INTEL,         
    TVCARD_DIAMOND,        
    TVCARD_AVERMEDIA,      
    TVCARD_MATRIX_VISION,
    TVCARD_FLYVIDEO,      
    TVCARD_TURBOTV,       
    TVCARD_HAUPPAUGE878,  
    TVCARD_MIROPRO,       
    TVCARD_ADSTECH_TV,    
    TVCARD_AVERMEDIA98,   
    TVCARD_VHX,           
    TVCARD_ZOLTRIX,       
    TVCARD_PIXVIEWPLAYTV, 
    TVCARD_WINVIEW_601,   
    TVCARD_AVEC_INTERCAP, 
    TVCARD_LIFE_FLYKIT,   
    TVCARD_CEI_RAFFLES,   
    TVCARD_CONFERENCETV,  
    TVCARD_PHOEBE_TVMAS,  
    TVCARD_MODTEC_205,    
    TVCARD_MAGICTVIEW061, 
    TVCARD_VOBIS_BOOSTAR, 
    TVCARD_HAUPPAUG_WCAM, 
    TVCARD_MAXI,          
    TVCARD_TERRATV,       
    TVCARD_PXC200,        
    TVCARD_FLYVIDEO_98,   
    TVCARD_IPROTV,        
    TVCARD_INTEL_C_S_PCI, 
    TVCARD_TERRATVALUE,   
    TVCARD_WINFAST2000,   
    TVCARD_CHRONOS_VS2,   
    TVCARD_TYPHOON_TVIEW, 
    TVCARD_PXELVWPLTVPRO, 
    TVCARD_MAGICTVIEW063, 
    TVCARD_PINNACLERAVE,  
    TVCARD_STB2,          
    TVCARD_AVPHONE98,     
    TVCARD_PV951,         
    TVCARD_ONAIR_TV,      
    TVCARD_SIGMA_TVII_FM, 
    TVCARD_MATRIX_VISION2d,
    TVCARD_ZOLTRIX_GENIE, 
    TVCARD_TERRATVRADIO, 
    TVCARD_DYNALINK,
    // MAE 20 Nov 2000 Start of change
    TVCARD_CONEXANTNTSCXEVK,
    TVCARD_ROCKWELLNTSCXEVK,
    // MAE 20 Nov 2000 End of change
    // MAE 5 Dec 2000 Start of change
    TVCARD_CONEXANTFOGHORNREVA,
    TVCARD_CONEXANTFOGHORNREVB,
    TVCARD_CONEXANTFOGHORNREVC,
    // MAE 5 Dec 2000 End of change
    TVCARD_RS_BT,
    TVCARD_CYBERMAIL,
    TVCARD_VIEWCAST,
    TVCARD_ATI_TVWONDER,
    TVCARD_ATI_TVWONDERVE,
    TVCARD_GVBCTV3PCI,
    TVCARD_PROLINK,
    TVCARD_EAGLE,
    TVCARD_PINNACLEPRO,
    TVCARD_THYPHOON,
    TVCARD_LIFETEC,
    TVCARD_BESTBUY_OLD,
    TVCARD_FLYVIDEO_98FM,
    TVCARD_GRANDTEC,
    TVCARD_PHOEBE,
    TVCARD_TVCAPTURER,
    TVCARD_MM100PCTV,
    TVCARD_GMV1,
    TVCARD_BESTBUY_NEW,
    TVCARD_SASEM4CHNLSVID_S,
    TVCARD_PINNACLESAT,
    TVCARD_VHXOLD,           
    TVCARD_VOODOOTV_200,
    TVCARD_VOODOOTV_FM,
    TVCARD_STANDARD_BT878_NO_INIT_MSP,
    TVCARD_TERRATVALUER,
    TVCARD_FLYVIDEO98EZ,
    TVCARD_AIMSS,
    TVCARD_FLYVIDEO2000,
    TVCARD_GVBCTV4PCI,
    TVCARD_PXELVWPLTVPAK,
    TVCARD_TVIEW_RDS_FM,
    TVCARD_SASEM4CHNLSVID_C,
    TVCARD_SDISILK_S,
    TVCARD_SDISILK_C,
    TVCARD_SKYWELL_MAGIC,
    TVCARD_SDISILK200_S,
    TVCARD_SDISILK200_C,
    TVCARD_SENSORAY311,
    TVCARD_WINDVR,
    TVCARD_AVERTVOLD,
    TVCARD_KWORLD_MPEGTV,
    TVCARD_APAC_VIEWCOMP,
    TVCARD_PMSVIDEO_PDI,
    TVCARD_PMSVIDEO_SDI,
    TVCARD_PMSDELUXE,
    TVCARD_NEBULA_DIGITV,
    TVCARD_SWEETSPOT,
    TVCARD_HERCULES_SMART_TV_STEREO,
    TVCARD_LASTONE,
} eTVCardId;

typedef enum
{
    SOUNDCHANNEL_MONO = 1,
    SOUNDCHANNEL_STEREO,
    SOUNDCHANNEL_LANGUAGE1,
    SOUNDCHANNEL_LANGUAGE2,
} eSoundChannel;


static void StandardBT848InputSelect( TVCARD * pTvCard, int nInput);
static void InitRSBT( TVCARD * pTvCard );
static void InitSasem( TVCARD * pTvCard );
static void InitPXC200( TVCARD * pTvCard );
static void RSBTCardInputSelect( TVCARD * pTvCard, int nInput );
static void Sasem4ChannelInputSelect( TVCARD * pTvCard, int nInput );
static void Silk200InputSelect( TVCARD * pTvCard, int nInput );
static void CtrlTDA8540( TVCARD * pTvCard, BYTE SLV, BYTE SUB, BYTE SW1, BYTE GCO, BYTE OEN );
static void CtrlSilkSDISwitch( TVCARD * pTvCard, BYTE SLV, BYTE IEN );


#define TIMER_MSP           8
#define TIMER_MSP_MS        1000

// 10/19/2000 Mark Rejhon
// Better NTSC defaults
#define DEFAULT_HUE_NTSC 0
#define DEFAULT_BRIGHTNESS_NTSC 20
#define DEFAULT_CONTRAST_NTSC 207
#define DEFAULT_SAT_U_NTSC 254
#define DEFAULT_SAT_V_NTSC 219

// PAL defaults these work for OTA PAL signals
#define DEFAULT_HUE_PAL 0
#define DEFAULT_BRIGHTNESS_PAL 0
#define DEFAULT_CONTRAST_PAL 219
#define DEFAULT_SAT_U_PAL 254
#define DEFAULT_SAT_V_PAL 219

typedef enum
{
    /// standard composite input
    INPUTTYPE_COMPOSITE,
    /// standard s-video input
    INPUTTYPE_SVIDEO,
    /// standard analogue tuner input composite
    INPUTTYPE_TUNER,
    /// Digital CCIR656 input on the GPIO pins
    INPUTTYPE_CCIR,
    /// Radio input so no video
    INPUTTYPE_RADIO,
} eInputType;

/// Sounds chips we expect to find on a card
typedef enum
{
    SOUNDCHIP_NONE,
    SOUNDCHIP_MSP,
    SOUNDCHIP_TDA9875,
    SOUNDCHIP_TDA9874,
} eSoundChip;

/// Does the card have a PLL generator - used for PAL & SECAM
typedef enum
{
    PLL_NONE = 0,
    PLL_28,
    PLL_35,
} ePLLFreq;

/// Defines each input on a card
typedef struct
{
    /// Name of the input
    LPCSTR szName;
    /// Type of the input
    eInputType InputType;
    /// Which mux on the card is to be used
    BYTE MuxSelect;
} TInputType;

/// Defines the specific settings for a given card
#define INPUTS_PER_CARD 7
typedef struct
{
    LPCSTR szName;
    int NumInputs;
    TInputType Inputs[INPUTS_PER_CARD];
    ePLLFreq PLLFreq;
    eSoundChip SoundChip;
    /// Any card specific initialization - may be NULL
    void (*pInitCardFunction)(TVCARD * pTvCard);
    /** Function used to switch between sources
        Cannot be NULL
        Default is StandardBT848InputSelect
    */
    void (*pInputSwitchFunction)(TVCARD * pTvCard, int nInput);
    /// Any card specific method used to select stereo - may be NULL
    //CAudioDecoder::eAudioDecoderType AudioDecoderType;
    /// Bit Mask for audio GPIO operations
    DWORD GPIOMask;
    /** GPIO Flags for the various inputs
        We seem to use 
        AUDIOINPUT_TUNER to be selected when tuner is selected and no MSP
        AUDIOINPUT_RADIO to be selected when tuner is selected and ther is a MSP
        AUDIOINPUT_EXTERNAL to be selected when anything but the tuner is required
        AUDIOINPUT_MUTE for muting
    */
    DWORD AudioMuxSelect[6];
} TCardType;

/// used to store the ID for autodection
typedef struct
{
    DWORD ID;
    eTVCardId CardId;
    char* szName;
} TAutoDectect878;

static const TCardType m_TVCards[TVCARD_LASTONE] =
{
    // Card Number 0 - Unknown Card
    {
        "*Unknown Card*",
        4,
        {
            {
                "Tuner",
                INPUTTYPE_TUNER,
                2,
            },
            {
                "Composite",
                INPUTTYPE_COMPOSITE,
                3,
            },
            {
                "S-Video",
                INPUTTYPE_SVIDEO,
                1,
            },
            {
                "Composite over S-Video",
                INPUTTYPE_COMPOSITE,
                1,
            },
        },
        PLL_28,
        SOUNDCHIP_NONE,
        NULL,
        StandardBT848InputSelect,
        //CAudioDecoder::AUDIODECODERTYPE_DETECT,
        0x0,
        {0, 0, 0, 0, 0, 0, }
    },
    // Card Number 10 - Standard BT878
    {
        "Hauppauge BT878 / Standard BT878",
        5,
        {
            {
                "Tuner",
                INPUTTYPE_TUNER,
                2,
            },
            {
                "Composite",
                INPUTTYPE_COMPOSITE,
                0,
            },
            {
                "Composite 2",
                INPUTTYPE_COMPOSITE,
                0,
            },
            {
                "S-Video",
                INPUTTYPE_SVIDEO,
                1,
            },
            {
                "Composite over S-Video",
                INPUTTYPE_COMPOSITE,
                1,
            },
        },
        /*PLL_28*/ PLL_NONE,
        SOUNDCHIP_MSP,
        NULL,
        StandardBT848InputSelect,
        //CAudioDecoder::AUDIODECODERTYPE_DETECT,
        0x7,
        {0, 0x1, 0x2, 0x3, 0x4, 0, }
    },
};

static const TAutoDectect878 m_AutoDectect878[] =
{
    { 0x00011002, TVCARD_ATI_TVWONDER,  "ATI TV Wonder" },
    { 0x00011461, TVCARD_AVPHONE98,     "AVerMedia TVPhone98" },
    { 0x00021461, TVCARD_AVERMEDIA98,   "Avermedia TVCapture 98" },
    { 0x00031002, TVCARD_ATI_TVWONDERVE,"ATI TV Wonder/VE" },
    { 0x00031461, TVCARD_AVPHONE98,     "AVerMedia TVPhone98" },
    { 0x00041461, TVCARD_AVERMEDIA98,   "AVerMedia TVCapture 98" },
    { 0x001211bd, TVCARD_PINNACLERAVE,  "Pinnacle PCTV" },
    { 0x001c11bd, TVCARD_PINNACLESAT,   "Pinnacle PCTV Sat" },
    { 0x10b42636, TVCARD_HAUPPAUGE878,  "STB ???" },
    { 0x1118153b, TVCARD_TERRATVALUE,   "Terratec TV Value" },
    { 0x1123153b, TVCARD_TERRATVRADIO,  "Terratec TV/Radio+" },
    { 0x1200bd11, TVCARD_PINNACLERAVE,  "Pinnacle PCTV Rave" },
    { 0x13eb0070, TVCARD_HAUPPAUGE878,  "Hauppauge WinTV" },
    { 0x18501851, TVCARD_CHRONOS_VS2,   "Chronos Video Shuttle II" },
    { 0x18521852, TVCARD_TYPHOON_TVIEW, "Typhoon TView TV/FM Tuner" },
    { 0x217d6606, TVCARD_WINFAST2000,   "Leadtek WinFast TV 2000" },
    { 0x263610b4, TVCARD_STB2,          "STB TV PCI FM, P/N 6000704" },
    { 0x3000144f, TVCARD_MAGICTVIEW063, "TView 99 (CPH063)" },
    { 0x300014ff, TVCARD_MAGICTVIEW061, "TView 99 (CPH061)" },
    { 0x3002144f, TVCARD_MAGICTVIEW061, "Askey Magic TView" },
    { 0x300214ff, TVCARD_PHOEBE_TVMAS,  "Phoebe TV Master" },
    { 0x39000070, TVCARD_HAUPPAUGE878,  "Hauppauge WinTV-D" },
    { 0x400a15b0, TVCARD_ZOLTRIX_GENIE, "Zoltrix Genie TV" },
    { 0x400d15b0, TVCARD_ZOLTRIX_GENIE, "Zoltrix Genie TV / Radio" },
    { 0x401015b0, TVCARD_ZOLTRIX_GENIE, "Zoltrix Genie TV / Radio" },
    { 0x402010fc, TVCARD_GVBCTV3PCI,    "I-O Data Co. GV-BCV3/PCI" },
    { 0x405010fc, TVCARD_GVBCTV4PCI,    "I-O Data Co. GV-BCV4/PCI" },
    { 0x45000070, TVCARD_HAUPPAUGE878,  "Hauppauge WinTV/PVR" },
    { 0xff000070, TVCARD_VIEWCAST,      "Osprey-100" },
    { 0xff010070, TVCARD_VIEWCAST,      "Osprey-200" },
    { 0x010115cb, TVCARD_GMV1,          "AG GMV1" },
    { 0x14610002, TVCARD_AVERMEDIA98,   "Avermedia TVCapture 98" },
    { 0x182214F1, TVCARD_CONEXANTNTSCXEVK,  "Conexant Bt878A NTSC XEVK" },
    { 0x1322127A, TVCARD_ROCKWELLNTSCXEVK,  "Rockwell Bt878A NTSC XEVK" },
    { 0x013214F1, TVCARD_CONEXANTFOGHORNREVA,  "Conexant Foghorn NTSC/ATSC-A" },
    { 0x023214F1, TVCARD_CONEXANTFOGHORNREVB,  "Conexant Foghorn NTSC/ATSC-B" },
    { 0x033214F1, TVCARD_CONEXANTFOGHORNREVC,  "Conexant Foghorn NTSC/ATSC-C" },
    { 0x3000121A, TVCARD_VOODOOTV_200, "3dfx VoodooTV 200 (USA) / FM (Europa)"},
    { 0x3100121A, TVCARD_VOODOOTV_200, "3dfx VoodooTV 200 (USA) / FM (Europa) (OEM)"},
    // { 0x3060121A, TVCARD_VOODOOTV_100, "3dfx VoodooTV 100"},
    { 0x010114c7, TVCARD_MODTEC_205,    "Modular Technology PCTV" },
    { 0x1117153b, TVCARD_TERRATVALUE,   "Terratec TValue" },
    { 0x1119153b, TVCARD_TERRATVALUE,   "Terratec TValue" },
    { 0x111a153b, TVCARD_TERRATVALUE,   "Terratec TValue" },
    { 0x1127153b, TVCARD_TERRATV,       "Terratec TV+"    },
    { 0x1134153b, TVCARD_TERRATVALUE,   "Terratec TValue" },
    { 0x1135153b, TVCARD_TERRATVALUER,  "Terratec TValue Radio" },
    { 0x18511851, TVCARD_FLYVIDEO98EZ,  "Flyvideo 98EZ (LR51)/ CyberMail AV" },
    { 0x3005144f, TVCARD_MAGICTVIEW061, "(Askey Magic/others) TView99 CPH061/06L (T1/LC)" },
    { 0x401615b0, TVCARD_ZOLTRIX_GENIE, "Zoltrix Genie TV / Radio" },
    { 0x6606107d, TVCARD_WINFAST2000,   "Leadtek WinFast TV 2000" },
    { 0x5018153b, TVCARD_TERRATVALUE,   "Terratec TValue" },
    { 0x03116000, TVCARD_SENSORAY311,   "Sensoray 311" },
    { 0x00790e11, TVCARD_WINDVR,        "Canopus WinDVR PCI" },
    { 0xa0fca1a0, TVCARD_ZOLTRIX,       "Face to Face Tvmax" },
    { 0x31323334, TVCARD_GRANDTEC,      "GrandTec XCapture" },
    { 0x109e306e, TVCARD_KWORLD_MPEGTV, "KWorld MPEGTV RF Pro" },
    { 0, (eTVCardId)-1, NULL }
};


// ---------------------------------------------------------------------------
// Query TDA9887 parameters stored in card INI file
// - returns TRUE if TDA9887 is configured via card INI
//
static bool GetTda9887Modes( TVCARD * pTvCard, bool * pHasTda9887, void ** ppModes )
{
    if ((pTvCard != NULL) && (pHasTda9887 != NULL) && (ppModes != NULL))
    {
    }
    else
        fatal0("Bt8x8-GetTda9887Modes: illegal NULL ptr param");

    return FALSE;
}

// ---------------------------------------------------------------------------
// Query how IF demodulator TDA9887 needs to be programmed
//   Derived from CBT848Card::InitTuner
//   DScaler CVS #Id: BT848Card_Tuner.cpp,v 1.13 2003/10/27 10:39:50 adcockj Exp #
//
static void GetIffType( TVCARD * pTvCard, bool * pIsPinnacle, bool * pIsMono )
{
    DWORD Id;

    if ((pTvCard != NULL) && (pIsPinnacle != NULL) && (pIsMono != NULL))
    {
        *pIsMono = FALSE;

        switch (pTvCard->params.cardId)
        {
        case TVCARD_MIRO:
            *pIsMono = TRUE;
            // fall-through
        case TVCARD_MIROPRO:
        case TVCARD_PINNACLERAVE:
        case TVCARD_PINNACLEPRO:
            {
                // Get Card ID
                WriteDword(BT848_GPIO_OUT_EN,( 0x0000 )&0x00FFFFFFL);
                Id = ReadDword(BT848_GPIO_DATA);
                Id = ((Id >> 10) & 63) - 1;

                if (Id>=32)
                {
                    // Only newer cards use MT2032 & TDA9885/6/7
                    Id = 63 - Id;
                    *pIsPinnacle = TRUE;
                }
            }
            break;

        case TVCARD_VOODOOTV_200:
        case TVCARD_VOODOOTV_FM:
            {
               //CPreTuneVoodooFM *pPreTuneVoodooFM = new CPreTuneVoodooFM(this);
               //pExternalIFDemodulator = pPreTuneVoodooFM;
               //IFDemDeviceAddress = 0;
            }
            break;

        default:
            break;
        }
    }
    else
        fatal0("Bt8x8-GetIffType:illegal NULL ptr param");
}

static uint GetPllType( TVCARD * pTvCard, uint CardId )
{
    uint  m_CardType;
    uint  pll = 0;

    if (pTvCard != NULL)
    {
        m_CardType = pTvCard->params.cardId;

        if (m_CardType < TVCARD_LASTONE)
        {
            pll = m_TVCards[m_CardType].PLLFreq;
        }
        else
            debug2("Bt8x8-GetPllType: invalid card idx %d (>= %d)", m_CardType, TVCARD_LASTONE);
    }
    else
        fatal0("Bt8x8-GetPllType:illegal NULL ptr param");

    return pll;
}

// ---------------------------------------------------------------------------
// Query if the PCI card supports ACPI (power management)
//
static bool SupportsAcpi( TVCARD * pTvCard )
{
   // return TRUE if BT878 or BT878A is present
   return ( (pTvCard->params.DeviceId == 0x036E) ||
            (pTvCard->params.DeviceId == 0x036F) );
}

static uint GetNumInputs( TVCARD * pTvCard )
{
    uint  m_CardType;
    uint  count = 0;

    if (pTvCard != NULL)
    {
        m_CardType = pTvCard->params.cardId;

        if (m_CardType < TVCARD_LASTONE)
        {
            count = m_TVCards[m_CardType].NumInputs;
        }
        else
            debug2("Bt8x8-GetNumInputs: invalid card idx %d (>= %d)", m_CardType, TVCARD_LASTONE);
    }
    else
        fatal0("Bt8x8-GetNumInputs: illegal NULL ptr param");

    return count;
}

static bool IsInputATuner( TVCARD * pTvCard, uint nInput )
{
    uint  m_CardType;
    bool  result = FALSE;

    if (pTvCard != NULL)
    {
        m_CardType = pTvCard->params.cardId;

        if ((m_CardType < TVCARD_LASTONE) && (nInput < m_TVCards[m_CardType].NumInputs))
        {
            return (m_TVCards[m_CardType].Inputs[nInput].InputType == INPUTTYPE_TUNER);
        }
        else
            debug4("Bt8x8-IsInputATuner: invalid card idx %d (>= %d) or input idx %d (>%d)", m_CardType, TVCARD_LASTONE, nInput, m_TVCards[m_CardType].NumInputs);
    }
    else
        fatal0("Bt8x8-IsInputATuner: illegal NULL ptr param");

    return result;
}

static bool IsInputSVideo( TVCARD * pTvCard, uint nInput )
{
    uint  m_CardType;
    bool  result = FALSE;

    if (pTvCard != NULL)
    {
        m_CardType = pTvCard->params.cardId;

        if ((m_CardType < TVCARD_LASTONE) && (nInput < m_TVCards[m_CardType].NumInputs))
        {
            return (m_TVCards[m_CardType].Inputs[nInput].InputType == INPUTTYPE_SVIDEO);
        }
        else
            debug4("Bt8x8-IsInputSVideo: invalid card idx %d (>= %d) or input idx %d (>%d)", m_CardType, TVCARD_LASTONE, nInput, m_TVCards[m_CardType].NumInputs);
    }
    else
        fatal0("Bt8x8-IsInputSVideo: illegal NULL ptr param");

    return result;
}

static const char * GetInputName( TVCARD * pTvCard, uint nInput )
{
    uint  m_CardType;
    const char * pName = NULL;

    if (pTvCard != NULL)
    {
        m_CardType = pTvCard->params.cardId;

        if ((m_CardType < TVCARD_LASTONE) && (nInput < m_TVCards[m_CardType].NumInputs))
        {
            pName = m_TVCards[m_CardType].Inputs[nInput].szName;
        }
        else
            debug4("Bt8x8-GetInputName: invalid card idx %d (>= %d) or input idx %d (>%d)", m_CardType, TVCARD_LASTONE, nInput, m_TVCards[m_CardType].NumInputs);
    }
    else
        fatal0("Bt8x8-GetInputName: illegal NULL ptr param");

    return pName;
}

static bool SetVideoSource( TVCARD * pTvCard, uint nInput )
{
    uint  m_CardType = pTvCard->params.cardId;
    bool  result = FALSE;

    if (pTvCard != NULL)
    {
        if ((m_CardType < TVCARD_LASTONE) && (nInput < m_TVCards[m_CardType].NumInputs))
        {
            (m_TVCards[m_CardType].pInputSwitchFunction)(pTvCard, nInput);
            result = TRUE;
        }
        else
            debug4("Bt8x8-SetVideoSource: invalid card idx %d (>= %d) or input idx %d (>%d)", m_CardType, TVCARD_LASTONE, nInput, m_TVCards[m_CardType].NumInputs);
    }
    else
        fatal0("Bt8x8-SetVideoSource: illegal NULL ptr param");

    return result;
}

static const char * GetCardName( TVCARD * pTvCard, uint CardId )
{
    const char * pName = NULL;

    if (pTvCard != NULL)
    {
        if (CardId < TVCARD_LASTONE)
        {
            pName = m_TVCards[CardId].szName;
        }
        //else: no warning because during card enumeration this function
        // is called until NULL is returned
    }
    else
        fatal0("Bt8x8Typ-GetCardName: illegal NULL ptr param");

    return pName;
}

static uint AutoDetectCardType( TVCARD * pTvCard )
{
    DWORD Id;

    if (pTvCard != NULL)
    {
        Id = pTvCard->params.SubSystemId;

        if (Id != 0 && Id != 0xffffffff)
        {
            int i;
            for (i = 0; m_AutoDectect878[i].ID != 0; i++)
            {
                if (m_AutoDectect878[i].ID  == Id)
                {
                    //Try to detect PRO models
                    eTVCardId CardID = m_AutoDectect878[i].CardId;
                    switch (CardID)
                    {
                    case TVCARD_MIRO:
                        //if (AutoDetectMSP3400())
                        //{
                        //     CardID = TVCARD_MIROPRO;
                        //}
                        break;
                    case TVCARD_PINNACLERAVE:
                        //if (AutoDetectMSP3400())
                        //{
                        //     CardID = TVCARD_PINNACLEPRO;
                        //}
                        break;
                    case TVCARD_FLYVIDEO2000:
                        //if (TDA9874)
                        //{
                        //     CardID = TVCARD_FLYVIDEO2000S;
                        //}
                        break;
                    default:
                        break;
                    }

                    return CardID;
                }
            }
        }

        // STB cards have a eeprom @ 0xae (old bt848)
        {
            BYTE Out[] = { 0xAE , 0 };
            BYTE Val = 0;
            if (pTvCard->i2cBus->I2cRead(pTvCard, Out,2,&Val,1))
            {
                return TVCARD_STB;
            }
        }
    }
    else
        fatal0("Bt8x8Typ-AutoDetectCardType: illegal NULL ptr param");

    return TVCARD_UNKNOWN;
}


static void RSBTCardInputSelect( TVCARD * pTvCard, int nInput)
{
    StandardBT848InputSelect(pTvCard, nInput);
    switch(nInput)
    {
    case 3:
        MaskDataDword(BT848_GPIO_DATA, 0x1000, 0x1F800);
        Sleep(50);
        MaskDataDword(BT848_GPIO_DATA, 0x0000, 0x1000);
        break;
    case 4:
        MaskDataDword(BT848_GPIO_DATA, 0x9000, 0x1F800);
        Sleep(50);
        MaskDataDword(BT848_GPIO_DATA, 0x0000, 0x1000);
        break;
    case 5:
        MaskDataDword(BT848_GPIO_DATA, 0x11000, 0x1F800);
        Sleep(50);
        MaskDataDword(BT848_GPIO_DATA, 0x0000, 0x1000);
        break;
    case 6:
        MaskDataDword(BT848_GPIO_DATA, 0x19000, 0x1F800);
        Sleep(50);
        MaskDataDword(BT848_GPIO_DATA, 0x0000, 0x1000);
        break;
    default:
        break;
    }
}


static void StandardBT848InputSelect( TVCARD * pTvCard, int nInput)
{
    uint  m_CardType = pTvCard->params.cardId;
    DWORD MuxSel;

    AndDataByte(BT848_IFORM, (BYTE)~BT848_IFORM_MUXSEL);

    // set the comp bit for svideo
    switch (m_TVCards[m_CardType].Inputs[nInput].InputType)
    {
    case INPUTTYPE_SVIDEO:
        OrDataByte(BT848_E_CONTROL, BT848_CONTROL_COMP);
        OrDataByte(BT848_O_CONTROL, BT848_CONTROL_COMP);
        break;
    case INPUTTYPE_TUNER:
    case INPUTTYPE_COMPOSITE:
    case INPUTTYPE_CCIR:
    default:
        AndDataByte(BT848_E_CONTROL, ~BT848_CONTROL_COMP);
        AndDataByte(BT848_O_CONTROL, ~BT848_CONTROL_COMP);
        break;
    }

    MuxSel = m_TVCards[m_CardType].Inputs[nInput].MuxSelect;
    MaskDataByte(BT848_IFORM, (BYTE) (MuxSel << 5), BT848_IFORM_MUXSEL);
}

static void Sasem4ChannelInputSelect( TVCARD * pTvCard, int nInput)
{
    uint  m_CardType = pTvCard->params.cardId;
    BYTE OutputSelect;

    // these cards always use mux 2
    MaskDataByte(BT848_IFORM, (BYTE) (2 << 5), BT848_IFORM_MUXSEL);

    if(m_TVCards[m_CardType].Inputs[nInput].InputType == INPUTTYPE_SVIDEO)
    {
        OrDataByte(BT848_E_CONTROL, BT848_CONTROL_COMP);
        OrDataByte(BT848_O_CONTROL, BT848_CONTROL_COMP);
        // Set the Philips TDA8540 4x4 switch matrix for s-video input
        // 0xD2 SW1 choose OUT3=IN3; OUT2=IN1; OUT1=IN0; OUT0=IN2
        // 0x07 GCO choose (0000) gain; (01) clamp; (11) aux
        // 0x03 OEN choose OUT0 and OUT1 high (s-video)
        CtrlTDA8540(pTvCard, 0x90, 0x00, 0xD2, 0x07, 0x03);
    }
    else
    {
        AndDataByte(BT848_E_CONTROL, ~BT848_CONTROL_COMP);
        AndDataByte(BT848_O_CONTROL, ~BT848_CONTROL_COMP);
        // Set the Philips TDA8540 4x4 switch matrix for composite switch 1 input
        // 0x00 SW1 choose the MuxSelect setting
        // 0x87 GCO choose (1000) gain; (01) clamp; (11) aux
        // 0x01 OEN choose OUT0 high
        OutputSelect = m_TVCards[m_CardType].Inputs[nInput].MuxSelect;
        CtrlTDA8540(pTvCard, 0x90, 0x00, OutputSelect, 0x87, 0x01);
    }
}

static void Silk200InputSelect( TVCARD * pTvCard, int nInput)
{
    uint  m_CardType = pTvCard->params.cardId;
    //Is this an analog input?
    if(nInput < 0x4)
    {
        Sasem4ChannelInputSelect (pTvCard, nInput);
    }
    else
    //No, it's an SDI input
    {
        BYTE OutputSelect = m_TVCards[m_CardType].Inputs[nInput].MuxSelect;
        CtrlSilkSDISwitch(pTvCard, 0xB0, OutputSelect);
    }
}

static void  InitRSBT( TVCARD * pTvCard )
{
    WriteDword(BT848_GPIO_OUT_EN, 0xFFFFFF);
    WriteDword(BT848_GPIO_DATA, 0xFFFFFF);
    Sleep(50);
    WriteDword(BT848_GPIO_DATA, 0x000000);
    Sleep(50);
    WriteDword(BT848_GPIO_DATA, 0xFFFFFF);
    Sleep(50);
    WriteDword(BT848_GPIO_OUT_EN, 0x1F800);
}

static void InitSasem( TVCARD * pTvCard )
{
    // Initialize and set the Philips TDA8540 4x4 switch matrix
    // 0xD2 SW1 choose OUT3=IN3; OUT2=IN1; OUT1=IN0; OUT0=IN2
    // 0x07 GCO choose (0000) gain; (01) clamp; (11) aux
    // 0x03 OEN choose OUT0 and OUT1 high (i.e., s-video)
    CtrlTDA8540(pTvCard, 0x90, 0x00, 0xD2, 0x07, 0x03);
}

// -----------------------------------------------------------------------
//  Imagenation L-Model PXC200 Framegrabber
//  This is basically the same procedure as
//  used by Alessandro Rubini in his pxc200
//  driver, but using BTTV functions

static void InitPXC200( TVCARD * pTvCard )
{
    //I2C_Lock();
    //I2C_Write(0x5E, 0, 0x80, 1);
    const BYTE InitDac[] =
    {
        0x5E, 0,
        0x80, 1,
    };

    const BYTE InitPic[] =
    {
        0x1E, 0,
        0x08, 0x09,
        0x0a, 0x0b,
        0x0d, 0x0d,
        0x01, 0x02,
        0x03, 0x04,
        0x05, 0x06,
        0x00
    };

    // Initialise GPIO-connevted stuff
    WriteWord(BT848_GPIO_OUT_EN, 1<<13); // Reset pin only
    WriteWord(BT848_GPIO_DATA, 0);
    Sleep(30);
    WriteWord(BT848_GPIO_DATA, 1<<13);
    // GPIO inputs are pulled up, so no need to drive
    // reset pin any longer
    WriteWord(BT848_GPIO_OUT_EN, 0);

    //  we could/should try and reset/control the AD pots? but
    //  right now  we simply  turned off the crushing.  Without
    //  this the AGC drifts drifts
    //  remember the EN is reverse logic -->
    //  setting BT848_ADC_AGC_EN disable the AGC
    //  tboult@eecs.lehigh.edu

    WriteByte(BT848_ADC, BT848_ADC_RESERVED | BT848_ADC_AGC_EN);

    //  Initialise MAX517 DAC
    pTvCard->i2cBus->I2cWrite(pTvCard, InitDac, sizeof(InitDac));

    //  Initialise 12C508 PIC
    //  The I2CWrite and I2CRead commmands are actually to the
    //  same chips - but the R/W bit is included in the address
    //  argument so the numbers are different
    pTvCard->i2cBus->I2cWrite(pTvCard, InitPic, sizeof(InitPic));
}

// -----------------------------------------------------------------------
// TDA8540 Control Code
// Philips composite/s-video 4x4 switch IC
// 19 Jul 2001 Dan Schmelzer
//
// See datasheet at:
// http://www.semiconductors.philips.com/acrobat/datasheets/TDA8540_3.pdf
//
// Slave address byte (SLV) possibilities for the TDA8540 chip
// See page 5 of 2/6/95 datasheet
// 0x90, 0x92, 0x94, 0x96, 0x98, 0x9A, 0x9C
//
// Subcommand byte (SUB) possibilities choosing function
// See page 6 of 2/6/95 datasheet
// 0x00 = Access to switch control (SW1)
// 0x01 = Access to gain/clamp/auxilliary pins control (GCO)
// 0x02 = Access to output enable control (OEN)
// If commands are given in succession, then SUB is automatically incremented
// and the next register is written to
//
// Switch Control Register (SW1)
// See page 6 of 2/6/95 datasheet
// Four output bit pairs P1-4 make up control byte; input chosen for each output
// 00 for Input 0; 01 for Input 1; 10 for Input 2; 11 for input 3
//
// Gain & Clamp Control Register (GCO)
// See page 6 of 2/6/95 datasheet
// MS 4 bits control gain on outputs[3:0] (low is 1x, high is 2x)
// 2 bits control clamp action or mean value on inputs [1:0] ; LS 2 bits
// control value of auxilliary outputs D1, D0
//
// Output Enable Control Register (OEN)
// See page 7 of 2/6/95 datasheet
// MS 4 bits reserved; LS 4 bits controls which output(s) from 3 to 0 are
// active (high)
//
// Upon reset, the outputs are set to active and connected to IN0; the gains
// are set at 2x and inputs IN0 and IN1 are clamped.

static void CtrlTDA8540( TVCARD * pTvCard, BYTE SLV, BYTE SUB, BYTE SW1, BYTE GCO, BYTE OEN)
{
    BYTE Buffer[] = {SLV, SUB, SW1, GCO, OEN};
    pTvCard->i2cBus->I2cWrite(pTvCard, Buffer, 5);
}

// -----------------------------------------------------------------------
// SDI Silk SDI Input Switch
// 21 May 2002 Dan Schmelzer
//
// Slave addres (SLV) = 0xB0 for the Silk 200; write only
// Input enable (IEN) = from 0 to x
//
// You can never have too many inputs!

static void CtrlSilkSDISwitch( TVCARD * pTvCard, BYTE SLV, BYTE IEN)
{
    BYTE Buffer[] = {SLV, IEN};
    // The switch is flakey sometimes; it will return true, but won't
    // switch it; if you do multiple times in a row, however, it switches
    // fine.  Not sure whether it's DScaler or the switch.  Guessing the
    // switch.
    pTvCard->i2cBus->I2cWrite(pTvCard, Buffer, 2);
    pTvCard->i2cBus->I2cWrite(pTvCard, Buffer, 2);
    pTvCard->i2cBus->I2cWrite(pTvCard, Buffer, 2);
}

// ---------------------------------------------------------------------------
// Free allocated resources
//
static void FreeCardList( void )
{
   // nothing to do
}

// ---------------------------------------------------------------------------
// Fill interface struct
//
static const TVCARD_CFG Bt8x8Typ_Interface =
{
   GetCardName,
   AutoDetectCardType,
   GetTda9887Modes,
   GetIffType,
   GetPllType,
   SupportsAcpi,
   GetNumInputs,
   GetInputName,
   IsInputATuner,
   IsInputSVideo,
   SetVideoSource,
   FreeCardList,
};

void Bt8x8Typ_GetInterface( TVCARD * pTvCard )
{
   if (pTvCard != NULL)
   {
      pTvCard->cfg = &Bt8x8Typ_Interface;
   }
   else
      fatal0("Bt8x8Typ-GetInterface: NULL ptr param");
}

