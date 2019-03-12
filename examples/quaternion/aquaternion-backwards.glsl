void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec3 up = texture2D(aquaternion, (fragCoord + vec2(dx, 0))/iResolution).rgb;
    vec3 down = texture2D(aquaternion, (fragCoord + vec2(-dx, 0))/iResolution).rgb;
    vec3 left = texture2D(aquaternion, (fragCoord + vec2(0, dx))/iResolution).rgb;
    vec3 right = texture2D(aquaternion, (fragCoord + vec2(0, dy))/iResolution).rgb;

    vec3 up2 = texture2D(aquaternion, (fragCoord + vec2(dx*2, 0))/iResolution).rgb;
    vec3 down2 = texture2D(aquaternion, (fragCoord + vec2(-dx*2, 0))/iResolution).rgb;
    vec3 left2 = texture2D(aquaternion, (fragCoord + vec2(0, dx*2))/iResolution).rgb;
    vec3 right2 = texture2D(aquaternion, (fragCoord + vec2(0, dy*2))/iResolution).rgb;
    fragColor = vec4((up + down + left + right+up2 + down2 + left2 + right2)/ 8., 1);
}
