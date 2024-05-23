
#include "gtest/gtest.h"

#include <array>
#include <iostream>
#include "capacitors.h"

namespace {


struct Dummmy : public CapacitorInterface{
    // Define a trivial default constructor
    CapacitorSpec _spec;
    Dummmy() { }
    void initialize() override{}
    double xc(double f) const override { return 0; }
    double current(double f, double voltage) const override { return 0; }
    double allowed_current(double f) const override { return 0; }
    double voltage(double f, double current) const override { return 0; }
    const CapacitorSpec& spec() const override { return _spec; }
    std::string name() const override { return ""; }

};

union Capacitors {
    void* address;
    Dummmy dummy;    
    Capacitor single_capacitor;
    ParallelCapacitor parallel_capacitor;

    // Define a default constructor for the union
    Capacitors() : dummy() {} // Initialize one of the members by default
    ~Capacitors() {}
};


struct PoolElement
{
    union Capacitors cap;
    bool is_taken = false;
};

std::array<PoolElement, 10> pool;
PoolElement& get_free_from_pool()
{
    for(auto& element : pool)
    {
        if(!element.is_taken)
        {
            element.is_taken = true;
            return element;
        }
    }
    exit(1);
}

// Elements pool singleton class
class CapacitorsPool
{
std::array<PoolElement, 10> _pool;
public:
    static CapacitorsPool& get_instance()
    {
        static CapacitorsPool instance;
        return instance;
    }

    Capacitors& get_free_from_pool()
    {
        for(auto& element : _pool)
        {
            if(!element.is_taken)
            {
                element.is_taken = true;
                return element.cap;
            }
        }
        throw std::runtime_error("No free elements in the pool");
        exit(1);
    }

    void return_to_pool(void* element)
    {
        for(auto& pool_element : _pool)
        {
            if(&pool_element.cap == element && pool_element.is_taken)
            {
                pool_element.is_taken = false;
                memset(element, 0, sizeof(Capacitors));
                return;
            }
        }
    }
};



class CapacitorFactory
{
public:
    static CapacitorInterface* create_single(double cap_uF, double vmax, double imax, double power_max, std::string cap_name = "") 
    {
        Capacitors& cap = CapacitorsPool::get_instance().get_free_from_pool();
        // move assignment
        new (&cap.single_capacitor) Capacitor(cap_uF, vmax, imax, power_max, cap_name);
        return &cap.single_capacitor;
    }

    static void release_single(CapacitorInterface* cap) 
    {
        // cast to Capacitor and call destructor
        auto* cap_ = dynamic_cast<Capacitor*>(cap);
        cap_->~Capacitor();
        CapacitorsPool::get_instance().return_to_pool(cap);
    }

    static CapacitorInterface* create_parallel(std::array<CapacitorInterface*, 5>* capacitors, const std::string& cap_name = "") 
    {
        Capacitors& cap = CapacitorsPool::get_instance().get_free_from_pool();
        // move assignment
        new (&cap.parallel_capacitor) ParallelCapacitor(capacitors, cap_name);
        return &cap.parallel_capacitor;
    }

    static CapacitorInterface* create_parallel(const std::string& cap_name = "", CapacitorInterface* cap1 = nullptr, CapacitorInterface* cap2 = nullptr, CapacitorInterface* cap3 = nullptr, CapacitorInterface* cap4 = nullptr, CapacitorInterface* cap5 = nullptr) 
    {
        Capacitors& cap = CapacitorsPool::get_instance().get_free_from_pool();
        // move assignment
        new (&cap.parallel_capacitor) ParallelCapacitor(cap_name, cap1, cap2, cap3, cap4, cap5);
        return &cap.parallel_capacitor;
    }

    static void release_parallel(CapacitorInterface* cap) 
    {
        // cast to Capacitor and call destructor
        auto* cap_ = dynamic_cast<ParallelCapacitor*>(cap);
        cap_->~ParallelCapacitor();
        CapacitorsPool::get_instance().return_to_pool(cap);
    }
};

TEST(Pool, TestCapacitorsPool)
{
    void* address = &CapacitorsPool::get_instance().get_free_from_pool().address;
    ASSERT_NE(address, nullptr);
    CapacitorsPool::get_instance().return_to_pool(address);
    
    address = &CapacitorsPool::get_instance().get_free_from_pool().address;
    ASSERT_NE(address, nullptr);
    for(int i = 0; i < 9; i++)
    {
        address = &CapacitorsPool::get_instance().get_free_from_pool().address;
        ASSERT_NE(address, nullptr);
    }
    ASSERT_THROW(CapacitorsPool::get_instance().get_free_from_pool().address, std::runtime_error);
    CapacitorsPool::get_instance().return_to_pool(address);
}

TEST(Pool, TestCapacitorsFactory)
{
    // CapacitorInterface* cap1 = Capacitor::create(1);
    auto cap1 = CapacitorFactory::create_single(1, 1000, 500, 500e3, "1uF_1000V");
    ASSERT_NE(cap1, nullptr);
    // ASSERT_EQ(cap1, &pool[0].cap.single_capacitor);
    ASSERT_EQ(cap1->name(), "1uF_1000V");
    CapacitorFactory::release_single(cap1);

    cap1 = CapacitorFactory::create_single(1, 1000, 500, 500e3, "2uF_1000V");
    ASSERT_NE(cap1, nullptr);
    ASSERT_EQ(cap1->name(), "2uF_1000V");
    for(int i = 0; i < 9; i++)
    {
        auto cap = CapacitorFactory::create_single(1, 1000, 500, 500e3, "2uF_1000V");
        ASSERT_NE(cap, nullptr);
    }
    ASSERT_THROW(CapacitorFactory::create_single(1, 1000, 500, 500e3, "33uF_1000V"), std::runtime_error);
    CapacitorsPool::get_instance().return_to_pool(cap1);
}

TEST(Pool, Test1) 
{
    // CapacitorInterface* cap1 = Capacitor::create(1);
    auto cap1 = CapacitorFactory::create_single(1, 1000, 500, 500e3, "1uF_1000V");
    ASSERT_NE(cap1, nullptr);
    ASSERT_EQ(cap1->name(), "1uF_1000V");

    std::array<CapacitorInterface*, 5> caps{cap1, nullptr, nullptr, nullptr, nullptr};
    auto cap2 = CapacitorFactory::create_parallel(&caps, "par group1");
    cap2->initialize();
    ASSERT_EQ(cap2->name(), "par group1");

    // auto cap3 = CapacitorFactory::create_parallel("par group2", cap1);
    // // cap3->initialize();
    // ASSERT_EQ(cap3, &pool[2].cap.parallel_capacitor);
    // ASSERT_EQ(cap3->name(), "par group2");

    CapacitorsPool::get_instance().return_to_pool(cap1);
}

} // namespace
