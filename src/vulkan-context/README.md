# Библиотека `vulkan-context`

Процедурная статическая библиотека на C++ (стандарт C++20/C++26) для Vulkan 1.2, спроектированная специально для приложений **просмотра документов (PDF, изображений)** и **синхронной подсветки текста для моделей голосовой озвучки (TTS)**.

---

## Содержание
- [Библиотека `vulkan-context`](#библиотека-vulkan-context)
  - [Содержание](#содержание)
  - [1. Архитектурный дизайн](#1-архитектурный-дизайн)
  - [2. Подробное обоснование параметров объектов Vulkan](#2-подробное-обоснование-параметров-объектов-vulkan)
    - [2.1. Экземпляр и логическое устройство (`VkInstance`, `VkDevice`)](#21-экземпляр-и-логическое-устройство-vkinstance-vkdevice)
    - [2.2. Цепочка показа (`VkSwapchainKHR`)](#22-цепочка-показа-vkswapchainkhr)
    - [2.3. Проход рендеринга (`VkRenderPass`)](#23-проход-рендеринга-vkrenderpass)
    - [2.4. Конвейер 2D и альфа-блендинг для подсветки текста](#24-конвейер-2d-и-альфа-блендинг-для-подсветки-текста)
    - [2.5. Сэмплер текстур (`VkSampler`)](#25-сэмплер-текстур-vksampler)
    - [2.6. Динамические буферы вершин и кадры в полёте](#26-динамические-буферы-вершин-и-кадры-в-полёте)
    - [2.7. Единый конвейер и белая текстура 1x1](#27-единый-конвейер-и-белая-текстура-1x1)
  - [3. Сборка GLSL-шейдеров через CMake](#3-сборка-glsl-шейдеров-через-cmake)
  - [4. Руководство по интеграции](#4-руководство-по-интеграции)
    - [4.1. Отображение страницы PDF](#41-отображение-страницы-pdf)
    - [4.2. Синхронизация подсветки с голосовой моделью (TTS)](#42-синхронизация-подсветки-с-голосовой-моделью-tts)
    - [4.3. Очистка ресурсов](#43-очистка-ресурсов)

---

## 1. Архитектурный дизайн

Библиотека построена по принципам **процедурного программирования**:
* Отсутствуют классы, виртуальные методы и скрытое ООП-состояние.
* Все данные инкапсулированы в прозрачные структуры: `VulkanContext`, `VulkanTexture`, `Vertex2D`, `FrameData`.
* Все операции реализованы в виде чистых процедур, принимающих указатель на контекст: `vulkan_context_init`, `vulkan_begin_frame`, `vulkan_draw_textured_rect`, `vulkan_draw_highlight`, `vulkan_end_frame`.
* Для инициализации структур используется синтаксис **Designated Initializers** (`.param = value`), строго соблюдающий порядок объявления полей в спецификации Vulkan API.

---

## 2. Подробное обоснование параметров объектов Vulkan

### 2.1. Экземпляр и логическое устройство (`VkInstance`, `VkDevice`)

```cpp
VkApplicationInfo app_info = {
    .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .apiVersion         = VK_API_VERSION_1_2
};
```
* **`apiVersion = VK_API_VERSION_1_2`**: Выбрана версия 1.2, так как она включает в ядро поддержку отрицательной высоты вьюпорта (`VK_KHR_maintenance1`) и расширенные Push Constants, избавляя от необходимости подключать устаревшие расширения.
* **`samplerAnisotropy = VK_FALSE`**: Для плоских документов (страниц PDF, плашек и шрифтов) анизотропная фильтрация не нужна, так как геометрия не имеет наклонов в пространстве 3D. Отключение анизотропии снижает энергопотребление и ускоряет работу с текстурами.
* **`VK_SHARING_MODE_EXCLUSIVE` / `CONCURRENT`**: Если видеокарта использует одно и то же семейство очередей для отрисовки и показа (самый частый случай на дискретных и встроенных GPU), включается режим `EXCLUSIVE`, дающий максимальную пропускную способность шины памяти.

---

### 2.2. Цепочка показа (`VkSwapchainKHR`)

```cpp
VkSwapchainCreateInfoKHR create_info = {
    .minImageCount   = capabilities.minImageCount + 1,
    .imageFormat     = surface_format.format, // VK_FORMAT_B8G8R8A8_UNORM / R8G8B8A8_UNORM
    .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
    .imageUsage      = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    .compositeAlpha  = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    .presentMode     = VK_PRESENT_MODE_FIFO_KHR,
    .clipped         = VK_TRUE
};
```
* **`minImageCount = minImageCount + 1`**: Тройная буферизация. Запрос на одно дополнительное изображение сверх аппаратного минимума гарантирует, что ЦП не будет простаивать в ожидании освобождения кадра драйвером.
* **`VK_FORMAT_B8G8R8A8_UNORM`**: Стандартный 32-битный формат буфера кадров, поддерживаемый 100% графических адаптеров Windows.
* **`presentMode = VK_PRESENT_MODE_FIFO_KHR`**: Обязательный режим V-Sync (вертикальной синхронизации). Для чтения книг и документов он критически важен:
  1. Полностью устраняет «разрывы» изображения (tearing) при плавном скролле страниц.
  2. Ограничивает частоту кадров частотой обновления монитора (60/120/144 Гц), не перегревая GPU при отображении статичного текста.
* **`compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR`**: Окно ридера непрозрачно по отношению к рабочему столу Windows.
* **`clipped = VK_TRUE`**: Драйвер Vulkan отсекает отрисовку невидимых пикселей, если окно частично перекрыто другими окнами Windows.

---

### 2.3. Проход рендеринга (`VkRenderPass`)

```cpp
VkAttachmentDescription color_attachment = {
    .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
    .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
    .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
    .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
};
```
* **`loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR`**: В начале кадра экран заполняется цветом фона рабочего пространства ридера (например, тёмно-серым `rgba(0.18, 0.19, 0.21, 1.0)`).
* **`initialLayout = UNDEFINED`**: Указывает видеокарте, что предыдущее содержимое кадра сохранять не требуется, что экономит полосу пропускания видеопамяти.
* **`finalLayout = PRESENT_SRC_KHR`**: По завершении прохода изображение автоматически переводится в состояние, готовое к выводу на экран через Swapchain, без дополнительных ручных барьеров памяти.

---

### 2.4. Конвейер 2D и альфа-блендинг для подсветки текста

Подсветка слов, читаемых голосовой моделью, должна мягко накладываться поверх текста страницы PDF, не затирая буквы:

```cpp
VkPipelineColorBlendAttachmentState color_blend = {
    .blendEnable         = VK_TRUE,
    .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
    .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .colorBlendOp        = VK_BLEND_OP_ADD,
    .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .alphaBlendOp        = VK_BLEND_OP_ADD,
    .colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                           VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
};
```

Математика смешивания:

$$
\text{Color}_{\text{result}} = \text{Color}_{\text{src}} \cdot \alpha_{\text{src}} + \text{Color}_{\text{dst}} \cdot (1 - \alpha_{\text{src}})
$$

> **В виде программного выражения:**  
> `Color_result = (Color_src * Alpha_src) + (Color_dst * (1.0 - Alpha_src))`

Пример: на чёрный текст страницы PDF `rgb(0, 0, 0)` накладывается жёлтый выделитель `rgba(1.0, 0.92, 0.23, 0.40)`:
* Цвет пикселя становится мягким жёлто-серым оттенком, а текст остаётся полностью разборчивым.

```cpp
VkPipelineRasterizationStateCreateInfo rasterizer = {
    .polygonMode = VK_POLYGON_MODE_FILL,
    .cullMode    = VK_CULL_MODE_NONE
};
```
* **`cullMode = VK_CULL_MODE_NONE`**: Для плоских прямоугольников интерфейса отсечение обратных граней отключено, что исключает ошибки исчезновения полигонов при любом порядке обхода вершин.

---

### 2.5. Сэмплер текстур (`VkSampler`)

```cpp
VkSamplerCreateInfo sampler_info = {
    .magFilter    = VK_FILTER_LINEAR,
    .minFilter    = VK_FILTER_LINEAR,
    .mipmapMode   = VK_SAMPLER_MIPMAP_MODE_LINEAR,
    .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
};
```
* **`VK_FILTER_LINEAR`**: Билинейная интерполяция обеспечивает гладкое сглаживание глифов текста на страницах PDF при масштабировании (зуме).
* **`CLAMP_TO_EDGE`**: Координаты за пределами `[0.0, 1.0]` прижимаются к крайним пикселям. Это предотвращает артефакты в виде тонких паразитных полос по периметру страницы документа.

---

### 2.6. Динамические буферы вершин и кадры в полёте

* **`VULKAN_MAX_FRAMES_IN_FLIGHT = 2` (Double Buffering)**: Пока GPU отрисовывает кадр $N$, ЦП формирует геометрию для кадра $N+1$.
* **`HOST_VISIBLE | HOST_COHERENT`**: Буферы вершин и индексов постоянно отображены в адресное пространство процесса (`mapped_vertices`, `mapped_indices`). ЦП записывает координаты прямоугольников напрямую через указатель без вызовов `vkMapMemory` / `vkUnmapMemory` на каждый кадр.
* **`VULKAN_MAX_2D_QUADS = 16384`**: Ёмкость одного кадра — до 16 384 прямоугольников (65 536 вершин), чего с избытком хватает для отрисовки страницы PDF, нескольких сотен подсвеченных слов, рамок и элементов UI.

---

### 2.7. Единый конвейер и белая текстура 1x1

Для оптимизации в библиотеке применён паттерн **1x1 White Texture**:
* При инициализации создаётся текстура размером 1x1 пиксель с цветом `0xFFFFFFFF`.
* **Страница PDF**: передаётся текстура страницы, цвет вершин `tint = (1, 1, 1, 1)`.
* **Подсветка текста / Фон**: передаётся `white_texture`, цвет вершин задаёт оттенок маркера `rgba(...)`.

**Преимущество:** Отсутствует переключение графических конвейеров (`vkCmdBindPipeline`) между отрисовкой текста, страниц и интерфейса. Всё рендерится в один универсальный проход.

---

## 3. Сборка GLSL-шейдеров через CMake

Шейдеры хранятся в исходном виде GLSL:
* `src/vulkan-context/shaders/shader_2d.vert`
* `src/vulkan-context/shaders/shader_2d.frag`

В `src/vulkan-context/CMakeLists.txt` настроена цель компиляции через компилятор `glslc`:

```cmake
find_package(Vulkan REQUIRED COMPONENTS glslc)

add_custom_command(
    OUTPUT ${SPV_OUT}
    COMMAND ${GLSL_COMPILER} ${SHADER} -o ${SPV_OUT}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SPV_OUT} "${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME}.spv"
    DEPENDS ${SHADER}
    COMMENT "Compiling Vulkan GLSL shader: ${SHADER_NAME} -> ${SHADER_NAME}.spv"
    VERBATIM
)
```

1. Компиляция запускается автоматически только при изменении файлов `.vert` или `.frag`.
2. Бинарные файлы `.spv` копируются в `build/shaders/`.
3. В C++ передаётся макрос `SHADERS_DIR`, благодаря чему функция `create_shader_module_from_spv` гарантированно находит бинарный шейдер при любом способе запуска (из терминала, из Visual Studio / VS Code или напрямую кликом по `.exe`).

---

## 4. Руководство по интеграции

### 4.1. Отображение страницы PDF

```cpp
// 1. Создание текстуры из буфера пикселей RGBA32 (полученного из PDF-движка)
VulkanTexture page_texture{};
vulkan_create_texture(&vk, page_width, page_height, rgba_pixel_buffer, &page_texture);

// 2. В цикле отрисовки:
vulkan_begin_frame(&vk, 0.2f, 0.2f, 0.22f, 1.0f);

// Отрисовка страницы документа
vulkan_draw_textured_rect(&vk, &page_texture, page_x, page_y, page_w, page_h);
```

### 4.2. Синхронизация подсветки с голосовой моделью (TTS)

Модель озвучки обычно возвращает координаты текущего слова на странице (bounding box):

```cpp
// Координаты слова, озвучиваемого моделью TTS в данный момент
float word_x = page_x + word_box.left;
float word_y = page_y + word_box.top;
float word_w = word_box.width;
float word_h = word_box.height;

// 1. Мягкая жёлтая подсветка-маркер поверх текста
vulkan_draw_highlight(&vk, word_x, word_y, word_w, word_h, 
                      ColorRGBA{ 1.0f, 0.92f, 0.23f, 0.40f });

// 2. Акцентная рамка курсора чтения вокруг текущего слова
vulkan_draw_rect_outline(&vk, word_x, word_y, word_w, word_h, 1.5f,
                         ColorRGBA{ 0.95f, 0.70f, 0.10f, 0.90f });

vulkan_end_frame(&vk);
```

### 4.3. Очистка ресурсов

```cpp
vulkan_destroy_texture(&vk, &page_texture);
vulkan_context_cleanup(&vk);
window_destroy(&window);
```

