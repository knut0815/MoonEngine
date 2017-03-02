#version 400 core
in vec3 fragPos;
in vec3 fragNor;
in vec2 fragTex;

const int NUM_SHADOWS = 3;

in vec4 LSPosition[NUM_SHADOWS];
in float worldZ;

uniform sampler2D shadowMap[NUM_SHADOWS];
uniform float shadowZSpace[NUM_SHADOWS];

layout (location = 0) out vec4 posOut;
layout (location = 1) out vec4 colorOut;
layout (location = 2) out vec4 normalOut;

uniform sampler2D diffuse;


vec3 color[3] = vec3[](vec3(0.1,0,0), vec3(0,0.1,0), vec3(0,0,0.1));

float calcShadowFactor(int ShadowIndex, vec4 LSPosition) 
{
    vec3 projCoords = LSPosition.xyz / LSPosition.w;
    projCoords  = 0.5 * projCoords + 0.5;  
    float currentDepth = projCoords.z;     
    float shadowDepth = texture(shadowMap[ShadowIndex], projCoords.xy).r; 
    float bias = 0.01;
    //PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap[ShadowIndex], 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap[ShadowIndex], 
                                     projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.5;        
        }    
    }
    shadow /= 9.0;
    return 1-shadow;
    
}  

void main()
{
    posOut = vec4(fragPos, 1.0);
    colorOut.rgb = texture(diffuse, fragTex).rgb;
    colorOut.a = 1; //Currently hardcoded specular
    normalOut.xyz = normalize(fragNor);

    float ShadowFactor = 0.0;
    for (int i = 0 ; i < NUM_SHADOWS ; i++) {
        if (worldZ <= -shadowZSpace[i]) {
            ShadowFactor = calcShadowFactor(i, LSPosition[i]);
            //colorOut.rgb += color[i];
            normalOut.a = ShadowFactor;
            break;
            
        }
   }

}