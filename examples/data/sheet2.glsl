vec3 sliderColor = vec3(0, 122./255, 217./255);
vec3 barColor = vec3(160./255);

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    fragColor = vec4(1);

    float mouseX = clamp((iMouse/iResolution).x, 0, 1);
    vec2 frag = fragCoord/iResolution;
    vec3 color = texture(texture_color_picker, vec2(0,0)).rgb;
    float size = texture(texture_size, vec2(0,0)).x;

    #define chunks 5
    #define PI 3.141590
    float positions[chunks] = {.1, 1, 2, 3, 4};

    float prevTheta = 0;
    for(int i = 0; i < chunks; ++i) {
        float theta = positions[i] - .1;
        while(theta > 2*PI) { 
            theta -= 2*PI;
        }
        float fragTheta = atan(frag.y + .5, frag.x + .5);
        if(fragTheta > prevTheta && fragTheta < theta)
            fragColor = vec4(color, 1);

        prevTheta = theta;
    }
}
