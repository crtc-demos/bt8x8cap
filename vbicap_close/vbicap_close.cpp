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
        h.write<int>(0);                
    }
};