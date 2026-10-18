#include "windows.hpp"
#include "vulkan-context.hpp"
#include <iostream>
#include <vector>
#include <cmath>

// Вспомогательная функция: генерация тестового растрового изображения страницы PDF
static std::vector<uint32_t> create_dummy_pdf_page(uint32_t width, uint32_t height) {
    std::vector<uint32_t> pixels(width * height, 0xFFFFFFFF); // Белый лист бумаги

    // Рисуем имитацию строк текста (серые полосы)
    for (uint32_t y = 60; y < height - 60; y += 32) {
        for (uint32_t x = 50; x < width - 50; x++) {
            // Имитация пробелов между словами
            if ((x % 90) > 15) {
                pixels[y * width + x]       = 0xFF333333; // Тёмно-серый текст
                pixels[(y + 1) * width + x] = 0xFF333333;
                pixels[(y + 2) * width + x] = 0xFF555555;
            }
        }
    }
    return pixels;
}

int main() {
    WindowConfig win_config{
        .title     = "New Horizons - PDF Reader & Voice Narration Viewer",
        .width     = 1280,
        .height    = 720,
        .resizable = true
    };

    Window window{};
    if (!window_init(&window, &win_config)) {
        std::cerr << "Failed to initialize Win32 window!\n";
        return -1;
    }

    VulkanConfig vk_config{
        .enable_validation = false, // Можно включить true при отладке
        .vsync             = true
    };

    VulkanContext vk{};
    if (!vulkan_context_init(&vk, &window, &vk_config)) {
        std::cerr << "Failed to initialize Vulkan Context!\n";
        window_destroy(&window);
        return -1;
    }

    std::cout << "[VulkanContext] Successfully initialized.\n";

    // Создаем текстуру страницы PDF (размер формата A4 в пикселях: 600x800)
    const uint32_t page_w = 600;
    const uint32_t page_h = 800;
    auto page_pixels = create_dummy_pdf_page(page_w, page_h);

    VulkanTexture pdf_page_texture{};
    if (!vulkan_create_texture(&vk, page_w, page_h, page_pixels.data(), &pdf_page_texture)) {
        std::cerr << "Failed to create PDF page texture!\n";
    }

    float t = 0.0f;

    // Основной цикл приложения
    while (!window_should_close(&window)) {
        window_poll_events(&window);

        // Пропуск рендеринга при сворачивании окна
        if (window_is_minimized(&window)) {
            Sleep(10);
            continue;
        }

        // 1. Начало кадра (темно-серый цвет рабочей области просмотрщика)
        if (vulkan_begin_frame(&vk, 0.18f, 0.19f, 0.21f, 1.0f)) {
            uint32_t fb_w = 0, fb_h = 0;
            window_get_framebuffer_size(&window, &fb_w, &fb_h);

            // Центрируем страницу PDF по горизонтали
            float page_x = (static_cast<float>(fb_w) - page_w) * 0.5f;
            float page_y = 40.0f;

            // Тень под страницей PDF
            vulkan_draw_rect(&vk, page_x + 6.0f, page_y + 6.0f, page_w, page_h, {0.05f, 0.05f, 0.06f, 0.5f});

            // 2. Отрисовка страницы PDF
            vulkan_draw_textured_rect(&vk, &pdf_page_texture, page_x, page_y, page_w, page_h);

            // 3. Имитация подсветки текста, читаемого голосовой моделью
            // Вычисляем текущее читаемое слово по синусоиде/времени
            int line_index = static_cast<int>(t * 0.8f) % 20;
            float word_rel_x = std::fmod(t * 120.0f, static_cast<float>(page_w - 120));

            float highlight_x = page_x + 50.0f + word_rel_x;
            float highlight_y = page_y + 58.0f + static_cast<float>(line_index * 32);
            float highlight_w = 70.0f;
            float highlight_h = 18.0f;

            // Мягкая жёлтая подсветка текущего слова (TTS Text Highlight)
            vulkan_draw_highlight(&vk, highlight_x, highlight_y, highlight_w, highlight_h,
                                  {1.0f, 0.92f, 0.23f, 0.45f});

            // Акцентная рамка фокуса вокруг текущего читаемого слова
            vulkan_draw_rect_outline(&vk, highlight_x, highlight_y, highlight_w, highlight_h, 1.5f,
                                     {0.95f, 0.70f, 0.10f, 0.85f});

            // 4. Отрисовка верхней панели управления ридера
            vulkan_draw_rect(&vk, 0.0f, 0.0f, static_cast<float>(fb_w), 32.0f, {0.12f, 0.13f, 0.15f, 0.95f});
            // Статусный индикатор синтеза речи (зеленый кружок/квадрат)
            vulkan_draw_rect(&vk, 12.0f, 8.0f, 16.0f, 16.0f, {0.2f, 0.85f, 0.4f, 1.0f});

            // 5. Завершение кадра и вывод на экран
            vulkan_end_frame(&vk);
        }

        t += 0.016f;
    }

    // Освобождение всех ресурсов
    vulkan_destroy_texture(&vk, &pdf_page_texture);
    vulkan_context_cleanup(&vk);
    window_destroy(&window);

    std::cout << "[NewHorizons] Application exited cleanly.\n";
    return 0;
}