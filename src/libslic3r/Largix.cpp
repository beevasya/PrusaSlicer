#include <TeddyConvert.h>
#include <TeddySlice4Convert.h>
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


namespace Slic3r {

bool LargixExport::do_export(Print *print, const char *path)
{
    Largix::Slices          slices;
    Largix::ConvertSettings settings = Largix::DefaultSettings::getDefaultSettings();


    for (auto object : print->objects()) {
        std::vector<GCode::LayerToPrint> layers_to_print;
        layers_to_print.reserve(object->layers().size() +
                                object->support_layers().size());
        for (auto layer : object->layers()) {
            Largix::Slice slice;
            slice.first = layer->slice_z;
            for (auto region : layer->regions()) {
                auto pLines = region->fills.as_polylines();
                std::vector<Largix::Point2D> line_out;
                for (auto line : pLines) 
                {
                    LargixHelper::convertPolylineToLargix(line, line_out);
                }
                slice.second.swap(line_out);
            }
             
            settings.laserRotationAxisOffset =
                object->config().largix_laser_rotation_axis_offset;
            settings.laserRotationRadius = object->config()
                                               .largix_laser_rotation_radius;
            settings.laserSpotOffset = object->config()
                                           .largix_laser_spot_offset;
            settings.MaxAngle = object->config().largix_max_angle;
            settings.maxRotationAngle = object->config()
                                            .largix_max_rotation_angle;
            settings.minRotationAngle = object->config()
                                            .largix_min_rotation_angle;
            settings.wheelAxisOffset = object->config()
                                           .largix_wheel_axis_offset;
            settings.wheelRotationRadius = object->config()
                                            .largix_wheel_rotation_radius;

            settings.StrandHeight = object->config()
                                            .largix_strands_height;
            settings.StrandWidth = object->config()
                                            .largix_strands_width;
            settings.toolOffset = object->config()
                                            .largix_tool_offset;
            Largix::TeddySliceConvert conv(slice, settings);
            conv.convert();
            slices.push_back(conv.getSlice());
        }
    }

    Largix::TeddyConvert convert(slices, settings);
    if (!convert.convert()) { return false; }

    if (!writeTeddyCSV(path, convert.getProgram())) { return false; }

    return true;
}

} // namespace Slic3r
