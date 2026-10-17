#ifndef HVPHYSICALDEVICEHPP
#define HVPHYSICALDEVICEHPP

class HVphysicalDevice {
    VkPhysicalDevice _phys_device;
    
public: 
    HVphysicalDevice(VkPhysicalDevice phys_device) noexcept : _phys_device(phys_device) {}
   ~HVphysicalDevice() noexcept {}

    [[nodiscard]] VkPhysicalDevice get_phys_device() const noexcept { return _phys_device; }
   
    operator VkPhysicalDevice() const noexcept { return _phys_device; }
};

#endif // HVPHYSICALDEVICEHPP