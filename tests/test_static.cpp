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
std::array<CapacitorInterface*, 5> _caps_pool1({&cap_pool1[0], &cap_pool1[1], &cap_pool1[2], &cap_pool1[3], &cap_pool1[4]});
std::array<CapacitorInterface*, 5> _caps_pool2({&cap_pool2[0], &cap_pool2[1], &cap_pool2[2], &cap_pool2[3], &cap_pool2[4]});

std::array<ParallelCapacitor, 2> parallel_cap_pool({ParallelCapacitor(_caps_pool1), ParallelCapacitor(_caps_pool2)});
std::array<CapacitorInterface*, 2> _parallel_cap_pool({&parallel_cap_pool[0], &parallel_cap_pool[1]});
SeriesCapacitor serial_cap(_parallel_cap_pool);

// TEST(TankTestStatic, TestSeriesPointers) 
// {
//     cap_pool1[0] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");
//     cap_pool1[1] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");
//     for(auto& caps : _caps_pool1)
//     {
//         caps = nullptr;
//     }
//     ASSERT_EQ(_caps_pool1.size(), 5);
//     ASSERT_EQ(_caps_pool1[0], nullptr);
//     ASSERT_EQ(_caps_pool1[1], nullptr);
//     ASSERT_EQ(_caps_pool1.size(), 5);
//     ASSERT_EQ(_caps_pool2.size(), 5);
//     ASSERT_EQ(parallel_cap_pool.size(), 2);
//     ASSERT_EQ(_parallel_cap_pool.size(), 2);
//     serial_cap.initialize();

//     ASSERT_NEAR(parallel_cap_pool[0].current(1, 1000), 0.012566, 1e-4);
//     ASSERT_NEAR(parallel_cap_pool[1].current(1, 1000), 0.0, 1e-4);
    
//     ASSERT_NEAR(serial_cap.current(1, 1000), 0.0, 1e-4);
// }

TEST(TankTestStatic, TestSeriesBuild) 
{
    cap_pool1[0] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");
    cap_pool1[1] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");

    ASSERT_EQ(_caps_pool1.size(), 5);
    ASSERT_EQ(_caps_pool2.size(), 5);
    ASSERT_EQ(parallel_cap_pool.size(), 2);
    ASSERT_EQ(_parallel_cap_pool.size(), 2);
    serial_cap.initialize();

    ASSERT_NEAR(parallel_cap_pool[0].current(1, 1000), 0.012566, 1e-4);
    ASSERT_NEAR(parallel_cap_pool[1].current(1, 1000), 0.0, 1e-4);
    
    ASSERT_NEAR(serial_cap.current(1, 1000), 0.0, 1e-4);
}

TEST(TankTestStatic, TestSeriesBuild1) 
{
    cap_pool1[0] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");
    cap_pool1[1] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");
    cap_pool2[0] = Capacitor(1, 1000, 500, 500e3, "1uF_1000V");

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
