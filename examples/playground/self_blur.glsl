void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	float initPerc = .4;
	float timeDelta = clamp(iTimeDelta, 0, 1);
	vec3 new_color = vec3(0);
	vec3 old_color = texture2D(texture_self_blur, fragCoord/iResolution).rgb;
	new_color += texture2D(texture_quaternion,fragCoord/iResolution).rgb * initPerc;
	new_color += texture2D(texture_self_blur, (fragCoord+vec2(1,0))/iResolution).rgb * (1-initPerc)/4;
	new_color += texture2D(texture_self_blur, (fragCoord+vec2(-1,0))/iResolution).rgb * (1-initPerc)/4;
	new_color += texture2D(texture_self_blur, (fragCoord+vec2(0,1))/iResolution).rgb * (1-initPerc)/4;
	new_color += texture2D(texture_self_blur, (fragCoord+vec2(0,-1))/iResolution).rgb * (1-initPerc)/4;

	vec3 stepped_color =  old_color * (1-timeDelta) + new_color * timeDelta;

    fragColor = vec4(clamp(stepped_color, 0, 1).xyz,1);
}
