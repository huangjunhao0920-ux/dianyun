#pragma once

namespace dianyun {

class SensorBase {
public:
    virtual ~SensorBase() = default;
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual const char* name() const = 0;
};

}  // namespace dianyun
