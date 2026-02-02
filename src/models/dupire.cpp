#include "models/dupire/dupire.hpp"

#include "surface/local_vol.hpp"

Dupire::Dupire(float r, float q, 
           std::shared_ptr<LocalVolatilitySurface> loc_vol_surface):

           r_(r), q_(q), lv_surface_(loc_vol_surface)

{}


