#version 450

// Входные атрибуты 2D вершины
layout(location = 0) in vec2 inPos;   // Экранные координаты (x, y) в пикселях
layout(location = 1) in vec2 inUV;    // Текстурные координаты (u, v)
layout(location = 2) in vec4 inColor; // Цвет RGBA (для подсветки текста или оттенка)

// Константы быстрой передачи (Push Constants)
// Преобразуют экранное пространство пикселей в координаты отсечения Vulkan (NDC)
layout(push_constant) uniform PushConstants {
    vec2 uScale;      // (2.0 / viewport_width, 2.0 / viewport_height)
    vec2 uTranslate;  // (-1.0, -1.0)
} pc;

layout(location = 0) out vec2 fragUV;
layout(location = 1) out vec4 fragColor;

void main() {
    fragUV = inUV;
    fragColor = inColor;
    
    // Преобразуем экранные пиксели (0..W, 0..H) в диапазон Vulkan NDC [-1.0 .. 1.0]
    gl_Position = vec4(inPos * pc.uScale + pc.uTranslate, 0.0, 1.0);
}

