struct VertexAndInstanceInput {
    @location(0) pos: vec3<f32>,
    @location(1) uv: vec2<f32>,
    @location(2) center: vec3<f32>,
    @location(3) orientation: u32,
    @location(4) blockID: u32,
}

struct VertexOutput {
    @builtin(position) pos: vec4<f32>,
    @location(0) uv: vec2<f32>,
    @location(1) normal: vec3<f32>,
    @location(2) @interpolate(flat) blockID: u32,
}


struct ChunkUniforms {
    uProj: mat4x4<f32>,
    uView: mat4x4<f32>,
    uModel: mat4x4<f32>
}

struct WorldRendererUniforms {
    fogNear: f32,
    fogFar: f32,
    fogColor: vec3<f32>
};

@group(0) @binding(0) var<uniform> chunkUniforms: ChunkUniforms;
@group(0) @binding(1) var uSampler: sampler;
@group(0) @binding(2) var uTexture: texture_2d_array<f32>;
//@group(1) @binding(0) var<uniform> globalUniforms: WorldRendererUniforms;




const FRONT: u32 = 0;
const BACK: u32 = 1;
const LEFT: u32 = 2;
const RIGHT: u32 = 3;
const TOP: u32 = 4;
const BOTTOM: u32 = 5;

fn get_rot_matrix(face: u32) -> mat3x3<f32> {


    if (face == FRONT) {
        return mat3x3<f32>(
            1.0, 0.0, 0.0,
            0.0, 1.0, 0.0,
            0.0, 0.0, 1.0
        );
    }
    else if (face == BACK) {
        return mat3x3<f32>(
            -1.0, 0.0, 0.0,
            0.0, 1.0, 0.0,
            0.0, 0.0, -1.0
        );
    }
    else if (face == LEFT) {
        return mat3x3<f32>(
            0.0, 0.0, -1.0,
            0.0, 1.0, 0.0,
            1.0, 0.0, 0.0
        );
    }
    else if (face == RIGHT) {
        return mat3x3<f32>(
            0.0, 0.0, 1.0,
            0.0, 1.0, 0.0,
            -1.0, 0.0, 0.0
        );
    }
    else if (face == TOP) {
        return mat3x3<f32>(
            1.0, 0.0, 0.0,
            0.0, 0.0, -1.0,
            0.0, 1.0, 0.0
        );
    }
    else if (face == BOTTOM) {
        return mat3x3<f32>(
            1.0, 0.0, 0.0,
            0.0, 0.0, 1.0,
            0.0, -1.0, 0.0
        );
    }

    //identity
    return mat3x3<f32>(
       0.0, 0.0, 0.0,
       0.0, 0.0, 0.0,
       0.0, 0.0, 0.0
    );
}







@vertex
fn vs_main(in: VertexAndInstanceInput) -> VertexOutput {
    var out: VertexOutput;

    var rot_mat = get_rot_matrix(in.orientation);

    out.uv = in.uv;
    out.blockID = in.blockID;

    var rotated_pos = rot_mat * in.pos;
    var translated_pos = rotated_pos + in.center;

    out.normal = rot_mat * vec3<f32>(0.0, 0.0, 1.0);

    out.pos = chunkUniforms.uProj * chunkUniforms.uView * chunkUniforms.uModel * vec4<f32>(translated_pos, 1.0);

    return out;
}



//ambient light color

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
    var color = textureSample(uTexture, uSampler, in.uv, in.blockID);

    return color;
}