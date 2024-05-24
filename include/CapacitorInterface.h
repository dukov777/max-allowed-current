#pragma once

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
    virtual const std::string name() const = 0;
    virtual ~CapacitorInterface() {}
};

