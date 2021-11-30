#ifndef slic3r_Largix_hpp_
#define slic3r_Largix_hpp_

#include "libslic3r.h"
#include "Print.hpp"

#include <memory>
#include <map>
#include <string>
#include <vector>
#include <Point.h>

namespace Largix 
{
    struct ConvertSettings;
}

namespace Slic3r
{    
    class PrintObjectConfig;

    class LargixExport
    {
    public:
        LargixExport() {}
        virtual ~LargixExport() = default;

        bool do_export(Print* print, const char *path);

    protected: 

        void fillSettings(const PrintObjectConfig& config, Largix::ConvertSettings &set);

        size_t getNumOfSlices(const Print *print);
    };
} // namespace Slic3r

#endif
