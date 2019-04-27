#include "QuadTree.h"

QuadTree::QuadTree(void)
{
}

/// <summary>
/// Crée un quad tree à partir d'une image en niveaux de gris de couleur de fond noire.
/// </summary>
/// <param name="data">Pointeur vers le contenu de l'image.</param>
/// <param name="totalSizeX">Largeur de l'image.</param>
/// <param name="totalSizeY">Hauteur de l'image.</param>
QuadTree::QuadTree(const BYTE *data, unsigned int totalSizeX, unsigned int totalSizeY)
	: m_isLeaf(true), m_depth(0), m_nLeaves(0), m_isEmpty(true), m_data(data), m_totalSizeX(totalSizeX), m_totalSizeY(totalSizeY), m_x(0), m_y(0), m_sizeU(totalSizeX), m_sizeV(totalSizeY), m_isRoot(true), m_nLeavesAtDepth(NULL), m_orderedLeaves(NULL)
{
	unsigned int n = 1 + (unsigned int)ceil(log((double)min(totalSizeX, totalSizeY)) / log(2.));
	m_nLeavesAtDepth = new unsigned int[n];
	memset(m_nLeavesAtDepth, 0, n * sizeof(unsigned int));	

	initNode();
}

/// <summary>
/// Crée un noeud de quad tree à partir d'une image en niveaux de gris de couleur de fond noire.
/// </summary>
/// <param name="data">Pointeur vers le contenu de l'image.</param>
/// <param name="totalSizeX">Largeur de l'image.</param>
/// <param name="totalSizeY">Hauteur de l'image.</param>
/// <param name="sizeX">Largeur de la portion d'image à traiter (si 0, égale à la largeur totale).</param>
/// <param name="sizeY">Hauteur de la portion d'image à traiter (si 0, égale à la Hauteur totale).</param>
/// <param name="x">Première coordonnée horizontale de la portion d'image à traiter.</param>
/// <param name="y">Première coordonnée verticale de la portion d'image à traiter.</param>
/// <param name="nLeavesAtDepth">Pointeur vers un tableau contenant le nombre de feuilles existant pour chaque profondeur (ignoré si <c>isRoot</c> vaut <c>true</c>).</param>
/// <param name="depth">Profondeur du noeud à créer.</param>
QuadTree::QuadTree(const BYTE *data, unsigned int totalSizeX, unsigned int totalSizeY, unsigned int sizeX, unsigned int sizeY, unsigned int x, unsigned int y, unsigned int *nLeavesAtDepth, unsigned int depth)
	: m_isLeaf(true), m_depth(depth), m_nLeaves(0), m_isEmpty(true), m_data(data), m_totalSizeX(totalSizeX), m_totalSizeY(totalSizeY), m_x(x), m_y(y), m_sizeU(sizeX), m_sizeV(sizeY), m_isRoot(false), m_nLeavesAtDepth(nLeavesAtDepth), m_orderedLeaves(NULL)
{	
	initNode();
}


