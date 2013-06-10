varying vec4 diffuse,ambientGlobal,ambient;
varying vec3 normal,lightDir,halfVector;
uniform sampler2D tex;
uniform sampler2DShadow shadowMap;
uniform bool textureEnable;

void main()
{ vec3 n,halfV;
  float NdotL,NdotHV;
  // if (texel.a<0.2) discard;
  float shade = shadow2DProj(shadowMap, gl_TexCoord[1]).r;
  shade = clamp(shade, 0.0, 1.0);
  vec4 texel;
  if (textureEnable) {
  	texel=diffuse*texture2D(tex,gl_TexCoord[0].st); 
  } else {
    	texel=diffuse;
  } 
  vec4 color = ambientGlobal + ambient;
  vec3 D = normalize(gl_LightSource[0].spotDirection);
  vec3 L = normalize(lightDir);
  
  if (dot(-L, D) > gl_LightSource[0].spotCosCutoff )
  { n = normalize(normal);
    NdotL = max(dot(n,L),0.0);
	color += shade*(texel * NdotL);
	halfV = normalize(halfVector);
	NdotHV = max(dot(n,halfV),0.0);
	color += shade*gl_FrontMaterial.specular * gl_LightSource[0].specular * 
	pow(NdotHV,gl_FrontMaterial.shininess);
  }	
  gl_FragColor = vec4(color.rgb,1.0);	
}
