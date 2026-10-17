#ifndef HVSWAPCHAINHPP
#define HVSWAPCHAINHPP

#include "vulkan/swapchain/strategy/IHVswapchainConfigurationStrategy.hpp"

class HVswapchain {
    VkSwapchainKHR _swapchain;

    HVdevice*                          _device;
    HVsurface*                         _surface;
    Hwindow*                           _window;
    IHVswapchainConfigurationStrategy* _strategy;

    HVswapchain(VkSwapchainKHR swapchain,
                HVdevice* device, 
                HVsurface* surface,
                Hwindow* window,
                IHVswapchainConfigurationStrategy* strategy) : _swapchain(swapchain),
                                                               _device(device),
                                                               _surface(surface),
                                                               _window(window),
                                                               _strategy(strategy) {}

public:
    static HVswapchain* initialize(HVdevice* device, 
                                   HVsurface* surface,
                                   Hwindow* window,
                                   IHVswapchainConfigurationStrategy* strategy);
    
    void rebuild_swapchain();

    constexpr operator VkSwapchainKHR() { return _swapchain; }

   ~HVswapchain() {
        if (_swapchain != VK_NULL_HANDLE) vkDestroySwapchainKHR(*_device, _swapchain, nullptr);
    }

    HVswapchain(HVswapchain&& other) : _swapchain(other._swapchain),
                                       _device(other._device),
                                       _surface(other._surface),
                                       _window(other._window),
                                       _strategy(other._strategy) {
        other._swapchain = VK_NULL_HANDLE;
        other._device = VK_NULL_HANDLE;
        other._surface = VK_NULL_HANDLE;
        other._window = VK_NULL_HANDLE;
        other._strategy = VK_NULL_HANDLE;
    }
    HVswapchain& operator=(HVswapchain&& other) {
        if (this == &other) return *this;

        this->_swapchain = other._swapchain;
        this->_device = other._device;
        this->_surface = other._surface;
        this->_window = other._window;
        this->_strategy = other._strategy;

        other._swapchain = VK_NULL_HANDLE;
        other._device = VK_NULL_HANDLE;
        other._surface = VK_NULL_HANDLE;
        other._window = VK_NULL_HANDLE;
        other._strategy = VK_NULL_HANDLE;
        
        return *this;
    }
    HVswapchain(const HVswapchain& other) = delete;
    HVswapchain& operator=(const HVswapchain& other) = delete;

};

inline HVswapchain* HVswapchain::initialize(HVdevice* device, 
                                            HVsurface* surface,
                                            Hwindow* window,
                                            IHVswapchainConfigurationStrategy* strategy) {
    VkSwapchainKHR swapchain;

    /* SWAPCHAIN CREATION */ {
        VkResult result;

        uint32_t queue_family_indices[2];

        auto get_queue_family_indices = [&]() -> const uint32_t* {
            queue_family_indices[0] = device->get_queue_family_controller()->get_queue_family(0)->get_family_index();
            queue_family_indices[1] = device->get_queue_family_controller()->get_queue_family(1)->get_family_index();
            
            return queue_family_indices;
        };

        VkSwapchainCreateInfoKHR swapchain_create_info = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .surface = *surface,
            .minImageCount = strategy->get_min_image_count(), 
            .imageFormat = strategy->get_image_format(),
            .imageColorSpace = strategy->get_color_space(),
            .imageExtent = window->get_extent(),
            .imageArrayLayers = strategy->image_array_layers(),
            .imageSharingMode = device->get_queue_family_controller()->get_queue_family_count() <= 3 ? VkSharingMode::VK_SHARING_MODE_EXCLUSIVE : VkSharingMode::VK_SHARING_MODE_CONCURRENT, // other mod for another queues setup, etc one for render and one for presentation
            .queueFamilyIndexCount = device->get_queue_family_controller()->get_queue_family_count() <= 3 ? 0 : (uint32_t) 2, 
            .pQueueFamilyIndices = device->get_queue_family_controller()->get_queue_family_count() <= 3 ? nullptr : get_queue_family_indices(), 
            .preTransform = strategy->get_pre_transform(),
            .compositeAlpha = strategy->get_composite_alpha(),
            .presentMode = strategy->get_present_mode(),
            .clipped = VK_TRUE, // default
            .oldSwapchain = VK_NULL_HANDLE,
        };

        result = vkCreateSwapchainKHR(*device, &swapchain_create_info, nullptr, &swapchain);

        if (result != VK_SUCCESS) throw std::runtime_error("HVswapchain::initialize : failed to create VkSwapchainKHR.");
    } /* SWAPCHAIN CREATION */

    return new HVswapchain(swapchain, device, 
                           surface, window, strategy);
}

inline void HVswapchain::rebuild_swapchain() {
    VkSwapchainKHR new_swapchain;

    /* SWAPCHAIN CREATION */ {
        VkResult result;

        uint32_t queue_family_indices[2];

        auto get_queue_family_indices = [&]() -> const uint32_t* {
            queue_family_indices[0] = _device->get_queue_family_controller()->get_queue_family(0)->get_family_index();
            queue_family_indices[1] = _device->get_queue_family_controller()->get_queue_family(1)->get_family_index();
            
            return queue_family_indices;
        };

        VkSwapchainCreateInfoKHR swapchain_create_info = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .surface = *_surface,
            .minImageCount = _strategy->get_min_image_count(), 
            .imageFormat = _strategy->get_image_format(),
            .imageColorSpace = _strategy->get_color_space(),
            .imageExtent = _window->get_extent(),
            .imageArrayLayers = _strategy->image_array_layers(),
            .imageSharingMode = _device->get_queue_family_controller()->get_queue_family_count() <= 3 ? VkSharingMode::VK_SHARING_MODE_EXCLUSIVE : VkSharingMode::VK_SHARING_MODE_CONCURRENT, // other mod for another queues setup, etc one for render and one for presentation
            .queueFamilyIndexCount = _device->get_queue_family_controller()->get_queue_family_count() <= 3 ? 0 : (uint32_t) 2, 
            .pQueueFamilyIndices = _device->get_queue_family_controller()->get_queue_family_count() <= 3 ? nullptr : get_queue_family_indices(), 
            .preTransform = _strategy->get_pre_transform(),
            .compositeAlpha = _strategy->get_composite_alpha(),
            .presentMode = _strategy->get_present_mode(),
            .clipped = VK_TRUE, // default
            .oldSwapchain = _swapchain,
        };

        result = vkCreateSwapchainKHR(*_device, &swapchain_create_info, nullptr, &new_swapchain);

        if (result != VK_SUCCESS) throw std::runtime_error("HVswapchain::initialize : failed to create VkSwapchainKHR.");
    } /* SWAPCHAIN CREATION */

    if (_swapchain != VK_NULL_HANDLE) vkDestroySwapchainKHR(*_device, _swapchain, nullptr);

    _swapchain = new_swapchain;
}

#endif // HVSWAPCHAINHPP