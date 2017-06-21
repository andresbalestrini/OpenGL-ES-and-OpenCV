#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform sampler2D texture;
varying mediump vec4 texc;  // varying vec2 v_texcoord;

void main()  {
    gl_FragColor = texture2D( texture, texc.st );  // gl_FragColor = texture2D(texture, v_texcoord);
}





