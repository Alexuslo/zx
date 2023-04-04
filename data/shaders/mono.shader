#define MODIFY_BASE_PRE
uniform lowp vec4 Value;
lowp vec4 modify_base_pre(lowp vec4 base)
{
	lowp float c = (base.r + base.g + base.b) / 3.0;
	return mix(vec4(c, c, c, base.a), base, 1.0 - Value.r);
}