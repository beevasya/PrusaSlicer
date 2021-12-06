#ifndef slic3r_LargixHelper_hpp_
#define slic3r_LargixHelper_hpp_

#include <polygon.h>

namespace Largix {
class Layer;
}

namespace Slic3r {

class ExPolygon;
class Surface;
class Point;

class LargixHelper
{
public:

	static bool convert_polygon_2_largix(const ExPolygon &src, Largix::Polygon &dst);
    static bool convert_layer_2_prusa(const Largix::Layer &src, Polylines &dst, double strandWidth);
    static bool convert_layer_2_prusa_1(const Largix::Layer &src, Polylines &dst);

    static bool convertPolylineToLargix(const Polyline& pLine, std::vector<Largix::Point2D> &pLineOut);
    static bool convertPolylineToLargixShift(const Point& shift, const Polyline &pLine, std::vector<Largix::Point2D> &pLineOut);

    static bool convertPolylineToLargix(
        const Polyline &                                   pLine1,
        const Polyline &                                   pLine2,
        const Polyline &                                   pLine3,
        const Polyline &                                   pLine4,
        std::vector<std::array<Largix::Point2D, 4>> &pLineOut);
    static void saveLargixStrand(
        const std::vector<std::array<Largix::Point2D, 4>> &strand);
};

}; // namespace Slic3r

#endif // slic3r_LargixHelper_hpp_
