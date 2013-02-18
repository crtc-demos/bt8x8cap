#include "alfe/main.h"

#include <stdio.h>
#include <stdint.h>
#include <aclapi.h>
#include <stdlib.h>
#include <winsvc.h>
#include <io.h>
#include <fcntl.h>
#include <stdarg.h>
#include <winioctl.h>

// ---------------------------------------------------------------------------
// Brooktree 848 registers

#define BT848_DSTATUS          0x000
#define BT848_DSTATUS_PRES     (1<<7)
#define BT848_DSTATUS_HLOC     (1<<6)
#define BT848_DSTATUS_FIELD    (1<<5)
#define BT848_DSTATUS_NUML     (1<<4)
#define BT848_DSTATUS_CSEL     (1<<3)
#define BT848_DSTATUS_LOF      (1<<1)
#define BT848_DSTATUS_COF      (1<<0)

#define BT848_IFORM            0x004
#define BT848_IFORM_HACTIVE    (1<<7)
#define BT848_IFORM_MUXSEL     (3<<5)
#define BT848_IFORM_MUX0       (2<<5)
#define BT848_IFORM_MUX1       (3<<5)
#define BT848_IFORM_MUX2       (1<<5)
#define BT848_IFORM_XTSEL      (3<<3)
#define BT848_IFORM_XT0        (1<<3)
#define BT848_IFORM_XT1        (2<<3)
#define BT848_IFORM_XTAUTO     (3<<3)
#define BT848_IFORM_XTBOTH     (3<<3)
#define BT848_IFORM_NTSC       1
#define BT848_IFORM_PAL_BDGHI  3
#define BT848_IFORM_PAL_M      4
#define BT848_IFORM_PAL_N      5
#define BT848_IFORM_SECAM      6
#define BT848_IFORM_AUTO       0
#define BT848_IFORM_NORM       7


#define BT848_FCNTR            0x0E8
#define BT848_PLL_F_LO         0x0F0
#define BT848_PLL_F_HI         0x0F4
#define BT848_PLL_XCI          0x0F8

#define BT848_TGCTRL           0x084
#define BT848_TGCTRL_TGCKI_PLL 0x08
#define BT848_TGCTRL_TGCKI_NOPLL 0x00

#define BT848_TDEC             0x008
#define BT848_TDEC_DEC_FIELD   (1<<7)
#define BT848_TDEC_FLDALIGN    (1<<6)
#define BT848_TDEC_DEC_RAT     (0x1f)

#define BT848_E_CROP           0x00C
#define BT848_O_CROP           0x08C

#define BT848_E_VDELAY_LO      0x010
#define BT848_O_VDELAY_LO      0x090

#define BT848_E_VACTIVE_LO     0x014
#define BT848_O_VACTIVE_LO     0x094

#define BT848_E_HDELAY_LO      0x018
#define BT848_O_HDELAY_LO      0x098

#define BT848_E_HACTIVE_LO     0x01C
#define BT848_O_HACTIVE_LO     0x09C

#define BT848_E_HSCALE_HI      0x020
#define BT848_O_HSCALE_HI      0x0A0

#define BT848_E_HSCALE_LO      0x024
#define BT848_O_HSCALE_LO      0x0A4

#define BT848_BRIGHT           0x028

#define BT848_E_CONTROL        0x02C
#define BT848_O_CONTROL        0x0AC
#define BT848_CONTROL_LNOTCH    (1<<7)
#define BT848_CONTROL_COMP      (1<<6)
#define BT848_CONTROL_LDEC      (1<<5)
#define BT848_CONTROL_CBSENSE   (1<<4)
#define BT848_CONTROL_CON_MSB   (1<<2)
#define BT848_CONTROL_SAT_U_MSB (1<<1)
#define BT848_CONTROL_SAT_V_MSB (1<<0)

#define BT848_CONTRAST_LO      0x030
#define BT848_SAT_U_LO         0x034
#define BT848_SAT_V_LO         0x038
#define BT848_HUE              0x03C

#define BT848_E_SCLOOP         0x040
#define BT848_O_SCLOOP         0x0C0
#define BT848_SCLOOP_CAGC       (1<<6)
#define BT848_SCLOOP_CKILL      (1<<5)
#define BT848_SCLOOP_HFILT_AUTO (0<<3)
#define BT848_SCLOOP_HFILT_CIF  (1<<3)
#define BT848_SCLOOP_HFILT_QCIF (2<<3)
#define BT848_SCLOOP_HFILT_ICON (3<<3)

#define BT848_WC_UP            0x044
#define BT848_VTOTAL_LO        0x0B0
#define BT848_VTOTAL_HI        0x0B4
#define BT848_DVSIF            0x0FC

#define BT848_OFORM            0x048
#define BT848_OFORM_RANGE      (1<<7)
#define BT848_OFORM_CORE0      (0<<5)
#define BT848_OFORM_CORE8      (1<<5)
#define BT848_OFORM_CORE16     (2<<5)
#define BT848_OFORM_CORE32     (3<<5)

#define BT848_E_VSCALE_HI      0x04C
#define BT848_O_VSCALE_HI      0x0CC
#define BT848_VSCALE_YCOMB     (1<<7)
#define BT848_VSCALE_COMB      (1<<6)
#define BT848_VSCALE_INT       (1<<5)
#define BT848_VSCALE_HI        31

#define BT848_E_VSCALE_LO      0x050
#define BT848_O_VSCALE_LO      0x0D0
#define BT848_TEST             0x054
#define BT848_ADELAY           0x060
#define BT848_BDELAY           0x064

#define BT848_ADC              0x068
#define BT848_ADC_RESERVED     (2<<6)
#define BT848_ADC_SYNC_T       (1<<5)
#define BT848_ADC_AGC_EN       (1<<4)
#define BT848_ADC_CLK_SLEEP    (1<<3)
#define BT848_ADC_Y_SLEEP      (1<<2)
#define BT848_ADC_C_SLEEP      (1<<1)
#define BT848_ADC_CRUSH        (1<<0)

#define BT848_E_VTC            0x06C
#define BT848_O_VTC            0x0EC
#define BT848_VTC_HSFMT        (1<<7)
#define BT848_VTC_VFILT_2TAP   0
#define BT848_VTC_VFILT_3TAP   1
#define BT848_VTC_VFILT_4TAP   2
#define BT848_VTC_VFILT_5TAP   3

#define BT848_SRESET           0x07C

#define BT848_COLOR_FMT             0x0D4
#define BT848_COLOR_FMT_RAW         0xee

#define BT848_COLOR_CTL                0x0D8
#define BT848_COLOR_CTL_EXT_FRMRATE    (1<<7)
#define BT848_COLOR_CTL_COLOR_BARS     (1<<6)
#define BT848_COLOR_CTL_RGB_DED        (1<<5)
#define BT848_COLOR_CTL_GAMMA          (1<<4)
#define BT848_COLOR_CTL_WSWAP_ODD      (1<<3)
#define BT848_COLOR_CTL_WSWAP_EVEN     (1<<2)
#define BT848_COLOR_CTL_BSWAP_ODD      (1<<1)
#define BT848_COLOR_CTL_BSWAP_EVEN     (1<<0)

#define BT848_CAP_CTL                  0x0DC
#define BT848_CAP_CTL_DITH_FRAME       (1<<4)
#define BT848_CAP_CTL_CAPTURE_VBI_ODD  (1<<3)
#define BT848_CAP_CTL_CAPTURE_VBI_EVEN (1<<2)
#define BT848_CAP_CTL_CAPTURE_ODD      (1<<1)
#define BT848_CAP_CTL_CAPTURE_EVEN     (1<<0)

#define BT848_VBI_PACK_SIZE    0x0E0

