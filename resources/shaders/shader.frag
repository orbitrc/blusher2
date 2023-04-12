#version 330 core

precision mediump float;

out vec4 fragColor;

in vec2 texCoord;

uniform int fillType;
uniform sampler2D textureIn;
uniform vec4 fillColor;
uniform vec2 parentPosition;    // Deprecated
uniform vec2 parentSize;        // Deprecated
uniform vec4 validGeometry;
uniform vec4 validViewport;
uniform vec4 viewport;
uniform vec2 resolution;

#define BL_FILL_TYPE_COLOR      0
#define BL_FILL_TYPE_TEXTURE    1

void discardClip()
{
    if (validViewport.x == 0.0 && validViewport.y == 0.0 &&
            validViewport.z == 0.0 && validViewport.w == 0.0) {
        return;
    }

    vec2 limitMin = vec2(
        max(validViewport.x, viewport.x),
        max(validViewport.y, viewport.y)
    ) / resolution.xy;
    vec2 limitMax = vec2(
        validViewport.x + validViewport.z,
        validViewport.y + validViewport.w
    ) / resolution.xy;

    vec2 coord = gl_FragCoord.xy / resolution.xy;

    if (coord.x < limitMin.x || coord.y < limitMin.y) {
        discard;
    }

    if (coord.x > limitMax.x || coord.y > limitMax.y) {
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
