
    #define points 100
    #define PI 3.141590

    for(int i = 0; i < points; ++i) {
        float posX = abs(sin(i*1514+7));
        float posY = abs(sin(i*6554+5));
        vec2 pos = vec2(posX, posY);
        vec2 dist = frag - pos;
        float d = sqrt(dist.x * dist.x + dist.y * dist.y);
        if(d < size/10)
        fragColor = vec4(color, 1);
    }

    if(frag.x < .02)
    fragColor = vec4(.5);
    if(frag.y < .02)
    fragColor = vec4(.5);
    if(frag.x < .01)
    fragColor = vec4(1);
    if(frag.y < .01)
    fragColor = vec4(1);
