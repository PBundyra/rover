#ifndef SENSOR_H
#define SENSOR_H

#include "directions.h"

#include <memory>

// Virtual sensor class.
class Sensor {
public:
    Sensor() = default;

    virtual ~Sensor() = default;

    virtual bool is_safe([[maybe_unused]] coordinate_t x,
                         [[maybe_unused]] coordinate_t y) = 0;
};

using sensor_ptr = std::unique_ptr<Sensor>;

#endif //SENSOR_H
