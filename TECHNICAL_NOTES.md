# Technical Notes - Ocean FFT Implementation

## FFT Ocean Theory

### Phillips Spectrum Derivation

The Phillips spectrum models the statistical distribution of ocean wave energy based on wind parameters.

**Full Formula**:
```
P(k) = A * exp(-1/(kL)²) / k⁴ * |k̂·ŵ|² * exp(-k²l²)
```

**Components**:
- `A`: Overall amplitude scaling (user parameter, typically ~0.0002)
- `k`: Wave vector (kx, kz) in frequency domain
- `L = V²/g`: Fully developed sea length (V = wind speed, g = gravity)
- `k̂·ŵ`: Alignment factor (suppresses perpendicular waves)
- `l`: Small wave cutoff (L/1000, suppresses capillary waves)

**Physical Meaning**:
- `1/k⁴`: Power law - energy decreases rapidly at high frequencies
- `exp(-1/(kL)²)`: Suppresses waves longer than L
- `(k̂·ŵ)²`: Wind directional dependence (maximum at 0°, zero at 90°)

### Time Evolution

**h(k,t) Formula**:
```
h(k,t) = h₀(k) * exp(iωt) + h₀*(-k) * exp(-iωt)
```

**Why Conjugate Pair?**:
- FFT requires Hermitian symmetry for real-valued output
- h(k) and h*(-k) are conjugate pairs
- Ensures height field is real (no imaginary component)

**Dispersion Relation**:
```
ω(k) = √(g|k|)
```

Deep water approximation. For shallow water:
```
ω(k) = √(g|k| * tanh(|k|d))  // d = depth
```

### Choppy Displacement

Standard FFT gives vertical displacement only. Choppy adds horizontal:

```
Dx(x) = -λ * ∂h/∂x
Dz(z) = -λ * ∂h/∂z
```

In frequency domain:
```
Dx(k) = -iλ * kx/|k| * h(k)
Dz(k) = -iλ * kz/|k| * h(k)
```

Where λ (choppy parameter) controls sharpness. Typical values: 1.5 - 3.5

**Effect**: Pulls vertices horizontally toward wave crests, creating sharper peaks.

### Normal Calculation

Normals computed from gradients:
```
N = normalize((-∂h/∂x, 1, -∂h/∂z))
```

In frequency domain (multiplication by ik):
```
∂h/∂x(k) = i*kx * h(k)
∂h/∂z(k) = i*kz * h(k)
```

After inverse FFT, get spatial derivatives directly.

## Implementation Details

### FFTW3 Usage

**Plan Creation** (expensive, do once):
```cpp
fftwf_plan plan = fftwf_plan_dft_c2r_2d(
    N, N,                                // Dimensions
    (fftwf_complex*)input,               // Frequency domain (complex)
    output,                              // Spatial domain (real)
    FFTW_ESTIMATE                        // Planning rigor
);
```

**Execution** (fast, per frame):
```cpp
fftwf_execute(plan);
```

**Normalization**:
FFTW doesn't normalize inverse transforms. Divide by N²:
```cpp
for (int i = 0; i < N*N; ++i) {
    output[i] /= (N * N);
}
```

**Memory Layout**:
- Input: `N × N` complex numbers = `N × N × 2` floats
- Output: `N × N` real numbers

### OpenGL Texture Upload

**Initial Creation**:
```cpp
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, N, N, 0, 
             GL_RGB, GL_FLOAT, nullptr);
```

**Per-Frame Update** (faster):
```cpp
glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, N, N,
                GL_RGB, GL_FLOAT, data);
```

**Why RGB32F?**:
- Need signed values (displacement can be negative)
- Float precision essential for smooth interpolation
- RGB: 3 channels for (dx, dy, dz) or (nx, ny, nz)

### Shader Optimizations

**Fresnel Calculation**:
Schlick's approximation (5 ops) vs full equation (expensive):
```glsl
// Schlick (used)
F = F0 + (1-F0) * pow(1 - cosTheta, 5)

// Full Fresnel-Dielectric
F = 0.5 * (rs² + rp²)  // where rs, rp from Snell's law
```

Error: <2% for water, huge performance gain.

**Normal Reconstruction**:
Instead of storing full (nx, ny, nz), could store only (nx, nz):
```glsl
vec3 N;
N.x = normalTexture.r;
N.z = normalTexture.g;
N.y = sqrt(1.0 - N.x*N.x - N.z*N.z);  // Assume unit length
```

Saves 1/3 bandwidth, adds 1 sqrt per fragment.

## Performance Analysis

### CPU Bottlenecks

**FFT Computation** (5 transforms):
- 256×256: ~1-2 ms total
- 512×512: ~4-8 ms total

**Memory Access Pattern**:
FFTW is cache-optimized, but our texture packing adds overhead:
```cpp
// Cache-friendly: sequential write
displacementData[idx*3 + 0] = choppyX[idx];
displacementData[idx*3 + 1] = heightField[idx];
displacementData[idx*3 + 2] = choppyZ[idx];
```

