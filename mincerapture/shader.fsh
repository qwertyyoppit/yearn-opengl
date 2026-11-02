#version 330 core

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;
	
	float constant;
	float linear;
	float quadratic;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	float cutOffStart;
	float cutOffEnd;
	
	float constant;
	float linear;
	float quadratic;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

#define NR_POINT_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result = CalcDirLight(dirLight, norm, viewDir);
	for (int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
	}
	result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

	FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);

	float diff = max(dot(normal, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	vec3 ambient = light.ambient * (texture(material.diffuse, TexCoords).rgb + texture(material.emission, TexCoords).rgb);
	vec3 diffuse = light.diffuse * diff * (texture(material.diffuse, TexCoords).rgb + texture(material.emission, TexCoords).rgb);
	vec3 specular = light.specular * (spec * texture(material.specular, TexCoords).rgb);
	
	return vec3(ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);

	float diff = max(dot(normal, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	
	float distance = length(light.position - fragPos);
	float attenuation = 1/(light.constant + light.linear * distance + light.quadratic * distance * distance);
	
	vec3 ambient = light.ambient * (texture(material.diffuse, TexCoords).rgb + texture(material.emission, TexCoords).rgb)* attenuation;
	vec3 diffuse = light.diffuse * diff * (texture(material.diffuse, TexCoords).rgb + texture(material.emission, TexCoords).rgb)* attenuation;
	vec3 specular = light.specular * (spec * texture(material.specular, TexCoords).rgb) * attenuation;

	return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);

	float diff = max(dot(normal, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	
	float distance = length(light.position - fragPos);
	float attenuation = 1/(light.constant + light.linear * distance + light.quadratic * distance * distance);
	
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOffStart - light.cutOffEnd;
	float intensity = clamp((theta-light.cutOffEnd) / epsilon, 0.0, 1.0);

	vec3 ambient = light.ambient * (texture(material.diffuse, TexCoords).rgb + texture(material.emission, TexCoords).rgb);
	vec3 diffuse = light.diffuse * (texture(material.diffuse, TexCoords).rgb + texture(material.emission, TexCoords).rgb) * (diff * intensity * attenuation);
	vec3 specular = light.specular * (texture(material.specular, TexCoords).rgb) * (spec * intensity * attenuation);

	return (ambient + diffuse + specular);
}