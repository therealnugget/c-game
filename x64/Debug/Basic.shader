#shader vertex
#version 430 core
#define healthBarBorder 3.0f
#define healthBar 4.0f
//#define EARTHQUAKE
#define EARTHQUAKE_MULT 1.0f
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec4 Color;
layout(location = 3) in float TextureID;
uniform float u_elapsedT;
uniform vec2 u_cameraPos;
out float elapsedT;
out vec2 Texcoord;
out vec4 color;
out float textureID;
//two separate programs, so each one needs a redefinition of the same function :'( (i don't like code duplication), anyway, deterministic.
float random(vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}
void main()
{
    textureID = TextureID;
    color = Color;
    elapsedT = u_elapsedT;
	Texcoord = texcoord;
    gl_Position.xy = position.xy + u_cameraPos.xy * float(TextureID != healthBarBorder && TextureID != healthBar)
#ifdef EARTHQUAKE
    + (random(position.xy + vec2(u_elapsedT)) - .5f) * u_elapsedT * EARTHQUAKE_MULT
#endif
    ;
    gl_Position.zw = position.zw;
}
#shader fragment
#version 430 core
in vec4 color;
in vec2 Texcoord;
in float textureID;
in float elapsedT;
out vec4 v_Texcoord;
uniform vec4 u_Color;
uniform sampler2D u_Texture[32];
uniform vec2 u_resolution = vec2(1920.0, 1080.0);//TODO: assign on cpu side.
uniform vec2 invResolution = vec2(.0005208f, .0009259f);
//deterministic
float random(vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}
bool started;
float randomOffset;
void Start()
{
    started = true;
    randomOffset = random(gl_FragCoord.xy) + elapsedT * (gl_FragCoord.y + .8f) / u_resolution.y;
}
int textureID_Int;
void main()
{
    if (!started) Start();
//use gl_FragCoord to get the fragment coordinate
    v_Texcoord = texture(u_Texture[textureID_Int = int(textureID)], Texcoord) * color;
    if (bool(v_Texcoord.a)) v_Texcoord.a = float((gl_FragCoord.y * invResolution.y) > (elapsedT * .25f * randomOffset) || !bool(textureID - textureID_Int));
}
//don't add empty line at end of file otherwise it breaks the shader interpretation