/// <summary>
/// Initialise un noeud de quad tree.
/// </summary>
void QuadTree::initNode()
{
	for (unsigned int i = 0; i < 4; ++i)
	{
		m_children[i] = NULL;
	}	

	// On parcours la protion d'image associée au noeud :
	//	- Si elle ne contient que du fond, le noeud sera considéré vide.
	//	- Si elle ne contient pas du tout de fond ou si l'une de ses dimensions est strictement inférieure à 2, le noeud est une feuille non vide.
	//	- Si elle contient en partie du fond le noeud est un noeud intermédiaire.
	bool containsEdge = false;
	for (unsigned int i = 0; i < m_sizeU; ++i)
	{
		for (unsigned int j = 0; j < m_sizeV; ++j)
		{
			unsigned int x = m_x + i;
			unsigned int y = m_y + j;
			if (m_isEmpty) m_isEmpty = m_data[packXY(x, y, m_totalSizeX)] == 0;
			if (!m_isEmpty) containsEdge = m_data[packXY(x, y, m_totalSizeX)] == 0;
			if (containsEdge) break;
		}
		if (containsEdge) break;
	}

	if (m_isEmpty) return;

	// Si le noeud est une feuille non vide, le nombre de feuilles est égale à 1, et on incrémente le nombre total de feuilles de même profondeur.
	if (m_sizeU < 2 || m_sizeV < 2 || !containsEdge) 
	{
		m_nLeaves = 1;
		++m_nLeavesAtDepth[m_depth];
		return;
	}

	// Si le noeud n'est pas une feuille, on crée ses quatre fils et on en déduit le nombre de feuilles du noeud.
	m_isLeaf = false;

	unsigned int sizeX0 = m_sizeU - (m_sizeU / 2);
	unsigned int sizeX1 = m_sizeU / 2;
	unsigned int sizeY0 = m_sizeV - (m_sizeV / 2);
	unsigned int sizeY1 = m_sizeV / 2;
	m_children[0] = new QuadTree(m_data, m_totalSizeX, m_totalSizeY, sizeX0, sizeY0, m_x, m_y, m_nLeavesAtDepth, m_depth + 1);
	m_children[1] = new QuadTree(m_data, m_totalSizeX, m_totalSizeY, sizeX1, sizeY0, m_x + sizeX0, m_y, m_nLeavesAtDepth, m_depth + 1);
	m_children[2] = new QuadTree(m_data, m_totalSizeX, m_totalSizeY, sizeX0, sizeY1, m_x, m_y + sizeY0, m_nLeavesAtDepth, m_depth + 1);
	m_children[3] = new QuadTree(m_data, m_totalSizeX, m_totalSizeY, sizeX1, sizeY1, m_x + sizeX0, m_y + sizeY0, m_nLeavesAtDepth, m_depth + 1);

	m_nLeaves = m_children[0]->getNLeaves() + m_children[1]->getNLeaves() + m_children[2]->getNLeaves() + m_children[3]->getNLeaves();

}

QuadTree::~QuadTree(void)
{
	if (!isLeaf())
	{
		delete m_children[0];
		delete m_children[1];
		delete m_children[2];
		delete m_children[3];
	}
	if(m_isRoot) delete[] m_nLeavesAtDepth;
	if (m_orderedLeaves != NULL) delete[] m_orderedLeaves;
}

/// <summary>
/// Indique si un noeud est une feuille.
/// </summary>
/// <returns><c>true</c> si le noeud est une feuille, <c>false</c> sinon.</returns>
bool QuadTree::isLeaf(void) const
{
	return m_isLeaf;
}

/// <summary>
/// Indique si un noeud est une feuille vide (correspondant à une zone de couleur de fond).
/// </summary>
/// <returns><c>true</c> si le noeud est une feuille vide, <c>false</c> sinon.</returns>
bool QuadTree::isEmpty(void) const
{
	return m_isEmpty;
}

/// <summary>
/// Renvoie la taille horizontale de la portion d'image couverte par le noeud.
/// </summary>
/// <returns>Taille horizontale de la portion d'image couverte par le noeud.</returns>
unsigned int QuadTree::getSizeU(void) const
{
	return m_sizeU;
}

/// <summary>
/// Renvoie la taille verticale de la portion d'image couverte par le noeud.
/// </summary>
/// <returns>Taille verticale de la portion d'image couverte par le noeud.</returns>
unsigned int QuadTree::getSizeV(void) const
{
	return m_sizeV;
}

/// <summary>
/// Pour une feuille, renvoie la première coordonée horizontale du patch correspondant dans la texture générée.
/// </summary>
/// <returns>Première coordonée horizontale du patch correspondant dans la texture générée.</returns>
unsigned int QuadTree::getU(void) const
{
	return m_u;
}

/// <summary>
/// Pour une feuille, renvoie la première coordonée verticale du patch correspondant dans la texture générée.
/// </summary>
/// <returns>Première coordonée verticale du patch correspondant dans la texture générée.</returns>
unsigned int QuadTree::getV(void) const
{
	return m_v;
}

