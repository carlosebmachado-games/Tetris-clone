#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "BPS.hpp"
#include "util.hpp"
#include "watch.hpp"

const int SCREEN_WIDTH = 200;
const int SCREEN_HEIGHT = 264;
const int TILE_SIZE = 12;
const int FIELD_WIDTH = 12;
const int FIELD_HEIGHT = 22;
const int PIECE_SIZE = 4;

enum { BLACK, GREY, RED, GREEN, BLUE, CYAN, MAGENTA, YELLOW, SPR_AMOUNT };

bool btnRight = false;
bool btnLeft = false;
bool btnDown = false;
bool btnUp = false;

int field[22][12] = {
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

struct Piece {
	int space[PIECE_SIZE][PIECE_SIZE] = {
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0}};
	int x;
	int y;
	Timer timerDown = Timer((1 * SECOND));
	Timer timerPressDown = Timer((0.05f * SECOND));
	Timer timerBtn = Timer((0.1f * SECOND));

	Piece() {
		x = FIELD_WIDTH / 2 - PIECE_SIZE / 2;
		y = 0;
		timerDown.start();
		timerPressDown.start();
		timerBtn.start();
		int piece = rand() % 7;
		int color = rand() % 6 + 2;

		//std::cout << "Piece: " << piece << "\nColor: " << color << std::endl;
		//piece = 0;
		
		switch (piece)
		{
		case 0:
			space[1][0] = color;
			space[1][1] = color;
			space[1][2] = color;
			space[1][3] = color;
			break;
		case 1:
			space[1][0] = color;
			space[2][0] = color;
			space[2][1] = color;
			space[2][2] = color;
			break;
		case 2:
			space[1][2] = color;
			space[2][0] = color;
			space[2][1] = color;
			space[2][2] = color;
			break;
		case 3:
			space[1][1] = color;
			space[2][1] = color;
			space[1][2] = color;
			space[2][2] = color;
			break;
		case 4:
			space[1][1] = color;
			space[1][2] = color;
			space[2][0] = color;
			space[2][1] = color;
			break;
		case 5:
			space[1][0] = color;
			space[1][1] = color;
			space[2][1] = color;
			space[2][2] = color;
			break;
		case 6:
			space[2][0] = color;
			space[2][1] = color;
			space[2][2] = color;
			space[1][1] = color;
		}
	}

	bool update() {
		// default move
		if (timerDown.timeout()) {
			if(!move(olc::vi2d(0, 1)))
				return false;
		}

		// process inputs
		if (timerBtn.timeout()) {
			if (btnRight) {
				move(olc::vi2d(1, 0));
				btnRight = false;
			} else if (btnLeft) {
				move(olc::vi2d(-1, 0));
				btnLeft = false;
			}
			if (btnUp) {
				rotate();
				btnUp = false;
			}
		}
		if (timerPressDown.timeout()) {
			if (btnDown) {
				move(olc::vi2d(0, 1));
				btnDown = false;
			}
		}
		return true;
	}

	void rotate() {
		int aux[][PIECE_SIZE] = {
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0}};

		clean();
		for (int sy = 0; sy < PIECE_SIZE; sy++) {
			for (int sx = 0; sx < PIECE_SIZE; sx++) {
				aux[sx][sy] = space[sy][sx];
			}
		}
		if (canRotate(aux)) {
			//std::cout << "isFree rotate\n";
			for (int sy = 0; sy < PIECE_SIZE; sy++) {
				for (int sx = 0; sx < PIECE_SIZE; sx++) {
					space[sy][sx] = aux[sy][sx];
				}
			}
		}
		move(olc::vd2d(0, 0));
	}

	void clean() {
		int px = 0;
		int py = 0;
		for (int fy = y; fy < y + PIECE_SIZE; fy++) {
			for (int fx = x; fx < x + PIECE_SIZE; fx++) {
				if (space[py][px] > 0) {
					field[fy][fx] = 0;
				}
				px++;
			}
			py++;
			px = 0;
		}
	}

	bool move(olc::vi2d movement) {
		if (!isFree(movement)) return false;
		//std::cout << "isFree()\n";

		clean();
		x += movement.x;
		y += movement.y;
		//std::cout << "x: " << x << "\ny: " << y << std::endl;
		int px = 0;
		int py = 0;
		for (int fy = y; fy < y + PIECE_SIZE; fy++) {
			for (int fx = x; fx < x + PIECE_SIZE; fx++) {
				if (space[py][px] > 0) {
					field[fy][fx] = space[py][px];
				}
				px++;
			}
			py++;
			px = 0;
		}
		return true;
	}

	bool isFree(olc::vi2d movement) {
		for (int sy = 0; sy < PIECE_SIZE; sy++) {
			for (int sx = 0; sx < PIECE_SIZE; sx++) {
				if (sy < PIECE_SIZE - 1) {
					if (space[sy][sx] > 0 &&
						space[sy + movement.y][sx + movement.x] == 0 &&
						field[y + sy + movement.y][x + sx + movement.x] > 0) {
						return false;
					}
				} else {
					if (space[sy][sx] > 0 &&
						field[y + sy + movement.y][x + sx + movement.x] > 0) {
						return false;
					}
				}
			}
		}
		return true;
	}

	bool canRotate(int mat[][PIECE_SIZE]) {
		for (int sy = 0; sy < PIECE_SIZE; sy++) {
			for (int sx = 0; sx < PIECE_SIZE; sx++) {
				if (mat[sy][sx] > 0 &&
					field[y + sy][x + sx] > 0) {
					return false;
				}
			}
		}
		return true;
	}
};

