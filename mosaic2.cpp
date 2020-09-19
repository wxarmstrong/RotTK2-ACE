#include "CImg.h"
#include "mckp.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>
#include <sstream>

using namespace cimg_library;
using namespace std;

const int BAGSIZE = 1250;
const int PATTERNSWAP = 19;
const int TILESWAP = 33;
const int DUALSWAP = 47;

int curFrameInput = 0;
int frameLimiter = 0;

class tile {
public:
	string bHash;
	int tileID;
	int patternID;
};

int max(int bright1, int bright2) {
	return 64 - abs(bright1 - bright2);
}

void genPad(int hex, ofstream& outfile)
{
	outfile << "|    0,..|";
	if ((hex & 0x08) > 0) outfile << "U";
	else outfile << ".";
	if ((hex & 0x04) > 0) outfile << "D";
	else outfile << ".";
	if ((hex & 0x02) > 0) outfile << "L";
	else outfile << ".";
	if ((hex & 0x01) > 0) outfile << "R";
	else outfile << ".";
	if ((hex & 0x10) > 0) outfile << "S";
	else outfile << ".";
	if ((hex & 0x20) > 0) outfile << "s";
	else outfile << ".";
	if ((hex & 0x40) > 0) outfile << "B";
	else outfile << ".";
	if ((hex & 0x80) > 0) outfile << "A";
	else outfile << ".";
	outfile << "|........|" << endl;
	curFrameInput++;
}

int convHex(char hex)
{
	if (isalpha(hex)) return hex - 'A' + 10;
	return hex - '0';
}

int sumBits(int num)
{
	int total = 0;
	for (int i = 0; i < 8; i++)
	{
		if (num % 2 == 1) total++;
		num = num >> 1;
	}
	return total;
}

int bitDiff(string hash1, string hash2)
{
	int total = 0;
	for (int i = 0; i < 16; i++)
	{
		total += sumBits(convHex(hash1[i]) xor convHex(hash2[i]));
	}
	return total;
}

char toHex(int hex)
{
	if (hex >= 10) return 'A' + (hex - 10);
	return '0' + hex;
}

