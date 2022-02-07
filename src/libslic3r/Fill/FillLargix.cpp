#include "../ClipperUtils.hpp"
#include "../ExPolygon.hpp"
#include "../ShortestPath.hpp"
#include "../Surface.hpp"

#include "FillLargix.hpp"
#include "LargixHelper.hpp"

#include <sstream> 

#include <PathNavigator/PolygonValidator.h>
#include <PathNavigator/Settings.h>
#include <PathNavigator/Layer.h>
#include <PathNavigator/DefaultSettings.h>
#include <PathNavigator/Navigator.h>

#include <PathNavigator/PolygonIO.h>
#include <PathNavigator/StrandIO.h>

namespace Slic3r 
{

    void FillLargix::fillSettings(const PrintObjectConfig& config, LargixNavigator::Settings& set) const
    {
        set = LargixNavigator::DefaultSettings::getDefaultSettings();
 
        set.maxNumbersStrandsPerLayer = config.largix_strands_number;
        set.minStrandRadius = config.largix_min_radius;
        set.minStrandLength = config.largix_min_strand_length;
        set.strandWidth = config.largix_strands_width;
        set.szBin[0]    = set.strandWidth * config.largix_number_of_stripes;
        set.szBin[1] = config.largix_bin_length;
        set.maxNumberOfStrandCombinations = config.largix_maximal_combinations_number;
        set.bUseAnglePattern = config.largix_angle_pattern;
        set.bBuildBySpline = config.largix_spline_interpolation_flag;
        if (set.bUseAnglePattern) 
        {
            set.clockwiseFlag = config.largix_anticlockwise_param;
            set.angleStart = config.largix_angle_pattern_start_angle;
            set.angleShift[0] = config.largix_angle_pattern0;
            set.angleShift[1] = config.largix_angle_pattern1;
            set.angleShift[2] = config.largix_angle_pattern2;
            set.angleShift[3] = config.largix_angle_pattern3;
        }
    }

    void FillLargix::_fill_surface_single(
        const FillParams                &params,
        unsigned int                     thickness_layers,
        const std::pair<float, Point>   &direction,
        ExPolygon                        expolygon,
        Polylines                       &polylines_out)
    {
        // prepare polygon
        LargixNavigator::Polygon pol;
        LargixHelper::convert_polygon_2_largix(expolygon, pol);

        // prepare settings
        LargixNavigator::Settings set;
        if (params.print_options)
            fillSettings(params.print_options->config(), set);

        // define layer number
        size_t layerNum = layer_id + 1;

        // define center point
        Point prusaPoint = bounding_box.center();
        LargixNavigator::Point2D center(prusaPoint[0] * SCALING_FACTOR, prusaPoint[1] * SCALING_FACTOR);

        // build layer path
        LargixNavigator::Navigator navigator(pol, set);
        LargixNavigator::Layer     layer;
        navigator.build(layerNum, center, layer);

        static bool bSavePolygons_ = false;
        if (bSavePolygons_) 
        {
            std::stringstream ss;
            ss << "C:\\Temp\\Polygons\\polygon" << layerNum << ".wkt";
            LargixNavigator::PolygonIO::saveToWktFile(pol, ss.str());
        }

        static bool bSaveStrands_ = false;
        if (bSaveStrands_) 
        {
            size_t countStrand = 0;
            for (const auto &s : layer.strands()) 
            {
                std::stringstream ss;
                ss << "C:\\Temp\\Strands\\strand" << layerNum << '_' << ++countStrand << ".csv";
                LargixNavigator::StrandIO::saveToCsvFile(s->getStrand(), ss.str());
            }
        }
        LargixHelper::convert_layer_2_prusa_1(layer, polylines_out);
    }

} // namespace Slic3r
