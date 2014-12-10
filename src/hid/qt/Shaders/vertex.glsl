#version 330

in vec3 aPos;
vec4    tPos;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform mat4 Scale;
uniform mat4 Translation;

void main( void )
{
tPos = vec4(aPos.x, aPos.y, aPos.z, 1.0);
gl_Position = Projection * Translation * Scale * tPos;
}
