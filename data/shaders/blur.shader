#define MODIFY_BASE_PRE
uniform lowp vec4 Value;
lowp vec4 modify_base_pre(lowp vec4 base)
{
	//vec2 uv = fragCoord.xy / Value.yz;
	//uv.y = 1.0-uv.y;
   
	float max_siz = 32.0 * Value.x;
    
	return base;
	//return texture( iChannel0, uv, log2(max_siz) );
}