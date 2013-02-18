#include "alfe/main.h"

class Program : public ProgramBase
{
public:
    void run()
    {
        String process = "0";
        if (_arguments.count() > 1)
            process = _arguments[1];

        AutoHandle h = File("\\\\.\\pipe\\vbicap", true).openPipe();
        h.write<int>(1);                

        AutoHandle out = File("output.dat").openWrite();
        Byte buffer[1024];
        for (int i = 0; i < 450 * 8; ++i) {
            h.read(buffer, 1024);
            out.write(buffer, 1024);
        }
    }
};