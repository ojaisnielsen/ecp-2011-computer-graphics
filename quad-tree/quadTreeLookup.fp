// On définit les macros permettant de passer aisément entre coordonnées normalisées et non normalisées.
#define normIndexI(i) (float(i) / float(indirectionPoolWidth)) 
#define normIndexJ(j) (float(j) / float(indirectionPoolHeight)) 
#define unNormIndexI(i) round(i * float(indirectionPoolWidth))
#define unNormIndexJ(j) round(j * float(indirectionPoolHeight))
#define unNormImU(u) round(u * float(imageWidth))
#define unNormImV(v) round(v * float(imageHeight))
#define normTexU(u) (float(u) / float(textureWidth)) 
#define normTexV(v) (float(v) / float(textureHeight))
#define unNormTexU(u) round(u * float(textureWidth))
#define unNormTexV(v) round(v * float(textureHeight))
#define round(x) ((x - floor(x) < 0.5) ? int(x) : (int(x) + 1))


uniform sampler2D u_texture;
uniform sampler2D u_indirectionPool;
	
void main()
{

	float fracU = gl_TexCoord[0].s;
	float fracV = gl_TexCoord[0].t;

	int dataType = 0; // 0 -> 0 ; 1 -> next ; 2 -> texture
	float data0 = 0.;
	float data1 = 0.;
	int i, j, indexI, indexJ;
	float scale = 1.;
	vec4 indirectionPoolLookup;

	// Dans toute la suite :
	//  - fracU et fracV représentent les coordonnées normalisées du point courant par rapport au noeud courant.
	//  - scale représente l'échelle de fracU et fracV par rapport aux coordonnées normalisées initiales.
	//  - i et j représentent les coordonnées de la grid correspondant au point courant dans l'indirection pool locale du noeud courant.
	//  - si le noeud courant est une feuille vide :
	//     - dataType vaut 0
	//     - data0 et data1 vallent 0
	//  - si le noeud courant est une feuille non vide :
	//     - dataType vaut 2
	//     - data0 et data1 sont les coordonnées normalisées du patch correspondant au noeud courant dans la texture
	//	- si le noeud courant n'est pas une feuille :
	//     - dataType vaut 1
	//     - data0 et data1 sont les coordonnées normalisées du noeud suivant.

	// On parcours l'arbre tant que l'on n'atteint pas une feuille.
	do
	{
		i = (fracU > 0.5) ? 1 : 0;
		j = (fracV > 0.5) ? 1 : 0;
		indexI = unNormIndexI(data0);
		indexJ = unNormIndexJ(data1);
		indirectionPoolLookup = texture2D(u_indirectionPool, vec2(normIndexI(indexI + i), normIndexJ(indexJ + j)));
		dataType = round(2. * indirectionPoolLookup.r);
		data0 = indirectionPoolLookup.g;
		data1 = indirectionPoolLookup.b;

		fracU = (i > 0) ? (2. * fracU - 1.) : (2. * fracU);
		fracV = (j > 0) ? (2. * fracV - 1.) : (2. * fracV);

		scale /= 2.;
	}
	while (dataType != 2 && dataType != 0);

	// Lorsqu'une feuille est atteinte, si elle est vide, la couleur du pixel est noire, sinon, on récupère la valeur du pixel dans la texture.
	vec4 pixel = vec4(0, 0, 0, 0);
	if (dataType == 2) 
	{
		int u = unNormImU(fracU * scale);
		int v = unNormImV(fracV * scale);
		int du = unNormTexU(data0);
		int dv = unNormTexV(data1);
		pixel = texture2D(u_texture, vec2(normTexU(u + du), normTexV(v + dv)));
	}
	gl_FragColor = pixel;
}