#define BT848_VBI_PACK_DEL     0x0E4
#define BT848_VBI_PACK_DEL_VBI_HDELAY 0xfc
#define BT848_VBI_PACK_DEL_EXT_FRAME  2
#define BT848_VBI_PACK_DEL_VBI_PKT_HI 1

#define BT848_INT_STAT         0x100
#define BT848_INT_MASK         0x104

#define BT848_INT_ETBF         (1<<23)

#define BT848_INT_RISCS   (0xf<<28)
#define BT848_INT_RISC_EN (1<<27)
#define BT848_INT_RACK    (1<<25)
#define BT848_INT_FIELD   (1<<24)
#define BT848_INT_SCERR   (1<<19)
#define BT848_INT_OCERR   (1<<18)
#define BT848_INT_PABORT  (1<<17)
#define BT848_INT_RIPERR  (1<<16)
#define BT848_INT_PPERR   (1<<15)
#define BT848_INT_FDSR    (1<<14)
#define BT848_INT_FTRGT   (1<<13)
#define BT848_INT_FBUS    (1<<12)
#define BT848_INT_RISCI   (1<<11)
#define BT848_INT_GPINT   (1<<9)
#define BT848_INT_VPRES   (1<<5)
#define BT848_INT_HLOCK   (1<<4)
#define BT848_INT_OFLOW   (1<<3)
#define BT848_INT_HSYNC   (1<<2)
#define BT848_INT_VSYNC   (1<<1)
#define BT848_INT_FMTCHG  (1<<0)

#define BT848_GPIO_DMA_CTL             0x10C
#define BT848_GPIO_DMA_CTL_GPINTC      (1<<15)
#define BT848_GPIO_DMA_CTL_GPINTI      (1<<14)
#define BT848_GPIO_DMA_CTL_GPWEC       (1<<13)
#define BT848_GPIO_DMA_CTL_GPIOMODE    (3<<11)
#define BT848_GPIO_DMA_CTL_GPCLKMODE   (1<<10)
#define BT848_GPIO_DMA_CTL_PLTP23_4    (0<<6)
#define BT848_GPIO_DMA_CTL_PLTP23_8    (1<<6)
#define BT848_GPIO_DMA_CTL_PLTP23_16   (2<<6)
#define BT848_GPIO_DMA_CTL_PLTP23_32   (3<<6)
#define BT848_GPIO_DMA_CTL_PLTP1_4     (0<<4)
#define BT848_GPIO_DMA_CTL_PLTP1_8     (1<<4)
#define BT848_GPIO_DMA_CTL_PLTP1_16    (2<<4)
#define BT848_GPIO_DMA_CTL_PLTP1_32    (3<<4)
#define BT848_GPIO_DMA_CTL_PKTP_4      (0<<2)
#define BT848_GPIO_DMA_CTL_PKTP_8      (1<<2)
#define BT848_GPIO_DMA_CTL_PKTP_16     (2<<2)
#define BT848_GPIO_DMA_CTL_PKTP_32     (3<<2)
#define BT848_GPIO_DMA_CTL_RISC_ENABLE (1<<1)
#define BT848_GPIO_DMA_CTL_FIFO_ENABLE (1<<0)

#define BT848_RISC_STRT_ADD    0x114
#define BT848_GPIO_OUT_EN      0x118
#define BT848_GPIO_OUT_EN_HIBYTE      0x11A
#define BT848_GPIO_REG_INP     0x11C
#define BT848_GPIO_REG_INP_HIBYTE     0x11E
#define BT848_RISC_COUNT       0x120
#define BT848_GPIO_DATA        0x200
#define BT848_GPIO_DATA_HIBYTE 0x202

/* Bt848 RISC commands */

/* only for the SYNC RISC command */
#define BT848_FIFO_STATUS_FM1  0x06
#define BT848_FIFO_STATUS_FM3  0x0e
#define BT848_FIFO_STATUS_SOL  0x02
#define BT848_FIFO_STATUS_EOL4 0x01
#define BT848_FIFO_STATUS_EOL3 0x0d
#define BT848_FIFO_STATUS_EOL2 0x09
#define BT848_FIFO_STATUS_EOL1 0x05
#define BT848_FIFO_STATUS_VRE  0x04
#define BT848_FIFO_STATUS_VRO  0x0c
#define BT848_FIFO_STATUS_PXV  0x00

#define BT848_RISC_RESYNC      (1<<15)

/* WRITE and SKIP */
/* disable which bytes of each DWORD */
#define BT848_RISC_BYTE0       (1<<12)
#define BT848_RISC_BYTE1       (1<<13)
#define BT848_RISC_BYTE2       (1<<14)
#define BT848_RISC_BYTE3       (1<<15)
#define BT848_RISC_BYTE_ALL    (0x0f<<12)
#define BT848_RISC_BYTE_NONE   0
/* cause RISCI */
#define BT848_RISC_IRQ         (1<<24)
/* RISC command is last one in this line */
#define BT848_RISC_EOL         (1<<26)
/* RISC command is first one in this line */
#define BT848_RISC_SOL         (1<<27)

#define BT848_RISC_WRITE       (0x01<<28)
#define BT848_RISC_SKIP        (0x02<<28)
#define BT848_RISC_WRITEC      (0x05<<28)
#define BT848_RISC_JUMP        (0x07<<28)
#define BT848_RISC_SYNC        (0x08<<28)


class ServiceHandle : Uncopyable
{
public:
    ServiceHandle() : _hSCObject(NULL) { }
    ServiceHandle(SC_HANDLE hSCObject) : _hSCObject(hSCObject) { }
    const ServiceHandle& operator=(SC_HANDLE hSCObject)
    {
        reset();
        _hSCObject = hSCObject;
        return *this;
    }
    ~ServiceHandle() { reset(); }
    operator SC_HANDLE() { return _hSCObject; }
private:
    void reset()
    {
        if (_hSCObject != NULL)
            CloseServiceHandle(_hSCObject);
    }

    SC_HANDLE _hSCObject;
};


// define version number to be compiled into both files
// we use this to make sure that we are speaking the same language in both the
// drivers and the dll
#define DSDRV_COMPAT_MIN_VERSION    0x4003
#define DSDRV_COMPAT_MASK         (~0x0fff)
#define DSDRV_COMPAT_MAJ_VERSION  (DSDRV_COMPAT_MIN_VERSION & DSDRV_COMPAT_MASK)

#define ALLOC_MEMORY_CONTIG 1

typedef struct
{
    DWORD dwSize;
    DWORD dwPhysical;
} TPageStruct, * PPageStruct;

typedef struct
{
    DWORD  dwMemoryAddress;
    DWORD  dwMemoryLength;
    DWORD  dwSubSystemId;
    DWORD  dwBusNumber;
    DWORD  dwSlotNumber;
} TPCICARDINFO;


/** Define the IOCTL's used by DSDrv
    We need to ensure that we don't conflict with other drivers
    so move up both device and base for function code
*/


#define FILE_DEVICE_DSCALER 0x8D00
#define DSDRV_BASE          0xA00

