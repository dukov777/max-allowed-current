
#include "gtest/gtest.h"

#include <array>
#include <iostream>
#include "capacitors.h"

#include "MemoryMngr.h"
#include "capacitors.h"

namespace {

SeriesCapacitor* series_global;

TEST(Pool, TestSeriesHack)
{
    auto cap1 = CapacitorFactory::create_single(1, 1000, 500, 500e3, "1uF_1000V");
    ASSERT_EQ(cap1->name(), "1uF_1000V");
    
    auto cap2 = CapacitorFactory::create_single(2, 800, 500, 500e3, "2uF_800V");
    ASSERT_EQ(cap2->name(), "2uF_800V");

    auto parallel1 = CapacitorFactory::create_parallel("parallel capacitor 1", cap2.get());

    // SeriesCapacitor series("series", cap1, cap2);
    // series_global = new SeriesCapacitor("series", cap1, cap2);
    series_global = new SeriesCapacitor("series", cap1, parallel1);
    
    // ASSERT_EQ(series.spec().get_v_max(), 1800);
}

// TEST(Pool, test2)
// {
//     ASSERT_EQ(series_global->spec().get_v_max(), 1800);
//     delete series_global;
// }

TEST(Pool, TestCapacitor)
{
    auto cap_ptr1 = CapacitorFactory::create_single(1, 1000, 500, 500e3, "1uF_1000V");
    CapacitorFactory::create_single(1, 1000, 500, 500e3, "2uF_1000V");
    auto cap2_ptr = CapacitorFactory::create_single(1, 1000, 500, 500e3, "2uF_1000V");
    ASSERT_EQ(cap2_ptr->name(), "2uF_1000V");
}

TEST(Pool, TestParallelCapacitor) 
{
    auto cap1 = CapacitorFactory::create_single(1, 1000, 500, 500e3, "1uF_1000V");
    auto cap1_expected = new Capacitor(1, 1000, 500, 500e3, "1uF_1000V");

    auto parallel_cap2 = CapacitorFactory::create_parallel("parallel capacitor 2", cap1);

    ASSERT_EQ(parallel_cap2->name(), "parallel capacitor 2");
    ASSERT_EQ(parallel_cap2->spec().get_v_max(),cap1_expected->spec().get_v_max());
    ASSERT_EQ(parallel_cap2->spec().get_i_max(), cap1_expected->spec().get_i_max());
    ASSERT_EQ(parallel_cap2->spec().get_power_max(), cap1_expected->spec().get_power_max());
    ASSERT_EQ(parallel_cap2->voltage(1, 1000), cap1_expected->voltage(1, 1000));
    ASSERT_EQ(parallel_cap2->current(1, 1000), cap1_expected->current(1, 1000));
    ASSERT_EQ(parallel_cap2->allowed_current(1), cap1_expected->allowed_current(1));
}

TEST(Pool, TestParallelCapacitorWith2CapacitorsSameCapacitance) 
{
    auto cap11 = CapacitorFactory::create_single(1, 1000, 500, 500e3, "1uF_1000V");
    auto cap12 = CapacitorFactory::create_single(1, 800, 600, 500e3, "1uF_800V");
    
    auto cap11_expected = new Capacitor(1, 1000, 500, 500e3, "1uF_1000V");
    auto cap12_expected = new Capacitor(1, 800, 600, 500e3, "1uF_800V");

    auto parallel_cap2 = CapacitorFactory::create_parallel("parallel capacitor 2", cap11, cap12);
    
    ASSERT_EQ(parallel_cap2->name(), "parallel capacitor 2");
    ASSERT_NE(parallel_cap2->spec().get_v_max(), cap11_expected->spec().get_v_max());
    ASSERT_EQ(parallel_cap2->spec().get_v_max(), cap12_expected->spec().get_v_max());
    ASSERT_EQ(parallel_cap2->spec().get_i_max(), cap11_expected->spec().get_i_max() + cap12_expected->spec().get_i_max());

    ASSERT_EQ(parallel_cap2->spec().get_power_max(), cap11_expected->spec().get_power_max() + cap12_expected->spec().get_power_max());
    
    double xc = 1 / (1/cap11_expected->xc(1) + 1/cap12_expected->xc(1));
    ASSERT_EQ(parallel_cap2->voltage(1, 1000), xc * 1000);
    ASSERT_EQ(parallel_cap2->current(1, 1000), cap11_expected->current(1, 1000) + cap12_expected->current(1, 1000));
    
    double allowed_current = cap11_expected->current(1, 800) + cap12_expected->allowed_current(1);
    ASSERT_EQ(parallel_cap2->allowed_current(1), allowed_current);
}

TEST(Pool, TestParallelCapacitorWith2CapacitorsDifferentCapacitance) 
{
    auto cap11 = CapacitorFactory::create_single(1, 1000, 500, 500e3, "1uF_1000V");
    auto cap12 = CapacitorFactory::create_single(2, 800, 600, 500e3, "1uF_800V");

    auto cap11_expected = new Capacitor(1, 1000, 500, 500e3, "1uF_1000V");
    auto cap12_expected = new Capacitor(2, 800, 600, 500e3, "1uF_800V");

    auto parallel_cap2 = CapacitorFactory::create_parallel("parallel capacitor 2", cap11, cap12);
    
    ASSERT_EQ(parallel_cap2->name(), "parallel capacitor 2");
    ASSERT_NE(parallel_cap2->spec().get_v_max(), cap11_expected->spec().get_v_max());
    ASSERT_EQ(parallel_cap2->spec().get_v_max(), cap12_expected->spec().get_v_max());
    ASSERT_EQ(parallel_cap2->spec().get_i_max(), cap11_expected->spec().get_i_max() + cap12_expected->spec().get_i_max());

    ASSERT_EQ(parallel_cap2->spec().get_power_max(), cap11_expected->spec().get_power_max() + cap12_expected->spec().get_power_max());
    
    double xc = 1 / (1/cap11_expected->xc(1) + 1/cap12_expected->xc(1));
    ASSERT_EQ(parallel_cap2->voltage(1, 1000), xc * 1000);
    ASSERT_EQ(parallel_cap2->current(1, 1000), cap11_expected->current(1, 1000) + cap12_expected->current(1, 1000));
    
    double allowed_current = cap11_expected->current(1, 800) + cap12_expected->allowed_current(1);
    ASSERT_EQ(parallel_cap2->allowed_current(1), allowed_current);
}


TEST(Pool, TestSeriesCapacitor) 
{
    auto cap1 = CapacitorFactory::create_single(1, 1000, 500, 500e3, "1uF_1000V");
    auto cap1_expected =          new Capacitor(1, 1000, 500, 500e3, "1uF_1000V");

    auto cap2 = CapacitorFactory::create_single(1, 1000, 500, 500e3, "1uF_1000V");
    auto cap2_expected =          new Capacitor(1, 1000, 500, 500e3, "1uF_1000V");

    auto parallel1 = CapacitorFactory::create_parallel("parallel capacitor 1", cap1);
    auto parallel2 = CapacitorFactory::create_parallel("parallel capacitor 2", cap2);

    ParallelCapacitor parallel_cap1_values("parallel capacitor 1", cap1_expected);
    ParallelCapacitor parallel_cap2_values("parallel capacitor 2", cap2_expected);

    SeriesCapacitor series("serial capacitor 1", parallel1, parallel2);
    ASSERT_EQ(series.name(), "serial capacitor 1");
    ASSERT_EQ(series.spec().get_v_max(), parallel_cap1_values.spec().get_v_max() + parallel_cap2_values.spec().get_v_max());
    ASSERT_EQ(series.spec().get_i_max(), parallel_cap1_values.spec().get_i_max());
    
    ASSERT_EQ(series.spec().get_power_max(), parallel_cap1_values.spec().get_power_max() + parallel_cap2_values.spec().get_power_max());
    
    double xc = parallel_cap1_values.xc(1) + parallel_cap2_values.xc(1) + 10;
    ASSERT_EQ(series.voltage(1, 1000), xc * 1000);
    
    ASSERT_EQ(series.current(1, 1000), parallel_cap1_values.current(1, 1000/2));
    ASSERT_EQ(series.allowed_current(1), parallel_cap1_values.allowed_current(1));
}

TEST(Pool, Pool_TestSeriesCapacitorCornerCase) 
{
    auto cap11 = CapacitorFactory::create_single(1, 1000, 500, 500e3, "1uF_1000V");
    auto cap11_expected =          new Capacitor(1, 1000, 500, 500e3, "1uF_1000V");

    auto cap21 = CapacitorFactory::create_single(1, 800, 600, 500e3, "1uF_800V");
    auto cap21_expected =          new Capacitor(1, 800, 600, 500e3, "1uF_800V");

    auto parallel1 = CapacitorFactory::create_parallel("parallel capacitor 1", cap11);
    auto parallel2 = CapacitorFactory::create_parallel("parallel capacitor 2", cap21);

    ASSERT_EQ(parallel1->spec().get_v_max(), cap11_expected->spec().get_v_max());
    ASSERT_EQ(parallel2->spec().get_v_max(), cap21_expected->spec().get_v_max());

    ParallelCapacitor parallel_cap1_values("parallel capacitor 1", cap11_expected);
    ParallelCapacitor parallel_cap2_values("parallel capacitor 1", cap21_expected);

    SeriesCapacitor series("serial capacitor 1", parallel1, parallel2);

    ASSERT_EQ(series.name(), "serial capacitor 1");
    ASSERT_EQ(series.spec().get_v_max(), parallel_cap1_values.spec().get_v_max() + parallel_cap2_values.spec().get_v_max());

    ASSERT_NE(series.spec().get_i_max(), parallel_cap2_values.spec().get_i_max());
    ASSERT_EQ(series.spec().get_i_max(), parallel_cap1_values.spec().get_i_max());

    ASSERT_EQ(series.spec().get_power_max(), parallel_cap1_values.spec().get_power_max() + parallel_cap2_values.spec().get_power_max());

    ASSERT_EQ(series.voltage(1, 1000), parallel_cap1_values.voltage(1, 1000) + parallel_cap2_values.voltage(1, 1000));
    ASSERT_EQ(series.current(1, 1000), parallel_cap1_values.current(1, 1000/2));
    
    ASSERT_NE(series.allowed_current(1), parallel_cap1_values.allowed_current(1));
    ASSERT_EQ(series.allowed_current(1), parallel_cap2_values.allowed_current(1));
}

// TEST(Pool, Pool_TestSeriesCapacitorCornerCase2DifferentCapacitors) 
// {
//     auto cap11 = CapacitorFactory::create_single(1, 1000, 500, 500e3, "1uF_1000V");
//     auto cap21 = CapacitorFactory::create_single(2, 800, 600, 500e3, "2uF_800V");

//     auto parallel_cap1 = CapacitorFactory::create_parallel("parallel capacitor 1", cap11.get());
//     ASSERT_EQ(parallel_cap1->spec().get_v_max(), cap11->spec().get_v_max());
    
//     auto parallel_cap2 = CapacitorFactory::create_parallel("parallel capacitor 2", cap21.get());
//     ASSERT_EQ(parallel_cap2->spec().get_v_max(), cap21->spec().get_v_max());

//     // SeriesCapacitor series("serial capacitor 1", parallel_cap1.get(), parallel_cap2.get());
//     SeriesCapacitor series("serial capacitor 1", parallel_cap1, parallel_cap2);
//     ASSERT_EQ(series.name(), "serial capacitor 1");
//     ASSERT_EQ(series.spec().get_v_max(), parallel_cap1->spec().get_v_max() + parallel_cap2->spec().get_v_max());

//     ASSERT_NE(series.spec().get_i_max(), parallel_cap2->spec().get_i_max());
//     ASSERT_EQ(series.spec().get_i_max(), parallel_cap1->spec().get_i_max());

//     ASSERT_EQ(series.spec().get_power_max(), parallel_cap1->spec().get_power_max() + parallel_cap2->spec().get_power_max());

//     ASSERT_EQ(series.voltage(1, 1000), parallel_cap1->voltage(1, 1000) + parallel_cap2->voltage(1, 1000));

//     double current = 1000 / (parallel_cap1->xc(1) + parallel_cap2->xc(1));
//     ASSERT_EQ(series.current(1, 1000), current);
    
//     double allowed_current = std::min(parallel_cap1->allowed_current(1), parallel_cap2->allowed_current(1));

//     ASSERT_EQ(series.allowed_current(1), allowed_current);

// }


} // namespace
