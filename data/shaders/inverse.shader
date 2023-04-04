#define MODIFY_BASE_PRE
uniform lowp vec4 Value;
lowp vec4 modify_base_pre(lowp vec4 base)
{
	lowp vec4 inverted = vec4(1.0, 1.0, 1.0, 1.0) - base;
	lowp vec4 res = mix(inverted * base.a, base, 1.0 - Value.r);
	return vec4(res.rgb, base.a);
}