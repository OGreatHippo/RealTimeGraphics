#version 330

struct PointLight 
{  
    vec3 position;
	vec3 colour;
    float intensity;

    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 1
uniform PointLight pointLights[NR_POINT_LIGHTS];

struct Material 
{
    sampler2D diffuse;
    sampler2D specular;
}; 
  
uniform Material material;

uniform sampler2D sampler_tex;
uniform vec3 viewPos;

in vec3 varying_position;
in vec2 varying_coord;
in vec3 varying_normal;

out vec4 fragment_colour;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

	// diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); 
    
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, varying_coord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, varying_coord));
    ambient *= attenuation;
    diffuse *= attenuation;
    return (light.colour + ambient + diffuse);
} 

void main(void)
{
	vec3 tex_colour = texture(sampler_tex, varying_coord).rgb;

    vec3 varying_normal = normalize(varying_normal);

    vec3 viewDir = normalize(viewPos - varying_position);

	for(int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		tex_colour *= CalcPointLight(pointLights[i], varying_normal, varying_position, viewDir);
	}

	fragment_colour = vec4(tex_colour, 1);
}