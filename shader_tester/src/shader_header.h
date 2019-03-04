#pragma once

#include <string>

std::string shaderHeader = "#version 450\n\
\n\
uniform vec2      iResolution;           // viewport resolution (in pixels)\n\
uniform float     iTime;                 // shader playback time (in seconds)\n\
//uniform float     iTimeDelta;            // render time (in seconds)\n\
//uniform int       iFrame;                // shader playback frame\n\
//uniform float     iChannelTime[4];       // channel playback time (in seconds)\n\
//uniform vec3      iChannelResolution[4]; // channel resolution (in pixels)\n\
uniform vec2      iMouse;                // mouse pixel coords. xy: current (if MLB down), zw: click\n\
//uniform vec4      iDate;                 // (year, month, day, time in seconds)\n\
//uniform float     iSampleRate;           // sound sample rate (i.e., 44100)\n\
\n\
void mainImage(out vec4 fragColor, in vec2 fragCoord);\n\
void main() {\n\
mainImage(gl_FragColor, gl_FragCoord.xy);\n\
}\n\
";

std::string fractalShader = "//#define SUPERSAMP\n\
\n\
struct quaternion\n\
{\n\
    vec4 d;\n\
};\n\
\n\
\n\
quaternion qtimes(in quaternion a, in quaternion b) {\n\
    float y0 = dot(a.d.xyzw, b.d.xyzw * vec4(1., -1., -1., -1.));\n\
    float y1 = dot(a.d.xyzw, b.d.yxwz * vec4(1., 1., 1., -1.));\n\
    float y2 = dot(a.d.xyzw, b.d.zwxy * vec4(1., -1., 1., 1.));\n\
    float y3 = dot(a.d.xyzw, b.d.wzyx * vec4(1., 1., -1., 1.));\n\
\n\
    quaternion q;\n\
    q.d = vec4(y0, y1, y2, y3);\n\
\n\
    return q;\n\
}\n\
\n\
quaternion qtimes(in float f, in quaternion a) {\n\
    quaternion q;\n\
    q.d = vec4(f) * a.d;\n\
    return q;\n\
}\n\
\n\
quaternion qplus(in quaternion a, in quaternion b) {\n\
    quaternion q;\n\
    q.d = a.d + b.d;\n\
\n\
    return q;\n\
}\n\
\n\
float qabsq(in quaternion q) {\n\
    return dot(q.d, q.d);\n\
}\n\
\n\
float calcDistance(quaternion z, quaternion c) {\n\
    quaternion dz;\n\
    dz.d = vec4(1., 0., 0., 0.);\n\
\n\
    float m2 = 0.;\n\
    float keep_going = 1.;\n\
\n\
\n\
    for (float i = 0.; i < 12.; i++) {\n\
        if (keep_going > 0.) {\n\
            dz = qtimes(2., qtimes(dz, z));\n\
            z = qplus(qtimes(z, z), c);\n\
            m2 = qabsq(z);\n\
            if (m2 > 100.) {\n\
                keep_going = 0.;\n\
            }\n\
        }\n\
    }\n\
\n\
    return sqrt(m2 / qabsq(dz)) * 0.5 * log(sqrt(m2));\n\
}\n\
\n\
float calcDistance_atDelta(in vec3 surf_center, in quaternion c, in vec3 delta) {\n\
    vec3 z_pos = surf_center + delta;\n\
    quaternion z;\n\
    z.d = vec4(z_pos.x, z_pos.y, z_pos.z, 0.);\n\
\n\
    return calcDistance(z, c);\n\
}\n\
\n\
vec3 make_abs(in vec3 v) {\n\
    return vec3(abs(v.x), abs(v.y), abs(v.z));\n\
}\n\
\n\
float cam_slow = 25.;\n\
vec4 do_everything(in vec3 e,\n\
    in vec3 w,\n\
    in vec3 u,\n\
    in vec3 v,\n\
    in quaternion c,\n\
    in float l,\n\
    in float r,\n\
    in float t,\n\
    in float b,\n\
    in float d,\n\
    in float dx,\n\
    in float dy,\n\
    in vec2 fragCoord) {\n\
    float rad = 2.;\n\
    float U = l + (r - l) * (fragCoord.x + dx) / (iResolution.x);\n\
    float V = b + (t - b) * (fragCoord.y + dy) / (iResolution.y);\n\
\n\
    vec3 ray_dir = normalize(w*vec3(-d) + u * vec3(U) + v * vec3(V));\n\
    vec3 ray_e = e;\n\
    float MAX_DIST = 2. * length(e);\n\
\n\
\n\
    quaternion z0;\n\
    z0.d = vec4(ray_e, 0.);\n\
\n\
    float init_dist = calcDistance(z0, c);\n\
\n\
    float dist = init_dist;\n\
\n\
    const float max_iters = 64.;\n\
    float marched_iters = 0.;\n\
    float keep_going = 1.;\n\
\n\
    const float alpha = 0.0001;\n\
    const float clarity = 0.;\n\
\n\
    for (float iters = 0.; iters < max_iters; iters++) {\n\
        if (keep_going > 0.) {\n\
            vec3 z2_pos = ray_e + ray_dir * vec3(dist);\n\
\n\
            quaternion z2;\n\
            z2.d = vec4(z2_pos, 0.);\n\
\n\
            float newdist = calcDistance(z2, c);\n\
            if (abs(dist) > MAX_DIST || abs(newdist) > MAX_DIST || (keep_going > 0. && dist == 0.)) {\n\
                dist = 0.;\n\
                keep_going = 0.;\n\
            }\n\
\n\
            if (newdist < alpha * pow(dist, clarity)) {\n\
                keep_going = 0.;\n\
            }\n\
\n\
            dist += newdist * keep_going;\n\
            marched_iters = marched_iters + keep_going;\n\
        }\n\
    }\n\
\n\
    if (dist == 0.) {\n\
        return vec4(0.33, 0., 0., 0.);\n\
    }\n\
    else {\n\
        float AO_VEC = max(1. - marched_iters / max_iters, .05);\n\
        vec3 grad_center = ray_e + ray_dir * vec3(dist);\n\
        //float grad_delta = 0.0001;\n\
        float grad_delta = alpha * pow(dist, clarity);\n\
        // calculate gradiant!\n\
        float dist_left = calcDistance_atDelta(grad_center, c, vec3(grad_delta) * u);\n\
        float dist_right = calcDistance_atDelta(grad_center, c, vec3(-grad_delta) * u);\n\
        float dist_up = calcDistance_atDelta(grad_center, c, vec3(-grad_delta) * v);\n\
        float dist_down = calcDistance_atDelta(grad_center, c, vec3(grad_delta) * v);\n\
        float dist_for = calcDistance_atDelta(grad_center, c, vec3(-grad_delta) * ray_dir);\n\
        float dist_bak = calcDistance_atDelta(grad_center, c, vec3(grad_delta) * ray_dir);\n\
        vec3 GRAD_VEC = normalize(make_abs(vec3((dist_left - dist_right) / (2. * grad_delta),\n\
            (dist_up - dist_down) / (2. * grad_delta),\n\
            (dist_for - dist_bak) / (2. * grad_delta))));\n\
\n\
        // done with gradiant\n\
\n\
\n\
        vec3 light_pos = vec3(rad * sin(iTime / cam_slow), rad * cos(iTime / cam_slow), rad);\n\
\n\
        vec3 ray_light_e = ray_e + ray_dir * vec3(dist * .99);\n\
        vec3 ray_light_dir = normalize(light_pos - ray_light_e);\n\
\n\
        float dist_to_light = abs(distance(light_pos, ray_light_e));\n\
\n\
        keep_going = 1.;\n\
        marched_iters = 0.;\n\
        dist = 0.;\n\
        const float max_iters_l = max_iters;\n\
        for (float iters = 0.; iters < max_iters_l; iters++) {\n\
            vec3 z2_pos = ray_light_e + ray_light_dir * vec3(dist);\n\
\n\
            quaternion z2;\n\
            z2.d = vec4(z2_pos.x, z2_pos.y, z2_pos.z, 0.);\n\
\n\
            float newdist = calcDistance(z2, c);\n\
            if (abs(dist) > MAX_DIST || abs(newdist) > MAX_DIST || (keep_going == 0. && dist == 0.)) {\n\
                dist = 0.;\n\
                keep_going = 0.;\n\
            }\n\
\n\
            if (newdist < alpha * pow(dist, clarity)) {\n\
                keep_going = 0.;\n\
            }\n\
\n\
            dist += newdist * keep_going;\n\
            marched_iters = marched_iters + keep_going;\n\
        }\n\
\n\
        float LIGHT_VEC = 1.;\n\
        if (dist == 0. || dist > dist_to_light) {\n\
            LIGHT_VEC = 1.;\n\
        }\n\
        else {\n\
            LIGHT_VEC = .5;\n\
        }\n\
\n\
        vec3 COL_VEC = LIGHT_VEC * AO_VEC * GRAD_VEC;\n\
        return vec4(COL_VEC, 1.);\n\
    }\n\
}\n\
\n\
void mainImage(out vec4 fragColor, in vec2 fragCoord)\n\
{\n\
    float bandIntensity0 = 0;\n\
    float bandIntensity1 = 0;\n\
    float bandIntensity2 = 0;\n\
    float bandIntensity3 = 0;\n\
\n\
    // ray generation\n\
    float rad = 2.;\n\
    float e_x = rad * sin(iTime / cam_slow);\n\
    float e_y = rad * cos(iTime / cam_slow);\n\
    float e_z = 0.;\n\
\n\
#ifdef SUPERSAMP\n\
    float SS = 2.;\n\
#else\n\
    float SS = 1.;\n\
#endif\n\
\n\
    float l = -iResolution.x * SS;\n\
    float r = iResolution.x * SS;\n\
    float t = iResolution.y * SS;\n\
    float b = -iResolution.y * SS;\n\
    float d = iResolution.y * SS;\n\
\n\
    vec3 e = vec3(e_x, e_y, e_z);\n\
    vec3 lookpoint = vec3(0., 0., 0.);\n\
    vec3 upnorm = vec3(0., 0., 1.);\n\
\n\
\n\
    vec3 lookdir = lookpoint - e;\n\
\n\
    vec4 TOTAL_COLOR = vec4(0.);\n\
\n\
    vec3 w = normalize(lookdir) * vec3(-1., -1., -1.);\n\
    vec3 u = normalize(cross(upnorm, w));\n\
    vec3 v = normalize(cross(w, u));\n\
\n\
    quaternion c;\n\
    c.d.x = (iMouse.x > 0. ? iMouse.x / iResolution.x * 2. - 1. : -1.0);\n\
    c.d.z = (iMouse.x > 0. ? iMouse.y / iResolution.y * 2. - 1. : 0.0);\n\
    c.d.y = bandIntensity2 / 1.5;\n\
    c.d.w = bandIntensity3 / 1.5;\n\
\n\
    TOTAL_COLOR += do_everything(e, w, u, v, c, l, r, t, b, d, 0., 0., fragCoord);\n\
#ifdef SUPERSAMP\n\
    TOTAL_COLOR += do_everything(e, w, u, v, c, l, r, t, b, d, 1. / SS, 0., fragCoord);\n\
    TOTAL_COLOR += do_everything(e, w, u, v, c, l, r, t, b, d, 1. / SS, 1. / SS, fragCoord);\n\
    TOTAL_COLOR += do_everything(e, w, u, v, c, l, r, t, b, d, 0., 1. / SS, fragCoord);\n\
#endif\n\
\n\
    fragColor = TOTAL_COLOR / vec4(SS*SS);\n\
}\n\
";