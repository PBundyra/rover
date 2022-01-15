#ifndef ROVER_H
#define ROVER_H

#include "directions.h"
#include "rover_exceptions.h"
#include "sensor.h"

#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <memory>

class Rover;

// Class holding all information about the state of the rover,
// e.g. it's location.
class RoverState {
private:
    coordinate_t x, y;
    Direction dir;
    bool is_stopped;
    bool landed;
    friend Rover;

public:
    RoverState() : is_stopped(false), landed(false) {};

    ~RoverState() = default;

    void update_location() {    
        x += val.at(dir).first;
        y += val.at(dir).second;
    }

    coordinates_t next_location() {
        return {x + val.at(dir).first, y + val.at(dir).second};
    }

    void turn_right() {
        dir = Direction((static_cast<int>(dir) + 1) % 4);
    }

    void turn_left() {
        dir = Direction((static_cast<int>(dir) + 3) % 4);
    }

    friend std::ostream &
    operator<<(std::ostream &out, const RoverState &state) {
        if (!state.landed) {
            out << "unknown";
        } else {
            out << "(" << state.x << ", " << state.y << ")"
                << print_dir.at(state.dir)
                << (state.is_stopped ? " stopped" : "");
        }
        return out;
    }
};

// Virtual base class for all commands.
class Command {
public:
    Command() = default;

    virtual ~Command() = default;

    virtual void execute(RoverState &state,
                         std::vector<sensor_ptr> &sensors) = 0;
};

using command_ptr = std::shared_ptr<Command>;

class RoverBuilder;

class Rover {
private:
    std::unordered_map<char, command_ptr> commands;
    std::vector<sensor_ptr> sensors;
    RoverState state;
    friend RoverBuilder;

    Rover(std::unordered_map<char, command_ptr> commands,
          std::vector<sensor_ptr> sensors)
            : commands(std::move(commands)),
              sensors(std::move(sensors)),
              state(RoverState()) {}

public:
    Rover() = delete;

    ~Rover() = default;

    friend std::ostream &operator<<(std::ostream &out, const Rover &rover) {
        out << rover.state;
        return out;
    }

    auto execute(const std::string &commands_to_exec) {
        if (!state.landed)
            throw RoverNotLanded();
        state.is_stopped = false;
        for (auto &command: commands_to_exec) {
            try {
                commands.at(command)->execute(state, sensors);
            }
            catch (UnsafeLocation const &e) {
                // Unsafe location detected.
                state.is_stopped = true;
                break;
            }
            catch (std::out_of_range const &e) {
                // Command not found.
                state.is_stopped = true;
                break;
            }
        }
    }

    void land(const coordinates_t &coords,
              const Direction &direction) {
        state.dir = direction;
        state.x = coords.first;
        state.y = coords.second;
        state.landed = true;
        state.is_stopped = false;
    }
};

class Move_forward : public Command {
public:
    Move_forward() = default;

    ~Move_forward() override = default;

    void execute(RoverState &state,
                 std::vector<sensor_ptr> &sensors) override {
        coordinates_t new_coords = state.next_location();
        for (auto &sensor: sensors)
            if (!sensor->is_safe(new_coords.first, new_coords.second))
                throw UnsafeLocation();
        state.update_location();
    };
};

class Move_backward : public Command {
public:
    Move_backward() = default;

    ~Move_backward() override = default;

    void execute(RoverState &state,
                 std::vector<sensor_ptr> &sensors) override {
        // We reverse the direction and use the move_forward command.
        state.turn_right();
        state.turn_right();
        auto move_forward = Move_forward();
        try {
            move_forward.execute(state, sensors);
        } catch (UnsafeLocation const &e) {
            state.turn_right();
            state.turn_right();
            throw;
        }
        state.turn_right();
        state.turn_right();
    };
};

class Rotate_right : public Command {
public:
    Rotate_right() = default;

    ~Rotate_right() override = default;

    void execute(RoverState &state,
                 [[maybe_unused]] std::vector<sensor_ptr> &sensors) override {
        state.turn_right();
    };
};

class Rotate_left : public Command {
public:
    Rotate_left() = default;

    ~Rotate_left() override = default;

    void execute(RoverState &state,
                 [[maybe_unused]] std::vector<sensor_ptr> &sensors) override {
        state.turn_left();
    };
};

class Compose : public Command {
private:
    std::vector<command_ptr> commands;

public:
    Compose(std::initializer_list<command_ptr> args) : commands(args) {};

    ~Compose() override = default;

    void execute(RoverState &state,
                 std::vector<sensor_ptr> &sensors) override {
        for (auto &command: commands) {
            command->execute(state, sensors);
        }
    };
};

std::shared_ptr<Move_forward> move_forward() {
    return std::make_shared<Move_forward>();
}

std::shared_ptr<Move_backward> move_backward() {
    return std::make_shared<Move_backward>();
}

std::shared_ptr<Rotate_left> rotate_left() {
    return std::make_shared<Rotate_left>();
}

std::shared_ptr<Rotate_right> rotate_right() {
    return std::make_shared<Rotate_right>();
}

std::shared_ptr<Compose> compose(
        std::initializer_list<command_ptr> args) {
    return std::make_shared<Compose>(args);
}

class RoverBuilder {
private:
    std::unordered_map<char, command_ptr> commands;
    std::vector<sensor_ptr> sensors;

public:
    RoverBuilder() = default;

    RoverBuilder &program_command(const char &c, command_ptr const &func) {
        commands[c] = func;
        return *this;
    }

    RoverBuilder &add_sensor(sensor_ptr &&sensor) {
        sensors.push_back(std::move(sensor));
        return *this;
    };

    Rover build() { return {std::move(commands), std::move(sensors)}; }
};

#endif