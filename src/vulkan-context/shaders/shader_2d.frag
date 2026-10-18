#version 450

layout(location = 0) in vec2 fragUV;
layout(location = 1) in vec4 fragColor;

// Текстурный семплер:
// При отрисовке страниц PDF сюда передаётся текстура страницы.
// При отрисовке подсветки текста и элементов UI сюда передаётся белая 1x1 текстура.
layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 texColor = texture(texSampler, fragUV);
    
    // Модуляция цвета текстуры цветом вершины (поддерживает альфа-блендинг для полупрозрачной подсветки)
    outColor = texColor * fragColor;
}