/// <summary>
/// Pour une feuille, renvoie la première coordonée horizontale normalisée du patch correspondant dans la texture générée.
/// </summary>
/// <returns>Première coordonée horizontale normalisée du patch correspondant dans la texture générée.</returns>
double QuadTree::getU0d(void) const
{
	return m_u / (double)m_totalSizeU;
}

/// <summary>
/// Pour une feuille, renvoie la première coordonée verticale normalisée du patch correspondant dans la texture générée.
/// </summary>
/// <returns>Première coordonée verticale normalisée du patch correspondant dans la texture générée.</returns>
double QuadTree::getV0d(void) const
{
	return m_v / (double)m_totalSizeV;
}

/// <summary>
/// Pour une feuille, renvoie la dernière coordonée horizontale normalisée du patch correspondant dans la texture générée.
/// </summary>
/// <returns>Dernière coordonée horizontale normalisée du patch correspondant dans la texture générée.</returns>
double QuadTree::getU1d(void) const
{
	return (m_u + m_sizeU) / (double)m_totalSizeU;
}

/// <summary>
/// Pour une feuille, renvoie la dernière coordonée verticale normalisée du patch correspondant dans la texture générée.
/// </summary>
/// <returns>Dernière coordonée verticale normalisée du patch correspondant dans la texture générée.</returns>
double QuadTree::getV1d(void) const
{
	return (m_v + m_sizeV) / (double)m_totalSizeV;
}

/// <summary>
/// Renvoie la première coordonée horizontale normalisée de la portion d'image couverte par le noeud.
/// </summary>
/// <returns>Première coordonée horizontale normalisée de la portion d'image couverte par le noeud.</returns>
double QuadTree::getX0d(void) const
{
	return m_x / (double)m_totalSizeX;
}

/// <summary>
/// Renvoie la première coordonée verticale normalisée de la portion d'image couverte par le noeud.
/// </summary>
/// <returns>Première coordonée verticale normalisée de la portion d'image couverte par le noeud.</returns>
double QuadTree::getY0d(void) const
{
	return m_y / (double)m_totalSizeY;
}

/// <summary>
/// Renvoie la dernière coordonée horizontale normalisée de la portion d'image couverte par le noeud.
/// </summary>
/// <returns>Dernière coordonée horizontale normalisée de la portion d'image couverte par le noeud.</returns>
double QuadTree::getX1d(void) const
{
	return (m_x + m_sizeU) / (double)m_totalSizeX;
}

/// <summary>
/// Renvoie la dernière coordonée verticale normalisée de la portion d'image couverte par le noeud.
/// </summary>
/// <returns>Dernière coordonée verticale normalisée de la portion d'image couverte par le noeud.</returns>
double QuadTree::getY1d(void) const
{
	return (m_y + m_sizeV) / (double)m_totalSizeY;
}

/// <summary>
/// Renvoie la première coordonée horizontale de la portion d'image couverte par le noeud.
/// </summary>
/// <returns>Première coordonée horizontale de la portion d'image couverte par le noeud.</returns>
unsigned int QuadTree::getX(void) const
{
	return m_x;
}

/// <summary>
/// Renvoie la première coordonée verticale de la portion d'image couverte par le noeud.
/// </summary>
/// <returns>Première coordonée verticale de la portion d'image couverte par le noeud.</returns>
unsigned int QuadTree::getY(void) const
{
	return m_y;
}

/// <summary>
/// Renvoie le nombre de feuilles non vides du noeud.
/// </summary>
/// <returns>Nombre de feuilles non vides du noeud.</returns>
unsigned int QuadTree::getNLeaves(void) const
{
	return m_nLeaves;
}

