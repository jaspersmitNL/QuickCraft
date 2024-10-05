struct VertexInput {
    @location(0) pos: vec3<f32>,
    @location(1) uv: vec2<f32>
}

struct VertexOutput {
    @builtin(position) pos: vec4<f32>,
    @location(0) uv: vec2<f32>
}

struct Uniforms {
    uProjection: mat4x4<f32>
}

@group(0) @binding(0) var<uniform> uniforms: Uniforms;

@vertex fn vs_main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    out.pos = uniforms.uProjection * vec4<f32>(in.pos, 1.0);
    out.uv = in.uv;
    return out;
}

@fragment fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    //color using uv
    return vec4<f32>(1.0, 0.0, 0.0, 1.0);
}