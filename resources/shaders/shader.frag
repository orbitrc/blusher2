#version 330 core

precision mediump float;

out vec4 fragColor;

in vec2 texCoord;

uniform int fillType;
uniform sampler2D textureIn;
uniform vec4 fillColor;

#define BL_FILL_TYPE_COLOR      0
#define BL_FILL_TYPE_TEXTURE    1

void main()
{
    if (fillType == BL_FILL_TYPE_COLOR) {
        fragColor = fillColor;
    } else if (fillType == BL_FILL_TYPE_TEXTURE) {
        fragColor = texture(textureIn, texCoord);
    } else {
        fragColor = vec4(0.0, 1.0, 0.0, 1.0);
    }
}