/// <summary>
/// Classe les feuilles non vides par ordre de profondeur (et donc de taille du patch correspondant).
/// </summary>
/// <param name="orderedLeaves">Pointeur vers la portion du tableau (contenant les feuilles classées) dédiée aux feuilles de la profondeur actuelle.</param>
void QuadTree::orderLeaves(QuadTree **orderedLeaves)
{
	// Si le noeud est une feuille non vide, on place un pointeur correspondant dans le premier emplacement disponible.
	if (isLeaf() && !isEmpty())
	{
		unsigned int i = 0; 
		while (orderedLeaves[i] != NULL) 
		{
			++i;
		}
		orderedLeaves[i] = this;
	}
	// Sinon, on classe les feuilles des noeuds enfants.
	else if(!isEmpty())
	{
		for (unsigned int i = 0; i < 4; ++i)
		{
			m_children[i]->orderLeaves(orderedLeaves + m_nLeavesAtDepth[m_depth]);
		}
	}
}

/// <summary>
/// Pour la racine, renvoie une feuille non vide spécifiée par son rang dans le classement selon la taille du patch correspondant.
/// </summary>
/// <param name="i">Rang de la feuille.</param>
/// <returns>Pointeur vers la feuille.</returns>
const QuadTree *QuadTree::getLeaf(unsigned int i) const
{
	return m_orderedLeaves[i];
}

/// <summary>
/// Renvoie la profondeur du noeud.
/// </summary>
/// <returns>profondeur du noeud.</returns>
unsigned int QuadTree::getDepth(void) const
{
	return m_depth;
}