int main() {
	ifstream inFile("root-full.txt");
	vector<vector<tile>> brightnesses;
	tile* chrTiles[0x100][0x100];

	for (int i = 0; i < 65; i++) {
		vector<tile> curBright;
		int size;
		inFile >> size;
		for (int j = 0; j < size; j++)
		{
			tile curTile;
			inFile >> curTile.bHash;
			inFile >> curTile.tileID;
			inFile >> curTile.patternID;
			curBright.push_back(curTile);
		}
		brightnesses.push_back(curBright);
	}

	for (int i = 0; i < 65; i++)
	{
		vector<tile> curBright = brightnesses[i];
		int size = curBright.size();
		for (int j = 0; j < size; j++)
		{
			int curPatternID = curBright[j].patternID;
			int curTileID = curBright[j].tileID;
			chrTiles[curPatternID][curTileID] = &(brightnesses[i][j]);
		}
	}

	string path = "C:\\Users\\wxarm\\Desktop\\mario2\\";
	string searchPattern = "*.png";
	string fullSearchPath = path + searchPattern;
	WIN32_FIND_DATA FindData;
	HANDLE hFind;
	hFind = FindFirstFile(fullSearchPath.c_str(), &FindData);
	if (hFind == INVALID_HANDLE_VALUE)
		cout << "Error searching directory\n";

	ofstream outfile("mosaic.txt");
	vector<tile> lastMosaic;
	tile blankTile;
	blankTile.tileID = 0;
	blankTile.patternID = 0;
	blankTile.bHash = "0000000000000000";
	for (int i = 0; i < 32 * 29; i++)
		lastMosaic.push_back(blankTile);

	do {
		string filePath = path + FindData.cFileName;
		cout << filePath << endl;
		CImg<unsigned char> sourceFrame(filePath.c_str());
		for (int repeat = 0; repeat < 2; repeat++)
		{
			int curOpNum = 0;
			curFrameInput = 0;
			outfile << "|    0,..|........|U.......|" << endl;
			vector<int> weight = { 0 };
			vector<int> profit = { 0 };
			vector<int> classes = { 0 };
			vector<int> tilePos = { 0 };
			vector<tile> chosenTiles = { brightnesses[0][0] };

			for (int j = 0; j < 29; j++)
			{
				for (int i = 0; i < 32; i++)
				{
//					cout << "Now assessing tile @" << i << "," << j << endl;
					CImg<unsigned char> sourceTile = sourceFrame.get_crop(i * 8, j * 8, (i + 1) * 8 - 1, (j + 1) * 8 - 1);
					int bright = 0;
					string aHash = "";
					for (int k = 0; k < 8; k++) {
						int numHash = 0;
						ostringstream tempHash;

						for (int m = 0; m < 8; m++)
						{
							numHash = numHash << 1;
							if (sourceTile(k, m, 0, 0)) {
								bright++;
								numHash += 1;
							}
						}
						tempHash << std::hex << setw(2) << std::setfill('0') << uppercase << numHash;
						aHash += tempHash.str();
					}

					int tileNum = 32 * j + i;
					tile LastTile = lastMosaic[tileNum];
					int curDiff = bitDiff(aHash, LastTile.bHash);
			//		cout << curDiff << endl;
					
					if (curDiff > 0)
					{
						curOpNum++;
						bool found = false;
						//Find best tileswap match		
						int lastPatternID = LastTile.patternID;
						int bestPatternIDDiff = curDiff;
						int bestPatternIDIdx = -1;
						for (int k = 0; k < 0x100; k++)
						{
							tile curTile = *(chrTiles[lastPatternID][k]);
							int thisDiff = bitDiff(aHash, curTile.bHash);
							if (thisDiff < bestPatternIDDiff)
							{
								bestPatternIDDiff = thisDiff;
								bestPatternIDIdx = k;
							}
						}
						if (bestPatternIDIdx != -1)
						{
				//			cout << "Possible tileswap @ " << tileNum << ": " << (curDiff - bestPatternIDDiff) << " pixels " << endl;
							weight.push_back(TILESWAP);
							profit.push_back(curDiff - bestPatternIDDiff);
							classes.push_back(curOpNum);
							tilePos.push_back(tileNum);
							chosenTiles.push_back(*(chrTiles[lastPatternID][bestPatternIDIdx]));
							found = true;
						}

						
						//Find best patternswap match		
						int lastTileID = LastTile.tileID;
						int bestTileIDDiff = curDiff;
						int bestTileIDIdx = -1;
						for (int k = 0; k < 0x100; k++)
						{
							tile curTile = *(chrTiles[k][lastTileID]);
							int thisDiff = bitDiff(aHash, curTile.bHash);
							if (thisDiff < bestTileIDDiff)
							{
								bestTileIDDiff = thisDiff;
								bestTileIDIdx = k;
							}
						}
						
						if (bestTileIDIdx != -1)
						{
				//			cout << "Possible patternswap @ " << tileNum << ": " << (curDiff - bestTileIDDiff) << " pixels " << endl;
							weight.push_back(PATTERNSWAP);
							profit.push_back(curDiff - bestTileIDDiff);
							classes.push_back(curOpNum);
							tilePos.push_back(tileNum);
							chosenTiles.push_back(*(chrTiles[bestTileIDIdx][lastTileID]));
							found = true;
						}
						
						//Find best overall match
						int bestMatch = 32 + abs(32 - bright) - 1;
						tile bestTile = brightnesses[0][0];
						if (bright > 32) bestTile = brightnesses[64][0];

						int minBright = 0;
						int maxBright = 64;
						if (bright > 32) minBright = 2 * (bright - 32);
						if (bright < 32) maxBright = 2 * bright;
						vector<int> brightList;
						for (int k = minBright; k < maxBright + 1; k++) {
							brightList.push_back(k);
						}
						for (int k = 0; k < brightList.size() - 1; k++)
						{
							int max_idx = k;
							for (int m = k + 1; m < brightList.size(); m++)
							{
								if (abs(bright - brightList[m]) < abs(bright - brightList[max_idx]))
								{
									max_idx = m;
								}
							}
							swap(brightList[max_idx], brightList[k]);
						}

						int curBright;
						int curMax;

						while (!brightList.empty())
						{
							curBright = brightList[0];
							curMax = max(bright, curBright);
							brightList.erase(brightList.begin());
							int curSize = brightnesses[curBright].size();
							for (int k = 0; k < curSize; k++)
							{
								tile curTarget = brightnesses[curBright][k];
								int thisMatch = 64 - bitDiff(aHash, curTarget.bHash);


								if (thisMatch > bestMatch) {
									bestMatch = thisMatch;
									bestTile = brightnesses[curBright][k];
									//cout << "New match of " << bestMatch << ": " << curTarget.bHash << endl;
									vector<int> toErase;

									for (int m = 0; m < brightList.size(); m++)
									{
										if (!(max(bright, brightList[m]) > bestMatch))
										{
											toErase.push_back(m);
										}
									}

									for (int m = toErase.size() - 1; m > -1; m--)
									{
										brightList.erase(brightList.begin() + toErase[m]);
									}

									if (!(max(bright, curBright) > bestMatch)) break;


								}
							}
						}
						
						int localProfit = bestMatch - (64 - curDiff);
						if (localProfit > 0)
						{
							weight.push_back(DUALSWAP);
							profit.push_back(localProfit);
							classes.push_back(curOpNum);
							tilePos.push_back(tileNum);
							chosenTiles.push_back(bestTile);	
							found = true;
						}

						if (!found)
							curOpNum--;
				
					}
					


				}
			}
		
			/*
			cout << "Tile classes to be assessed:" << endl;
			for (int i = 0; i < classes.size(); i++)
			{
				cout << classes[i] << " ";
			}
			cout << endl;
			cout << "Their corresponding tile positions: " << endl;
			for (int i = 0; i < tilePos.size(); i++)
			{
				cout << tilePos[i] << " ";
			}
			cout << endl;
			*/

			ComputeSolution* c = new ComputeSolution(BAGSIZE, profit, weight, classes);
			MultipleChoiceKnapsackProblem* k = new MultipleChoiceKnapsackProblem(c);
			vector<int> solution = k->solveProblem();
			int sum = 0;
			/*
			cout << "Solution for frame " << frameLimiter << ": " << endl;
			for (int i = 0; i < solution.size(); i++)
			{
				cout << solution[i] << " ";
			}
			cout << endl;
			for (int i = 0; i < solution.size(); i++)
			{
				if (solution[i]) cout << i << " ";
			}
			cout << endl;
			*/

			int numDualSwaps = 0;
			int numTileSwaps = 0;
			int numPatternSwaps = 0;
			

			for (int i = 0; i < solution.size(); i++)
			{
				if (solution[i])
				{
					int opNum = classes[i];
					int tileNum = tilePos[i];
					int thisWeight = weight[i];
					int thisProfit = profit[i];
					tile curTile = chosenTiles[i];
					switch (thisWeight) {
					case PATTERNSWAP:
						numPatternSwaps++;
			//			cout << "Pattern ID swap @ ";
						break;
					case TILESWAP:
						numTileSwaps++;
			//			cout << "Tile ID swap @ ";
						break;
					case DUALSWAP:
						numDualSwaps++;
			//			cout << "Tile+Pattern ID swap @ ";
						break;
					}
			//		cout << tileNum << ": " << thisProfit << " pixel improvement" << endl;
			//		sum += thisWeight;
					lastMosaic[tileNum] = curTile;
				}

			}
			// cout << "Total cycles used: " << sum << " out of " << BAGSIZE << endl;

			genPad(numDualSwaps, outfile);
			for (int i = 0; i < solution.size(); i++)
			{
				if (solution[i] && weight[i] == DUALSWAP)
				{
					int loc = tilePos[i];
					int extendedLoc = 0x5C20 + loc;
					int extendedHigh = extendedLoc / 0x100;
					int extendedLow = extendedLoc % 0x100;
					int val = chosenTiles[i].patternID;
					genPad(extendedLow, outfile);
					genPad(extendedHigh, outfile);
					genPad(chosenTiles[i].tileID, outfile);
					genPad(chosenTiles[i].patternID, outfile);
				}
			}

			genPad(numTileSwaps, outfile);
			for (int i = 0; i < solution.size(); i++)
			{
				if (solution[i] && weight[i] == TILESWAP)
				{
					int loc = tilePos[i];
					int ppuLoc = 0x2020 + loc;
					int ppuHigh = ppuLoc / 0x100;
					int ppuLow = ppuLoc % 0x100;
					int val = chosenTiles[i].tileID;
					genPad(ppuHigh, outfile);
					genPad(ppuLow, outfile);
					genPad(val, outfile);
				}
			}

			genPad(numPatternSwaps, outfile);
			for (int i = 0; i < solution.size(); i++)
			{
				if (solution[i] && weight[i] == PATTERNSWAP)
				{
					int loc = tilePos[i];
					int extendedLoc = 0x5C20 + loc;
					int extendedHigh = extendedLoc / 0x100;
					int extendedLow = extendedLoc % 0x100;
					int val = chosenTiles[i].patternID;
					genPad(val, outfile);
					genPad(extendedLow, outfile);
					genPad(extendedHigh, outfile);
				}
			}

			for (int i = curFrameInput; i < 198; i++)
			{
				genPad(0x00, outfile);
			}
			/*
			frameLimiter++;
			if (frameLimiter >= 100) break;
			*/
		}

	} while (FindNextFile(hFind, &FindData) > 0);
	system("pause");
}

