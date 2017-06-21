#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

attribute highp vec4 vertex;  // attribute vec4 a_position;
attribute mediump vec4 texCoord;  // attribute vec2 a_texcoord;
varying mediump vec4 texc;  // varying vec2 v_texcoord;

uniform mediump mat4 projection;  // uniform mat4 mvp_matrix;

void main()  {
    // Calculate vertex position in screen space
    gl_Position = projection * vertex;

    // Pass texture coordinate to fragment shader
    // Value will be automatically interpolated to fragments inside polygon faces
    texc = texCoord;
}


//#ifdef GL_ES
//// Set default precision to medium
//precision mediump int;
//precision mediump float;
//#endif

//uniform mat4 mvp_matrix;

//attribute vec4 a_position;
//attribute vec2 a_texcoord;

//varying vec2 v_texcoord;

////! [0]
//void main()
//{
//    // Calculate vertex position in screen space
//    gl_Position = mvp_matrix * a_position;

//    // Pass texture coordinate to fragment shader
//    // Value will be automatically interpolated to fragments inside polygon faces
//    v_texcoord = a_texcoord;
//}
////! [0]






