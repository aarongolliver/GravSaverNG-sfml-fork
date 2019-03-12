void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    bool reset = false;
    if (reset) {
        fragColor = vec4(0,0,0,1);
    } else {
        if(fragCoord.x < 10 && fragCoord.y < 10) {
            fragColor = vec4(1,1,1,1);
        } else {
            if(fragCoord.x <= 1) {
                fragColor = vec4(texture2D(texture_progress_bar, vec2(1, (fragCoord.y - 10) / iResolution.x)).rgb, 1);
            } else {
                fragColor = vec4(texture2D(texture_progress_bar, vec2((fragCoord.x-10)/iResolution.x, (fragCoord.y)/iResolution.y)).rgb, 1);
            }
        }
    }
}
