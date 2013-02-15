#include <windows.h>
#include <stdio.h>
#include "btdrv.h"

FILE* outputFile;
bool capture = false;

int main()
{
    outputFile = fopen("output.dat","wb");
    BtDriver_Init();
    bool isTuner;
    BtDriver_SetInputSource(1, 0, NULL);
    BtDriver_Configure(0, BTDRV_SOURCE_PCI, 0, 0x109e036e, 1, 0, 0 /*1*/, 1);
    BtDriver_StartAcq();
    Sleep(5000);
    capture = true;
    Sleep(500);
    BtDriver_StopAcq();
    BtDriver_Exit();
    fclose(outputFile);
}