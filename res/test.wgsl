struct VertexInput {
    @location(0) pos: vec3<f32>,
    @location(1) uv: vec2<f32>,
    @location(2) blockID: u32
}

struct VertexOutput {
    @builtin(position) pos: vec4<f32>,
    @location(0) uv: vec2<f32>,
    @location(1) @interpolate(flat) blockID: u32
}

struct Uniforms {
    uProjection: mat4x4<f32>,
    uView: mat4x4<f32>,
}

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var uSampler: sampler;
@group(0) @binding(2) var uMyTexture: texture_2d_array<f32>;


@vertex fn vs_main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    out.pos = uniforms.uProjection * uniforms.uView * vec4<f32>(in.pos, 1.0);
    out.uv = in.uv;
    out.blockID = in.blockID;
    return out;
}

@fragment fn fs_main(in: VertexOutput) -> @location(0) vec4f {

    var color = textureSample(uMyTexture, uSampler, in.uv, in.blockID);
    return color;
    //color using uv
//    return vec4<f32>(1.0, 0.0, 0.0, 1.0);
}