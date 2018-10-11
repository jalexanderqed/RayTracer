#version 330 core
out vec4 color;

struct PointLight{
	vec3 position;
	vec3 color;
};

uniform samplerCube shadowMap0;
uniform samplerCube shadowMap1;
uniform samplerCube shadowMap2;

uniform sampler2D positionTex;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_specular1;
uniform float farClip;

uniform PointLight pointLights[3];
uniform vec3 pPos;

in vec2 TexCoords;
in vec3 Normal;
in vec4 worldFragPos;
in vec4 viewFragPos;

uniform vec3 probes[15];
uniform mat4 projection;

int numProbes = 15;
float radius = 2.0f;

float ambientStrength = 0.3f;

vec3 pointLightContrib(PointLight light, vec3 normal, vec3 eyeDir, vec3 wFragPos){
	vec3 outDir = normalize(light.position - wFragPos);
	vec3 inDir = -outDir;
	float diffuse = max(dot(normal, outDir), 0);

	vec3 reflected = reflect(inDir, normal);
	float specular = pow(max(dot(eyeDir, reflected), 0), 1.5);

	float lightDist = length(light.position - wFragPos) * .05;
	float atten = 1.0f / (0.5f * lightDist + 0.5f * (lightDist * lightDist));

	vec3 diffColor = light.color * diffuse * vec3(texture(texture_diffuse1, TexCoords));
	vec3 specColor = light.color * specular * vec3(texture(texture_specular1, TexCoords));

	return (diffColor + specColor) * atten;
}

void main()
{
	vec3 wFragPos = vec3(worldFragPos);
	vec3 norm = normalize(Normal);
	vec3 eyeDir = normalize(pPos - wFragPos);
	color = vec4(0, 0, 0, 0);

    color += vec4(pointLightContrib(pointLights[0], norm, eyeDir, wFragPos), 1);
	color += vec4(pointLightContrib(pointLights[1], norm, eyeDir, wFragPos), 1);
	color += vec4(pointLightContrib(pointLights[2], norm, eyeDir, wFragPos), 1);
	color += vec4(ambientStrength * vec3(texture(texture_diffuse1, TexCoords)), 1);
}