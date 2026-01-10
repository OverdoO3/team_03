#pragma once
#include <vector>
#include "Node.h"
#include <algorithm>
#include <cmath>

class Stage;

class Pathfinding
{
public:
	void Initialize(Stage* stageMap);
	std::vector<Node*> FindPath(int startX, int startZ, int goalX, int goalZ);

private:
	Stage* stage;
	std::vector<Node> nodes;
	Node* GetNode(int x, int z);
	bool CanMoveDiagonal(Node* current, int nx, int nz);

	float Heuristic(int x1, int z1, int x2, int z2)
	{
		int dx = abs(x1 - x2);
		int dz = abs(z1 - z2);
		return (dx + dz) + (1.41421356f - 2.0f) * std::min<int>(dx, dz);
	}

	int width;
	int height;
};