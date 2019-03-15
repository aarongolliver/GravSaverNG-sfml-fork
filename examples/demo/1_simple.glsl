void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    fragColor = vec4(iMouse/iResolution, 0, 1);
    //fragColor = vec4(fragCoord/iResolution, 0, 1);
    //fragColor = vec4(fragCoord/iResolution, sin(iTime), 1);
}