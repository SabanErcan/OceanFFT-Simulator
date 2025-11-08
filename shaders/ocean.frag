#version 430 core

// Inputs from vertex shader
in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vTexCoord;
in float vFresnelFactor;
in float vHeight;

// Uniforms
uniform vec3 uCameraPos;
uniform vec3 uWaterColor;        // Deep water color
uniform vec3 uSkyColor;          // Simplified skybox (single color)
uniform vec3 uSunDirection;      // Normalized sun direction
uniform float uFoamThreshold;    // Height threshold for foam
uniform float uTime;             // Animation time

// Output
out vec4 FragColor;

// Noise function for foam texture
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f); // Smoothstep
    
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    
    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

void main() {
    // Normalize interpolated normal
    vec3 N = normalize(vNormal);
    vec3 V = normalize(uCameraPos - vWorldPos);
    vec3 L = normalize(uSunDirection);
    
    // Reflection vector for sky
    vec3 R = reflect(-V, N);
    
    // Sky reflection (simple gradient based on reflection direction)
    // In a full implementation, this would sample a cubemap
    float skyFactor = R.y * 0.5 + 0.5; // Map [-1,1] to [0,1]
    vec3 skyReflection = mix(uSkyColor * 0.6, uSkyColor, skyFactor);
    
    // Deep water color (darker, blue-green)
    vec3 deepWater = uWaterColor;
    
    // Fresnel effect: mix water color with sky reflection
    // More reflection at grazing angles, more water color when looking down
    vec3 waterColor = mix(deepWater, skyReflection, vFresnelFactor);
    
    // Subsurface scattering approximation (simple)
    float subsurface = max(0.0, dot(N, L)) * 0.3;
    waterColor += vec3(0.0, 0.3, 0.2) * subsurface;
    
    // Specular highlight (Blinn-Phong)
    vec3 H = normalize(V + L);
    float specular = pow(max(dot(N, H), 0.0), 256.0);
    vec3 specularColor = vec3(1.0, 1.0, 0.9) * specular;
    waterColor += specularColor * 0.8;
    
    // Foam on wave crests
    float foamAmount = 0.0;
    if (vHeight > uFoamThreshold) {
        // Generate foam texture using noise
        vec2 foamUV = vTexCoord * 50.0 + uTime * 0.1;
        float foamNoise = noise(foamUV);
        foamNoise = foamNoise * noise(foamUV * 2.0) * noise(foamUV * 4.0);
        
        // Fade foam based on height
        float heightFactor = smoothstep(uFoamThreshold, uFoamThreshold + 0.3, vHeight);
        foamAmount = foamNoise * heightFactor;
        
        // Mix in foam color
        vec3 foamColor = vec3(1.0, 1.0, 1.0);
        waterColor = mix(waterColor, foamColor, foamAmount * 0.8);
    }
    
    // Depth-based darkening (simulate deeper water)
    // In a real scenario, you'd raymarch or use a depth map
    float depthFade = smoothstep(-50.0, -5.0, vWorldPos.y);
    waterColor = mix(waterColor * 0.3, waterColor, depthFade);
    
    // Add slight blue tint to shadowed areas
    float shadow = max(0.0, dot(N, L));
    waterColor = mix(waterColor * vec3(0.5, 0.6, 0.8), waterColor, shadow * 0.5 + 0.5);
    
    // Final color with transparency
    float alpha = 0.95;
    if (foamAmount > 0.5) alpha = 1.0; // Foam is opaque
    
    FragColor = vec4(waterColor, alpha);
}
