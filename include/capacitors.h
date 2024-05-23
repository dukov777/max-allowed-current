#pragma once

#include <vector>
#include <array>
#include <string>

// CapacitorSpec class definition
class CapacitorSpec {
private:
    double cap_uF;
    double cap_F;
    double i_max;
    double v_max;
    double power_max;

public:
    CapacitorSpec() : cap_uF(0.0), cap_F(0.0), i_max(0.0), v_max(0.0), power_max(0.0) {}
    CapacitorSpec(double cap_uF, double v_max, double i_max, double power_max)
        : cap_uF(cap_uF), cap_F(cap_uF * 1e-6), i_max(i_max), v_max(v_max), power_max(power_max) {}

    double get_i_max() const { return i_max; }
    double get_v_max() const { return v_max; }
    double get_power_max() const { return power_max; }
    double get_cap_F() const { return cap_F; }
    double get_cap_uF() const { return cap_uF; }
};

// CapacitorInterface class definition
class CapacitorInterface {
public:
    virtual double xc(double f) const = 0;
    virtual double current(double f, double voltage) const = 0;
    virtual double allowed_current(double f) const = 0;
    virtual double voltage(double f, double current) const = 0;
    virtual const CapacitorSpec& spec() const = 0;
    virtual std::string name() const = 0;
    virtual void initialize() = 0;
    virtual ~CapacitorInterface() {}
};

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
    virtual std::string name() const override;
    void initialize() override{};
};

// Capacitor class definition
class Capacitor : public CapacitorBase {
public:
    Capacitor() = default;
    // move constructor
    Capacitor(Capacitor&&) = default;
    Capacitor(double cap_uF, double vmax, double imax, double power_max, std::string cap_name = "");
};

class GroupCapacitorBase : public CapacitorBase 
{
protected:
    GroupCapacitorBase() = default;
    // GroupCapacitorBase(const std::array<CapacitorInterface*, 5>& capacitors);
    static const std::string _get_name(const std::string& cap_name, const CapacitorSpec& cap_spec, const std::string& type = "group");
};

// ParallelCapacitor class definition
class ParallelCapacitor : public GroupCapacitorBase
{
    std::array<CapacitorInterface*, 5>* _capacitors;
    std::array<CapacitorInterface*, 5> capacitors;
public:
    ParallelCapacitor() = default;
    ParallelCapacitor(ParallelCapacitor&&) = default;
    ParallelCapacitor(const std::string& cap_name, CapacitorInterface* cap1, CapacitorInterface* cap2 = nullptr, CapacitorInterface* cap3 = nullptr, CapacitorInterface* cap4 = nullptr, CapacitorInterface* cap5 = nullptr);
    ParallelCapacitor(std::array<CapacitorInterface*, 5>* capacitors, const std::string& cap_name = "");
    double xc(double f) const override;

    double current(double f, double voltage) const override;

    double allowed_current(double f) const override;

    double voltage(double f, double current) const override;

    void initialize() override;

};

// SeriesCapacitor class definition
class SeriesCapacitor : public GroupCapacitorBase {
    const std::array<CapacitorInterface*, 2>* _capacitors;
public:
    SeriesCapacitor(const std::array<CapacitorInterface*, 2>* capacitors, const std::string& cap_name = "");

    double xc(double f) const override;

    double current(double f, double voltage) const override;

    double allowed_current(double f) const override;

    double voltage(double f, double current) const override;

    void initialize() override;
};

