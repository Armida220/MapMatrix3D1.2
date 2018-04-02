uniform sampler2D Texture;
uniform float ImageWidth;
// uniform mat4 m;

const vec4 half = vec4(0.5, 0.5, 0.5, 0.0);
const vec4 one = vec4(1.0, 1.0, 1.0, 0.0);

void main(void)
{ 
   	float x, y, k1, k2;
	vec4 TexCoord, TexCoord1;
	// vec4 totalColor = vec4(0.0, 0.0, 0.0, 0.0);
	// vec4 is = vec4(1.0, 0.0, 0.0, 0.0) / ImageWidth;
	// vec4 it = vec4(0.0, 1.0, 0.0, 0.0) / ImageWidth;
	
	// vec4 tmp = (floor(gl_TexCoord[0] * ImageWidth + half) - half);
	// vec4 xy = gl_TexCoord[0] * ImageWidth - tmp;
	
	vec4 xy = gl_TexCoord[0] * ImageWidth;
	vec4 tmp = (floor(xy + half) - half);
	xy -= tmp;
	
	// vec4 xx, yy;
	vec4 aa, bb;
	// , tt;
	
	// vec4 totalColor = one;
	
	// x = gl_TexCoord[0].x*ImageWidth - (floor(gl_TexCoord[0].x*ImageWidth+(0.5) )-0.5);
	// y = gl_TexCoord[0].y*ImageWidth - (floor(gl_TexCoord[0].y*ImageWidth+(0.5) )-0.5);
	x = xy.x;
	y = xy.y;
	
	/*
	xx[3] = 1.0;
	xx[0] = (xx[1] = (xx[2] = x) * x) * x;
	aa = m * xx;
	*/
	
	k1 = x*x;
	k2 = k1*x;
	aa[0] = (-x+k1+k1-k2);
	aa[1] = (1-k1-k1+k2);
	aa[2] = (x+k1-k2);
	aa[3] = (-k1+k2);
	
	/*
	yy[3] = 1.0;
	yy[0] = (yy[1] = (yy[2] = y) * y) * y;
	bb = m * yy;
	*/
	
	k1 = y*y;
	k2 = k1*y;
	bb[0] = (-y+k1+k1-k2);
	bb[1] = (1-k1-k1+k2);
	bb[2] = (y+k1-k2);
	bb[3] = (-k1+k2);  
	
	TexCoord = (tmp - one) / ImageWidth;
	
	// tt = aa * bb[0];
	
	mat4 c;
	mat4 c1;
	
	for (int j = 0; j < 4; j++)
	{
		// TexCoord1 = TexCoord;
		// TexCoord1 = TexCoord + vec4(1.0, 1.0;
		for (int i = 0; i < 4; i++)
		{
			TexCoord1 = TexCoord + vec4(i, j, 0.0, 0.0) / ImageWidth;
			c[i] = texture2D(Texture, TexCoord1);
			// TexCoord1 += is;
		}
		c1[j] = c * aa;
		// TexCoord += it;
	}
	
	// totalColor -= c * aa * bb[3];
	// totalColor -= c1 * bb;

	// gl_FragColor = totalColor;  
	gl_FragColor = c1 * bb;  
}