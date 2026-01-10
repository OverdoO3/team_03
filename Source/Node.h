#pragma once

struct Node
{
	int x, z = 0;
	float g, h, f = 0;
	Node* parent = nullptr;
};