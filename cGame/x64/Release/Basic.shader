#shader vertex
#version 430 core
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec4 Color;
layout(location = 3) in float TextureID;
uniform vec2 u_cameraPos;
out vec2 Texcoord;
out vec4 color;
out float textureID;
out float isntHealthBar;
void main()
{
    isntHealthBar = float(TextureID != 3.0f && TextureID != 4.0f);
    textureID = TextureID;
    color = Color;
	Texcoord = texcoord;
    gl_Position.x = position.x + u_cameraPos.x * isntHealthBar;
    gl_Position.y = position.y + u_cameraPos.y * isntHealthBar;
    gl_Position.z = position.z;
    gl_Position.w = position.w;
}
#shader fragment
#version 430 core
in vec4 color;
in vec2 Texcoord;
in float textureID;
out vec4 v_Texcoord;
uniform vec4 u_Color;
uniform sampler2D u_Texture[32];
void main()
{
    v_Texcoord = texture(u_Texture[int(textureID)], Texcoord) * color;
}
//don't add empty line at end of file otherwise it breaks the shader interpretation