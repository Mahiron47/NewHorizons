#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

namespace faiss {

using idx_t = int64_t;

// Базовый класс индекса FAISS для векторного поиска тембра RVC v2
class Index {
public:
    int d = 768;              // Размерность векторов признаков (для RVC v2: 768)
    idx_t ntotal = 0;         // Общее число векторов в индексе
    bool is_trained = true;   // Флаг обученности индекса

    virtual ~Index() = default;

    // Поиск k ближайших векторов тембра
    // n: число фреймов
    // x: массив запросных векторов [n * d]
    // k: число соседей (для RVC WebUI: k = 8)
    // distances: выходной массив квадратов расстояний [n * k]
    // labels: выходной массив индексов найденных векторов [n * k]
    virtual void search(
        idx_t n, 
        const float* x, 
        idx_t k, 
        float* distances, 
        idx_t* labels
    ) = 0;

    // Реконструкция (извлечение) вектора по его индексу
    virtual void reconstruct(
        idx_t key, 
        float* recons
    ) = 0;
};

} // namespace faiss

