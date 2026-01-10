#include "Pathfinding.h"
#include "Stage.h"


void Pathfinding::Initialize(Stage* stageMap)
{
	stage = stageMap;
	width = Stage::WIDTH;
	height = Stage::HEIGHT;

	nodes.resize(width * height);

	for (int z = 0; z < height;z++)
	{
		for (int x = 0; x < width; x++)
		{
			Node* n = &nodes[z * width + x];
			n->x = x;
			n->z = z;
			n->parent = nullptr;
		}
	}

	assert(nodes.size() == width * height);
}

std::vector<Node*> Pathfinding::FindPath(int sx, int sz, int gx, int gz)
{
	for (auto& n : nodes)
	{
		n.g = 0;
		n.h = 0;
		n.f = 0;
		n.parent = nullptr;
	}
	std::vector<Node*> open;
	std::vector<Node*> closed;

	Node* start = GetNode(sx, sz);
	Node* goal = GetNode(gx, gz);

	start->g = 0;
	start->h = abs(gx - sx) + abs(gz - sz);
	start->f = start->g + start->h;
	start->parent = nullptr;

	open.push_back(start);

	const int dirX[8] = { 1, -1,  0,  0,  1,  1, -1, -1 };
	const int dirZ[8] = { 0,  0,  1, -1,  1, -1,  1, -1 };

	while (!open.empty())
	{
		Node* current = *std::min_element(
			open.begin(), open.end(),
			[](Node* a, Node* b) { return a->f < b->f; });

		if (current == goal)
			break;
		
		assert(current >= nodes.data());
		assert(current < nodes.data() + nodes.size());

		open.erase(std::find(open.begin(), open.end(), current));
		closed.push_back(current);

		for (int i = 0; i < 8; i++)
		{
			int nx = current->x + dirX[i];
			int nz = current->z + dirZ[i];

			if (!stage->IsWalkable(nx, nz))
				continue;

			Node* neighbor = GetNode(nx, nz);
			if (!neighbor)
				continue;

			if (!CanMoveDiagonal(current, nx, nz))
				continue;

			if (std::find(closed.begin(), closed.end(), neighbor) != closed.end())
				continue;

			float cost = (i < 4) ? 1.0f : 1.41421356f;
			float g = current->g + cost;

			bool inOpen = std::find(open.begin(), open.end(), neighbor) != open.end();
			if (!inOpen || g < neighbor->g)
			{
				neighbor->g = g;
				neighbor->h = Heuristic(nx, nz, gx, gz);
				neighbor->f = neighbor->g + neighbor->h;
				neighbor->parent = current;

				if (!inOpen)
					open.push_back(neighbor);
			}
		}

	}

	// 経路復元
	std::vector<Node*> path;
	Node* p = goal;
	while (p)
	{
		path.push_back(p);
		p = p->parent;
	}
	std::reverse(path.begin(), path.end());
	return path;
}


Node* Pathfinding::GetNode(int x, int z)
{
	//assert(x >= 0 && x < width);
	//assert(z >= 0 && z < height);
	if (x < 0 || x >= width || z < 0 || z >= height)
		return nullptr;
	return &nodes[z * width + x];
}

bool Pathfinding::CanMoveDiagonal(Node* current, int nx, int nz)
{
	int dx = nx - current->x;
	int dz = nz - current->z;

	// 斜めじゃない
	if (abs(dx) + abs(dz) < 2)
		return true;

	// 両脇チェック
	return stage->IsWalkable(current->x + dx, current->z) &&
		stage->IsWalkable(current->x, current->z + dz);
}