#define IOCTL_DSDRV_READPORTBYTE  \
    CTL_CODE(FILE_DEVICE_DSCALER, (DSDRV_BASE + 1), METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_DSDRV_READPORTWORD \
    CTL_CODE(FILE_DEVICE_DSCALER, (DSDRV_BASE + 2), METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_DSDRV_READPORTDWORD \
    CTL_CODE(FILE_DEVICE_DSCALER, (DSDRV_BASE + 3), METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_DSDRV_WRITEPORTBYTE \
    CTL_CODE(FILE_DEVICE_DSCALER, (DSDRV_BASE + 4), METHOD_IN_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_DSDRV_WRITEPORTWORD \
    CTL_CODE(FILE_DEVICE_DSCALER, (DSDRV_BASE + 5), METHOD_IN_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_DSDRV_WRITEPORTDWORD \
    CTL_CODE(FILE_DEVICE_DSCALER, (DSDRV_BASE + 6), METHOD_IN_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_DSDRV_ALLOCMEMORY \
    CTL_CODE(FILE_DEVICE_DSCALER, (DSDRV_BASE + 7), METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_DSDRV_FREEMEMORY  \
    CTL_CODE(FILE_DEVICE_DSCALER, (DSDRV_BASE + 8), METHOD_IN_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_DSDRV_GETPCIINFO \
    CTL_CODE(FILE_DEVICE_DSCALER, (DSDRV_BASE + 9), METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_DSDRV_MAPMEMORY \
    CTL_CODE(FILE_DEVICE_DSCALER, (DSDRV_BASE + 10), METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_DSDRV_UNMAPMEMORY \
    CTL_CODE(FILE_DEVICE_DSCALER, (DSDRV_BASE + 11), METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_DSDRV_READMEMORYDWORD \
    CTL_CODE(FILE_DEVICE_DSCALER, (DSDRV_BASE + 12), METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_DSDRV_WRITEMEMORYDWORD \
    CTL_CODE(FILE_DEVICE_DSCALER, (DSDRV_BASE + 13), METHOD_IN_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_DSDRV_READMEMORYWORD \
    CTL_CODE(FILE_DEVICE_DSCALER, (DSDRV_BASE + 14), METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_DSDRV_WRITEMEMORYWORD \
    CTL_CODE(FILE_DEVICE_DSCALER, (DSDRV_BASE + 15), METHOD_IN_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_DSDRV_READMEMORYBYTE \
    CTL_CODE(FILE_DEVICE_DSCALER, (DSDRV_BASE + 16), METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_DSDRV_WRITEMEMORYBYTE \
    CTL_CODE(FILE_DEVICE_DSCALER, (DSDRV_BASE + 17), METHOD_IN_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_DSDRV_GETVERSION \
    CTL_CODE(FILE_DEVICE_DSCALER, (DSDRV_BASE + 18), METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_DSDRV_GETPCICONFIG \
    CTL_CODE(FILE_DEVICE_DSCALER, (DSDRV_BASE + 19), METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_DSDRV_SETPCICONFIG \
    CTL_CODE(FILE_DEVICE_DSCALER, (DSDRV_BASE + 20), METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_DSDRV_GETPCICONFIGOFFSET \
    CTL_CODE(FILE_DEVICE_DSCALER, (DSDRV_BASE + 21), METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_DSDRV_SETPCICONFIGOFFSET \
    CTL_CODE(FILE_DEVICE_DSCALER, (DSDRV_BASE + 22), METHOD_OUT_DIRECT, FILE_ANY_ACCESS)


typedef struct tagDSDrvParam
{
    DWORD   dwAddress;
    DWORD   dwValue;
    DWORD   dwFlags;
} TDSDrvParam, * PDSDrvParam;

//---------------------------------------------------------------------------
// This structure is taken from NTDDK.H, we use this only in WIN32 user mode
//---------------------------------------------------------------------------

typedef struct _PCI_COMMON_CONFIG
{
    USHORT  VendorID;                   // (ro)
    USHORT  DeviceID;                   // (ro)
    USHORT  Command;                    // Device control
    USHORT  Status;
    UCHAR   RevisionID;                 // (ro)
    UCHAR   ProgIf;                     // (ro)
    UCHAR   SubClass;                   // (ro)
    UCHAR   BaseClass;                  // (ro)
    UCHAR   CacheLineSize;              // (ro+)
    UCHAR   LatencyTimer;               // (ro+)
    UCHAR   HeaderType;                 // (ro)
    UCHAR   BIST;                       // Built in self test

    union
    {
        struct _PCI_HEADER_TYPE_0
        {
            DWORD   BaseAddresses[6];
            DWORD   CIS;
            USHORT  SubVendorID;
            USHORT  SubSystemID;
            DWORD   ROMBaseAddress;
            DWORD   Reserved2[2];

            UCHAR   InterruptLine;      //
            UCHAR   InterruptPin;       // (ro)
            UCHAR   MinimumGrant;       // (ro)
            UCHAR   MaximumLatency;     // (ro)
        } type0;
    } u;
    UCHAR   DeviceSpecific[192];
} PCI_COMMON_CONFIG, *PPCI_COMMON_CONFIG;


DWORD HwDrv_SendCommandEx(
               DWORD dwIOCommand,
               LPVOID pvInput,
               DWORD dwInputLength,
               LPVOID pvOutput,
               DWORD dwOutputLength,
               LPDWORD pdwReturnedLength
            );

DWORD HwDrv_SendCommand(
               DWORD dwIOCommand,
               LPVOID pvInput,
               DWORD dwInputLength
            );

void HwPci_WriteByte(DWORD Offset, BYTE Data);
void HwPci_WriteWord(DWORD Offset, WORD Data);
void HwPci_WriteDword(DWORD Offset, DWORD Data);

BYTE HwPci_ReadByte(DWORD Offset);
WORD HwPci_ReadWord(DWORD Offset);
DWORD HwPci_ReadDword(DWORD Offset);

void HwPci_MaskDataByte(DWORD Offset, BYTE Data, BYTE Mask);
void HwPci_MaskDataWord(DWORD Offset, WORD Data, WORD Mask);
void HwPci_MaskDataDword(DWORD Offset, DWORD Data, DWORD Mask);
void HwPci_AndOrDataByte(DWORD Offset, BYTE Data, BYTE Mask);
void HwPci_AndOrDataWord(DWORD Offset, WORD Data, WORD Mask);
void HwPci_AndOrDataDword(DWORD Offset, DWORD Data, DWORD Mask);
void HwPci_AndDataByte(DWORD Offset, BYTE Data);
void HwPci_AndDataWord(DWORD Offset, WORD Data);
void HwPci_AndDataDword(DWORD Offset, DWORD Data);
void HwPci_OrDataByte(DWORD Offset, BYTE Data);
void HwPci_OrDataWord(DWORD Offset, WORD Data);
void HwPci_OrDataDword(DWORD Offset, DWORD Data);

BOOL HwPci_GetPCIConfig(PCI_COMMON_CONFIG* pPCI_COMMON_CONFIG, DWORD Bus, DWORD Slot);
BOOL HwPci_SetPCIConfig(PCI_COMMON_CONFIG* pPCI_COMMON_CONFIG, DWORD Bus, DWORD Slot);
BOOL HwPci_GetPCIConfigOffset(BYTE* pbPCIConfig, DWORD Offset, DWORD Bus, DWORD Slot);
BOOL HwPci_SetPCIConfigOffset(BYTE* pbPCIConfig, DWORD Offset, DWORD Bus, DWORD Slot);

void WriteByte(DWORD Offset, BYTE Data);
void WriteWord(DWORD Offset, WORD Data);
void WriteDword(DWORD Offset, DWORD Data);
BYTE ReadByte(DWORD Offset);
WORD ReadWord(DWORD Offset);
DWORD ReadDword(DWORD Offset);

void MaskDataByte(DWORD Offset, BYTE Data, BYTE Mask);
void MaskDataWord(DWORD Offset, WORD Data, WORD Mask);
void MaskDataDword(DWORD Offset, DWORD Data, DWORD Mask);
void AndDataByte(DWORD Offset, BYTE Data);
void AndDataWord (DWORD Offset, WORD Data);
void AndDataDword (DWORD Offset, DWORD Data);
void OrDataByte(DWORD Offset, BYTE Data);
void OrDataWord (DWORD Offset, WORD Data);
void OrDataDword (DWORD Offset, DWORD Data);

void HwPci_RestoreState( void );
void ManageDword(DWORD Offset);
void ManageWord(DWORD Offset);
void ManageByte(DWORD Offset);

void DsDrv_LockCard( void );
void DsDrv_UnlockCard( void );

typedef struct
{
   DWORD  BusNumber;
   DWORD  SlotNumber;
   DWORD  SubSystemId;
} TVCARD_PARAMS;

typedef struct TVCARD_struct
{
   TVCARD_PARAMS       params;
} TVCARD;

typedef enum
{
   BTDRV_SOURCE_WDM = 0,
   BTDRV_SOURCE_PCI = 1,
   BTDRV_SOURCE_NONE = -1,
   BTDRV_SOURCE_UNDEF = -2
} BTDRV_SOURCE_TYPE;

#define PCI_ID_BROOKTREE  0x109e


// ----------------------------------------------------------------------------
// Declaration of internal variables
//
#define VBI_LINES_PER_FIELD      450
#define VBI_LINES_PER_FRAME     (VBI_LINES_PER_FIELD * 2)
#define VBI_LINE_SIZE            2048
#define VBI_DATA_SIZE           (VBI_LINE_SIZE * VBI_LINES_PER_FRAME)
#define VBI_DMA_PAGE_SIZE        4096
#define VBI_SPL                  1028
#define VDELAY                   2
#define HDELAY                   2

#define VBI_FRAME_CAPTURE_COUNT   5
#define VBI_FIELD_CAPTURE_COUNT  (VBI_FRAME_CAPTURE_COUNT * 2)


#define RISC_CODE_LENGTH         (4096 + VBI_LINES_PER_FIELD*8*VBI_FIELD_CAPTURE_COUNT) // currently apx. 36 DWORDs per field, total 1488
typedef DWORD PHYS;

AutoHandle m_hFile;
DWORD HwDrv_SendCommandEx( DWORD dwIOCommand,
                           LPVOID pvInput,
                           DWORD dwInputLength,
                           LPVOID pvOutput,
                           DWORD dwOutputLength,
                           LPDWORD pdwReturnedLength
                         )
{
    if (DeviceIoControl(
                        m_hFile,
                        dwIOCommand,
                        pvInput,
                        dwInputLength,
                        pvOutput,
                        dwOutputLength,
                        pdwReturnedLength,
                        NULL
                      ))
    {
        return 0;
    }
    else
    {
        // Suppress the error when DoesThisPCICardExist() probes for a non-existing card
        return GetLastError();
    }
}

DWORD HwDrv_SendCommand( DWORD dwIOCommand,
                         LPVOID pvInput,
                         DWORD dwInputLength
                        )
{
    DWORD dwDummy;

    if(DeviceIoControl(
                        m_hFile,
                        dwIOCommand,
                        pvInput,
                        dwInputLength,
                        NULL,
                        0,
                        &dwDummy,
                        NULL
                      ))
    {
        return 0;
    }
    else
    {
        return GetLastError();
    }
}


typedef struct
{
    DWORD dwTotalSize;
    DWORD dwPages;
    DWORD dwHandle;
    DWORD dwFlags;
    void* dwUser;
} TMemStruct, * PMemStruct;

class HardwareMemory
{
public:
    HardwareMemory() : _valid(false) { }

    void* GetUserPointer()
    {
        if (pMemStruct != NULL)
            return pMemStruct->dwUser;
        else
            return NULL;
    }

    DWORD TranslateToPhysical( void * pUser, DWORD dwSizeWanted, DWORD* pdwSizeAvailable )
    {
        if(pMemStruct != NULL) {
            TPageStruct* pPages = (TPageStruct*)(pMemStruct + 1);
            DWORD Offset = (DWORD)pUser - (DWORD)pMemStruct->dwUser;;
            DWORD i = 0;
            DWORD sum = 0;
            DWORD pRetVal = 0;

            while (i < pMemStruct->dwPages) {
                if (sum + pPages[i].dwSize > (unsigned)Offset) {
                    Offset -= sum;
                    pRetVal = pPages[i].dwPhysical + Offset;
                    if ( pdwSizeAvailable != NULL )
                        *pdwSizeAvailable = pPages[i].dwSize - Offset;
                    break;
                }
                sum += pPages[i].dwSize;
                i++;
            }

            return pRetVal;
        }
        else
            return 0;
    }

protected:
    bool _valid;
    TMemStruct  * pMemStruct;
    void        * AllocatedBlock;
};

/** Memory that is allocated in user space and mapped to driver space */
class UserMemory : public HardwareMemory
{
public:
    BOOL alloc( size_t Bytes )
    {
        TDSDrvParam paramIn;
        DWORD dwReturnedLength;
        DWORD status;
        DWORD nPages = 0;
        DWORD dwOutParamLength;

        pMemStruct = 0;

        AllocatedBlock = malloc(Bytes + 0xFFF);
        if(AllocatedBlock == NULL)
            return FALSE;

        memset(AllocatedBlock, 0, Bytes + 0xFFF);

        nPages = Bytes / 0xFFF + 1;

        dwOutParamLength = sizeof(TMemStruct) + nPages * sizeof(TPageStruct);
        pMemStruct = (TMemStruct*) malloc(dwOutParamLength);
        if (pMemStruct == NULL)
        {
            free(AllocatedBlock);
            AllocatedBlock = NULL;
            return FALSE;
        }

        memset(pMemStruct, 0, dwOutParamLength);

        paramIn.dwValue = Bytes;
        paramIn.dwFlags = 0;

        // align memory to page boundary
        if(((DWORD)AllocatedBlock & 0xFFFFF000) < (DWORD)AllocatedBlock)
            paramIn.dwAddress = (((DWORD)AllocatedBlock + 0xFFF) & 0xFFFFF000);
        else
            paramIn.dwAddress = (DWORD)AllocatedBlock;

        status = HwDrv_SendCommandEx(IOCTL_DSDRV_ALLOCMEMORY,
                                &paramIn,
                                sizeof(paramIn),
                                pMemStruct,
                                dwOutParamLength,
                                &dwReturnedLength);

        if(status != ERROR_SUCCESS || pMemStruct->dwUser == 0)
        {
            free(pMemStruct);
            free((void*)AllocatedBlock);
            AllocatedBlock = NULL;
            pMemStruct = NULL;
            return FALSE;
        }
        _valid = true;
        return TRUE;
    }

    ~UserMemory()
    {
        if (!_valid)
            return;
        if (pMemStruct != NULL)
        {
            DWORD dwInParamLength = sizeof(TMemStruct) + pMemStruct->dwPages * sizeof(TPageStruct);
            HwDrv_SendCommand(IOCTL_DSDRV_FREEMEMORY, pMemStruct, dwInParamLength);
            free(pMemStruct);
        }
        if (AllocatedBlock != NULL)
            free((void*)AllocatedBlock);
    }

private:
    bool _valid;
};


class ContigMemory : public HardwareMemory
{
public:
    BOOL alloc(size_t bytes)
    {
        TDSDrvParam paramIn;
        DWORD dwReturnedLength;
        DWORD status;

        DWORD dwOutParamLength = sizeof(TMemStruct) + sizeof(TPageStruct);
        pMemStruct = (TMemStruct*) malloc(dwOutParamLength);
        if (pMemStruct == NULL)
            return FALSE;

        paramIn.dwValue = bytes;
        paramIn.dwFlags = ALLOC_MEMORY_CONTIG;
        paramIn.dwAddress = 0;
        status = HwDrv_SendCommandEx(IOCTL_DSDRV_ALLOCMEMORY,
                                &paramIn,
                                sizeof(paramIn),
                                pMemStruct,
                                dwOutParamLength,
                                &dwReturnedLength);

        if(status != ERROR_SUCCESS || pMemStruct->dwUser == 0)
        {
            free(pMemStruct);
            return FALSE;
        }
        _valid = true;
        return TRUE;
    }

    ~ContigMemory()
    {
        if (!_valid)
            return;
        if(pMemStruct != NULL)
        {
            DWORD dwInParamLength = sizeof(TMemStruct) + sizeof(TPageStruct);
            HwDrv_SendCommand(IOCTL_DSDRV_FREEMEMORY, pMemStruct, dwInParamLength);
            free(pMemStruct);
        }
    }
private:
    bool _valid;
};



static DWORD  m_BusNumber;
static DWORD  m_SlotNumber;
static DWORD  m_MemoryBase;
static DWORD  m_InitialACPIStatus;

// forward declarations
static void HwPci_SetACPIStatus(int ACPIStatus);

static CRITICAL_SECTION m_CriticalSection;

void HwPci_WriteByte(DWORD Offset, BYTE Data)
{
    TDSDrvParam hwParam;
    DWORD dwStatus;

    hwParam.dwAddress = m_MemoryBase + Offset;
    hwParam.dwValue = Data;

    dwStatus = HwDrv_SendCommand(IOCTL_DSDRV_WRITEMEMORYBYTE, &hwParam, sizeof(hwParam));
}

void HwPci_WriteWord(DWORD Offset, WORD Data)
{
    TDSDrvParam hwParam;
    DWORD dwStatus;

    hwParam.dwAddress = m_MemoryBase + Offset;
    hwParam.dwValue = Data;

    dwStatus = HwDrv_SendCommand(IOCTL_DSDRV_WRITEMEMORYWORD, &hwParam, sizeof(hwParam));
}

void HwPci_WriteDword(DWORD Offset, DWORD Data)
{
    TDSDrvParam hwParam;
    DWORD dwStatus;

    hwParam.dwAddress = m_MemoryBase + Offset;
    hwParam.dwValue = Data;

    dwStatus = HwDrv_SendCommand(IOCTL_DSDRV_WRITEMEMORYDWORD, &hwParam, sizeof(hwParam));
}

BYTE HwPci_ReadByte(DWORD Offset)
{
    TDSDrvParam hwParam;
    DWORD dwReturnedLength;
    BYTE bValue = 0;
    DWORD dwStatus;

    hwParam.dwAddress = m_MemoryBase + Offset;

    dwStatus = HwDrv_SendCommandEx(IOCTL_DSDRV_READMEMORYBYTE,
                                            &hwParam,
                                            sizeof(hwParam.dwAddress),
                                            &bValue,
                                            sizeof(bValue),
                                            &dwReturnedLength);
    return bValue;
}

WORD HwPci_ReadWord(DWORD Offset)
{
    TDSDrvParam hwParam;
    DWORD dwReturnedLength;
    WORD wValue = 0;
    DWORD dwStatus;

    hwParam.dwAddress = m_MemoryBase + Offset;

    dwStatus = HwDrv_SendCommandEx(IOCTL_DSDRV_READMEMORYWORD,
                                            &hwParam,
                                            sizeof(hwParam.dwAddress),
                                            &wValue,
                                            sizeof(wValue),
                                            &dwReturnedLength);
    return wValue;
}

DWORD HwPci_ReadDword(DWORD Offset)
{
    TDSDrvParam hwParam;
    DWORD dwReturnedLength;
    DWORD dwValue = 0;
    DWORD dwStatus;

    hwParam.dwAddress = m_MemoryBase + Offset;

    dwStatus = HwDrv_SendCommandEx(IOCTL_DSDRV_READMEMORYDWORD,
                                            &hwParam,
                                            sizeof(hwParam.dwAddress),
                                            &dwValue,
                                            sizeof(dwValue),
                                            &dwReturnedLength);
    return dwValue;
}


void HwPci_MaskDataByte(DWORD Offset, BYTE Data, BYTE Mask)
{
    BYTE Result;

    Result = HwPci_ReadByte(Offset);
    Result = (Result & ~Mask) | (Data & Mask);
    HwPci_WriteByte(Offset, Result);
}

void HwPci_MaskDataWord(DWORD Offset, WORD Data, WORD Mask)
{
    WORD Result;
    Result = HwPci_ReadWord(Offset);
    Result = (Result & ~Mask) | (Data & Mask);
    HwPci_WriteWord(Offset, Result);
}

void HwPci_MaskDataDword(DWORD Offset, DWORD Data, DWORD Mask)
{
    DWORD Result;

    Result = HwPci_ReadDword(Offset);
    Result = (Result & ~Mask) | (Data & Mask);
    HwPci_WriteDword(Offset, Result);
}

void HwPci_AndOrDataByte(DWORD Offset, BYTE Data, BYTE Mask)
{
    BYTE Result;

    Result = HwPci_ReadByte(Offset);
    Result = (Result & Mask) | Data;
    HwPci_WriteByte(Offset, Result);
}

void HwPci_AndOrDataWord(DWORD Offset, WORD Data, WORD Mask)
{
    WORD Result;

    Result = HwPci_ReadWord(Offset);
    Result = (Result & Mask) | Data;
    HwPci_WriteWord(Offset, Result);
}

void HwPci_AndOrDataDword(DWORD Offset, DWORD Data, DWORD Mask)
{
    DWORD Result;

    Result = HwPci_ReadDword(Offset);
    Result = (Result & Mask) | Data;
    HwPci_WriteDword(Offset, Result);
}

void HwPci_AndDataByte(DWORD Offset, BYTE Data)
{
    BYTE Result;

    Result = HwPci_ReadByte(Offset);
    Result &= Data;
    HwPci_WriteByte(Offset, Result);
}

void HwPci_AndDataWord(DWORD Offset, WORD Data)
{
    WORD Result;

    Result = HwPci_ReadWord(Offset);
    Result &= Data;
    HwPci_WriteWord(Offset, Result);
}

void HwPci_AndDataDword(DWORD Offset, DWORD Data)
{
    DWORD Result;

    Result = HwPci_ReadDword(Offset);
    Result &= Data;
    HwPci_WriteDword(Offset, Result);
}

void HwPci_OrDataByte(DWORD Offset, BYTE Data)
{
    BYTE Result;

    Result = HwPci_ReadByte(Offset);
    Result |= Data;
    HwPci_WriteByte(Offset, Result);
}

void HwPci_OrDataWord(DWORD Offset, WORD Data)
{
    WORD Result;

    Result = HwPci_ReadWord(Offset);
    Result |= Data;
    HwPci_WriteWord(Offset, Result);
}

void HwPci_OrDataDword(DWORD Offset, DWORD Data)
{
    DWORD Result;

    Result = HwPci_ReadDword(Offset);
    Result |= Data;
    HwPci_WriteDword(Offset, Result);
}


BOOL HwPci_GetPCIConfig(PCI_COMMON_CONFIG* pPCI_COMMON_CONFIG, DWORD Bus, DWORD Slot)
{
    TDSDrvParam hwParam;
    DWORD dwStatus;
    DWORD dwLength;

    if(pPCI_COMMON_CONFIG == NULL)
    {
        return FALSE;
    }

    hwParam.dwAddress = Bus;
    hwParam.dwValue = Slot;

    dwStatus = HwDrv_SendCommandEx(IOCTL_DSDRV_GETPCICONFIG,
                                        &hwParam,
                                        sizeof(hwParam),
                                        pPCI_COMMON_CONFIG,
                                        sizeof(PCI_COMMON_CONFIG),
                                        &dwLength);

    if(dwStatus != ERROR_SUCCESS)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL HwPci_SetPCIConfig(PCI_COMMON_CONFIG* pPCI_COMMON_CONFIG, DWORD Bus, DWORD Slot)
{
    TDSDrvParam hwParam;
    DWORD dwStatus;
    DWORD dwLength;

    if(pPCI_COMMON_CONFIG == NULL)
    {
        return FALSE;
    }

    hwParam.dwAddress = Bus;
    hwParam.dwValue = Slot;

    dwStatus = HwDrv_SendCommandEx(IOCTL_DSDRV_SETPCICONFIG,
                                        &hwParam,
                                        sizeof(hwParam),
                                        pPCI_COMMON_CONFIG,
                                        sizeof(PCI_COMMON_CONFIG),
                                        &dwLength);

    if(dwStatus != ERROR_SUCCESS)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL HwPci_GetPCIConfigOffset(BYTE* pbPCIConfig, DWORD Offset, DWORD Bus, DWORD Slot)
{
    TDSDrvParam hwParam;
    DWORD dwStatus;
    DWORD dwLength;

    if(pbPCIConfig == NULL)
    {
        return FALSE;
    }

    hwParam.dwAddress = Bus;
    hwParam.dwValue = Slot;
    hwParam.dwFlags = Offset;

    dwStatus = HwDrv_SendCommandEx(IOCTL_DSDRV_GETPCICONFIGOFFSET,
                                        &hwParam,
                                        sizeof(hwParam),
                                        pbPCIConfig,
                                        1,
                                        &dwLength);

    if(dwStatus != ERROR_SUCCESS)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL HwPci_SetPCIConfigOffset(BYTE* pbPCIConfig, DWORD Offset, DWORD Bus, DWORD Slot)
{
    TDSDrvParam hwParam;
    DWORD dwStatus;
    DWORD dwLength;

    if(pbPCIConfig == NULL)
    {
        return FALSE;
    }

    hwParam.dwAddress = Bus;
    hwParam.dwValue = Slot;
    hwParam.dwFlags = Offset;

    dwStatus = HwDrv_SendCommandEx(IOCTL_DSDRV_SETPCICONFIGOFFSET,
                                        &hwParam,
                                        sizeof(hwParam),
                                        pbPCIConfig,
                                        1,
                                        &dwLength);

    if(dwStatus != ERROR_SUCCESS)
    {
        return FALSE;
    }
    return TRUE;
}

// ----------------------------------------------------------------------------
// Initialize PCI card
// - callback, invoked by the PCI driver during initial card setup
//
static void Bt8x8_ResetChip( DWORD m_BusNumber, DWORD m_SlotNumber )
{
   BYTE Command = 0;

   // try and switch on the card using the PCI Command value
   // this is to try and solve problems when a driver hasn't been
   // loaded for the card, which may be necessary when you have
   // multiple cards
   if (HwPci_GetPCIConfigOffset(&Command, 0x04, m_BusNumber, m_SlotNumber))
   {
      // switch on allow master and respond to memory requests
      if ((Command & 0x06) != 0x06)
      {
         Command |= 0x06;
         HwPci_SetPCIConfigOffset(&Command, 0x04, m_BusNumber, m_SlotNumber);
         Sleep(500);
      }
   }

   WriteByte(BT848_SRESET, 0);
}


// Set ACPIStatus to 0 for D0/full on state. 3 for D3/full off
static void HwPci_SetACPIStatus(int ACPIStatus)
{
    // only some cards are able to power down

    BYTE ACPIStatusNew = 0;
    if(HwPci_GetPCIConfigOffset(&ACPIStatusNew, 0x50, m_BusNumber, m_SlotNumber))
    {
        ACPIStatusNew &= ~3;
        ACPIStatusNew |= ACPIStatus;

        HwPci_SetPCIConfigOffset(&ACPIStatusNew, 0x50, m_BusNumber, m_SlotNumber);

        if(ACPIStatus == 0)
        {
            Sleep(500);
            Bt8x8_ResetChip(m_BusNumber, m_SlotNumber);
        }
    }
}

static BOOL  CardOpened = FALSE;     // BT Card has been opened ?

void WriteByte(DWORD Offset, BYTE Data)
{
    if (!CardOpened) return;
    HwPci_WriteByte(Offset, Data);
}
void WriteWord(DWORD Offset, WORD Data)
{
    if (!CardOpened) return;
    HwPci_WriteWord(Offset, Data);
}
void WriteDword(DWORD Offset, DWORD Data)
{
    if (!CardOpened) return;
    HwPci_WriteDword(Offset, Data);
}

BYTE ReadByte(DWORD Offset)
{
    if (!CardOpened) return 0;
    return HwPci_ReadByte(Offset);
}
WORD ReadWord(DWORD Offset)
{
    if (!CardOpened) return 0;
    return HwPci_ReadWord(Offset);
}
DWORD ReadDword(DWORD Offset)
{
    if (!CardOpened) return 0;
    return HwPci_ReadDword(Offset);
}

void MaskDataByte(DWORD Offset, BYTE Data, BYTE Mask)
{
    if (!CardOpened) return;
    HwPci_MaskDataByte(Offset, Data, Mask);
}
void MaskDataWord(DWORD Offset, WORD Data, WORD Mask)
{
    if (!CardOpened) return;
    HwPci_MaskDataWord(Offset, Data, Mask);
}
void MaskDataDword(DWORD Offset, DWORD Data, DWORD Mask)
{
    if (!CardOpened) return;
    HwPci_MaskDataDword(Offset, Data, Mask);
}
void AndDataByte(DWORD Offset, BYTE Data)
{
    if (!CardOpened) return;
    HwPci_AndDataByte(Offset, Data);
}
void AndDataWord (DWORD Offset, WORD Data)
{
    if (!CardOpened) return;
    HwPci_AndDataWord(Offset, Data);
}
void AndDataDword (DWORD Offset, DWORD Data)
{
    if (!CardOpened) return;
    HwPci_AndDataDword(Offset, Data);
}

void OrDataByte(DWORD Offset, BYTE Data)
{
    if (!CardOpened) return;
    HwPci_OrDataByte(Offset, Data);
}
void OrDataWord (DWORD Offset, WORD Data)
{
    if (!CardOpened) return;
    HwPci_OrDataWord(Offset, Data);
}
void OrDataDword (DWORD Offset, DWORD Data)
{
    if (!CardOpened) return;
    HwPci_OrDataDword(Offset, Data);
}


class DMAEnable
{
public:
    DMAEnable()
    {
        MaskDataByte(BT848_CAP_CTL, BT848_CAP_CTL_CAPTURE_EVEN | BT848_CAP_CTL_CAPTURE_ODD, 0x0f);
        OrDataWord (BT848_GPIO_DMA_CTL, 3);
    }
    ~DMAEnable()
    {
        AndDataWord (BT848_GPIO_DMA_CTL, ~3);
        MaskDataByte(BT848_CAP_CTL, 0, 0x0f);      
    }
};


class Program : public ProgramBase
{
public:
    void run()
    {
        ServiceHandle m_hService;
        {
            ServiceHandle hSCManager(OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT));
            IF_NULL_THROW(hSCManager);

            m_hService = OpenService(hSCManager, L"DSDrv4", SERVICE_START | SERVICE_STOP);
            IF_NULL_THROW(m_hService);
        }

        if (StartService(m_hService, 0, NULL) == FALSE)
        {
            IF_FALSE_THROW(GetLastError() == ERROR_SERVICE_ALREADY_RUNNING);
        }

        m_hFile = CreateFile(
                             L"\\\\.\\DSDrv4",
                             GENERIC_READ | GENERIC_WRITE,
                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                             NULL,
                             OPEN_EXISTING,
                             0,
                             INVALID_HANDLE_VALUE
                            );
        IF_FALSE_THROW(m_hFile != INVALID_HANDLE_VALUE);

        // OK so we've loaded the driver
        // we had better check that it's the same version as we are
        // otherwise all sorts of nasty things could happen
        // n.b. note that if someone else has already loaded our driver this may
        // happen.

        DWORD dwReturnedLength;
        DWORD dwVersion = 0;
        HwDrv_SendCommandEx(
                    IOCTL_DSDRV_GETVERSION,
                    NULL,
                    0,
                    &dwVersion,
                    sizeof(dwVersion),
                    &dwReturnedLength
                   );
        if ( (dwVersion < DSDRV_COMPAT_MIN_VERSION) ||
             ((dwVersion & DSDRV_COMPAT_MASK) != DSDRV_COMPAT_MAJ_VERSION) )
            throw Exception("Incompatible driver version.");

        m_MemoryBase = 0;
        m_InitialACPIStatus = 0;

        bool foundCard = false;
        int deviceId;
        DWORD dwBusNumber;
        DWORD dwSlotNumber;

        static const int deviceIds[] = {
            0x036e,   // Brooktree Bt878
            0x036f,   // Brooktree Bt878A
            0x0350,   // Brooktree Bt848
            0x0351};  // Brooktree Bt849

        for (int chipIdx = 0; chipIdx < 4; ++chipIdx) {
            TDSDrvParam hwParam;
            DWORD dwStatus;
            DWORD dwLength;
            TPCICARDINFO PCICardInfo;

            hwParam.dwAddress = PCI_ID_BROOKTREE;
            deviceId = deviceIds[chipIdx];
            hwParam.dwValue = deviceId;
            hwParam.dwFlags = 0;

            dwStatus = HwDrv_SendCommandEx(
                                    IOCTL_DSDRV_GETPCIINFO,
                                    &hwParam,
                                    sizeof(hwParam),
                                    &PCICardInfo,
                                    sizeof(TPCICARDINFO),
                                    &dwLength
                                  );

            if (dwStatus == ERROR_SUCCESS) {
                dwBusNumber   = PCICardInfo.dwBusNumber;
                dwSlotNumber  = PCICardInfo.dwSlotNumber;
                foundCard = true;
                break;
            }
        }
        if (!foundCard)
            throw Exception("No Bt8x8 capture card found on PCI bus.");

        BOOL supportsAcpi;

        // TRUE if BT878 or BT878A is present
        supportsAcpi = (deviceId == 0x036E || deviceId == 0x036F);

        TDSDrvParam hwParam;
        DWORD dwStatus;
        DWORD dwLength;
        TPCICARDINFO PCICardInfo;

        hwParam.dwAddress = PCI_ID_BROOKTREE;
        hwParam.dwValue = deviceId;
        hwParam.dwFlags = 0;

        dwStatus = HwDrv_SendCommandEx(IOCTL_DSDRV_GETPCIINFO,
                                            &hwParam,
                                            sizeof(hwParam),
                                            &PCICardInfo,
                                            sizeof(TPCICARDINFO),
                                            &dwLength);
        if (dwStatus != ERROR_SUCCESS)
            throw Exception("Could not open capture card.");

        DWORD m_MemoryLength = PCICardInfo.dwMemoryLength;
        m_BusNumber = PCICardInfo.dwBusNumber;
        m_SlotNumber = PCICardInfo.dwSlotNumber;

        hwParam.dwAddress = m_BusNumber;
        hwParam.dwValue = PCICardInfo.dwMemoryAddress;
        hwParam.dwFlags = m_MemoryLength;

        dwStatus = HwDrv_SendCommandEx(IOCTL_DSDRV_MAPMEMORY,
                                            &hwParam,
                                            sizeof(hwParam),
                                            &(m_MemoryBase),
                                            sizeof(DWORD),
                                            &dwReturnedLength);
        if (dwStatus != ERROR_SUCCESS)
            throw Exception("Could not open capture card.");

        m_InitialACPIStatus = 0;
        if (supportsAcpi) {
            // this functions returns 0 if the card is in ACPI state D0 or on error
            // returns 3 if in D3 state (full off)
            // only some cards are able to power down
            BYTE ACPIStatus = 0;
            if (HwPci_GetPCIConfigOffset(&ACPIStatus, 0x50, m_BusNumber, m_SlotNumber)) {
                ACPIStatus &= 3;
                m_InitialACPIStatus = ACPIStatus;
            }
            // if the chip is powered down we need to power it up
            if(m_InitialACPIStatus != 0)
                HwPci_SetACPIStatus(0);
        }

        CardOpened = TRUE;

        ContigMemory riscMemory;
        if (riscMemory.alloc(RISC_CODE_LENGTH) == FALSE)
           throw Exception("Failed to allocate RISC memory.");

        UserMemory userMemory[VBI_FRAME_CAPTURE_COUNT];
        for (int idx=0; idx < VBI_FRAME_CAPTURE_COUNT; idx++)
           if (userMemory[idx].alloc(VBI_LINE_SIZE * VBI_LINES_PER_FIELD * 2) == FALSE)
              throw Exception("Failed to allocate frame buffer memory.");

        Bt8x8_ResetChip(dwBusNumber, dwSlotNumber);

        // software reset, sets all registers to reset default values
        WriteByte (BT848_SRESET, 0);
        Sleep(50);

        WriteByte (BT848_TDEC, 0x00);
        WriteByte (BT848_COLOR_CTL, BT848_COLOR_CTL_GAMMA);
        WriteByte (BT848_ADELAY, 0x7f);
        // disable capturing
        WriteByte (BT848_BDELAY, 0x72);
        WriteByte (BT848_CAP_CTL, 0x00);
        // max length of a VBI line
        WriteByte (BT848_VBI_PACK_SIZE, 0xff);
        WriteByte (BT848_VBI_PACK_DEL, 1 | BT848_VBI_PACK_DEL_EXT_FRAME);

        // YUV 4:2:2 linear pixel format
        WriteByte(BT848_COLOR_FMT, BT848_COLOR_FMT_RAW);

        WriteByte (BT848_E_VDELAY_LO, VDELAY & 0xff);
        WriteByte (BT848_O_VDELAY_LO, VDELAY & 0xff);
        int crop = ((VBI_SPL >> 8) & 3) |
                   (((HDELAY >> 8) & 3) << 2) |
                   (((VBI_LINES_PER_FIELD >> 8) & 3) << 4) |
                   (((VDELAY >> 8) & 3) << 6);
        WriteByte (BT848_E_CROP, crop);
        WriteByte (BT848_O_CROP, crop);
        WriteByte (BT848_E_VACTIVE_LO, VBI_LINES_PER_FIELD & 0xff);
        WriteByte (BT848_O_VACTIVE_LO, VBI_LINES_PER_FIELD & 0xff);
        WriteByte (BT848_E_HSCALE_LO, 0x00);
        WriteByte (BT848_O_HSCALE_LO, 0x00);
        WriteByte (BT848_E_HSCALE_HI, 0x00);
        WriteByte (BT848_O_HSCALE_HI, 0x00);
        WriteByte (BT848_E_HDELAY_LO, HDELAY & 0xff);
        WriteByte (BT848_O_HDELAY_LO, HDELAY & 0xff);
        WriteByte (BT848_E_HACTIVE_LO, VBI_SPL & 0xff);
        WriteByte (BT848_O_HACTIVE_LO, VBI_SPL & 0xff);

        WriteWord (BT848_GPIO_DMA_CTL, BT848_GPIO_DMA_CTL_PKTP_32 |
                                       BT848_GPIO_DMA_CTL_PLTP1_16 |
                                       BT848_GPIO_DMA_CTL_PLTP23_16 |
                                       BT848_GPIO_DMA_CTL_GPINTC |
                                       BT848_GPIO_DMA_CTL_GPINTI);
        WriteByte (BT848_GPIO_REG_INP, 0x00);
        // input format (PAL, NTSC etc.) and input source
        WriteByte (BT848_IFORM, BT848_IFORM_MUX1 | BT848_IFORM_XTBOTH | BT848_IFORM_NTSC);

        WriteByte (BT848_CONTRAST_LO, 0xd8);
        WriteByte (BT848_BRIGHT, 0x00 /*0x10*/);
        WriteByte (BT848_E_VSCALE_HI, 0x20);
        WriteByte (BT848_O_VSCALE_HI, 0x20);
        WriteByte (BT848_E_VSCALE_LO, 0x00);
        WriteByte (BT848_O_VSCALE_LO, 0x00);
        WriteByte (BT848_SAT_U_LO, 0xfe);
        WriteByte (BT848_SAT_V_LO, 0xb4);
        WriteByte (BT848_HUE, 0);
        WriteByte (BT848_OFORM, 0x00);
        WriteByte (BT848_E_VTC, BT848_VTC_HSFMT);
        WriteByte (BT848_O_VTC, BT848_VTC_HSFMT);

        WriteByte (BT848_ADC, BT848_ADC_RESERVED | BT848_ADC_CRUSH);
        WriteByte (BT848_O_CONTROL, BT848_CONTROL_LDEC | BT848_CONTROL_LNOTCH);
        WriteByte (BT848_E_CONTROL, BT848_CONTROL_LDEC | BT848_CONTROL_LNOTCH);

        WriteByte (BT848_E_SCLOOP, BT848_SCLOOP_CKILL);
        WriteByte (BT848_O_SCLOOP, BT848_SCLOOP_CKILL);

        // interrupt mask; reset the status before enabling the interrupts
        WriteDword (BT848_INT_STAT, (DWORD) 0x0fffffffUL);
        WriteDword (BT848_INT_MASK, (1 << 23) | BT848_INT_RISCI);

#if 1
        WriteByte (BT848_TGCTRL, BT848_TGCTRL_TGCKI_NOPLL);
        WriteByte (BT848_PLL_XCI, 0x00);
#else
        // Start PLL at PAL frequency
        WriteByte (BT848_PLL_F_LO, 0xf9);
        WriteByte (BT848_PLL_F_HI, 0xdc);
        WriteByte (BT848_PLL_XCI, 0x8E);

        for (int idx = 0; idx < 100; idx++) {
            if (ReadByte (BT848_DSTATUS) & BT848_DSTATUS_CSEL)
                WriteByte (BT848_DSTATUS, 0x00);
            else {
                WriteByte (BT848_TGCTRL, BT848_TGCTRL_TGCKI_PLL);
                break;
            }
            Sleep (10);
        }
#endif

        AndDataByte(BT848_IFORM, (BYTE)~BT848_IFORM_MUXSEL);

        AndDataByte(BT848_E_CONTROL, ~BT848_CONTROL_COMP);
        AndDataByte(BT848_O_CONTROL, ~BT848_CONTROL_COMP);

        MaskDataByte(BT848_IFORM, 0, BT848_IFORM_MUXSEL);

//        Sleep(5000);

        // disable capturing while the RISC program is changed to avoid a crash
        MaskDataByte(BT848_CAP_CTL, 0, 0x0f);

        DWORD* pRiscCode = static_cast<DWORD*>(riscMemory.GetUserPointer());
        PHYS pRiscBasePhysical = riscMemory.TranslateToPhysical(pRiscCode, RISC_CODE_LENGTH, NULL);

        // create the RISC code for 10 fields
        // the first one (0) is even, last one (9) is odd
        for (int nField = 0; nField < VBI_FIELD_CAPTURE_COUNT; nField++)
        {
            // First we sync onto either the odd or even field
            if (nField & 1)
                *(pRiscCode++) = (DWORD) (BT848_RISC_SYNC | BT848_RISC_RESYNC | BT848_FIFO_STATUS_VRO);
            else
                *(pRiscCode++) = (DWORD) (BT848_RISC_SYNC | BT848_RISC_RESYNC | BT848_FIFO_STATUS_VRE);
            *(pRiscCode++) = 0;

            *(pRiscCode++) = (DWORD) (BT848_RISC_SYNC | BT848_FIFO_STATUS_FM1);
            *(pRiscCode++) = 0;

            BYTE* pVbiUser = static_cast<BYTE*>(userMemory[nField / 2].GetUserPointer());

            if (nField & 1)
                pVbiUser += VBI_LINES_PER_FIELD * VBI_LINE_SIZE;

            for (int nLine = 0; nLine < VBI_LINES_PER_FIELD; nLine++) {
                DWORD GotBytesPerLine;
                PHYS pVbiPhysical = userMemory[nField / 2].TranslateToPhysical(pVbiUser, VBI_SPL, &GotBytesPerLine);
                if ((pVbiPhysical == 0) || (VBI_SPL > GotBytesPerLine))
                    throw Exception("Memory error.");
                *(pRiscCode++) = BT848_RISC_WRITE | BT848_RISC_SOL | BT848_RISC_EOL | VBI_SPL;
                *(pRiscCode++) = pVbiPhysical;
                pVbiUser += VBI_LINE_SIZE;
            }
        }

        int BytesPerRISCField = ((long)pRiscCode - (long)riscMemory.GetUserPointer()) / VBI_FIELD_CAPTURE_COUNT;

        *(pRiscCode++) = BT848_RISC_JUMP;
        *(pRiscCode++) = pRiscBasePhysical;

        // start address for the DMA RISC code
        WriteDword(BT848_RISC_STRT_ADD, pRiscBasePhysical);

        while (true) {
            console.write("Waiting for connection\n");
            AutoHandle h = File("\\\\.\\pipe\\vbicap", true).createPipe();

            bool connected = (ConnectNamedPipe(h, NULL) != 0) ? true :
                (GetLastError() == ERROR_PIPE_CONNECTED);
            if (!connected)
                continue;

            console.write("Connected\n");

            int command = h.read<int>();
            if (command == 0) {
                // Stop vbicap command
                break;
            }
            if (command != 1)
                continue;

            DMAEnable dma;

            int oldFrame = -1;
            int frame;
            bool broken = false;

            do {
                // read the RISC program counter, i.e. pointer into the RISC code
                PHYS CurrentRiscPos = ReadDword(BT848_RISC_COUNT);

                int CurrentPos = (CurrentRiscPos - pRiscBasePhysical) / BytesPerRISCField;

                if (CurrentPos < VBI_FIELD_CAPTURE_COUNT) {
                    // the current position lies in the field which is currently being filled
                    // calculate the index of the previous (i.e. completed) frame
                    if (CurrentPos < 2)
                        frame = ((CurrentPos + VBI_FIELD_CAPTURE_COUNT) - 2) / 2;
                    else
                        frame = (CurrentPos - 2) / 2;
                }
                else
                    frame = 0;

                if (frame == oldFrame) {
                    Sleep(5);
                    continue;
                }
                if (oldFrame == -1) {
                    oldFrame = frame;
                    continue;
                }

                do {
                    oldFrame = (oldFrame + 1) % VBI_FRAME_CAPTURE_COUNT;
                    BYTE* pVBI = static_cast<BYTE*>(userMemory[oldFrame].GetUserPointer());
                    for (int row = 0; row < 450; row++, pVBI += VBI_LINE_SIZE) {
                        DWORD bytesWritten;
                        if (WriteFile(h, pVBI, 1024, &bytesWritten, NULL) == 0) {
                            DWORD error = GetLastError();
                            if (error == ERROR_BROKEN_PIPE || error == ERROR_NO_DATA) {
                                broken = true;
                                break;
                            }
                        }
                    }
                } while (oldFrame != frame && !broken);
            } while (!broken);
            console.write("Capture complete.\n");
        }

        if (m_MemoryBase != 0)
        {
            // if the chip was not in D0 state we restore the original ACPI power state
            if(m_InitialACPIStatus != 0)
            {
                HwPci_SetACPIStatus(m_InitialACPIStatus);
            }

            TDSDrvParam hwParam;
            hwParam.dwAddress = m_MemoryBase;
            hwParam.dwValue   = m_MemoryLength;

            HwDrv_SendCommand(IOCTL_DSDRV_UNMAPMEMORY, &hwParam, sizeof(hwParam));
        }

        CardOpened = FALSE;
    }
};
