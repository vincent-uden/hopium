#version 330 core

in vec2 vertexTexCoord;
in vec3 vertexPosition;
in vec3 vertexNormal;
in vec4 vertexColor;

out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragPosition;
out vec3 fragNormal;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

void main()
{
    fragPosition = vec3(matModel * vec4(vertexPosition, 1.0));
    fragNormal = vertexNormal;
    fragColor = vertexColor;
    fragTexCoord = vertexTexCoord;

    gl_Position = matProjection * matView * vec4(fragPosition, 1.0);
}
