#version 330 core

uniform sampler2D depth_tex; // texture uniform
uniform sampler2D sampler_tex; // texture uniform

uniform float model_distance;
uniform float near;
uniform float far;
uniform int u_DOFOn;

vec2 img_size = vec2(1280, 720);

in vec2 varying_coord;

out vec4 fragment_colour;

float to_distance(float depth_colour)
{
    float z = depth_colour * 2.0 - 1.0;
    float dist = (2.0*near*far) / (far + near - z *(far - near));	
    return dist;
}

void main()
{
    vec4 blurred_colour = texture(sampler_tex, varying_coord);

    // Possibility to toggle DOF on and off.
	if (u_DOFOn == 0)
	{
		fragment_colour = vec4(blurred_colour);
		
		return;
	}

    const float pi_times_2 = 6.28318530718; // Pi*2
    
    float directions = 16.0; // BLUR directions (Default 16.0 - More is better but slower)
    float quality = 10.0; // BLUR quality (Default 4.0 - More is better but slower)
    float size = 8.0; // BLUR size (radius)

	vec4 unblurred_colour = texture(sampler_tex, varying_coord);
	float depth_colour = texture(depth_tex, varying_coord).r;
 
    float distance_to_pixel = to_distance(depth_colour);
    float x = clamp(abs(distance_to_pixel - model_distance) / model_distance, 0.0, 1.0);
    x = max(1.0 - x, 0.0);
    x = 1.0 - pow(x, 1.0/10.0);

    vec2 radius = vec2(size/img_size.x, size/img_size.y);

    for( float d=0.0; d<pi_times_2; d+= pi_times_2/directions)
		for(float i=1.0/quality; i<=1.0; i+=1.0/quality)
			blurred_colour += texture( sampler_tex, varying_coord + vec2(cos(d),sin(d))*radius*x*i);	
    
    // Output to screen
    blurred_colour /= quality * directions - 15.0;

    fragment_colour.rgb = blurred_colour.rgb;//vec3(mix(unblurred_colour, blurred_colour, x));
    fragment_colour.a = 1.0;
}

//
//
//#define DISPLAY_GAMMA 1
//
//#define GOLDEN_ANGLE 2.39996323
//#define MAX_BLUR_SIZE 20.0
//
//// Smaller = nicer blur, larger = faster
//#define RAD_SCALE 0.5
//
//uniform float uFar;
//
//float getBlurSize(float depth, float focusPoint, float focusScale)
//{
//	float coc = clamp((1.0 / focusPoint - 1.0 / depth)*focusScale, -1.0, 1.0);
//    return abs(coc) * MAX_BLUR_SIZE;
//}
//
//vec3 depthOfField(vec2 texCoord, float focusPoint, float focusScale)
//{
//    vec4 Input = texture(colour_tex, varying_coord).rgba;
//    float InputDepth = texture(depth_tex, varying_coord).r;
//
//    float centerDepth = InputDepth * uFar;
//    float centerSize = getBlurSize(centerDepth, focusPoint, focusScale);
//    vec3 color = Input.rgb;
//    float tot = 1.0;
//    
//    vec2 texelSize = 1.0 / img_size;
//
//    float radius = RAD_SCALE;
//    for (float ang = 0.0; radius < MAX_BLUR_SIZE; ang += GOLDEN_ANGLE)
//    {
//        vec2 tc = texCoord + vec2(cos(ang), sin(ang)) * texelSize * radius;
//        
//        vec4 sampleInput = texture(colour_tex, tc).rgba;
//
//        vec3 sampleColor = sampleInput.rgb;
//        float sampleDepth = sampleInput.a * uFar;
//        float sampleSize = getBlurSize(sampleDepth, focusPoint, focusScale);
//        
//        if (sampleDepth > centerDepth)
//        {
//        	sampleSize = clamp(sampleSize, 0.0, centerSize*2.0);
//        }
//
//        float m = smoothstep(radius-0.5, radius+0.5, sampleSize);
//        color += mix(color/tot, sampleColor, m);
//        tot += 1.0;
//        radius += RAD_SCALE/radius;
//    }
//    
//    return color /= tot;
//}
//
//void main()
//{
//    vec2 uv = fragment_colour.xy / img_size;
//    
//    vec4 color = texture(colour_tex, uv).rgba;
//
//    float InputDepth = texture(depth_tex, varying_coord).r;
//
//     // Possibility to toggle DOF on and off.
//    if (u_DOFOn == 0)
//    {
//	    fragment_colour = vec4(color);
//	
//	    return;
//    }
//
//    float focusPoint = 78.0;
//    float focusScale = (0.5 * 0.5) * 50.0;
//    
//    color.rgb = depthOfField(uv, focusPoint, focusScale);
//
//    //tone mapping
//    color.rgb = vec3(1.7, 1.8, 1.9) * color.rgb / (1.0 + color.rgb);
//    
//    //inverse gamma correction
//	fragment_colour = vec4(pow(color.rgb, vec3(1.0 / DISPLAY_GAMMA)), 1.0);
//    
//    // Debug depth
//    fragment_colour.rgb = vec3(InputDepth);
//}
