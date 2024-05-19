#include <vector>
#include <stdexcept>
#include <cmath>
#include "capacitors.h"  // Assuming the translated classes are defined in this header file

#include "gtest/gtest.h"
namespace {


// TEST(CapacitorTest, TestCapacitor) {
//     double f = 1;
//     Capacitor cap(1, 1000, 500);
//     ASSERT_NEAR(cap.current(f, 1000), 0.006283, 1e-4);
// }

// TEST(CapacitorTest, TestCapacitorXc) {
//     double f = 1;
//     Capacitor cap(1, 1000, 500);
//     ASSERT_NEAR(cap.xc(f), 159154.943, 1e-2);
// }

// TEST(CapacitorTest, TestCapacitorNameEmpty) {
//     Capacitor cap(1, 1000, 500);
//     ASSERT_EQ(cap.name(), "1uF/1000V");
// }

// TEST(CapacitorTest, TestCapacitorName) {
//     Capacitor cap(1, 1000, 500, 0.0, "C1");
//     ASSERT_EQ(cap.name(), "C1");
// }

// TEST(CapacitorTest, TestParallelCapacitorSpecVmax) {
//     Capacitor cap1(1, 1000, 500);
//     Capacitor cap2(1, 100, 500);
//     ParallelCapacitor parallel({cap1, cap2});
//     ASSERT_EQ(parallel.spec().get_v_max(), 100);
// }

// TEST(CapacitorTest, TestParallelCapacitorXc) {
//     Capacitor cap1(1, 1000, 500);
//     Capacitor cap2(1, 1000, 500);
//     ParallelCapacitor parallel({cap1, cap2});
//     ASSERT_NEAR(parallel.xc(1), 79577.472, 1e-2);
// }

// TEST(CapacitorTest, TestParallelCapacitorCurrent) {
//     Capacitor cap1(1, 1000, 500);
//     Capacitor cap2(1, 1000, 500);
//     ParallelCapacitor parallel({cap1, cap2});
//     ASSERT_NEAR(parallel.current(1, 1000), 0.012566, 1e-4);
// }

// TEST(CapacitorTest, TestParallelCapacitorVoltage) {
//     Capacitor cap1(1, 1000, 500);
//     Capacitor cap2(1, 1000, 500);
//     ParallelCapacitor parallel({cap1, cap2});
//     ASSERT_NEAR(parallel.voltage(1, 0.012566), 1000, 1e-1);
// }

// TEST(CapacitorTest, TestSeriesCapacitorXc) {
//     Capacitor cap1(1, 1000, 500);
//     Capacitor cap2(1, 1000, 500);
//     SeriesCapacitor series({cap1, cap2});
//     ASSERT_NEAR(series.xc(1), 318309.886, 1e-2);
// }

// TEST(CapacitorTest, TestSeriesCapacitorCurrent) {
//     Capacitor cap1(1, 1000, 500);
//     Capacitor cap2(1, 1000, 500);
//     SeriesCapacitor series({cap1, cap2});
//     ASSERT_NEAR(series.current(1, 1000), 0.0031415926554037347, 1e-4);
// }

// TEST(CapacitorTest, TestSeriesCapacitorVoltage) {
//     Capacitor cap1(1, 1000, 500);
//     Capacitor cap2(1, 1000, 500);
//     SeriesCapacitor series({cap1, cap2});
//     ASSERT_NEAR(series.voltage(1, 0.0031415926554037347), 1000, 1e-3);
// }

// TEST(CapacitorTest, TestParallelCapacitorDecoratorCurrent) {
//     Capacitor cap1(1, 1000, 500);
//     Capacitor cap2(1, 1000, 500);
//     ParallelCapacitor parallel({cap1, cap2});
//     ParallelCapacitorMaxCurrentViolationDecorator decoratedParallel(&parallel);
//     ASSERT_NEAR(decoratedParallel.current(1, 1000), 0.012566, 1e-4);
// }

// TEST(CapacitorTest, TestParallelCapacitorDecoratorCurrentExceedingRange) {
//     double f = 50000;
//     Capacitor cap1(10, 1000, 500);
//     Capacitor cap2(20, 800, 600);
//     ParallelCapacitor parallel({cap1, cap2});
//     ParallelCapacitorMaxCurrentViolationDecorator decoratedParallel(&parallel);
//     ASSERT_THROW(decoratedParallel.current(f, 1000), std::runtime_error);
// }

TEST(CapacitorTest, TestCapacitorDecoratorExceedingRange) {
    double f = 50000;
    Capacitor cap(10, 1000, 500);
    CapacitorMaxCurrentViolationDecorator decoratedCap(&cap);
    ASSERT_THROW(decoratedCap.current(f, 1000), std::runtime_error);
}

TEST(CapacitorTest, TestParallelGroupSingleCapacitorExceedingRange) {
    double f = 500000;
    CapacitorMaxCurrentViolationDecorator cap1(new Capacitor(10, 1000, 500));
    CapacitorMaxCurrentViolationDecorator cap2(new Capacitor(20, 800, 600));
    ParallelCapacitor parallel({&cap1, &cap2});
    ASSERT_THROW(parallel.current(f, 1000), std::runtime_error);
}

// TEST(CapacitorTest, TestSeriesCapacitor) {
//     double f = 1;
//     Capacitor cap1(1, 1000, 500);
//     Capacitor cap2(1, 1000, 500);
//     SeriesCapacitor series({cap1, cap2});

//     double xc = 1 / (2 * M_PI * f * 0.5e-6);
//     double expected_current = 1000 / xc;
//     ASSERT_NEAR(series.current(f, 1000), expected_current, 1e-4);

//     Capacitor cap3(1, 1000, 500);
//     Capacitor cap4(3, 1000, 500);
//     SeriesCapacitor series2({cap3, cap4});

//     xc = 1 / (2 * M_PI * f * 0.75e-6);
//     expected_current = 1000 / xc;
//     ASSERT_NEAR(series2.current(f, 1000), expected_current, 1e-4);
// }

// TEST(CapacitorTest, TestSeriesCapacitorVoltage1) {
//     double f = 1;
//     Capacitor cap1(1, 1000, 500);
//     Capacitor cap2(1, 1000, 500);
//     SeriesCapacitor series({cap1, cap2});

//     double xc = 1 / (2 * M_PI * f * 0.5e-6);
//     double expected_voltage = 0.0031415926554037347 * xc;
//     ASSERT_NEAR(series.voltage(f, 0.0031415926554037347), expected_voltage, 1e-3);
// }

// TEST(CapacitorTest, TestParallelCapacitorsAllowedMaxCurrent) {
//     double f = 1;
//     Capacitor cap1(10, 1000, 500);
//     Capacitor cap2(20, 800, 600);
//     std::vector<CapacitorInterface*> caps({&cap1, &cap2});
//     ParallelCapacitor parallel(caps);

//     double c = 10 + 20;
//     double xc = 1 / (2 * M_PI * f * c * 1e-6);
//     double expected_current = 800 / xc;

//     ASSERT_NEAR(parallel.allowed_current(f), expected_current, 1e-0);
// }

// TEST(CapacitorTest, TestSeriesCapacitorsAllowedMaxCurrent) {
//     double f = 1;
//     Capacitor cap1(10, 1000, 500);
//     Capacitor cap2(20, 800, 600);
    
//     SeriesCapacitor serial({cap1, cap2});


//     double c = 20;
//     double xc = 1 / (2 * M_PI * f * c * 1e-6);
//     double expected_current = 800 / xc;

//     ASSERT_NEAR(serial.allowed_current(f), expected_current, 1e-0);
// }

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
