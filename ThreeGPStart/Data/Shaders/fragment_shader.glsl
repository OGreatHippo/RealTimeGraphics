#version 330 core

uniform sampler2D sampler_tex;

in vec3 varying_position;
in vec2 varying_coord;
in vec3 varying_normal;

out vec4 fragment_colour;

void main()
{
//    // ambient
//    float ambientStrength = 0.1;
//    vec3 ambient = ambientStrength * lightColour;
//  	
//    // diffuse 
//    vec3 norm = normalize(Normal);
//    vec3 lightDir = normalize(lightPos - FragPos);
//    float diff = max(dot(norm, lightDir), 0.0);
//    vec3 diffuse = diff * lightColour;
//    
//    // specular
//    float specularStrength = 0.5;
//    vec3 viewDir = normalize(viewPos - FragPos);
//    vec3 reflectDir = reflect(-lightDir, norm);  
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
//    vec3 specular = specularStrength * spec * lightColour;  
        
    vec3 tex_colour = texture(sampler_tex, varying_coord).rgb;
    fragment_colour = vec4(tex_colour * 0.1, 1.0);
} 