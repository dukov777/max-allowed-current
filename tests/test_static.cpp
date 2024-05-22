#include <vector>
#include <stdexcept>
#include <cmath>

#include "capacitors.h"
#include "capacitor_tank.h"
#include "capacitor_violation_check.h"


#include "gtest/gtest.h"
namespace {


std::array<Capacitor, 5> cap_pool1({Capacitor(), Capacitor(), Capacitor(), Capacitor(), Capacitor()});
std::array<Capacitor, 5> cap_pool2({Capacitor(), Capacitor(), Capacitor(), Capacitor(), Capacitor()});

std::array<CapacitorMaxViolationCheckDecorator, 5> capacitor_decorator_pool1({
    CapacitorMaxViolationCheckDecorator(&cap_pool1[0]),
    CapacitorMaxViolationCheckDecorator(&cap_pool1[1]),
    CapacitorMaxViolationCheckDecorator(&cap_pool1[2]),
    CapacitorMaxViolationCheckDecorator(&cap_pool1[3]),
    CapacitorMaxViolationCheckDecorator(&cap_pool1[4])});

std::array<CapacitorMaxViolationCheckDecorator, 5> capacitor_decorator_pool2({
    CapacitorMaxViolationCheckDecorator(&cap_pool2[0]),
    CapacitorMaxViolationCheckDecorator(&cap_pool2[1]),
    CapacitorMaxViolationCheckDecorator(&cap_pool2[2]),
    CapacitorMaxViolationCheckDecorator(&cap_pool2[3]),
    CapacitorMaxViolationCheckDecorator(&cap_pool2[4])});

std::array<CapacitorInterface*, 5> _caps_pool1;
std::array<CapacitorInterface*, 5> _caps_pool2;

std::array<ParallelCapacitor, 2> parallel_cap_pool({ParallelCapacitor(&_caps_pool1, "par group1"), ParallelCapacitor(&_caps_pool2, "par group2")});
std::array<CapacitorMaxViolationCheckDecorator, 2> parallel_capacitor_decorator_pool1({
    CapacitorMaxViolationCheckDecorator(&parallel_cap_pool[0]),
    CapacitorMaxViolationCheckDecorator(&parallel_cap_pool[1])});

std::array<CapacitorInterface*, 2> _parallel_cap_pool({&parallel_cap_pool[0], &parallel_cap_pool[1]});

// std::array<CapacitorInterface*, 2> _parallel_cap_pool({&parallel_capacitor_decorator_pool1[0], &parallel_capacitor_decorator_pool1[1]});


SeriesCapacitor serial_cap(&_parallel_cap_pool);


TEST(TankTestStatic, TestSeriesBuild) 
{
    cap_pool1[0] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");
    cap_pool1[1] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");

    _caps_pool1[0] = &cap_pool1[0];
    _caps_pool1[1] = &cap_pool1[1];

    ASSERT_EQ(_caps_pool1.size(), 5);
    ASSERT_EQ(_caps_pool2.size(), 5);
    ASSERT_EQ(parallel_cap_pool.size(), 2);
    ASSERT_EQ(_parallel_cap_pool.size(), 2);
    
    parallel_cap_pool[0].initialize();

    ASSERT_NEAR(parallel_cap_pool[0].spec().get_cap_F(), 2e-6, 1e-4);
    ASSERT_EQ(parallel_cap_pool[0].spec().get_v_max(), 1000);
    ASSERT_EQ(parallel_cap_pool[0].spec().get_i_max(), 1000);
    ASSERT_EQ(parallel_cap_pool[0].spec().get_power_max(), 2*500e3);
    ASSERT_NEAR(parallel_cap_pool[0].spec().get_cap_uF(), 2, 1e-4);
    ASSERT_NEAR(parallel_cap_pool[0].current(1, 1000), 0.012566370614359172, 1e-6);
}

TEST(TankTestStatic, TestParallelSpecs) 
{
    cap_pool1[0] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");
    cap_pool1[1] = Capacitor(33, 700, 600, 500e3, "33uF_700V");

    _caps_pool1[0] = &cap_pool1[0];
    _caps_pool1[1] = &cap_pool1[1];
    
    parallel_cap_pool[0].initialize();

    ASSERT_NEAR(parallel_cap_pool[0].spec().get_cap_F(), 34e-6, 1e-4);
    ASSERT_EQ(parallel_cap_pool[0].spec().get_v_max(), 700);
    ASSERT_EQ(parallel_cap_pool[0].spec().get_power_max(), 2*500e3);
    ASSERT_NEAR(parallel_cap_pool[0].spec().get_cap_uF(), 34, 1e-4);
    ASSERT_NEAR(parallel_cap_pool[0].current(1, 1000), 0.21362830, 1e-4);
}

TEST(TankTestStatic, TestParallelSpecsDecorated) 
{
    cap_pool1[0] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");
    cap_pool1[1] = Capacitor(33, 700, 600, 500e3, "33uF_700V");

    _caps_pool1[0] = &capacitor_decorator_pool1[0];
    _caps_pool1[1] = &capacitor_decorator_pool1[1];
    
    _parallel_cap_pool[0]->initialize();
    _parallel_cap_pool[1]->initialize();

    ASSERT_NEAR(parallel_cap_pool[0].spec().get_cap_F(), 34e-6, 1e-6);
    ASSERT_EQ(parallel_cap_pool[0].spec().get_v_max(), 700);
    ASSERT_EQ(parallel_cap_pool[0].spec().get_power_max(), 2*500e3);
    ASSERT_NEAR(parallel_cap_pool[0].spec().get_cap_uF(), 34, 1e-0);
    ASSERT_NEAR(parallel_cap_pool[0].current(1, 1000), 0.21362830, 1e-4);

    ASSERT_EQ(parallel_cap_pool[1].spec().get_cap_F(), 0);
    ASSERT_EQ(parallel_cap_pool[1].spec().get_v_max(), INFINITY);
    ASSERT_EQ(parallel_cap_pool[1].spec().get_power_max(), 0);
    ASSERT_EQ(parallel_cap_pool[1].spec().get_cap_uF(), 0);
    ASSERT_EQ(parallel_cap_pool[1].current(1, 1000), 0);
}


TEST(TankTestStatic, TestSeriesWith1ParallelGroup) 
{
    cap_pool1[0] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");
    cap_pool1[1] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");

    _caps_pool1[0] = &cap_pool1[0];
    _caps_pool1[1] = &cap_pool1[1];

    ASSERT_EQ(_caps_pool1.size(), 5);
    ASSERT_EQ(_caps_pool2.size(), 5);
    ASSERT_EQ(parallel_cap_pool.size(), 2);
    ASSERT_EQ(_parallel_cap_pool.size(), 2);
    
    serial_cap.initialize();

    ASSERT_NEAR(parallel_cap_pool[0].current(1, 1000), 0.012566, 1e-4);
    ASSERT_NEAR(parallel_cap_pool[1].current(1, 1000), 0.0, 1e-4);
    
    ASSERT_NEAR(serial_cap.current(1, 1000), 0.0, 1e-4);
}

TEST(TankTestStatic, TestSeries) 
{
    cap_pool1[0] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");
    _caps_pool1[0] = &cap_pool1[0];

    cap_pool1[1] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");
    _caps_pool1[1] = &cap_pool1[1];

    cap_pool2[0] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");
    _caps_pool2[0] = &cap_pool2[0];

    ASSERT_EQ(_caps_pool1.size(), 5);
    ASSERT_EQ(_caps_pool2.size(), 5);
    ASSERT_EQ(parallel_cap_pool.size(), 2);
    ASSERT_EQ(_parallel_cap_pool.size(), 2);

    serial_cap.initialize();

    ASSERT_NEAR(parallel_cap_pool[0].current(1, 1000), 0.012566, 1e-4);
    ASSERT_NEAR(parallel_cap_pool[1].current(1, 1000), 0.012566/2, 1e-4);

    ASSERT_NEAR(_parallel_cap_pool[0]->current(1, 1000), 0.012566, 1e-4);
    ASSERT_NEAR(_parallel_cap_pool[1]->current(1, 1000), 0.012566/2, 1e-4);
    
    ASSERT_NEAR(serial_cap.current(1, 2000), 0.00837758040, 1e-4);
    ASSERT_NO_THROW(serial_cap.current(100000, 2000));
}

TEST(TankTestStatic, TestRuntimeViolatorDecorator)
{
    cap_pool1[0] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");
    _caps_pool1[0] = &cap_pool1[0];

    cap_pool1[1] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");
    _caps_pool1[1] = &cap_pool1[1];

    cap_pool2[0] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");
    _caps_pool2[0] = &cap_pool2[0];

    ASSERT_EQ(_caps_pool1.size(), 5);
    ASSERT_EQ(_caps_pool2.size(), 5);
    ASSERT_EQ(parallel_cap_pool.size(), 2);
    ASSERT_EQ(_parallel_cap_pool.size(), 2);

    // runtime decorate the parallel capacitors
    _parallel_cap_pool[0] = &parallel_capacitor_decorator_pool1[0];
    _parallel_cap_pool[1] = &parallel_capacitor_decorator_pool1[1];

    serial_cap.initialize();

    ASSERT_NEAR(parallel_cap_pool[0].current(1, 1000), 0.012566, 1e-4);
    ASSERT_NEAR(parallel_cap_pool[1].current(1, 1000), 0.012566/2, 1e-4);

    ASSERT_NEAR(_parallel_cap_pool[0]->current(1, 1000), 0.012566, 1e-4);
    ASSERT_NEAR(_parallel_cap_pool[1]->current(1, 1000), 0.012566/2, 1e-4);
    
    ASSERT_NEAR(serial_cap.current(1, 2000), 0.00837758040, 1e-4);
    ASSERT_ANY_THROW(serial_cap.current(100000, 2000));
}


} // namespace