/// <summary>
/// Pour la racine, génère la texture contenant les patches correspondant aux feuilles.
/// </summary>
/// <param name="powerOfTwo">Spécifie si les dimensions de la texture générée doivent être des puissances entières de 2.</param>
/// <returns>Pointeur vers les données de la texture générée.</returns>
BYTE *QuadTree::generateTexture(bool powerOfTwo)
{	
	// On initialise la liste des feuilles non vides classées avec des pointeurs nuls, puis on classe les feuilles non vides.
	m_orderedLeaves = new QuadTree*[getNLeaves()];
	memset(m_orderedLeaves, NULL, getNLeaves() * sizeof(QuadTree*));

	orderLeaves(m_orderedLeaves);

	// On attribue ensuite à chaque feuille non vide une position dans la texture générée.
	// On commence par placer le patch le plus grand. Celui-ci définit la taille verticale de la texture.

	m_totalSizeU = 0;
	m_totalSizeV = m_orderedLeaves[0]->getSizeV();
	m_orderedLeaves[0]->m_u = 0;
	m_orderedLeaves[0]->m_v = 0;

	// On empile ensuite les patchs de le plus à gauche possible jusqu'à ce que l'on atteigne la taille verticale maximale.
	// Pour celà, On définit les tableaux stack, leftU et la variable leftV de sorte que :
	//	- le dernier élement de stack contiennne le premier patch de la pile actuelle,
	//  - le dernier élement de leftU contienne l'espace disponible à droite du premier bloc de la pile actuelle (-1 désignant l'infini),
	//  - leftV représente l'espace disponible en dessous de la pile actuelle.
	QuadTree **stack = new QuadTree*[getNLeaves()];
	memset(stack, NULL, getNLeaves() * sizeof(QuadTree*));
	unsigned int stackSize = 0;

	int *leftU = new int[getNLeaves()];
	unsigned int leftV = 0;

	stack[stackSize++] = m_orderedLeaves[0];
	leftU[0] = -1;

	// Pour chaque feuille :
	for (unsigned int i = 1; i < getNLeaves(); ++i)
	{
		QuadTree *leaf = m_orderedLeaves[i];
		// s'il ne reste pas suffisament d'espace en dessous de la pile actuelle,
		if (leftV < m_orderedLeaves[i]->getSizeV())
		{
			// on examine l'espace disponible à droite des piles précédentes jusqu'à en trouver suffisamment, où on  démarre une nouvelle pile;
			while (leftU[stackSize - 1] != -1 && leftU[stackSize - 1] < m_orderedLeaves[i]->getSizeU())
			{
				stackSize--;
			}
			if (leftU[stackSize - 1] == -1)
			{
				m_orderedLeaves[i]->m_u = stack[0]->getU() + stack[0]->getSizeU();
				m_orderedLeaves[i]->m_v = 0;
				stack[0] = m_orderedLeaves[i];
			}
			else if(leftU[stackSize - 1] >= m_orderedLeaves[i]->getSizeU())
			{
				m_orderedLeaves[i]->m_u = stack[stackSize - 1]->getU() + stack[stackSize - 1]->getSizeU();
				m_orderedLeaves[i]->m_v = stack[stackSize - 1]->getV();
				stack[stackSize - 1] = m_orderedLeaves[i];
				leftU[stackSize - 1] -= m_orderedLeaves[i]->getSizeU();
			}
		}
		// s'il en reste suffisament, 
		else
		{
			// on place lep atch actuel en dessous de la pile,
			m_orderedLeaves[i]->m_u = m_orderedLeaves[i - 1]->getU();
			m_orderedLeaves[i]->m_v = m_orderedLeaves[i - 1]->getV() + m_orderedLeaves[i - 1]->getSizeV();
			// si le patch actuel n'a pas la même taille que le précédent, il commence une nouvelle pile.
			if (m_orderedLeaves[i]->getDepth() != m_orderedLeaves[i - 1]->getDepth())
			{
				stack[stackSize++] = m_orderedLeaves[i];
				leftU[stackSize - 1] = m_orderedLeaves[i - 1]->getSizeU() - m_orderedLeaves[i]->getSizeU();
			}
		}
		// On enregistre la nouvelle largeur de la texture.
		m_totalSizeU = max(m_totalSizeU, m_orderedLeaves[i]->getU() + m_orderedLeaves[i]->getSizeU());
		leftV = m_totalSizeV - (m_orderedLeaves[i]->getV() + m_orderedLeaves[i]->getSizeV());
	}

	// Si besoin, on augmente les dimensions de la texture aux puissances de deux supérieures.
	if (powerOfTwo)
	{
		m_totalSizeU = nextPowerOfTwo(m_totalSizeU);
		m_totalSizeV = nextPowerOfTwo(m_totalSizeV);
	}

	// On aloue l'espace pour stocker la texture.
	BYTE *texture = new BYTE[m_totalSizeU * m_totalSizeV];
	memset(texture, 0, m_totalSizeU * m_totalSizeV);

	// Pour chaque feuille non vide, on copie le contenu du patch dans la texture à l'emplacement précedemment déterminé.
	for (unsigned int n = 0; n < getNLeaves(); ++n)
	{
		m_orderedLeaves[n]->m_totalSizeU = m_totalSizeU;
		m_orderedLeaves[n]->m_totalSizeV = m_totalSizeV;
		for (unsigned int i = 0; i < m_orderedLeaves[n]->getSizeU(); ++i)
			for (unsigned int j = 0; j < m_orderedLeaves[n]->getSizeV(); ++j)
			{
				unsigned int u = m_orderedLeaves[n]->getU() + i;
				unsigned int v = m_orderedLeaves[n]->getV() + j;
				unsigned int x = m_orderedLeaves[n]->getX() + i;
				unsigned int y = m_orderedLeaves[n]->getY() + j;
				texture[packXY(u, v, m_totalSizeU)] = m_data[packXY(x, y, m_totalSizeX)];
			}
	}

	delete[] leftU;
	delete[] stack;
	return texture;
}

/// <summary>
/// Renvoie la taille horizontale totale de la texture générée.
/// </summary>
/// <returns>Taille horizontale totale de la texture générée.</returns>
unsigned int QuadTree::getTotalSizeU(void) const
{
	return m_totalSizeU;
}

/// <summary>
/// Renvoie la taille verticale totale de la texture générée.
/// </summary>
/// <returns>Taille verticale totale de la texture générée.</returns>
unsigned int QuadTree::getTotalSizeV(void) const
{
	return m_totalSizeV;
}

