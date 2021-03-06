#define PATH "../Resource/Textures/"
#define SCALE_X 1
#define SCALE_Z -1
#define OFFSET_X 9 * SCALE_X
#define OFFSET_Z -9 * SCALE_Z
#define BLOCKADE 0
#define AVOID 128

#include "NavMesh.h"
#include <string>
#include <math.h>
#include <assert.h>

using namespace DirectX::SimpleMath;

void NavMesh::copy(NavMesh const &navMesh) {
	if (&navMesh != this) {
		deleteMemory();
		
		mSurface = navMesh.mSurface;
		mCopy = nullptr;
		assert(!mSurface); //for testing
		indexArray = new UINT8[getWidth() * getHeight()];

		for (int x = 0; x < getWidth(); x++) {
			for (int y = 0; y < getHeight(); y++) {
				indexArray[x + y * getWidth()] =
					navMesh.indexArray[x + y * getWidth()];
			}
		}
	}
}

void NavMesh::deleteMemory() {
	if (mSurface)
		SDL_FreeSurface(mSurface);
	if (mCopy)
		SDL_FreeSurface(mCopy);
	if (mCurrentThread) {
		mCurrentThread->join();
		delete mCurrentThread;
	}
}

NavMesh::NavMesh() {
	mCurrentThread = nullptr;
	mSurface = nullptr;
	indexArray = nullptr;
}

NavMesh::~NavMesh() {
	deleteMemory();
}

NavMesh::NavMesh(NavMesh const &navMesh) {
	copy(navMesh);
}

NavMesh* NavMesh::operator=(NavMesh const &navMesh) {
	copy(navMesh);
	return this;
}

void NavMesh::loadGrid(const char *gridName) {
	if (mCurrentThread) {
		mCurrentThread->join();
		delete mCurrentThread;
		mCurrentThread = nullptr;
	}

	std::string str(PATH);
	str += gridName;

	if (mSurface) SDL_FreeSurface(mSurface);
	mSurface = SDL_LoadBMP(str.c_str());
	indexArray = (UINT8*) mSurface->pixels;
}

SDL_Color NavMesh::getPixel(int x, int y) const {
	assert(mSurface);
	return mSurface->format->palette->colors
		[indexArray[x + y * getWidth()]]; // 2d to 1d
}

SDL_Color NavMesh::getPixelAtCoord(int x, int z) const {
	Vector2 pixelCoord = toPixelCoord(x, z);
	int pX = static_cast<int>(pixelCoord.x), 
		pY = static_cast<int>(pixelCoord.y);
	assert(mSurface);
	return mSurface->format->palette->colors
		[indexArray[pX + pY * getWidth()]];
}

Vector2 NavMesh::toPixelCoord(int x, int z) const {
	int pX = floor(x * SCALE_X) + OFFSET_X;
	int pY = -(floor(z * SCALE_Z) + OFFSET_Z);

	pX %= getWidth();
	pY %= getHeight();

	return Vector2(abs(pX), abs(pY));
}

bool NavMesh::canSeeFrom(int fromX, int fromZ, int toX, int toZ) const {
	Vector2 pixelCoordFrom = toPixelCoord(fromX, fromZ);
	Vector2 pixelCoordTo = toPixelCoord(toX, toZ);
	int x = static_cast<int> (pixelCoordFrom.x);
	int y = static_cast<int> (pixelCoordFrom.y);
	int pTX = static_cast<int> (pixelCoordTo.x);
	int pTY = static_cast<int> (pixelCoordTo.y);

	// Walks from x y to toX toY, if any pixels in the way is colliders, x y cant see toX toY
	while (true) { //WARNING: DANGEROUS CODE, CHANGE IT
		if (x != pTX) {
			x += x < pTX ? 1 : -1;
			if (x != pTX && getPixel(x, y).r == BLOCKADE) return false;
		}

		if (y != pTY) {
			y += y < pTY ? 1 : -1;
			if (y != pTY && getPixel(x, y).r == BLOCKADE) return false;
		}

		if (x == pTX && y == pTY) return true;
	}
}

