#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

in float fsTimer;
in vec3 fsVertexPosition;

uniform vec2 textureSize = vec2(256, 144);
uniform sampler2D texture0;
uniform vec4 colDiffuse;

out vec4 finalColor;

#define PI 3.1415926

float rand(vec2 c){
	return fract(sin(dot(c.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float noise(vec2 p, float freq ){
	float unit = 1280/freq;
	vec2 ij = floor(p/unit);
	vec2 xy = mod(p,unit)/unit;
	//xy = 3.*xy*xy-2.*xy*xy*xy;
	xy = .5*(1.-cos(PI*xy));
	float a = rand((ij+vec2(0.,0.)));
	float b = rand((ij+vec2(1.,0.)));
	float c = rand((ij+vec2(0.,1.)));
	float d = rand((ij+vec2(1.,1.)));
	float x1 = mix(a, b, xy.x);
	float x2 = mix(c, d, xy.x);
	return mix(x1, x2, xy.y);
}

float pNoise(vec2 p, int res){
	float persistance = .5;
	float n = 0.;
	float normK = 0.;
	float f = 4.;
	float amp = 1.;
	int iCount = 0;
	for (int i = 0; i<50; i++){
		n+=amp*noise(p, f);
		f*=2.;
		normK+=amp;
		amp*=persistance;
		if (iCount == res) break;
		iCount++;
	}
	float nf = n/normK;
	return nf*nf*nf*nf;
}

void main() {
    vec4 spriteColor = texture(texture0, fragTexCoord);

    float c = ((cos(fsTimer * 0.5 + fsVertexPosition.x * 0.1)) + 1.0) * -0.002;

    float c2 = (cos(fsTimer * 0.4 + fsVertexPosition.x * 0.1) * 0.5) + 1.0;

    bool isWhite = false;
    vec4 topColor = texture(texture0, fragTexCoord + vec2(0, c));
    isWhite = topColor.a == 0.0;

	if (texture(texture0, fragTexCoord + vec2(0.0, -0.005)).a > 0.0 && c2 > 0.7) {
		if (texture(texture0, fragTexCoord + vec2(0.0, -0.0075)).a == 0.0) {
            isWhite = true;
		}
	}

	if (texture(texture0, fragTexCoord + vec2(0.0, -0.007)).a > 0.0 && c2 > 0.2) {
		if (texture(texture0, fragTexCoord + vec2(0.0, -0.0078)).a == 0.0) {
			isWhite = true;
		}
	}

	if (isWhite) finalColor = vec4(1,1,1,1);
    else finalColor = spriteColor * fragColor;
}
