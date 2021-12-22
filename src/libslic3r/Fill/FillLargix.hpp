#ifndef slic3r_FillLargix_hpp_
#define slic3r_FillLargix_hpp_

#include "../libslic3r.h"

#include "FillBase.hpp"

namespace LargixNavigator
{
	struct Settings;
}

namespace Slic3r {

class PrintObjectConfig;

class FillLargix : public Fill
{
public:
    Fill *clone() const override { return new FillLargix(*this); };
    ~FillLargix() override = default;

protected:
	void _fill_surface_single(
	    const FillParams                &params, 
	    unsigned int                     thickness_layers,
	    const std::pair<float, Point>   &direction, 
	    ExPolygon    		             expolygon,
	    Polylines                       &polylines_out) override;

	void fillSettings(const PrintObjectConfig& config, LargixNavigator::Settings &set) const;
};

}; // namespace Slic3r

#endif // slic3r_FillLargix_hpp_
