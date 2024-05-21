#include <vector>
#include <stdexcept>
#include <cmath>

#include "capacitors.h"
#include "capacitor_tank.h"
#include "capacitor_violation_check.h"


#include "gtest/gtest.h"
namespace {


TEST(TankTest, TestCapacitor) {
    ASSERT_EQ(1, 1);
}

std::array<Capacitor, 5> cap_pool1({Capacitor(), Capacitor(), Capacitor(), Capacitor(), Capacitor()});
std::array<Capacitor, 5> cap_pool2({Capacitor(), Capacitor(), Capacitor(), Capacitor(), Capacitor()});
std::array<CapacitorInterface*, 5> _caps_pool1;
std::array<CapacitorInterface*, 5> _caps_pool2;

std::array<ParallelCapacitor, 2> parallel_cap_pool({ParallelCapacitor(&_caps_pool1), ParallelCapacitor(&_caps_pool2)});
std::array<CapacitorInterface*, 2> _parallel_cap_pool({&parallel_cap_pool[0], &parallel_cap_pool[1]});
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
    ASSERT_EQ(parallel_cap_pool[0].spec().get_power_max(), 2*500e3);
    ASSERT_NEAR(parallel_cap_pool[0].spec().get_cap_uF(), 2, 1e-4);
    ASSERT_EQ(parallel_cap_pool[0].current(1, 1000), 0.012566370614359172);
}

TEST(TankTestStatic, TestSeriesBuild2) 
{
    cap_pool1[0] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");
    cap_pool1[1] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");

    _caps_pool1[0] = &cap_pool1[0];
    _caps_pool1[1] = &cap_pool1[1];

    ASSERT_EQ(_caps_pool1.size(), 5);
    ASSERT_EQ(_caps_pool2.size(), 5);
    ASSERT_EQ(parallel_cap_pool.size(), 2);
    ASSERT_EQ(_parallel_cap_pool.size(), 2);
    
    // ASSERT_NEAR(parallel_cap_pool[1].current(1, 1000), 0.0, 1e-4);
    serial_cap.initialize();

    ASSERT_NEAR(parallel_cap_pool[0].current(1, 1000), 0.012566, 1e-4);
    ASSERT_NEAR(parallel_cap_pool[1].current(1, 1000), 0.0, 1e-4);
    
    ASSERT_NEAR(serial_cap.current(1, 1000), 0.0, 1e-4);
}

TEST(TankTestStatic, TestSeriesBuild1) 
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
    
    ASSERT_NEAR(serial_cap.current(1, 2000), 0.00837758040, 1e-4);
}


} // namespace
