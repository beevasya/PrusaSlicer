#include <TeddyConvert.h>
#include <Exports.h>
#include <ConvertSettings.h>
#include <vector>
#include <array>
#include <Point.h>

#include "libslic3r.h"
#include "I18N.hpp"
#include "GCode.hpp"
#include "Largix.hpp"
#include "Exception.hpp"
#include "ExtrusionEntity.hpp"
#include "Print.hpp"
#include "Layer.hpp"
#include "LargixHelper.hpp"
#include "DefaultSettings.h"
#include "Slice.h"
#include <TeddySliceConvert.h>


namespace Slic3r 
{

    void LargixExport::fillSettings(const PrintObjectConfig &config, Largix::ConvertSettings &set)
    {
        set = Largix::DefaultSettings::getDefaultSettings();

        set.laserRotationAxisOffset = config.largix_laser_rotation_axis_offset;
        set.laserRotationRadius = config.largix_laser_rotation_radius;
        set.laserSpotOffset = config.largix_laser_spot_offset;
        set.wheelAxisOffset = config.largix_wheel_axis_offset;
        set.wheelRotationRadius = config.largix_wheel_rotation_radius;

        set.StrandHeight = config.largix_strands_height;
        set.StrandWidth  = config.largix_strands_width;
        set.toolOffset   = config.largix_tool_offset;
        set.stairMode   = config.largix_stair_mode;
        set.constWheelAngleParams.useConstValue = config.largix_use_const_wheel_angle;
        set.constLaserAngleParams.useConstValue = config.largix_use_const_laser_angle;
        set.constWheelAngleParams.constAngle = config.largix_const_wheel_angle;
        set.constLaserAngleParams.constAngle = config.largix_const_laser_angle;
       
    }

    size_t LargixExport::getNumOfSlices(const Print* print) 
    { 
        size_t num = 0;
        for (auto object : print->objects())
            for (auto layer : object->layers())
                num += layer->regions().size();

        return num;
    }

    bool LargixExport::do_export(Print *print, const char *path)
    {
        size_t num = getNumOfSlices(print);
        if (num == 0) 
            throw Slic3r::ExportError(std::string("CSV export failed. No slices found for writing!"));

        Largix::Slices slices(num);

        const auto& objects = print->objects();
        size_t i = 0;
        for (auto object : objects) 
        {
            for (auto layer : object->layers()) 
            {
                for (auto region : layer->regions()) 
                {
                    slices[i].setZ(layer->slice_z);
                    auto pLines = region->fills.as_polylines();
                    std::vector<Largix::Point2D> line_out;
                    for (auto line : pLines) 
                    {
                        LargixHelper::convertPolylineToLargix(line, line_out);
                    }
                    slices[i].swapPoints(line_out);
                    i++;
                }
            }

        }

        Largix::ConvertSettings settings;
        fillSettings(objects.front()->config(), settings);

        Largix::TeddyConvert convert(slices, settings);
        if (!convert.convert())
            throw Slic3r::ExportError(std::string("Fail to convert slices to program!"));

        if (!writeTeddyCSV(path, convert.getProgram())) 
            throw Slic3r::RuntimeError(std::string("Fail to export program to file ") + path);

        return true;
    }

} // namespace Slic3r
