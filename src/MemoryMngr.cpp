#include <memory>
#include <string>
#include <array>
#include <cstring>
#include <stdexcept>

#include "CapacitorInterface.h"
#include "capacitors.h"

#include "MemoryMngr.h"

struct Dummmy : public CapacitorInterface{
    // Define a trivial default constructor
    CapacitorSpec _spec;
    Dummmy() { }
    double xc(double f) const override { return 0; }
    double current(double f, double voltage) const override { return 0; }
    double allowed_current(double f) const override { return 0; }
    double voltage(double f, double current) const override { return 0; }
    const CapacitorSpec& spec() const override { return _spec; }
    const std::string name() const override { return ""; }
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

// Elements pool singleton class
class CapacitorsPool
{
    typedef std::array<PoolElement, 10> Pool;
    std::array<PoolElement, 10> _pool;
public:
    static CapacitorsPool& get_instance();
    Capacitors& get_free_from_pool();
    void return_to_pool(void* element);
};


// template <typename T>
// void CapacitorDeleter<T>::operator()(CapacitorInterface* ptr) const
// {
//     // cast to Capacitor and call destructor
//     // auto* cap_ = static_cast<T*>(ptr);
//     // cap_->~T();
//     ptr->~CapacitorInterface();
//     CapacitorsPool::get_instance().return_to_pool(ptr);
// }

void CapacitorDeleter::operator()(CapacitorInterface* ptr) const
{
    // ptr->~CapacitorInterface();
    // CapacitorsPool::get_instance().return_to_pool(ptr);
    delete ptr;
}


CapacitorsPool& 
CapacitorsPool::get_instance()
{
    static CapacitorsPool instance;
    return instance;
}

Capacitors& 
CapacitorsPool::get_free_from_pool()
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

void 
CapacitorsPool::return_to_pool(void* element)
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

// CapacitorPtr
// CapacitorFactory::create_single(double cap_uF, double vmax, double imax, double power_max, std::string cap_name)
// {
//     Capacitors& cap = CapacitorsPool::get_instance().get_free_from_pool();
//     new (&cap.single_capacitor) Capacitor(cap_uF, vmax, imax, power_max, cap_name);
//     return CapacitorPtr(&cap.single_capacitor);
// }

// CapacitorPtr
// CapacitorFactory::create_parallel(const std::string& cap_name, CapacitorInterface* cap1, CapacitorInterface* cap2, CapacitorInterface* cap3, CapacitorInterface* cap4, CapacitorInterface* cap5) 
// {
//     Capacitors& cap = CapacitorsPool::get_instance().get_free_from_pool();
//     new (&cap.parallel_capacitor) ParallelCapacitor(cap_name, cap1, cap2, cap3, cap4, cap5);
//     return std::unique_ptr<CapacitorInterface, CapacitorDeleter<ParallelCapacitor>>(&cap.parallel_capacitor);
// }

CapacitorPtr
CapacitorFactory::create_single(double cap_uF, double vmax, double imax, double power_max, std::string cap_name)
{
    return CapacitorPtr(new Capacitor(cap_uF, vmax, imax, power_max, cap_name));
}

CapacitorPtr CapacitorFactory::create_parallel(const std::string &cap_name, CapacitorPtr &cap1)
{
    return CapacitorPtr(new ParallelCapacitor(cap_name, cap1));
}

CapacitorPtr CapacitorFactory::create_parallel(const std::string &cap_name, CapacitorPtr &cap1, CapacitorPtr &cap2)
{
    return CapacitorPtr(new ParallelCapacitor(cap_name, cap1, cap2));
}

CapacitorPtr CapacitorFactory::create_parallel(const std::string &cap_name, CapacitorPtr &cap1, CapacitorPtr &cap2, CapacitorPtr &cap3)
{
    return CapacitorPtr(new ParallelCapacitor(cap_name, cap1, cap2, cap3));
}

CapacitorPtr CapacitorFactory::create_parallel(const std::string &cap_name, CapacitorPtr &cap1, CapacitorPtr &cap2, CapacitorPtr &cap3, CapacitorPtr &cap4)
{
    return CapacitorPtr(new ParallelCapacitor(cap_name, cap1, cap2, cap3, cap4));
}

CapacitorPtr CapacitorFactory::create_parallel(const std::string &cap_name, CapacitorPtr &cap1, CapacitorPtr &cap2, CapacitorPtr &cap3, CapacitorPtr &cap4, CapacitorPtr &cap5)
{
    return CapacitorPtr(new ParallelCapacitor(cap_name, cap1, cap2, cap3, cap4, cap5));
}
