varying vec3 lightDir,normal,halfVector;
varying vec4 diffuse,ambientGlobal,ambient;
	
void main()
{
  vec4 ecPos;
  vec3 aux;
		
  normal = normalize(gl_NormalMatrix * gl_Normal);
		
  ecPos = gl_ModelViewMatrix * gl_Vertex;

  aux = vec3(gl_LightSource[0].position-ecPos);
  lightDir = normalize(aux);

  vec3 eyeDir = normalize(-ecPos.xyz);
  halfVector = normalize(lightDir + eyeDir);
  //halfVector = normalize(gl_LightSource[0].halfVector.xyz);
		
  diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;	
  ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
  ambientGlobal = gl_LightModel.ambient * gl_FrontMaterial.ambient;

			
  gl_TexCoord[0] = gl_MultiTexCoord0;
  gl_TexCoord[1] = gl_TextureMatrix[0]*ecPos;
  gl_Position = ftransform();
}
