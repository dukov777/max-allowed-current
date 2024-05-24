#pragma once

#include <vector>
#include <array>
#include <string>

#include "CapacitorInterface.h"
#include "MemoryMngr.h"


class CapacitorBase : public CapacitorInterface {
protected:
    CapacitorSpec _spec;
    std::string _cap_name;

public:
    virtual double xc(double f) const override;
    virtual double current(double f, double voltage) const override;
    virtual double allowed_current(double f) const override;
    virtual double voltage(double f, double current) const override;
    virtual const CapacitorSpec& spec() const override;
    virtual const std::string name() const override;
};

// Capacitor class definition
class Capacitor : public CapacitorBase {
public:
    Capacitor() = default;
    // move constructor
    Capacitor(Capacitor&&) = default;
    Capacitor(double cap_uF, double vmax, double imax, double power_max, const std::string cap_name = "");
};

// ParallelCapacitor class definition
class ParallelCapacitor : public CapacitorBase
{
    std::array<CapacitorPtr, 5> _capacitors;
    std::array<CapacitorInterface*, 5> capacitors;
public:
    ParallelCapacitor() = default;
    ParallelCapacitor(ParallelCapacitor&&) = default;
    ParallelCapacitor(ParallelCapacitor& other)
    {
        // implement copy constructor
        ParallelCapacitor(other._cap_name, other.capacitors[0], other.capacitors[1], other.capacitors[2], other.capacitors[3], other.capacitors[4]);
    }

    ParallelCapacitor(const std::string& cap_name, CapacitorInterface* cap1, CapacitorInterface* cap2 = nullptr, CapacitorInterface* cap3 = nullptr, CapacitorInterface* cap4 = nullptr, CapacitorInterface* cap5 = nullptr);
    ParallelCapacitor(const std::string& cap_name, CapacitorPtr& cap1);
    ParallelCapacitor(const std::string& cap_name, CapacitorPtr& cap1, CapacitorPtr& cap2);
    ParallelCapacitor(const std::string& cap_name, CapacitorPtr& cap1, CapacitorPtr& cap2, CapacitorPtr& cap3);
    ParallelCapacitor(const std::string& cap_name, CapacitorPtr& cap1, CapacitorPtr& cap2, CapacitorPtr& cap3, CapacitorPtr& cap4);
    ParallelCapacitor(const std::string &cap_name, CapacitorPtr &cap1, CapacitorPtr &cap2, CapacitorPtr &cap3, CapacitorPtr &cap4, CapacitorPtr &cap5);

    void _initialize(const std::string &cap_name);

    double xc(double f) const override;

    double current(double f, double voltage) const override;

    double allowed_current(double f) const override;

    double voltage(double f, double current) const override;
};

// SeriesCapacitor class definition
class SeriesCapacitor : public CapacitorBase 
{
    std::array<CapacitorPtr, 2> capacitors;

public:
    SeriesCapacitor() = default;
    SeriesCapacitor(const std::string& cap_name, CapacitorPtr& cap1, CapacitorPtr& cap2);

    double xc(double f) const override;

    double current(double f, double voltage) const override;

    double allowed_current(double f) const override;

    double voltage(double f, double current) const override;
};

