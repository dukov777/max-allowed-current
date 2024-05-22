
#include "gtest/gtest.h"
namespace {


class CapacitorInterface {
public:
    virtual void initialize() = 0;
};

struct Capacitor : public CapacitorInterface{
    // Define a trivial default constructor
    Capacitor(int i) { }
    void initialize() {}
    static CapacitorInterface* create(int i);
};

struct ParallelCapacitor : public CapacitorInterface{
    // Define a trivial default constructor
    ParallelCapacitor() { /* Initialize as needed */ }
    void initialize() {}
    static CapacitorInterface* create();
};

struct dummy : public CapacitorInterface{
    // Define a trivial default constructor
    dummy() { }
    void initialize() {}
};

union Capacitors {
    void* address;
    dummy dummy;    
    Capacitor single_capacitor;
    ParallelCapacitor parallel_capacitor;

    // Define a default constructor for the union
    Capacitors() : dummy() {} // Initialize one of the members by default
};


struct PoolElement
{
    union Capacitors cap;
    bool is_initialized = false;
};

std::array<PoolElement, 10> pool;
PoolElement& get_free_from_pool()
{
    for(auto& element : pool)
    {
        if(!element.is_initialized)
        {
            element.is_initialized = true;
            return element;
        }
    }
    exit(1);
}


CapacitorInterface* Capacitor::create(int i) 
{
    PoolElement& cap = get_free_from_pool();
    cap.cap.single_capacitor = Capacitor{i};
    return &cap.cap.single_capacitor;
}

CapacitorInterface* ParallelCapacitor::create() 
{
    PoolElement& cap = get_free_from_pool();
    cap.cap.parallel_capacitor = ParallelCapacitor{};
    return &cap.cap.parallel_capacitor;
}


TEST(Pool, Test1) 
{
    CapacitorInterface* cap1 = Capacitor::create(1);
    cap1->initialize();
    CapacitorInterface* cap2 = ParallelCapacitor::create();
    cap2->initialize();

    ASSERT_EQ(cap1, &pool[0].cap.single_capacitor);

    ASSERT_EQ(cap2, &pool[1].cap.parallel_capacitor);

}


} // namespace
