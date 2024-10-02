struct Uniforms {
    uProjection: mat4x4<f32>,
    uModel: mat4x4<f32>,
};

struct VertexInput {
    @location(0) pos: vec2<f32>,
    @location(1) color: vec3<f32>
}

struct VertexOutput {
    @builtin(position) pos: vec4<f32>,
    @location(0) color: vec3<f32>
}

@group(0) @binding(0) var<uniform> uniforms: Uniforms;

@vertex
fn vertexMain(input: VertexInput) -> VertexOutput {
    var output: VertexOutput;
    // Scale the position by the time
    output.pos = uniforms.uProjection * uniforms.uModel * vec4f(input.pos, 0, 1);
    output.color = input.color;
    return output;
}

@fragment
fn fragmentMain(input: VertexOutput) -> @location(0) vec4f {
    return vec4f(input.color, 1);
}
