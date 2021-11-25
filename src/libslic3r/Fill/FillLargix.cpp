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
#include <BuildLayerMgr.h>
#include <Size.h>
#include <TeddyDef.h>

namespace Slic3r {

void FillLargix::_fill_surface_single(
    const FillParams                &params,
    unsigned int                     thickness_layers,
    const std::pair<float, Point>   &direction,
    ExPolygon                        expolygon,
    Polylines                       &polylines_out)
{
    static int  _count = 0;
    Largix::Polygon pol;
    LargixHelper::convert_polygon_2_largix(expolygon, pol);
    if (pol.outer().size() == 0) 
    { 
        assert(!"Empty polygon for filling");
        return; 
    }
    Largix::PolygonValidator pv(pol);
    if (!pv.correct(pol)) {
        assert(!"Failed to correct polygon, it is not valid.");
        return;
    }

    pv.simplify(pol);

    Largix::Settings set;
    set.szBin = Largix::Size2D{ set.strandWidth, 3.5 };
    if (params.print_options)
    {
        set.maxNumbersStrandsPerLayer = params.print_options->config().largix_strands_number;
        set.minStrandRadius = params.print_options->config().largix_min_radius;
        set.minStrandLength = params.print_options->config().largix_min_strand_length;
        set.strandWidth = params.print_options->config().largix_strands_width;
        set.szBin[0] = set.strandWidth * params.print_options->config().largix_number_of_stripes;
        set.szBin[1] = params.print_options->config().largix_bin_length;
        set.maxNumberOfStrandCombinations = params.print_options->config().largix_maximal_combinations_number;
    }
    /*else
    {
        set.minStrandLength = set.szBin[1] * 2.5;
        set.minStrandRadius = 5;
        set.maxNumbersStrandsPerLayer = 1;
    }*/

    Largix::Layer layer;
    Largix::BuildLayerMgr buider(pol, set);

    buider.build(layer);

    if (layer.getNumBins() == 0 ||
        std::any_of(layer.strands().begin(), layer.strands().end(),
            [](const Largix::Strand& item) { return !item.isClosed(); }))
    {
        std::stringstream ss;
        ss << "C:\\Temp\\Polygons\\polygon" << (++_count) << ".wkt";
        //ss << "C:\\src\\temp\\temp" << (++_count) << ".wkt";
        Largix::PolygonIO::saveToWktFile(pol, ss.str());
    }

    LargixHelper::convert_layer_2_prusa_1(layer, polylines_out);

}

} // namespace Slic3r
