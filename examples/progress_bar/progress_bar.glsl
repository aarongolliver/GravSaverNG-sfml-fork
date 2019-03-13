// this progress gets blurier and blurier as it gets closer to the top because of additave AA errors
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    bool reset = false;
    vec4 c = vec4(1);
    if (reset) {
        c = vec4(0);
        fragColor = c;
    } else {
        if(fragCoord.y < 1) {
            float new_c = (1 - texture2D(texture_progress_bar, vec2(.5, 1)).r);
            fragColor = vec4(abs(new_c));
        } else {
            fragColor = vec4(texture2D(texture_progress_bar, vec2((fragCoord.x)/iResolution.x, (fragCoord.y-iTimeDelta*300)/iResolution.y)).rgb, 1);
        }
    }
}
