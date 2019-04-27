#pragma once
#include "stdafx.h"

/// <summary>
/// Classe représentant un noeud d'un quad tree.
/// </summary>
class QuadTree
{
public:
	QuadTree(void);
	QuadTree(const BYTE *data, unsigned int totalSizeX, unsigned int totalSizeY);
	~QuadTree(void);
	bool isLeaf(void) const;
	bool isEmpty(void) const; 
	BYTE *generateTexture(bool powerOfTwo = true);
	float *generateIndirectionPool(bool powerOfTwo = true, unsigned int maxWidth = 2048);
	unsigned int getNLeaves(void) const;
	unsigned int getSizeU(void) const;
	unsigned int getSizeV(void) const;
	unsigned int getTotalSizeU(void) const;
	unsigned int getTotalSizeV(void) const;
	unsigned int getU(void) const;
	unsigned int getV(void) const;
	double getU0d(void) const;
	double getV0d(void) const;
	double getU1d(void) const;
	double getV1d(void) const;
	double getX0d(void) const;
	double getY0d(void) const;
	double getX1d(void) const;
	double getY1d(void) const;
	unsigned int getX(void) const;
	unsigned int getY(void) const;
	unsigned int getDepth(void) const;	
	const QuadTree *getLeaf(unsigned int i) const;
	unsigned int getIndirectionPoolWidth(void) const;
	unsigned int getIndirectionPoolHeight(void) const;
	static unsigned int nextPowerOfTwo(double n);
	static unsigned int previousPowerOfTwo(double n);

private:
	QuadTree(const BYTE *data, unsigned int totalSizeX, unsigned int totalSizeY, unsigned int sizeX, unsigned int sizeY, unsigned int x, unsigned int y, unsigned int *nLeavesAtDepth, unsigned int depth);
	void initNode();
	void orderLeaves(QuadTree **orderedLeaves);
	void fillIndirectionPool(float *pool, unsigned int width, unsigned int height);
	unsigned int computeIndirectionPoolData(unsigned int maxWidth, unsigned int index = 0);
	unsigned int m_indirectionPoolWidth;
	unsigned int m_indirectionPoolHeight;
	unsigned int m_poolIndexI;
	unsigned int m_poolIndexJ;
	float m_pool[12];
	QuadTree **m_orderedLeaves;
	unsigned int *m_nLeavesAtDepth;
	unsigned int m_nLeaves;
	unsigned int m_depth;
	bool m_isRoot;
	const BYTE *m_data;
	unsigned int m_totalSizeX;
	unsigned int m_totalSizeY;
	unsigned int m_totalSizeU;
	unsigned int m_totalSizeV;
	bool m_isLeaf;
	bool m_isEmpty;
	QuadTree *m_children[4];
	unsigned int m_x;
	unsigned int m_y;
	unsigned int m_u;
	unsigned int m_v;
	unsigned int m_sizeU;
	unsigned int m_sizeV;
};

