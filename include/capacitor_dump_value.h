#pragma once

#include "capacitors.h"


// Decorator base class for current violation
class CapacitoDumpValueDecoratorBase : public CapacitorInterface {
protected:
    CapacitorInterface* cap;

public:
    CapacitoDumpValueDecoratorBase(CapacitorInterface* cap);

    virtual double xc(double f) const override;
    
    virtual std::string name() const override;
    
    virtual const CapacitorSpec& spec() const override;
    
    virtual double allowed_current(double f) const override;

    void initialize() override{};
};

// Decorator for max current violation
class CapacitoDumpValueDecorator : public CapacitoDumpValueDecoratorBase {
public:
    CapacitoDumpValueDecorator(CapacitorInterface* cap);

    virtual double current(double f, double voltage) const override;

    virtual double voltage(double f, double current) const override;
};
