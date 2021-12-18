#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/polygon.hpp>

#include <PolygonValidator.h>
#include <PolygonHelper.h>
#include <Size.h>
#include <TeddyDef.h>
#include <Settings.h>

#include "../ClipperUtils.hpp"
#include "../ExPolygon.hpp"
#include "../ShortestPath.hpp"
#include "../Surface.hpp"

#include <sstream> 

#include "FillLargix.hpp"
#include "LargixHelper.hpp"

#include <Layer.h>
#include <PolygonValidator.h>
#include <PolygonIO.h>
#include <StrandIO.h>
#include <BuildLayerMgr.h>
#include <FirstPolygonPoint.h>
#include <Size.h>
#include <TeddyDef.h>

namespace Slic3r 
{

    void FillLargix::fillSettings(const PrintObjectConfig& config, LargixNavigator::Settings& set) const
    {
        set.maxNumbersStrandsPerLayer = config.largix_strands_number;
        set.minStrandRadius = config.largix_min_radius;
        set.minStrandLength = config.largix_min_strand_length;
        set.strandWidth = config.largix_strands_width;
        set.szBin[0]    = set.strandWidth * config.largix_number_of_stripes;
        set.szBin[1] = config.largix_bin_length;
        set.maxNumberOfStrandCombinations = config.largix_maximal_combinations_number;
        set.bUseAnglePattern = config.largix_angle_pattern;
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
        LargixNavigator::Polygon pol;
        LargixHelper::convert_polygon_2_largix(expolygon, pol);
        if (pol.outer().size() == 0) 
        { 
            assert(!"Empty polygon for filling");
            return; 
        }
        LargixNavigator::PolygonValidator pv(pol);

        pv.simplify();
        if (pol.outer().empty()) {
            assert(!"Failed to build path for empty polygon!");
            return;
        }

        if (!pv.correct()) {
            assert(!"Failed to correct polygon, it is not valid.");
            return;
        }

        LargixNavigator::Settings set;
        if (params.print_options)
            fillSettings(params.print_options->config(), set);

 //     size_t layerNum = (size_t) (z /params.print_options->config().largix_strands_height + 0.5);
        size_t layerNum = layer_id + 1;

        LargixNavigator::Layer layer;
        if (set.bUseAnglePattern) 
        {
            Point prusaPoint = bounding_box.center();
            LargixNavigator::Point2D center(prusaPoint[0] * SCALING_FACTOR, prusaPoint[1] * SCALING_FACTOR);

            size_t index;
        
            LargixNavigator::FirstPolygonPoint pointFinder(pol, set);
            if (pointFinder.find(center, layerNum, index)) 
            { 
                LargixNavigator::BuildLayer buider(pol, set);
                buider.build(index, layer);
            }
        } 
        else 
        {
            LargixNavigator::BuildLayerMgr buider(pol, set);
            buider.build(layer);
        }


        // polygons
        //if (layer.getNumBins() == 0 ||
        //    std::any_of(layer.strands().begin(), layer.strands().end(),
        //        [](const LargixNavigator::Strand& item) { return !item.isClosed(); }))
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
                LargixNavigator::StrandIO::saveToCsvFile(s, ss.str());
            }
        }
        LargixHelper::convert_layer_2_prusa_1(layer, polylines_out);
    }

} // namespace Slic3r
