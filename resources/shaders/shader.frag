#version 330 core

precision mediump float;

out vec4 fragColor;

in vec2 texCoord;

uniform int fillType;
uniform sampler2D textureIn;
uniform vec4 fillColor;
uniform vec2 parentPosition;
uniform vec2 parentSize;
uniform vec4 validGeometry;
uniform vec2 resolution;

#define BL_FILL_TYPE_COLOR      0
#define BL_FILL_TYPE_TEXTURE    1

void discardClip()
{
    if (validGeometry.x == 0.0 && validGeometry.y == 0.0 &&
            validGeometry.z == 0.0 && validGeometry.w == 0.0) {
        return;
    }

    vec2 limit = vec2(
        (validGeometry.x + validGeometry.z) / resolution.x,
        (validGeometry.y + validGeometry.w) / resolution.y
    );

    vec2 coord = vec2(gl_FragCoord.x, resolution.y - gl_FragCoord.y) / resolution.xy;

    if (coord.x > limit.x || coord.y > limit.y) {
        discard;
    }
}

void main()
{
    if (fillType == BL_FILL_TYPE_COLOR) {
        discardClip();
        fragColor = fillColor;
    } else if (fillType == BL_FILL_TYPE_TEXTURE) {
        discardClip();
        fragColor = texture(textureIn, texCoord);
    } else {
        fragColor = vec4(0.0, 1.0, 0.0, 1.0);
    }
}
