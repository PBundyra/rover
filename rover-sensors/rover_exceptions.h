#ifndef ROVER_EXCEPTIONS_H
#define ROVER_EXCEPTIONS_H

#include <iostream>

// Exception called when a command is executed on a Rover before landing.
class RoverNotLanded : public std::logic_error {
public:
    RoverNotLanded() : logic_error("RoverNotLanded") {}
};

// Exception called when rover sensor detect an unsafe location.
class UnsafeLocation : public std::exception {
public:
    [[nodiscard]] const char * what () const noexcept override {
        return "UnsafeLocation";
    }
};

#endif //ROVER_EXCEPTIONS_H
