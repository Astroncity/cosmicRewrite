#version 100
precision mediump float;

uniform sampler2D texture0; // Input texture (the scene)
uniform vec2 resolution; // Resolution of the screen
uniform float time; // Optional: for animation effects

varying vec2 fragTexCoord;

void main() {
    vec2 uv = fragTexCoord;

    // 1. Apply curvature effect
    vec2 centeredUV = (uv - 0.5) * 1.5; // Scale UV for curvature
    float r = length(centeredUV);
    centeredUV *= 1.0 + 0.5 * (1.0 - r * r); // Adjust for curvature
    uv = centeredUV + 0.5; // Shift back to normal

    // 2. Add scanlines
    float scanline = 0.5 + 0.5 * sin(uv.y * resolution.y * 0.1 + time * 5.0);

    // 3. Apply chromatic aberration
    vec3 color;
    color.r = texture2D(texture0, uv + vec2(0.001, 0)).r; // Red shift
    color.g = texture2D(texture0, uv).g; // Green stays
    color.b = texture2D(texture0, uv - vec2(0.001, 0)).b; // Blue shift

    // 4. Apply vignetting
    float vignette = smoothstep(0.7, 1.0, length(uv - 0.5) * 2.0); // Smoothstep for vignetting
    color *= vignette; // Darken corners

    // 5. Combine scanlines with color
    gl_FragColor = vec4(color * scanline, 1.0); // Final color output
}