class Tetris : public olc::PixelGameEngine {
private:
	olc::Sprite* sprites[SPR_AMOUNT];
	std::vector<Piece> piece;
	Timer timerLoop = Timer((SECOND / 60));

public:
	Tetris() {
		sAppName = "Tetris";
	}

private:
	bool OnUserCreate() override {
		srand(time(nullptr));
		piece = std::vector<Piece>();
		sprites[BLACK] = spriteBlock(olc::BLACK, olc::BLACK, olc::BLACK);
		sprites[GREY] = spriteBlock(olc::GREY, olc::DARK_GREY, olc::VERY_DARK_GREY);
		sprites[RED] = spriteBlock(olc::RED, olc::DARK_RED, olc::VERY_DARK_RED);
		sprites[GREEN] = spriteBlock(olc::GREEN, olc::DARK_GREEN, olc::VERY_DARK_GREEN);
		sprites[BLUE] = spriteBlock(olc::BLUE, olc::DARK_BLUE, olc::VERY_DARK_BLUE);
		sprites[CYAN] = spriteBlock(olc::CYAN, olc::DARK_CYAN, olc::VERY_DARK_CYAN);
		sprites[MAGENTA] = spriteBlock(olc::MAGENTA, olc::DARK_MAGENTA, olc::VERY_DARK_MAGENTA);
		sprites[YELLOW] = spriteBlock(olc::YELLOW, olc::DARK_YELLOW, olc::VERY_DARK_YELLOW);
		piece.push_back(Piece());
		timerLoop.start();
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override {
		// Processando keys
		if (GetKey(olc::Key::RIGHT).bHeld || GetKey(olc::Key::D).bHeld) {
			btnRight = true;
		}/* else if (GetKey(olc::Key::RIGHT).bReleased || GetKey(olc::Key::D).bReleased) {
			btnRight = false;
		}*/
		if (GetKey(olc::Key::LEFT).bHeld || GetKey(olc::Key::A).bHeld) {
			btnLeft = true;
		}/* else if (GetKey(olc::Key::LEFT).bReleased || GetKey(olc::Key::A).bReleased) {
			btnLeft = false;
		}*/
		if (GetKey(olc::Key::DOWN).bHeld || GetKey(olc::Key::S).bHeld) {
			btnDown = true;
		}/* else if (GetKey(olc::Key::DOWN).bReleased || GetKey(olc::Key::S).bReleased) {
			btnDown = false;
		}*/
		if (GetKey(olc::Key::UP).bPressed || GetKey(olc::Key::W).bPressed) {
			btnUp = true;
		}/* else if (GetKey(olc::Key::UP).bReleased || GetKey(olc::Key::W).bReleased) {
			btnUp = false;
		}*/

		//std::cout << "OnUserUpdate...\n";
		if (timerLoop.timeout()) {
			render();
			update();
			//std::cout << "Update...\n";
		}
		return true;
	}

	void update() {
		for (int i = 0; i < piece.size(); i++) {
			if (!piece[i].update()) {
				checkRow();

				piece.clear();
				piece.push_back(Piece());
			}
			//std::cout << "Atualizou...\n";
		}
	}
	
	void render() {
		for (int y = 0; y < FIELD_HEIGHT; y++) {
			for (int x = 0; x < FIELD_WIDTH; x++) {
				DrawSprite(x * TILE_SIZE, y * TILE_SIZE, sprites[field[y][x]]);
				//std::cout << field[y][x] << " ";
			}
			//std::cout << std::endl;
		}
		//system("pause");
		//system("cls");

		// UI
		drawUI();
	}

	void drawUI() {
		int uiX = FIELD_WIDTH * TILE_SIZE;
		int uiY = 5;
		int uiWidth = (SCREEN_WIDTH - uiX);
		int uiHeight = SCREEN_HEIGHT;
		int uiCenterX = uiX + (uiWidth / 2);

		DrawString(uiCenterX - GetTextSize("NEXT").x / 2, uiY + 5, "NEXT");
		for (int y = 0; y < PIECE_SIZE; y++) {
			for (int x = 0; x < PIECE_SIZE; x++) {
				DrawSprite(
					uiCenterX - 2 * TILE_SIZE + x * TILE_SIZE,
					uiY + 20 + y * TILE_SIZE,
					sprites[piece[0].space[y][x]]);
			}
		}
	}

	bool checkRow() {
		for (int y = 0; y < FIELD_HEIGHT - 1; y++) {
			bool complete = true;
			for (int x = 1; x < FIELD_WIDTH - 1; x++) {
				if (field[y][x] == 0) {
					complete = false;
				}
			}
			if (complete) {
				//std::cout << "Complete Row\n";
				for (int my = y; my > 0; my--) {
					for (int mx = 1; mx < FIELD_WIDTH - 1; mx++) {
						field[my][mx] = field[my - 1][mx];
					}
				}
			}
		}
		return true;
	}

	olc::Sprite* spriteBlock(olc::Pixel color, olc::Pixel colord, olc::Pixel colorvd) {
		olc::Sprite* block = new olc::Sprite(TILE_SIZE, TILE_SIZE);
		int l1 = TILE_SIZE * 0.16666666667; // 12 = 2
		int l2 = TILE_SIZE * 0.75;          // 12 = 9
		for (int y = 0; y < TILE_SIZE; y++) {
			for (int x = 0; x < TILE_SIZE; x++) {
				if (x < l1 || y < l1) {
					block->SetPixel(x, y, color);
				} else if (x > l2 || y > l2) {
					block->SetPixel(x, y, colorvd);
				} else {
					block->SetPixel(x, y, colord);
				}
			}
		}
		for (int y = l2 + 1; y < TILE_SIZE; y++) {
			for (int x = 0; x < l1; x++) {
				block->SetPixel(x, y, colord);
			}
		}
		for (int y = 0; y < l1; y++) {
			for (int x = l2 + 1; x < TILE_SIZE; x++) {
				block->SetPixel(x, y, colord);
			}
		}
		return block;
	}
};

int main() {
	Tetris tetris;
	if (tetris.Construct(200, 264, 2, 2))
		tetris.Start();
	return 0;
}
