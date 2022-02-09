
#include "Largix.hpp"
#include "Layer.hpp"
#include "LargixHelper.hpp"

#include <ProgramBuilder/DefaultSettings.h>
#include <ProgramBuilder/Slice.h>
#include <ProgramBuilder/TeddyConvert.h>
#include <ProgramBuilder/Exports.h>
#include <ProgramBuilder/SliceIO.h>

#include <ProgramChecker/ProgramInfo.h>
#include <ProgramChecker/ProgramAnalyzer.h>
#include <ProgramChecker/ProgramInfoSer.h>
#include <ProgramChecker/CheckerSettings.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

namespace fs = boost::filesystem;

namespace Slic3r 
{

    void LargixExport::fillSettings(const PrintObjectConfig &config, LargixProgram::ConvertSettings &set)
    {
        set = LargixProgram::DefaultSettings::getDefaultSettings();
        
        set.laserRotationAxisOffset = config.largix_laser_rotation_axis_offset;
        set.laserRotationRadius = config.largix_laser_rotation_radius;
        set.laserSpotOffset = config.largix_laser_spot_offset;
        set.wheelAxisOffset = config.largix_wheel_axis_offset;
        set.wheelRotationRadius = config.largix_wheel_rotation_radius;

        set.strandHeight = config.largix_strands_height;
        set.strandWidth  = config.largix_strands_width;
        set.toolOffset   = config.largix_tool_offset;
        set.stairMode   = config.largix_stair_mode;
        set.constWheelAngleParams.useConstValue = config.largix_use_const_wheel_angle;
        set.constLaserAngleParams.useConstValue = config.largix_use_const_laser_angle;
        set.constWheelAngleParams.constAngle = config.largix_const_wheel_angle;
        set.constLaserAngleParams.constAngle = config.largix_const_laser_angle;
        set.bRetraction = config.largix_retraction;
        if (set.bRetraction)
        {
            set.retractionDelta = config.largix_retraction_delta;
            set.retractionExtra = config.largix_retraction_extra;
            set.retractionReturnLength = config.largix_retraction_return_length;
        }     
        set.bNextPoints = config.largix_next_point_flag;
        if (set.bNextPoints)
        {
            set.nextPointsLength = config.largix_next_point_length;
        }
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

        LargixProgram::Slices slices(num);

        const auto& objects = print->objects();
        size_t i = 0;
        for (auto object : objects) 
        {
            const Point& sh = object->instances()[0].shift;
            for (auto layer : object->layers()) 
            {
                for (auto region : layer->regions()) 
                {
                    slices[i].setZ(objects.front()->print()->config().z_offset + layer->print_z - layer->height);
                    auto pLines = region->fills.as_polylines();
                    std::vector<LargixProgram::Point2D> line_out;
                    for (const auto& line : pLines) 
                    {
                        LargixHelper::convertPolylineToLargixShift(sh, line, line_out);
                    }
                    slices[i].swapPoints(line_out);

                    static bool bSaveStrands_ = false;
                    if (bSaveStrands_) 
                    {
                        std::vector<LargixProgram::Point2D> line_out_noshift;
                        for (const auto &line : pLines) 
                        {
                            LargixHelper::convertPolylineToLargix(line, line_out_noshift);
                        }

                        LargixProgram::Slice slice(0.0, line_out_noshift);
                        std::stringstream ss;
                        ss << "C:\\Temp\\StrandsBeforeBuild\\strand" << i + 1  << "_1" << ".csv";
                        LargixProgram::SliceIO::saveToCsvFile(slice, ss.str());
                    }
                    i++;
                }
            }

        }

        LargixProgram::ConvertSettings settings;
        fillSettings(objects.front()->config(), settings);

       LargixProgram::TeddyConvert convert(slices, settings);
        if (!convert.convert())
            throw Slic3r::ExportError(std::string("Fail to convert slices to program!"));

        auto teddyProgram = convert.getProgram();
        if (!writeTeddyCSV(path, teddyProgram))
            throw Slic3r::RuntimeError(std::string("Fail to export program to file ") + path);
    
        if (objects.front()->config().largix_program_info_flag)
        {
            LargixChecker::ProgramInfo programInfo;
            //
            // no we use default setting
            // implement loading from file
            LargixChecker::CheckerSettings set; 
            LargixChecker::ProgramAnalyzer programAnalizer(set);


            if (!programAnalizer.getInfo(teddyProgram, programInfo))
                throw Slic3r::RuntimeError(std::string("Fail to get program information") + path);

            fs::path xmlFile = fs::path(path).replace_extension(".xml");

            if (!LargixChecker::ProgramInfoSer::writeToFile(xmlFile.string(),
                                                            programInfo))
                throw Slic3r::ExportError(std::string("Fail to export program information to xml file ") + xmlFile.string());
        }
        return true;
    }

} // namespace Slic3r