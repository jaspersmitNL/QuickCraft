


struct VertexOutput {
    @builtin(position) pos: vec4<f32>,
    @location(0) color: vec4<f32>
}

struct Uniforms {
    uProjection: mat4x4<f32>,
    uOffset: vec3<f32>
};

@group(0) @binding(0) var<uniform> uniforms: Uniforms;

@vertex fn vertexMain(@location(0) in_vertex_position: vec2<f32>, @location(1) in_vertex_color: vec3<f32>) -> VertexOutput {
    var output: VertexOutput;



    //Apply the projection matrix and offset
    output.pos = uniforms.uProjection * vec4<f32>(
        uniforms.uOffset.x + in_vertex_position.x,
        uniforms.uOffset.y + in_vertex_position.y,
        0.0, 1.0);



    output.color = vec4<f32>(in_vertex_color, 1.0);
    return output;
}

@fragment fn fragmentMain(
    output: VertexOutput
) -> @location(0) vec4<f32> {
    return output.color;
}