/// <summary>
/// Pour la racine, génère l'indirection pool représentant l'arbre.
/// </summary>
/// <param name="powerOfTwo">Spécifie si les dimensions de l'indirection pool doivent être des puissances entières de 2.</param>
/// <param name="maxWidth">Largeur maximale de l'indirection pool.</param>
/// <returns>Pointeur vers les données de l'indirection pool.</returns>
float *QuadTree::generateIndirectionPool(bool powerOfTwo, unsigned int maxWidth)
{
	// Si besoin, on diminue la largeur maximale à la puissance de 2 inférieure.
	if (powerOfTwo) maxWidth = previousPowerOfTwo(maxWidth);

	// On calcul le contenu de l'indirection pool codant l'arbre.
	unsigned int nPools = computeIndirectionPoolData(maxWidth);
	m_indirectionPoolWidth = min(2 * nPools, maxWidth);
	m_indirectionPoolHeight = 2 * ((2 * nPools) / maxWidth + 1);

	// Si besoin, on augmente les dimensions à la puissance de 2 supérieure.
	if (powerOfTwo)
	{
		m_indirectionPoolWidth = nextPowerOfTwo(m_indirectionPoolWidth);
		m_indirectionPoolHeight = nextPowerOfTwo(m_indirectionPoolHeight);
	}

	// On aloue l'espace pour stocker l'indirection pool puis on la remplie.
	float *pool = new float[m_indirectionPoolWidth * m_indirectionPoolHeight * 4];
	memset(pool, 0, m_indirectionPoolWidth * m_indirectionPoolHeight * 4 * sizeof(float));
	fillIndirectionPool(pool, m_indirectionPoolWidth, m_indirectionPoolHeight);
	return pool;
}

/// <summary>
/// Copie l'indirection pool locale du noeud vers l'indirection pool représentant l'arbre entier.
/// </summary>
/// <param name="pool">Pointeur vers les données de l'indirection pool globale.</param>
/// <param name="width">Largeur de l'indirection pool globale.</param>
/// <param name="height">Hauteur de l'indirection pool globale.</param>
void QuadTree::fillIndirectionPool(float *pool, unsigned int width, unsigned int height)
{
	// Afin de représenter l'indirection pool comme une texture en RGBA, on ajoute 0 en quatrième coordonée à chaque case.
	for (unsigned int i = 0; i < 4; ++i)
	{
		pool[packXYZ(0, m_poolIndexI + xFromXY(i, 2), m_poolIndexJ + yFromXY(i, 2), 4, width)] = m_pool[packXYZ(xFromXY(i, 2), yFromXY(i, 2), 0, 2, 2)];
		pool[packXYZ(1, m_poolIndexI + xFromXY(i, 2), m_poolIndexJ + yFromXY(i, 2), 4, width)] = m_pool[packXYZ(xFromXY(i, 2), yFromXY(i, 2), 1, 2, 2)];
		pool[packXYZ(2, m_poolIndexI + xFromXY(i, 2), m_poolIndexJ + yFromXY(i, 2), 4, width)] = m_pool[packXYZ(xFromXY(i, 2), yFromXY(i, 2), 2, 2, 2)];
		pool[packXYZ(3, m_poolIndexI + xFromXY(i, 2), m_poolIndexJ + yFromXY(i, 2), 4, width)] = 0.f;
		
		if (!m_children[i]->isEmpty() && !m_children[i]->isLeaf())
		{
			// Pour une grid représentant une feuille non vide, les seconde et troisième valeurs sont les coordonnées de l'indirection pool locale de la feuille et doivent être normalisées.
			pool[packXYZ(1, m_poolIndexI + xFromXY(i, 2), m_poolIndexJ + yFromXY(i, 2), 4, width)] /= width;
			pool[packXYZ(2, m_poolIndexI + xFromXY(i, 2), m_poolIndexJ + yFromXY(i, 2), 4, width)] /= height;
		}
	}
	for (unsigned int i = 0; i < 4; ++i)
	{
		if (!m_children[i]->isLeaf()) m_children[i]->fillIndirectionPool(pool, width, height);
	}
}

