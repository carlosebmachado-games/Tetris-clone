#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "BPS.hpp"
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

bool actRight = false;
bool actLeft = false;
bool actDown = false;
bool actUp = false;
bool actConfirm = false;
bool actCancel = false;

int score;
int lines;
int level;

Timer timerDown;

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
			if (actRight) {
				move(olc::vi2d(1, 0));
			} else if (actLeft) {
				move(olc::vi2d(-1, 0));
			}
			if (actUp) {
				rotate();
				actUp = false;
			}
		}
		if (timerPressDown.timeout()) {
			if (actDown) {
				if (move(olc::vi2d(0, 1))) {
					score++;
				}
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
			for (int sy = 0; sy < PIECE_SIZE; sy++) {
				for (int sx = 0; sx < PIECE_SIZE; sx++) {
					space[sy][sx] = aux[sy][sx];
				}
			}
		}
		move(olc::vd2d(0, 0));
	}

	bool move(olc::vi2d movement) {
		clean();
		if (!isFree(movement)) {
			putOnField();
			return false;
		}
		x += movement.x;
		y += movement.y;
		putOnField();
		return true;
	}

	void putOnField() {
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

	bool isFree(olc::vi2d movement) {
		for (int sy = 0; sy < PIECE_SIZE; sy++) {
			for (int sx = 0; sx < PIECE_SIZE; sx++) {
				if (space[sy][sx] > 0 &&
					field[y + sy + movement.y][x + sx + movement.x] > 0) {
					return false;
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
	Timer timerLoop;
	Timer timerLevel;
	Timer timerKeyInput;
	BPS::File* hiscores;
	bool newScoreSaved = false;
	std::string playerName = "";

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
		hiscores = BPS::read("scores");
		timerKeyInput = Timer(100);
		timerKeyInput.start();
		scoreBubbleSort(hiscores);
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override {
		// Processando keys
		if (GetKey(olc::Key::RIGHT).bPressed || GetKey(olc::Key::D).bPressed) {
			actRight = true;
		} else if (GetKey(olc::Key::RIGHT).bReleased || GetKey(olc::Key::D).bReleased) {
			actRight = false;
		}
		if (GetKey(olc::Key::LEFT).bPressed || GetKey(olc::Key::A).bPressed) {
			actLeft = true;
		} else if (GetKey(olc::Key::LEFT).bReleased || GetKey(olc::Key::A).bReleased) {
			actLeft = false;
		}
		if (GetKey(olc::Key::DOWN).bPressed || GetKey(olc::Key::S).bPressed) {
			actDown = true;
		} else if (GetKey(olc::Key::DOWN).bReleased || GetKey(olc::Key::S).bReleased) {
			actDown = false;
		}
		if (GetKey(olc::Key::UP).bPressed || GetKey(olc::Key::W).bPressed) {
			actUp = true;
		}
		if (GetKey(olc::Key::ENTER).bPressed || GetKey(olc::Key::SPACE).bPressed) {
			actConfirm = true;
		}
		if (GetKey(olc::Key::ESCAPE).bPressed || GetKey(olc::Key::BACK).bPressed) {
			actCancel = true;
		}

		// processando interface inputs
		if (state == MAIN_MENU) {
			if (actConfirm) {
				GameInit();
				state = PLAYING;
				actConfirm = false;
			}
			else if (actCancel) {
				// TODO: Close game
				actCancel = false;
			}
		}
		else if (state == PLAYING) {
			if (actConfirm) {
				actConfirm = false;
			}
			else if (actCancel) {
				state = PAUSE_MENU;
				actCancel = false;
			}
		}
		else if (state == PAUSE_MENU) {
			if (actConfirm) {
				state = PLAYING;
				actConfirm = false;
			}
			else if (actCancel) {
				state = MAIN_MENU;
				actCancel = false;
			}
		}
		else if (state == GAME_OVER) {
			if (actConfirm) {
				actConfirm = false;
			}
			else if (actCancel) {
				hiscores = BPS::read("scores");
				state = MAIN_MENU;
				actCancel = false;
			}
		}

		if (timerLoop.timeout()) {
			render();
			update();
		}
		return true;
	}

	void GameInit() {
		curPiece = std::vector<Piece>();
		nextPiece = std::vector<Piece>();
		curPiece.push_back(Piece());
		nextPiece.push_back(Piece());
		timerLoop = Timer((SECOND / 60));
		timerLevel = Timer((60 * SECOND));
		timerDown = Timer(PIECE_DOWN_TIME);
		timerLoop.start();
		timerLevel.start();
		timerDown.start();
		score = 0;
		lines = 0;
		level = 1;
		newScoreSaved = false;
		playerName = "";
		for (int y = 0; y < FIELD_HEIGHT - 1; y++) {
			for (int x = 1; x < FIELD_WIDTH - 1; x++) {
				field[y][x] = 0;
			}
		}
	}

	void update() {
		if (state == MAIN_MENU) {
		} else if (state == PLAYING) {
			for (int i = 0; i < curPiece.size(); i++) {
				if (!curPiece[i].update(this)) {
					// checa se existe uma linha completa
					for (int y = 0; y < FIELD_HEIGHT - 1; y++) {
						bool complete = true;
						for (int x = 1; x < FIELD_WIDTH - 1; x++) {
							if (field[y][x] == 0) {
								complete = false;
							}
						}
						if (complete) {
							for (int my = y; my > 0; my--) {
								for (int mx = 1; mx < FIELD_WIDTH - 1; mx++) {
									field[my][mx] = field[my - 1][mx];
								}
							}
							// se encontra, incrementa o score
							lines++;
							score += SCORE_INCREMENT;
						}
					}

					if (canGenerate()) {
						curPiece.clear();
						curPiece.push_back(nextPiece[0]);
						nextPiece.clear();
						nextPiece.push_back(Piece());
					} else {
						state = GAME_OVER;
					}
				}
			}
			if (timerLevel.timeout()) {
				level++;
				timerDown.setInterval(PIECE_DOWN_TIME / level);
			}
		} else if (state == PAUSE_MENU) {
		} else if (state == GAME_OVER) {
			// TODO: Save hiscore (ok) / take player name / sort by hiscores (ok)
			if (!newScoreSaved) {
				auto scoreFile = BPS::read("scores");
				std::string pn = std::to_string(scoreFile->findAll().size() + 1);
				playerName = "PLAYER " + pn;
				auto newScore = new BPS::Section(playerName);
				newScore->add(new BPS::IntData("score", score));
				newScore->add(new BPS::IntData("level", level));
				newScore->add(new BPS::IntData("lines", lines));
				scoreFile->add(newScore);
				scoreBubbleSort(scoreFile);
				BPS::write(scoreFile, "scores");
				newScoreSaved = true;
			}
		}
	}
	
	void render() {
		Clear(olc::BACK);

		if (state == MAIN_MENU) {
			drawCentralizedText("TETRIS", 20);
			drawCentralizedText("ENTER TO PLAY", 50);
			drawCentralizedText("ESC TO EXIT", 65);
			drawCentralizedText("HI-SCORES:", 95);
			for (int i = 0; i < hiscores->findAll().size(); i++) {
				auto name = hiscores->findAll()[i]->getName();
				auto score = ((BPS::IntData*)  hiscores->findAll()[i]->findAll()[0])->getValue();
				std::string str = name + " - " + std::to_string(score);
				drawCentralizedText(str.c_str(), 110 + i * 12);
				if (i >= 4) {
					break;
				}
			}
			drawCentralizedText("GAME: CARLOS MACHADO", 220);
			drawCentralizedText("olcPixelGameEngine", 240);
			drawCentralizedText("by OneLoneCoder", 250);
		} else if(state == PLAYING) {
			for (int y = 0; y < FIELD_HEIGHT; y++) {
				for (int x = 0; x < FIELD_WIDTH; x++) {
					if (field[y][x] > 0) {
						DrawSprite(x * TILE_SIZE, y * TILE_SIZE, sprites[field[y][x]]);
					}
				}
			}

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
			if (newScoreSaved) {
				std::string strScore = std::to_string(score);
				std::string strLevel = std::to_string(level);
				std::string strLines = std::to_string(lines);

				drawCentralizedText("TETRIS", 20);
				drawCentralizedText("GAME OVER", 60);
				drawCentralizedText("NAME", 100);
				drawCentralizedText(playerName.c_str(), 110);
				drawCentralizedText("SCORE", 130);
				drawCentralizedText(strScore.c_str(), 140);
				drawCentralizedText("LEVEL", 160);
				drawCentralizedText(strLevel.c_str(), 170);
				drawCentralizedText("LINES", 190);
				drawCentralizedText(strLines.c_str(), 200);
				drawCentralizedText("ESC TO BACK TO", 230);
				drawCentralizedText("MAIN MENU", 240);
			}
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

	void scoreBubbleSort(BPS::File* file) {
		for (int i = 0; i < file->sections.size() - 1; i++) {
			for (int j = 0; j < file->sections.size() - 1; j++) {
				auto jp1 = j + 1;
				auto j_data = ((BPS::IntData*) file->sections[j]->findAll()[0])->getValue();
				auto jp1_data = ((BPS::IntData*) file->sections[jp1]->findAll()[0])->getValue();

				if (j_data < jp1_data) {
					auto aux1 = file->sections[j];
					auto aux2 = file->sections[jp1];
					file->sections.erase(file->sections.begin() + j);
					file->sections.insert(file->sections.begin() + j, aux2);
					file->sections.erase(file->sections.begin() + jp1);
					file->sections.insert(file->sections.begin() + jp1, aux1);
				}
			}
		}
	}

	void drawCentralizedText(const char* str, int y) {
		DrawString(SCREEN_WIDTH / 2 - GetTextSize(str).x / 2, y, str);
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
