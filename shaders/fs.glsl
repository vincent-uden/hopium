#version 330

out vec4 finalColor;

in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragPosition;
in vec3 fragNormal;

uniform vec3 ambientColor;
uniform sampler2D texture0;

vec3 lightPosition = vec3(0.0, 10.0, -10.0);
float ambientStrength = 0.51;

void main()
{
    // texture
    vec4 texelColor = texture(texture0, fragTexCoord);
    // ambient
    vec3 ambient = ambientStrength * ambientColor;

    // diffuse
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPosition - fragPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * ambientColor;

    vec3 result = (ambient + diffuse) * texelColor.rgb;
    finalColor = vec4(result, fragColor.a);
}