/// <summary>
/// Calcul l'indirection pool du noeud.
/// </summary>
/// <param name="maxWidth">Largeur maximale de l'indirection pool globale.</param>
/// <param name="index">Nombre de noeuds ayant été déja traîtés.</param>
/// <returns>Nouveau nombre de noeuds ayant été traîtés.</returns>
unsigned int QuadTree::computeIndirectionPoolData(unsigned int maxWidth, unsigned int index)
{
	// On détermine les coordonnées dans l'indirection pool globale de l'indirection pool locale au noeud.
	m_poolIndexI = 2 * xFromXY(index, maxWidth / 2);
	m_poolIndexJ = 2 * yFromXY(index, maxWidth / 2);	
	++index;

	// On remplie les 4 cases de l'indirection pool locale comme suit.
	for (unsigned int i = 0; i < 4; ++i)
	{
		// Si le fils est une feuille vide, les 3 coordonnées sont nulles.
		if (m_children[i]->isEmpty())
		{
			m_pool[packXYZ(xFromXY(i, 2), yFromXY(i, 2), 0, 2, 2)] = 0.f;
			m_pool[packXYZ(xFromXY(i, 2), yFromXY(i, 2), 1, 2, 2)] = 0.f;
			m_pool[packXYZ(xFromXY(i, 2), yFromXY(i, 2), 2, 2, 2)] = 0.f;		
		}
		// Si le fils est une feuille non vide, la première coordonnée est 1 et les suivantes sont les coodronnées normalisées du patch dans la texture.
		else if (m_children[i]->isLeaf())
		{
			m_pool[packXYZ(xFromXY(i, 2), yFromXY(i, 2), 0, 2, 2)] = 1.f;
			m_pool[packXYZ(xFromXY(i, 2), yFromXY(i, 2), 1, 2, 2)] = (float)m_children[i]->getU0d();
			m_pool[packXYZ(xFromXY(i, 2), yFromXY(i, 2), 2, 2, 2)] = (float)m_children[i]->getV0d();
		}
		// Si le fils n'est pas une feuille, la première coordonnée est 0.5 et les suivantes sont les coordonnées de l'indirection pool du fils dans l'indirection pool globale.
		else
		{
			m_pool[packXYZ(xFromXY(i, 2), yFromXY(i, 2), 0, 2, 2)] = .5f;
			m_pool[packXYZ(xFromXY(i, 2), yFromXY(i, 2), 1, 2, 2)] = (float)(2 * xFromXY(index, maxWidth / 2));
			m_pool[packXYZ(xFromXY(i, 2), yFromXY(i, 2), 2, 2, 2)] = (float)(2 * yFromXY(index, maxWidth / 2));
			index = m_children[i]->computeIndirectionPoolData(maxWidth, index);
		}
	}
	return index;
}

/// <summary>
/// Pour la racine, renvoie la largeur de l'indirection pool.
/// </summary>
/// <returns>Largeur de l'indirection pool.</returns>
unsigned int QuadTree::getIndirectionPoolWidth(void) const
{
	return m_indirectionPoolWidth;
}

/// <summary>
/// Pour la racine, renvoie la hauteur de l'indirection pool.
/// </summary>
/// <returns>Hauteur de l'indirection pool.</returns>
unsigned int QuadTree::getIndirectionPoolHeight(void) const
{
	return m_indirectionPoolHeight;
}

/// <summary>
/// Calcul la plus grande puissance entière de 2 inférieure ou égale à un nombre.
/// </summary>
/// <param name="n">Nombre en question.</param>
/// <returns>Plus grande puissance entière de 2 inférieure ou égale à <c>n</c>.</returns>
unsigned int QuadTree::previousPowerOfTwo(double n)
{
	return (unsigned int)pow(2., (int)(log(n)/log(2.)));
}

/// <summary>
/// Calcul la plus petite puissance entière de 2 supérieure ou égale à un nombre.
/// </summary>
/// <param name="n">Nombre en question.</param>
/// <returns>Plus petite puissance entière de 2 supérieure ou égale à <c>n</c>.</returns>
unsigned int QuadTree::nextPowerOfTwo(double n)
{
	unsigned int newN = previousPowerOfTwo(n);
	return (newN >= n) ? newN : (2 * newN);
}