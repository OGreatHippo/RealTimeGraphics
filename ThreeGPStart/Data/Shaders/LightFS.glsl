#version 330

    uniform vec3 lightposition;
	uniform vec3 lightcolour;

    uniform float lightconstant;
    uniform float lightlinear;
    uniform float lightquadratic;  

    uniform vec3 lightambient;
    uniform vec3 lightdiffuse;
    uniform vec3 lightspecular;

#define NR_POINT_LIGHTS 2

  
//uniform sampler2D specular;

vec3 light_intensity;

uniform sampler2D sampler_tex;
uniform vec3 viewPos;

in vec3 varying_position;
in vec2 varying_coord;
in vec3 varying_normal;

out vec4 fragment_colour;

vec3 CalcPointLight(vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(lightposition - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32f);

    // attenuation
    float distance = length(lightposition - fragPos);
    float attenuation = 1.0 / (lightconstant + lightlinear * distance + lightquadratic * (distance * distance));    
    // combine results
    vec3 ambient = lightambient * vec3(texture(sampler_tex, varying_coord));
    vec3 diffuse = lightdiffuse * diff * vec3(texture(sampler_tex, varying_coord));
    vec3 specular = lightspecular * spec * vec3(texture(sampler_tex, varying_coord));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

void main(void)
{
	vec3 tex_colour = texture(sampler_tex, varying_coord).rgb;

    vec3 varying_normal = normalize(varying_normal);

    vec3 viewDir = normalize(viewPos - varying_position);

    float light_intensity = max(0, dot(viewDir, varying_normal));

    tex_colour *= CalcPointLight(varying_normal, varying_position, viewDir) * light_intensity;

	fragment_colour = vec4(tex_colour, varying_position.z * 0.001);
}