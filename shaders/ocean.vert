#version 430 core

layout(location = 0) in vec3 aPos;       // Grid base position
layout(location = 1) in vec2 aTexCoord;  // UV coordinates

// Uniforms - Matrices
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

// Uniforms - Textures
uniform sampler2D uDisplacement;  // RGB = (dx, dy, dz) displacement
uniform sampler2D uNormals;       // RGB = (nx, ny, nz) normal

// Uniforms - Camera
uniform vec3 uCameraPos;

// Outputs to fragment shader
out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vTexCoord;
out float vFresnelFactor;
out float vHeight;

void main() {
    // Sample displacement map
    vec3 displacement = texture(uDisplacement, aTexCoord).rgb;
    
    // Apply displacement to base grid position
    vec3 displacedPos = aPos + displacement;
    
    // Transform to world space
    vWorldPos = (uModel * vec4(displacedPos, 1.0)).xyz;
    
    // Sample and transform normal
    vec3 sampledNormal = texture(uNormals, aTexCoord).rgb;
    vNormal = normalize((uModel * vec4(sampledNormal, 0.0)).xyz);
    
    // Pass through texture coordinates
    vTexCoord = aTexCoord;
    
    // Store height for foam calculation
    vHeight = displacement.y;
    
    // Calculate Fresnel factor (Schlick's approximation)
    vec3 viewDir = normalize(uCameraPos - vWorldPos);
    float cosTheta = max(dot(viewDir, vNormal), 0.0);
    
    // F = F0 + (1 - F0) * (1 - cos(θ))^5
    // For water, F0 ≈ 0.02
    float F0 = 0.02;
    vFresnelFactor = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
    
    // Transform to clip space
    gl_Position = uProj * uView * vec4(vWorldPos, 1.0);
}
