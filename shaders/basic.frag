
#version 330 core

in vec3 WorldPosition;

out vec4 FragColor;

uniform vec3 objectColor;
uniform float time;
uniform int waterEffect;

void main()
{
    vec3 color = objectColor;

    // Estimate the surface normal
    vec3 normal = normalize(
        cross(dFdx(WorldPosition), dFdy(WorldPosition))
    );

    // Sunlight direction
    vec3 lightDirection = normalize(vec3(-0.5, 1.0, 0.6));

    float diffuse = abs(dot(normal, lightDirection));

    // Ambient + sunlight
    float lighting = 0.48 + 0.52 * diffuse;

    color *= lighting;

    // Animated river
    if (waterEffect == 1)
    {
        float wave1 = sin(
            WorldPosition.x * 4.0 + time * 2.0
        );

        float wave2 = sin(
            WorldPosition.z * 3.0 - time * 1.5
        );

        float wave3 = sin(
            (WorldPosition.x + WorldPosition.z) * 5.0
            + time * 1.2
        );

        float waves = (wave1 + wave2 + wave3) / 3.0;

        color += vec3(0.015, 0.045, 0.075) * waves;

        float highlights = smoothstep(
            0.72, 0.95,
            sin(WorldPosition.x * 8.0
                + WorldPosition.z * 5.0
                + time * 2.5)
        );

        color += vec3(0.10, 0.16, 0.20) * highlights;
    }

    FragColor = vec4(color, 1.0);
}
