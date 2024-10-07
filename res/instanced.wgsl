struct VertexAndInstanceInput {
    @location(0) pos: vec3<f32>,
    @location(1) center: vec3<f32>
}

struct VertexOutput {
    @builtin(position) pos: vec4<f32>
}


@vertex
fn vs_main(in: VertexAndInstanceInput) -> VertexOutput {
    var out: VertexOutput;
    out.pos = vec4<f32>(in.center + in.pos, 1.0);
    return out;
}

@fragment
fn fs_main() -> @location(0) vec4<f32> {
    return vec4<f32>(1.0, 0.0, 0.0, 1.0);
}