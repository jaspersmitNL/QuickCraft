struct VertexInput {
    @location(0) pos: vec3<f32>,
    @location(1) color: vec3<f32>,
}
struct VertexOutput {
    @builtin(position) pos: vec4<f32>,
    @location(0) color: vec4<f32>
}

struct Uniforms {
    uProj: mat4x4<f32>,
    uView: mat4x4<f32>,
    uModel: mat4x4<f32>,
}

@group(0) @binding(0) var<uniform> uniforms: Uniforms;

@vertex fn vertexMain(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    out.pos = uniforms.uProj * uniforms.uView * uniforms.uModel * vec4<f32>(in.pos, 1.0);
    out.color = vec4<f32>(in.color, 1.0);
    return out;
}
@fragment fn fragmentMain(in: VertexOutput) -> @location(0) vec4f {
    return in.color;
}