//int main() {
/*
					//Find best overall match
					int bestMatch = 32 + abs(32 - bright) - 1;
					tile bestTile = brightnesses[0][0];
					if (bright > 32) bestTile = brightnesses[64][0];

					int minBright = 0;
					int maxBright = 64;
					if (bright > 32) minBright = 2 * (bright - 32);
					if (bright < 32) maxBright = 2 * bright;

					vector<int> brightList;

					for (int k = minBright; k < maxBright + 1; k++) {
						brightList.push_back(k);
					}



					for (int k = 0; k < brightList.size() - 1; k++)
					{
						int max_idx = k;
						for (int m = k + 1; m < brightList.size(); m++)
						{
							if (abs(bright - brightList[m]) < abs(bright - brightList[max_idx]))
							{
								max_idx = m;
							}
						}
						swap(brightList[max_idx], brightList[k]);
					}



					int curBright;
					int curMax;

					while (!brightList.empty())
					{
						curBright = brightList[0];
						curMax = max(bright, curBright);
						brightList.erase(brightList.begin());
						int curSize = brightnesses[curBright].size();
						for (int k = 0; k < curSize; k++)
						{
							tile curTarget = brightnesses[curBright][k];
							int thisMatch = 64 - bitDiff(aHash, curTarget.bHash);


							if (thisMatch > bestMatch) {
								bestMatch = thisMatch;
								bestTile = brightnesses[curBright][k];
								//cout << "New match of " << bestMatch << ": " << curTarget.bHash << endl;
								vector<int> toErase;

								for (int m = 0; m < brightList.size(); m++)
								{
									if (!(max(bright, brightList[m]) > bestMatch))
									{
										toErase.push_back(m);
									}
								}

								for (int m = toErase.size() - 1; m > -1; m--)
								{
									brightList.erase(brightList.begin() + toErase[m]);
								}

								if (!(max(bright, curBright) > bestMatch)) break;


							}
						}
					}

					mosaicTiles.push_back(bestTile);

				}
			}

			priority_queue<tile> bestTiles;
			for (int i = 0; i < mosaicTiles.size(); i++) {
				tile curTile = mosaicTiles[i];
				tile lastTile = lastMosaic[i];
				curTile.loc = i;
				curTile.diff = bitDiff(curTile.bHash, lastTile.bHash);
				if (curTile.diff > 0) bestTiles.push(curTile);
			}

			int counter = 0;
			vector<tile> chosenTiles;
			while (!bestTiles.empty())
			{
				tile curTile = bestTiles.top();
				bestTiles.pop();
				chosenTiles.push_back(curTile);
				counter++;
				if (counter >= 34) break;
			}

			for (int i = 0; i < chosenTiles.size(); i++)
			{
				tile curTile = chosenTiles[i];
				int loc = curTile.loc;

				int extAddr = 0x5C20 + loc;
				int extLow = extAddr % 0x100;
				int extHigh = extAddr / 0x100;
				genPad(extLow, outfile);
				genPad(extHigh, outfile);
				genPad(curTile.tileID, outfile);
				genPad(curTile.patternID, outfile);
			}

			genPad(0x00, outfile);
			genPad(0x00, outfile);

			for (int i = 0; i < chosenTiles.size(); i++)
			{
				tile curTile = chosenTiles[i];
				int loc = curTile.loc;
				lastMosaic[loc] = curTile;
			}

			frameLimiter++;
			if (frameLimiter >= 100) break;
		}
	} 

	*/
//}