void NavMesh::loadPathToCoordThread(Enemy *enemy, int fromX, int fromZ,
	int toX, int toZ) {
	//A Star algorithm ! THIS IS CURRENTLY VERY UNOPTIMIZED !
	enemy->setPaused(true);

	Vector2 toPos = toPixelCoord(toX, toZ);
	std::vector<Vector3> path;
	std::vector<Node> openList;
	std::unordered_map<int, Node> closedList;
	openList.push_back({ toPixelCoord(fromX, fromZ), 0, 0, 0, 0 });
	std::make_heap(openList.begin(), openList.end()); //use stl heap thingy, can make my own heap class if neccesary

	if (toPos == openList[0].node) { // if at same node
		path.push_back(Vector3(toX, 0, toZ));
		enemy->setPath(path);
		enemy->setFollowPath(true);
		enemy->setPaused(false);
		return;
	}

	Vector2 node;
	Node parent;
	bool pathFound = false;
	int nodes = 0;

	while (!openList.empty() && !pathFound) {
		parent = getShortestNode(openList);
		if (contains(closedList, parent.node, getWidth()) != -1) continue; //in closed list just go

		closedList.insert({ hashMethod(parent.node, getWidth()), parent }); //add current node to closed list
		nodes++;

		for (int x = -1; x < 2; x++) {
			for (int y = -1; y < 2; y++) {
				if ((x != 0 || y != 0)) {
					node = Vector2(
						static_cast<int>(abs(x + parent.node.x)) % getWidth(),
						static_cast<int>(abs(y + parent.node.y)) % getHeight() // so node is in bounds
					);

					float cost = heuristic(node, toPos) + parent.F; //calculate cost of this node

					if (toPos == node) { // adding target node = Done
						pathFound = true;
					}
					else if (isWalkable(node) && contains(closedList, node, getWidth()) == -1) {
						openListInsert(node, openList, hashMethod(parent.node, getWidth()), cost, parent);
					}
				}
			}
		}
	}

	// path.push_back(Vector3(toX, 0, toZ)); Dont need to go to player exact pixel,
	// enemy can attack, wrestling is not needed
	while (true) {
		path.push_back(getPosition(parent.node));
		if (parent.parentIndex == 0) break;
		parent = closedList[parent.parentIndex];
	}

	for (int i = 0; i < floor(path.size() / 2); i++) {
		std::swap(path[i], path[path.size() - i - 1]);
	}

	savePathTest(path);

	enemy->setPath(path);
	enemy->setFollowPath(true);
	enemy->setPaused(false);
}

void NavMesh::loadPathToCoord(Enemy *enemy, int fromX, int fromZ, int toX, int toZ) {
	if (mCurrentThread) {
		mCurrentThread->join();
		delete mCurrentThread;
	}
	mCurrentThread = new std::thread(
		&NavMesh::loadPathToCoordThread, this, enemy,
		fromX, fromZ, toX, toZ);
}

// Save image so the path taken is visible
void NavMesh::savePathTest(std::vector<Vector3> &path) {
	UINT32 rMask, gMask, bMask, aMask;
	rMask = 0xFF000000;
	gMask = 0x00FF0000;
	bMask = 0x0000FF00;
	aMask = 0x000000FF;

	if (mCopy)
		SDL_FreeSurface(mCopy);

	mCopy =
		SDL_CreateRGBSurface(0, mSurface->w, mSurface->h, 32, rMask, gMask, bMask, 0);

	SDL_UnlockSurface(mSurface);
	SDL_BlitSurface(mSurface, nullptr, mCopy, nullptr);
	SDL_LockSurface(mSurface);
	UINT32* pixels = (UINT32*)mCopy->pixels;
	int counter = 0;
	unsigned int i = 0;

	for (Vector3 v3 : path) {
		Vector2 pos = toPixelCoord(v3.x, v3.z);
		i = static_cast<unsigned int> (pos.x + pos.y * mCopy->w);

		if (counter == 0) pixels[i] = (255 << 16);
		else if (counter == path.size() - 1) pixels[i] = (255 << 8);
		else pixels[i] = (counter * 10 % 255 << 24);

		counter++;
	}

	SDL_SaveBMP(mCopy, "../Resource/Textures/" "NAVIGATION_TEST.bmp");
	SDL_LockSurface(mCopy);
}

int NavMesh::getWidth() const {
	return mSurface->w;
}

int NavMesh::getHeight() const {
	return mSurface->h;
}

int NavMesh::contains(std::unordered_map<int, Node> const &list,
	Vector2 const &vec, int w) const {
	int key = hashMethod(vec, w);
	return list.find(key) == list.end() ? -1 : key;
}

inline bool NavMesh::isWalkable(Vector2 const &node) const {
	SDL_Color col = getPixel(node.x, node.y);
	return col.r != BLOCKADE;
}

Vector3 NavMesh::getPosition(Vector2 pixel) const {
	return Vector3(
		(pixel.x - OFFSET_X) / SCALE_X,
		0,
		(pixel.y - OFFSET_Z) / SCALE_Z
	);
}

float NavMesh::heuristic(Vector2 node, Vector2 toPos) const {
	SDL_Color col = getPixel(node.x, node.y);
	Vector2 to = toPos - node;
	return to.x * to.x + to.y * to.y - col.r / 16;
}

NavMesh::Node NavMesh::getShortestNode(std::vector<Node> &openList) const {
	std::pop_heap(openList.begin(), openList.end());
	Node node = openList.back();
	openList.pop_back();
	return node;
}


void NavMesh::openListInsert(Vector2 &node, std::vector<Node> &openList,
	unsigned int parentIndex, float cost, Node &parent) const {
	openList.push_back({
		node, parentIndex, cost, parent.F, cost - parent.F //F, G, H
	}); // add new node to open list
	std::push_heap(openList.begin(), openList.end());
}

int NavMesh::hashMethod(Vector2 const &pos, int w) const {
	return pos.x + pos.y * w;
}

void* NavMesh::getNavigationTexture() const {
	if (!mCopy) return nullptr;
	else mCopy->pixels;
}//this is for debugging