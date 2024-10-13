#version 330
precision mediump float;

uniform sampler2D texture0;
uniform vec2 texOffset; // Texture offset for sampling
varying vec2 fragTexCoord;

void main() {
    vec4 color = vec4(0.0);

    // Sample surrounding pixels
    color += texture2D(texture0, fragTexCoord + texOffset * vec2(-1.0, -1.0));
    color += texture2D(texture0, fragTexCoord + texOffset * vec2(1.0, -1.0));
    color += texture2D(texture0, fragTexCoord + texOffset * vec2(-1.0, 1.0));
    color += texture2D(texture0, fragTexCoord + texOffset * vec2(1.0, 1.0));
    color += texture2D(texture0, fragTexCoord + texOffset * vec2(-1.0, 0.0));
    color += texture2D(texture0, fragTexCoord + texOffset * vec2(1.0, 0.0));
    color += texture2D(texture0, fragTexCoord + texOffset * vec2(0.0, -1.0));
    color += texture2D(texture0, fragTexCoord + texOffset * vec2(0.0, 1.0));

    gl_FragColor = color / 9.0; // Average the samples
}
