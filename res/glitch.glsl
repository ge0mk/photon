#if defined(VS_BUILD)
	in vec4 position;
	void main(){
		gl_Position  = position;
	}
#elif defined(FS_BUILD)
	uniform vec2 resolution;
	uniform sampler2D source;
	uniform float time;
	uniform float glitchamnt;
	layout(location = 0) out vec4 fragcolor;

	float rand(vec2 co){
	    return fract(cos(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
	}
	vec3 mod289(vec3 x) {
	    return x - floor(x * (1.0 / 289.0)) * 289.0;
	}
	vec4 mod289(vec4 x) {
	    return x - floor(x * (1.0 / 289.0)) * 289.0;
	}
	vec4 permute(vec4 x) {
	    return mod289(((x*34.0)+1.0)*x);
	}
	vec4 taylorInvSqrt(vec4 r)
	{
	    return 1.79284291400159 - 0.85373472095314 * r;
	}
	float snoise3(vec3 v)
	{
	    const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
	    const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

	    // First corner
	    vec3 i  = floor(v + dot(v, C.yyy) );
	    vec3 x0 =   v - i + dot(i, C.xxx) ;

	    // Other corners
	    vec3 g = step(x0.yzx, x0.xyz);
	    vec3 l = 1.0 - g;
	    vec3 i1 = min( g.xyz, l.zxy );
	    vec3 i2 = max( g.xyz, l.zxy );

	    vec3 x1 = x0 - i1 + C.xxx;
	    vec3 x2 = x0 - i2 + C.yyy;
	    vec3 x3 = x0 - D.yyy;

	    // Permutations
	    i = mod289(i);
	    vec4 p = permute( permute( permute(
	                i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
	                + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
	                + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

	    float n_ = 0.142857142857; // 1.0/7.0
	    vec3  ns = n_ * D.wyz - D.xzx;

	    vec4 j = p - 49.0 * floor(p * ns.z * ns.z);

	    vec4 x_ = floor(j * ns.z);
	    vec4 y_ = floor(j - 7.0 * x_ );

	    vec4 x = x_ *ns.x + ns.yyyy;
	    vec4 y = y_ *ns.x + ns.yyyy;
	    vec4 h = 1.0 - abs(x) - abs(y);

	    vec4 b0 = vec4( x.xy, y.xy );
	    vec4 b1 = vec4( x.zw, y.zw );

	    vec4 s0 = floor(b0)*2.0 + 1.0;
	    vec4 s1 = floor(b1)*2.0 + 1.0;
	    vec4 sh = -step(h, vec4(0.0));

	    vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
	    vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

	    vec3 p0 = vec3(a0.xy,h.x);
	    vec3 p1 = vec3(a0.zw,h.y);
	    vec3 p2 = vec3(a1.xy,h.z);
	    vec3 p3 = vec3(a1.zw,h.w);

	    vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
	    p0 *= norm.x;
	    p1 *= norm.y;
	    p2 *= norm.z;
	    p3 *= norm.w;

	    vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
	    m = m * m;
	    return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),
	                                    dot(p2,x2), dot(p3,x3) ) );
	}

	const float interval = 3.0;

	vec4 glitch1(ivec2 iResolution, float iGlobalTime, vec2 vertex){
		vec3 uv = vec3(0.0);
		vec2 uv2 = vec2(0.0);
		vec2 nuv = gl_FragCoord.xy / iResolution.xy;
		vec3 texColor = vec3(0.0);

		if (rand(vec2(iGlobalTime)) < 0.7){
			texColor = texture2D(source, vertex.st).rgb;
		}
		else{
			texColor = texture2D(source, nuv * vec2(rand(vec2(iGlobalTime)), rand(vec2(iGlobalTime * 0.99)))).rgb;
		}

		float r = rand(vec2(iGlobalTime * 0.001));
		float r2 = rand(vec2(iGlobalTime * 0.1));
		if (nuv.y > rand(vec2(r2)) && nuv.y < r2 + rand(vec2(0.05 * iGlobalTime))){
			if (r < rand(vec2(iGlobalTime * 0.01))){
				if ((texColor.b + texColor.g + texColor.b)/3.0 < r * rand(vec2(0.4, 0.5)) * 2.0){
					uv.r -= sin(nuv.x * r * 0.1 * iGlobalTime ) * r * 7000.;
					uv.g += sin(vertex.y * vertex.x/2 * 0.006 * iGlobalTime) * r * 10 *rand(vec2(iGlobalTime * 0.1)) ;
					uv.b -= sin(nuv.y * nuv.x * 0.5 * iGlobalTime) * sin(nuv.y * nuv.x * 0.1) * r *  20. ;
					uv2 += vec2(sin(nuv.x * r * 0.1 * iGlobalTime) * r );
				}
			}
		}

		texColor = texture2D(source, vertex.st + uv2).rgb;
		texColor += uv;
		return vec4(texColor, 1.0);
	}

	vec4 glitch2(ivec2 iResolution, float iGlobalTime, vec2 vertex){
		float strength = smoothstep(interval * 0.5, interval, interval - mod(time, interval));
	    vec2 shake = vec2(strength * 8.0 + 0.5) * vec2(
	        rand(vec2(time)) * 2.0 - 1.0,
	        rand(vec2(time * 2.0)) * 2.0 - 1.0
	    ) / resolution;

	    float y = vertex.y * resolution.y;
	    float rgbWave = (
	        snoise3(vec3(0.0, y * 0.01, time * 400.0)) * (2.0 + strength * 32.0)
	        * snoise3(vec3(0.0, y * 0.02, time * 200.0)) * (1.0 + strength * 4.0)
	        + step(0.9995, sin(y * 0.005 + time * 1.6)) * 12.0
	        + step(0.9999, sin(y * 0.005 + time * 2.0)) * -18.0
	        ) / resolution.x;
	    float rgbDiff = (6.0 + sin(time * 500.0 + vertex.y * 40.0) * (20.0 * strength + 1.0)) / resolution.x;
	    float rgbUvX = vertex.x + rgbWave;
	    float r = texture2D(source, vec2(rgbUvX + rgbDiff, vertex.y) + shake).r;
	    float g = texture2D(source, vec2(rgbUvX, vertex.y) + shake).g;
	    float b = texture2D(source, vec2(rgbUvX - rgbDiff, vertex.y) + shake).b;

	    float whiteNoise = (rand(vertex + mod(time, 10.0)) * 2.0 - 1.0) * (0.15 + strength * 0.15);

	    float bnTime = floor(time * 20.0) * 200.0;
	    float noiseX = step((snoise3(vec3(0.0, vertex.x * 3.0, bnTime)) + 1.0) / 2.0, 0.12 + strength * 0.3);
	    float noiseY = step((snoise3(vec3(0.0, vertex.y * 3.0, bnTime)) + 1.0) / 2.0, 0.12 + strength * 0.3);
	    float bnMask = noiseX * noiseY;
	    float bnUvX = vertex.x + sin(bnTime) * 0.2 + rgbWave;
	    float bnR = texture2D(source, vec2(bnUvX + rgbDiff, vertex.y)).r * bnMask;
	    float bnG = texture2D(source, vec2(bnUvX, vertex.y)).g * bnMask;
	    float bnB = texture2D(source, vec2(bnUvX - rgbDiff, vertex.y)).b * bnMask;
	    vec4 blockNoise = vec4(bnR, bnG, bnB, 1.0);

	    float bnTime2 = floor(time * 25.0) * 300.0;
	    float noiseX2 = step((snoise3(vec3(0.0, vertex.x * 2.0, bnTime2)) + 1.0) / 2.0, 0.12 + strength * 0.5);
	    float noiseY2 = step((snoise3(vec3(0.0, vertex.y * 8.0, bnTime2)) + 1.0) / 2.0, 0.12 + strength * 0.3);
	    float bnMask2 = noiseX2 * noiseY2;
	    float bnR2 = texture2D(source, vec2(bnUvX + rgbDiff, vertex.y)).r * bnMask2;
	    float bnG2 = texture2D(source, vec2(bnUvX, vertex.y)).g * bnMask2;
	    float bnB2 = texture2D(source, vec2(bnUvX - rgbDiff, vertex.y)).b * bnMask2;
	    vec4 blockNoise2 = vec4(bnR2, bnG2, bnB2, 1.0);

	    float waveNoise = (sin(vertex.y * 1200.0) + 1.0) / 2.0 * (0.15 + strength * 0.2);

	    return vec4(r, g, b, 1.0) * (1.0 - bnMask - bnMask2) + (whiteNoise + blockNoise + blockNoise2 - waveNoise);
	}

	void main()
	{
		ivec2 iResolution = ivec2(resolution);
		float iGlobalTime = time;

		vec4 raw = vec4(texture(source, (gl_FragCoord.xy)/resolution).rgb, 1);
		vec4 g1 = glitch1(ivec2(resolution), time, gl_FragCoord.xy/resolution);
		vec4 g2 = glitch2(ivec2(resolution), time, gl_FragCoord.xy/resolution);
		fragcolor = mix(raw, g2, glitchamnt*0.9);//, g2, glitchamnt*0.9);
	}
#endif