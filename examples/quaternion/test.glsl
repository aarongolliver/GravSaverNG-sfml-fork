// based off the work here:
// https://github.com/spite/Wagner/blob/master/fragment-shaders/box-blur-fs.glsl

float random(vec2 scale, float seed, vec2 fragCoord){
    return fract(sin(dot(fragCoord+seed,scale))*43758.5453+seed);
}

float delta = 0.1;

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec4 color=vec4(0.0);
    float total=0.0;
    float offset=random(vec2(12.9898,78.233),0.0,fragCoord);
    for(float t=-30.0; t<=30.0; t++) {
        float percent=(t+offset-0.5)/30.0;
        float weight=1.0-abs(percent);
        vec4 s=texture2D(texture_quaternion,fragCoord/iResolution+delta*percent);
        s.rgb*=s.a;
        color+=s*weight;
        total+=weight;
    }
    
    fragColor=vec4(vec3(color/total).xyz, 1);
}