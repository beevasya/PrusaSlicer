#include "LargixHelper.hpp"

#include <Layer.h>

namespace Slic3r {

void convert(const Point& p, Largix::Point2D& pl) 
{
    pl.x() = static_cast<double>(p.x()) * SCALING_FACTOR;
    pl.y() = static_cast<double>(p.y()) * SCALING_FACTOR;
}

void convert(const Largix::Point2D &pl, Point &p)
{
    p.x() = scale_(pl.x());
    p.y() = scale_(pl.y());
}

bool LargixHelper::convert_polygon_2_largix(const ExPolygon &src, Largix::Polygon &dst)
{
    {
        auto &points = dst.outer();
        points.resize(src.contour.size());
        auto it = points.begin();
        for (const auto& point : src.contour) 
        {
            convert(point, *it);
            it++;
        }
    }

    dst.inners().resize(src.holes.size());
    auto it = dst.inners().begin();
    for (const auto& poly : src.holes) 
    {
        it->resize(poly.size());
        auto itp = it->begin();
        for (const auto& point : poly) 
        {
            convert(point, *itp);
            itp++;
        }
        it++;
    }
    
    return true;
}

bool LargixHelper::convert_layer_2_prusa(const Largix::Layer &src, Polylines &dst, double strandWidth)
{
    for (auto strand : src.strands()) 
    {
        std::vector<std::array<Largix::Point2D, 4>> points;
        strand.get4StrandPoints(points, strandWidth);

        std::array<Polyline, 4> pline;
        for (auto point : points) 
        {
            pline[0].points.push_back(
                Point::new_scale(point[0].x(), point[0].y()));
            pline[1].points.push_back(
                Point::new_scale(point[1].x(), point[1].y()));
            pline[2].points.push_back(
                Point::new_scale(point[2].x(), point[2].y()));
            pline[3].points.push_back(
                Point::new_scale(point[3].x(), point[3].y()));
        }
        dst.push_back(pline[0]);
        dst.push_back(pline[1]);
        dst.push_back(pline[2]);
        dst.push_back(pline[3]);
    }

    return true;
}

bool LargixHelper::convert_layer_2_prusa_1(const Largix::Layer &src, Polylines &dst)
{
    dst.resize(src.size());
    auto it = dst.begin();
    for (const auto& strand : src.strands()) 
    {
        std::vector<Largix::Point2D> points;
        strand.getStrandPoints(points);

        Polyline &pline = *it;
        pline.points.resize(points.size());
        auto itp = pline.points.begin();
        for (const auto& point : points) 
        {
            convert(point, *itp);
            itp++;
        }
        it++;
    }

    return true;
}

bool LargixHelper::convertPolylineToLargix(const Polyline &pLine, std::vector<Largix::Point2D> &pLineOut)
{
    pLineOut.resize(pLine.points.size());
    auto it = pLineOut.begin();
    for (const auto& point : pLine.points) 
    {
        convert(point, *it);
        it++;
    }
    return true;
}

bool LargixHelper::convertPolylineToLargixShift(const Point& shift, const Polyline& pLine, std::vector<Largix::Point2D>& pLineOut)
{
    pLineOut.resize(pLine.points.size());
    auto it = pLineOut.begin();
    for (auto point : pLine.points) 
    {
        convert(shift + point, *it);
        it++;
    }

    return true;
}

bool LargixHelper::convertPolylineToLargix(
    const Polyline &                             pLine1,
    const Polyline &                             pLine2,
    const Polyline &                             pLine3,
    const Polyline &                             pLine4,
    std::vector<std::array<Largix::Point2D, 4>> &pLineOut)
{
    if (pLine1.points.size() != pLine2.points.size() ||
        pLine2.points.size() != pLine3.points.size() ||
        pLine3.points.size() != pLine4.points.size())
        return false;
    for (int i = 0; i < pLine1.points.size(); i++) {
        pLineOut.push_back(std::array<Largix::Point2D, 4>{
            Largix::Point2D(pLine1.points[i].x() * SCALING_FACTOR,
                            pLine1.points[i].y() * SCALING_FACTOR),
            Largix::Point2D(pLine2.points[i].x() * SCALING_FACTOR,
                            pLine2.points[i].y() * SCALING_FACTOR),
            Largix::Point2D(pLine3.points[i].x() * SCALING_FACTOR,
                            pLine3.points[i].y() * SCALING_FACTOR),
            Largix::Point2D(pLine4.points[i].x() * SCALING_FACTOR,
                            pLine4.points[i].y() * SCALING_FACTOR)});
    }
    LargixHelper::saveLargixStrand(pLineOut);
    return true;
}

void LargixHelper::saveLargixStrand(const std::vector<std::array<Largix::Point2D, 4>> &strand)
{
    static int i = 0;
    std::stringstream ss;
    ss << "c:\\temp\\strand" << i++ << ".txt";
    std::ofstream file(ss.str());
    for (auto item : strand) {
        file << item[0].x() << " " << item[0].y() << " " 
             << item[1].x() << " " << item[1].y() << " " 
             << item[2].x() << " " << item[2].y() << " " 
             << item[3].x() << " " << item[3].y() << std::endl;
    }
    file.close();
}


} // namespace Slic3r
