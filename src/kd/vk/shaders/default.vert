#version 450

vec3 positions[] = {
  vec3(-0.5, -0.5, 0.0),
  vec3( 0.0,  0.5, 0.0),
  vec3( 0.5, -0.5, 0.0)
};

vec3 colors[] = {
  vec3(0.9, 0.2, 0.2),
  vec3(0.2, 0.9, 0.2),
  vec3(0.2, 0.2, 0.9)
};

layout(location = 0) out vec3 vColor;

void main() {
  gl_Position = vec4(positions[gl_VertexIndex], 1.0);
  vColor = colors[gl_VertexIndex];
}
