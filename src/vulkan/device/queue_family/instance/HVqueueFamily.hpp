#ifndef HVQUEUEFAMILYHPP
#define HVQUEUEFAMILYHPP

class HVqueueFamily {
    Hstack<uint8_t, VkQueue> _queues;
    uint32_t                 _queue_family_index;
    VkQueueFlags             _queue_flags;
    uint32_t                 _max_queue_count;
    VkExtent3D               _min_image_transfer_granularity;

public:
    HVqueueFamily() : _queues(),
                      _queue_family_index(0),
                      _queue_flags(),
                      _max_queue_count(0),
                      _min_image_transfer_granularity() {}
    HVqueueFamily(Hstack<uint8_t, VkQueue>&& queues,
                  uint32_t queue_family_index, 
                  VkQueueFlags queueFlags,
                  uint32_t maxQueueCount,
                  VkExtent3D minImageTransferGranularity) : _queues(std::move(queues)),
                                                            _queue_family_index(queue_family_index),
                                                            _queue_flags(queueFlags),
                                                            _max_queue_count(maxQueueCount),
                                                            _min_image_transfer_granularity(minImageTransferGranularity) {}
                                                        
    [[nodiscard]] constexpr VkQueueFlags get_type() const noexcept { return this->_queue_flags; }
    [[nodiscard]] constexpr uint32_t get_count() const noexcept { return this->_queues.size(); }

    [[nodiscard]] constexpr uint32_t get_family_index() const noexcept { return this->_queue_family_index; }
    [[nodiscard]] VkQueue& get_queue(uint8_t index) noexcept { return this->_queues[index]; }
    [[nodiscard]] constexpr VkQueueFlags get_queue_type() const noexcept { return this->_queue_flags; }
    [[nodiscard]] constexpr uint32_t get_max_queue_count() const noexcept { return this->_max_queue_count; }
    [[nodiscard]] constexpr VkExtent3D get_min_image_transfer_granularity() const noexcept { return this->_min_image_transfer_granularity; }

    [[nodiscard]] constexpr operator uint32_t() const noexcept { return this->_queue_family_index; }

    HVqueueFamily(HVqueueFamily&& other) noexcept : _queues(std::move(other._queues)), 
                                                    _queue_flags(other._queue_flags),
                                                    _max_queue_count(other._max_queue_count),
                                                    _min_image_transfer_granularity(other._min_image_transfer_granularity) {
        other._queue_flags = 0;
        other._queues = 0;
        other._min_image_transfer_granularity = VkExtent3D { 
            .width = 0,
            .height = 0,
            .depth = 0 
        };
    }
    HVqueueFamily& operator=(HVqueueFamily&& other) noexcept {
        if (this == &other) return *this;

        this->_queues = std::move(other._queues);
        this->_queue_flags = other._queue_flags;
        this->_max_queue_count = other._max_queue_count;
        this->_min_image_transfer_granularity = other._min_image_transfer_granularity;

        other._queue_flags = 0;
        other._max_queue_count = 0;
        other._min_image_transfer_granularity = VkExtent3D { 
            .width = 0,
            .height = 0,
            .depth = 0 
        };

        return *this;
    }
    HVqueueFamily(const HVqueueFamily& other) = delete;
    HVqueueFamily& operator=(const HVqueueFamily& other) = delete;
};

#endif // HVQUEUEFAMILYHPP