### GPU Bottlenecks

**Vertex Shader**:
- Texture fetches: 2 per vertex (displacement + normal)
- Math: minimal (matrix multiplies, Fresnel)
- Bandwidth: 256² × 2 textures × 12 bytes = ~1.5 MB/frame

**Fragment Shader**:
- Texture fetches: 2 (from interpolation)
- Math: Fresnel + specular + foam noise
- Fill rate: depends on resolution and window size

**Optimization**: Use mipmaps for distant ocean (LOD).

### Memory Usage

**CPU Side**:
```
Resolution 256²:
- h0, h0Conj: 2 × 256² × 8 bytes (complex) = 1 MB
- htilde variants: 5 × 256² × 8 bytes = 2.5 MB
- Spatial arrays: 5 × 256² × 4 bytes = 1.25 MB
Total: ~5 MB
```

**GPU Side**:
```
- Displacement texture: 256² × 3 × 4 bytes = 768 KB
- Normal texture: 256² × 3 × 4 bytes = 768 KB
- VBO (positions + UVs): 256² × 5 × 4 bytes = 1.25 MB
- IBO: (255² × 6) × 4 bytes = ~1.5 MB
Total: ~4.3 MB
```

## Numerical Stability

### Gaussian Random Generation

**Box-Muller Transform** (std::normal_distribution uses this):
```
u1, u2 ~ Uniform(0,1)
z0 = √(-2ln(u1)) * cos(2πu2)
z1 = √(-2ln(u1)) * sin(2πu2)
```

Both z0, z1 are N(0,1) distributed.

**Edge Case**: u1 = 0 → ln(0) = -∞. Implementation uses `u1 ∈ (0,1]`.

### Wave Vector Wrapping

```cpp
float kx = 2π * (x - N/2) / L;
```

For x ∈ [0, N), kx ∈ [-πN/L, πN/L).

**Nyquist Limit**: Maximum resolvable wavelength = 2L/N pixels.

### Phillips Spectrum Singularity

At k = 0, P(k) → ∞ (1/k⁴ term). Handle explicitly:
```cpp
if (kLen < 0.0001f) return 0.0f;
```

## Fresnel Physics

**Snell's Law**:
```
n1 * sin(θ1) = n2 * sin(θ2)
```

For air (n₁=1) to water (n₂=1.33):

| Angle | Reflection |
|-------|-----------|
| 0° (perpendicular) | 2% |
| 45° | 5% |
| 60° | 10% |
| 85° (grazing) | 95% |

**Schlick Approximation Error**:
- Max error: 1.5% at 45°
- Critical angles: exact at 0° and 90°

## Foam Generation

**Physical Basis**:
Foam forms when waves break (Jacobian of displacement < 0).

**Simplified Approach** (height-based):
```cpp
if (height > threshold) {
    foam = noise(uv) * smoothstep(threshold, threshold+range, height);
}
```

**Better Approach** (not implemented):
Compute Jacobian of displacement field:
```
J = |∂(x+Dx)/∂x  ∂(x+Dx)/∂z|
    |∂(z+Dz)/∂x  ∂(z+Dz)/∂z|

if (det(J) < 0) → foam
```

## Future Optimizations

### Compute Shaders

Move FFT to GPU:
```glsl
// Compute shader (workgroup 16×16)
layout(local_size_x = 16, local_size_y = 16) in;

void main() {
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);
    // Butterfly FFT passes...
}
```

**Benefit**: No CPU→GPU transfer per frame.  
**Challenge**: Complex implementation (16+ passes for 256²).

### Cascaded Grids

Multiple octaves at different scales:
```
Grid 1: 1024m patch, 256² resolution → large waves
Grid 2: 256m patch, 256² resolution → medium waves
Grid 3: 64m patch, 256² resolution → small detail
```

Blend in shader based on distance from camera.

### Temporal Reprojection

Reuse FFT from previous frame:
```
h(k, t+Δt) ≈ h(k,t) * exp(iωΔt)
```

Only recompute every N frames, interpolate between.

## Known Limitations

1. **Deep water only**: Dispersion relation ignores depth
2. **No wave breaking**: Jacobian not computed
3. **Periodic tiling**: Visible at patch boundaries (use blending)
4. **Static wind**: Direction/speed changes require h₀ regeneration
5. **No interaction**: Waves don't respond to objects

## References & Further Reading

1. **Tessendorf 2001** - Original ocean FFT paper
2. **GPU Gems (Chapter 1)** - NVIDIA implementation
3. **FFTW Manual** - Algorithm details
4. **Real-Time Rendering 4th Ed** - Fresnel equations (Chapter 9)
5. **PBR Book** - Light transport theory

---

*Last updated: 2025-11-08*
