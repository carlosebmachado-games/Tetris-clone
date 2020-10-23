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
const int PIECE_DOWN_TIME = (1 * SECOND);
const int SCORE_INCREMENT = 100;

enum { BLACK, GREY, RED, GREEN, BLUE, CYAN, MAGENTA, YELLOW, SPR_AMOUNT };

enum { MAIN_MENU, PLAYING, PAUSE_MENU, GAME_OVER };
int state = MAIN_MENU;

bool btnRight = false;
bool btnLeft = false;
bool btnDown = false;
bool btnUp = false;
bool btnConfirm = false;
bool btnReturn = false;

int score;
int lines;
int level;

Timer timerDown = Timer(PIECE_DOWN_TIME);

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
	Timer timerPressDown = Timer((0.05f * SECOND));
	Timer timerBtn = Timer((0.1f * SECOND));

	Piece() {
		x = FIELD_WIDTH / 2 - PIECE_SIZE / 2;
		y = 0;
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

	bool update(olc::PixelGameEngine* eng) {
		// default move
		if (timerDown.timeout()) {
			if(!move(olc::vi2d(0, 1)))
				return false;
		}

		// process inputs
		if (timerBtn.timeout()) {
			if (btnRight) {
				move(olc::vi2d(1, 0));
				//btnRight = false;
			} else if (btnLeft) {
				move(olc::vi2d(-1, 0));
				//btnLeft = false;
			}
			if (btnUp) {
				rotate();
				btnUp = false;
			}
		}
		if (timerPressDown.timeout()) {
			if (btnDown) {
				if (move(olc::vi2d(0, 1))) {
					score++;
				}
				//btnDown = false;
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
		
		// rotate
		int ny = 0;
		int nx = PIECE_SIZE - 1;
		for (int sy = 0; sy < PIECE_SIZE; sy++) {
			for (int sx = 0; sx < PIECE_SIZE; sx++) {
				aux[ny][nx] = space[sy][sx];
				ny++;
			}
			nx--;
			ny = 0;
		}
		// -----

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
	std::vector<Piece> curPiece;
	std::vector<Piece> nextPiece;
	Timer timerLoop = Timer((SECOND / 60));
	Timer timerLevel = Timer((60 * SECOND));

public:
	Tetris() {
		sAppName = "Tetris";
	}

private:
	bool OnUserCreate() override {
		srand(time(nullptr));
		sprites[BLACK] = spriteBlock(olc::BLACK, olc::BLACK, olc::BLACK);
		sprites[GREY] = spriteBlock(olc::GREY, olc::DARK_GREY, olc::VERY_DARK_GREY);
		sprites[RED] = spriteBlock(olc::RED, olc::DARK_RED, olc::VERY_DARK_RED);
		sprites[GREEN] = spriteBlock(olc::GREEN, olc::DARK_GREEN, olc::VERY_DARK_GREEN);
		sprites[BLUE] = spriteBlock(olc::BLUE, olc::DARK_BLUE, olc::VERY_DARK_BLUE);
		sprites[CYAN] = spriteBlock(olc::CYAN, olc::DARK_CYAN, olc::VERY_DARK_CYAN);
		sprites[MAGENTA] = spriteBlock(olc::MAGENTA, olc::DARK_MAGENTA, olc::VERY_DARK_MAGENTA);
		sprites[YELLOW] = spriteBlock(olc::YELLOW, olc::DARK_YELLOW, olc::VERY_DARK_YELLOW);
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override {
		// Processando keys
		if (GetKey(olc::Key::RIGHT).bPressed || GetKey(olc::Key::D).bPressed) {
			btnRight = true;
		} else if (GetKey(olc::Key::RIGHT).bReleased || GetKey(olc::Key::D).bReleased) {
			btnRight = false;
		}
		if (GetKey(olc::Key::LEFT).bPressed || GetKey(olc::Key::A).bPressed) {
			btnLeft = true;
		} else if (GetKey(olc::Key::LEFT).bReleased || GetKey(olc::Key::A).bReleased) {
			btnLeft = false;
		}
		if (GetKey(olc::Key::DOWN).bPressed || GetKey(olc::Key::S).bPressed) {
			btnDown = true;
		} else if (GetKey(olc::Key::DOWN).bReleased || GetKey(olc::Key::S).bReleased) {
			btnDown = false;
		}
		if (GetKey(olc::Key::UP).bPressed || GetKey(olc::Key::W).bPressed) {
			btnUp = true;
		}
		if (GetKey(olc::Key::ENTER).bPressed || GetKey(olc::Key::SPACE).bPressed) {
			btnConfirm = true;
		}
		if (GetKey(olc::Key::ESCAPE).bPressed || GetKey(olc::Key::BACK).bPressed) {
			btnReturn = true;
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

	void GameInit() {
		curPiece = std::vector<Piece>();
		nextPiece = std::vector<Piece>();
		curPiece.push_back(Piece());
		nextPiece.push_back(Piece());
		timerLoop.start();
		timerLevel.start();
		timerDown.start();
		score = 0;
		lines = 0;
		level = 1;
		for (int y = 0; y < FIELD_HEIGHT - 1; y++) {
			for (int x = 1; x < FIELD_WIDTH - 1; x++) {
				field[y][x] = 0;
			}
		}
	}

	void update() {
		if (state == MAIN_MENU) {
			if (btnConfirm) {
				state = PLAYING;
				GameInit();
				btnConfirm = false;
			} else if (btnReturn) {
				// TODO: Close game
				btnReturn = false;
			}
		} else if (state == PLAYING) {
			for (int i = 0; i < curPiece.size(); i++) {
				if (!curPiece[i].update(this)) {
					checkRow();

					if (canGenerate()) {
						curPiece.clear();
						curPiece.push_back(nextPiece[0]);
						nextPiece.clear();
						nextPiece.push_back(Piece());
					}
					else {
						state = GAME_OVER;
					}
				}
				//std::cout << "Atualizou...\n";
			}
			if (timerLevel.timeout()) {
				level++;
				timerDown.interval = PIECE_DOWN_TIME / level;
			}
			if (btnConfirm) {
				btnConfirm = false;
			} else if (btnReturn) {
				state = PAUSE_MENU;
				btnReturn = false;
			}
		} else if (state == PAUSE_MENU) {
			if (btnConfirm) {
				state = PLAYING;
				btnConfirm = false;
			} else if (btnReturn) {
				state = MAIN_MENU;
				btnReturn = false;
			}
		} else if (state == GAME_OVER) {
			if (btnConfirm) {
				btnConfirm = false;
			} else if (btnReturn) {
				state = MAIN_MENU;
				btnReturn = false;
			}
		}
	}
	
	void render() {
		Clear(olc::BACK);

		if (state == MAIN_MENU) {
			drawCentralizedText("TETRIS", 20);
			drawCentralizedText("ENTER TO PLAY", 100);
			drawCentralizedText("ESC TO EXIT", 125);
			drawCentralizedText("GAME: CARLOS MACHADO", 200);
			drawCentralizedText("olcPixelGameEngine", 220);
			drawCentralizedText("OneLoneCoder", 230);
		} else if(state == PLAYING) {
			for (int y = 0; y < FIELD_HEIGHT; y++) {
				for (int x = 0; x < FIELD_WIDTH; x++) {
					if (field[y][x] > 0) {
						DrawSprite(x * TILE_SIZE, y * TILE_SIZE, sprites[field[y][x]]);
					}
					//std::cout << field[y][x] << " ";
				}
				//std::cout << std::endl;
			}
			//system("pause");
			//system("cls");

			// UI
			int uiX = FIELD_WIDTH * TILE_SIZE;
			int uiY = 10;
			int uiWidth = (SCREEN_WIDTH - uiX);
			int uiHeight = SCREEN_HEIGHT;
			int uiCenterX = uiX + (uiWidth / 2);

			// NEXT PIECE
			DrawString(uiCenterX - GetTextSize("NEXT").x / 2, uiY, "NEXT");
			for (int y = 0; y < PIECE_SIZE; y++) {
				for (int x = 0; x < PIECE_SIZE; x++) {
					if (nextPiece[0].space[y][x] > 0) {
						DrawSprite(
							uiCenterX - 2 * TILE_SIZE + x * TILE_SIZE,
							uiY + 10 + y * TILE_SIZE,
							sprites[nextPiece[0].space[y][x]]);
					}
				}
			}

			// PLAYER SCORE
			DrawString(uiCenterX - GetTextSize("SCORE").x / 2, uiY + 70, "SCORE");
			std::string strScore = std::to_string(score);
			DrawString(uiCenterX - GetTextSize(strScore).x / 2, uiY + 85, strScore);

			// SPEED LEVEL
			DrawString(uiCenterX - GetTextSize("LEVEL").x / 2, uiY + 105, "LEVEL");
			std::string strLevel = std::to_string(level);
			DrawString(uiCenterX - GetTextSize(strLevel).x / 2, uiY + 120, strLevel);

			// FINALIZED LINES
			DrawString(uiCenterX - GetTextSize("LINES").x / 2, uiY + 140, "LINES");
			std::string strLines = std::to_string(lines);
			DrawString(uiCenterX - GetTextSize(strLines).x / 2, uiY + 155, strLines);
		} else if (state == PAUSE_MENU) {
			drawCentralizedText("TETRIS", 20);
			drawCentralizedText("ENTER TO CONTINUE", 100);
			drawCentralizedText("ESC TO BACK TO", 125);
			drawCentralizedText("MAIN MENU", 135);
		} else if (state == GAME_OVER) {
			std::string strScore = std::to_string(score);
			std::string strLevel = std::to_string(level);
			std::string strLines = std::to_string(lines);

			drawCentralizedText("TETRIS", 20);
			drawCentralizedText("GAME OVER", 60);
			drawCentralizedText("SCORE", 100);
			drawCentralizedText(strScore.c_str(), 110);
			drawCentralizedText("LEVEL", 130);
			drawCentralizedText(strLevel.c_str(), 140);
			drawCentralizedText("LINES", 160);
			drawCentralizedText(strLines.c_str(), 170);
			drawCentralizedText("ESC TO BACK TO", 210);
			drawCentralizedText("MAIN MENU", 220);
		}
	}

	bool canGenerate() {
		for (int y = 0; y < 5; y++) {
			for (int x = 4; x < 8; x++) {
				if (field[y][x] > 0) {
					return false;
				}
			}
		}
		return true;
	}

	void drawCentralizedText(const char* str, int y) {
		DrawString(SCREEN_WIDTH / 2 - GetTextSize(str).x / 2, y, str);
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
				lines++;
				score += SCORE_INCREMENT;
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
	if (tetris.Construct(SCREEN_WIDTH, SCREEN_HEIGHT, 2, 2))
		tetris.Start();
	return 0;
}
