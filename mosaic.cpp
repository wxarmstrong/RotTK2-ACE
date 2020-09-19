#include "CImg.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>
#include <sstream>

using namespace cimg_library;
using namespace std;

/*
class tile {
public:
	string bHash;
//	string path;
	int tileID;
	int patternID;
};

*/

/*
int main() {
	string path = "tiles";
	string searchPattern = "*.png";
	string fullSearchPath = path + searchPattern;
	WIN32_FIND_DATA FindData;
	HANDLE hFind;

	hFind = FindFirstFile(fullSearchPath.c_str(), &FindData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		cout << "Error searching directory\n";
	}

	vector<vector<tile>> brightnesses(0);
	for (int i = 0; i < 65; i++) {
		vector<tile> thisBrightness(0);
		brightnesses.push_back(thisBrightness);
	}

	ofstream root("root.txt");

	do
	{
		string filePath = path + FindData.cFileName;
		tile newTile;

		CImg<unsigned char> img(filePath.c_str());
		int brightness = 0;
		string curHash = "";

		for (int i = 0; i < 8; i++) {
			int numHash = 0;
			ostringstream tempHash;

			for (int j = 0; j < 8; j++)
			{
				numHash = numHash << 1;
				if (img(i, j, 0, 0)) {
					brightness++;
					numHash += 1;
				}
			}
			tempHash << std::hex << setw(2) << std::setfill('0') << uppercase<< numHash;
			curHash += tempHash.str();
		}

		newTile.hash = curHash;
		newTile.path = filePath;
		
		int palette = int(newTile.path[36] - 'A');
		int num = stoi(newTile.path.substr(38, 5)) - 10000;
		int pageNum = num / (16 * 16);

		newTile.tileID = num % (16 * 16);
		newTile.patternID = 64 * palette + pageNum;
		

		brightnesses[brightness].push_back(newTile);

		cout << "Added " << filePath << ": " << newTile.hash << " (" << setw(2) << setfill('0') << hex << uppercase << newTile.tileID << "," << setw(2) << setfill('0') << hex << uppercase << newTile.patternID << ")" << endl;
	} while (FindNextFile(hFind, &FindData) > 0);

	for (int i = 0; i < 65; i++) {
		cout << "Brightness: " << i << endl;

		cout << "Size: " << brightnesses[i].size() << endl;

		if (brightnesses[i].size() >= 2) {
			for (int j = 0; j < brightnesses[i].size() - 1; j++) {
				vector<int> matches;
				CImg<unsigned char> left(brightnesses[i][j].path.c_str());
				for (int k = j + 1; k < brightnesses[i].size(); k++) {
					if (brightnesses[i][j].hash == brightnesses[i][k].hash)
					{
						cout << brightnesses[i][j].path << " is a match with " << brightnesses[i][k].path << endl;
						matches.push_back(k);
					}
				}
				for (int k = matches.size() - 1; k > -1; k--)
				{
					brightnesses[i].erase(brightnesses[i].begin() + matches[k]);
				}
			}
		}
	}

	cout << endl << endl;

	for (int i = 0; i < 65; i++)
	{
		vector<tile> curBrightness = brightnesses[i];
		int size = curBrightness.size();
		root << size << " ";
		for (int j = 0; j < size; j++)
		{
			tile curTile = curBrightness[j];
			root << curTile.hash << " " << curTile.tileID << " " << curTile.patternID << " ";
		}
	}

	root.close();

	system("pause");

}
*/

class tile {
public:
	string bHash;
	int tileID;
	int patternID;
};

int max(int bright1, int bright2) {
	return 64 - abs(bright1 - bright2);
}

/*
int matchTiles(CImg<unsigned char> left, CImg<unsigned char> right, int maxRemain, int need)
{
	int match = 0;
	int total = 64;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if ( ( left(i, j, 0, 0) > 0) == ( right(i, j, 0, 0) > 0) )
			{
				match++;
				maxRemain--;
				need--;
			}
			total--;
			if (total < maxRemain) maxRemain = total;
			if (need > maxRemain) return 0;
		}
	}
	return match;
}
*/

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
	ifstream inFile;
	inFile.open("root.txt");

	vector<vector<tile>> brightnesses;

	for (int i = 0; i < 65; i++) {
		vector<tile> curBright;
		cout << "Loading brightness " << i << ": ";
		int size;
		inFile >> size;
		cout << "Size " << size << endl;
		for (int j = 0; j < size; j++)
		{
			tile curTile;
			inFile >> curTile.bHash;
			inFile >> curTile.tileID;
			inFile >> curTile.patternID;
			curBright.push_back(curTile);
		}
		brightnesses.push_back(curBright);
		cout << "Added brightness " << i << ": " << curBright.size() << endl;
	}

	string path = "frames";
	string searchPattern = "*.png";
	string fullSearchPath = path + searchPattern;
	WIN32_FIND_DATA FindData;
	HANDLE hFind;

	hFind = FindFirstFile(fullSearchPath.c_str(), &FindData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		cout << "Error searching directory\n";
	}

	ofstream outfile;
	outfile.open("mosaic2.txt");
	

	vector<tile> lastMosaic;
	tile blankTile;
	blankTile.tileID = 0;
	blankTile.patternID = 0;
	for (int i = 0; i < 32*29; i++)
	{
		lastMosaic.push_back(blankTile);
	}

	do
	{
		
		vector<tile> mosaicTiles;
		string filePath = path + FindData.cFileName;
		cout << filePath << endl;
		CImg<unsigned char> sourceFrame(filePath.c_str());
		for (int j = 0; j < 29; j++)
		{
			for (int i = 0; i < 32; i++)
			{
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

				//cout << j << "," << i << " " << "Brightness " << bright << endl;
				if (bright == 0)
				{
					mosaicTiles.push_back(brightnesses[0][0]);
					continue;
				}
				if (bright == 64)
				{
					mosaicTiles.push_back(brightnesses[64][0]);
					continue;
				}
				
				

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
						if ( abs(bright - brightList[m]) < abs(bright-brightList[max_idx]) )
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
					brightList.erase( brightList.begin() );
					int curSize = brightnesses[curBright].size();
					for (int k = 0; k < curSize; k++)
					{
						tile curTarget = brightnesses[curBright][k];
						int thisMatch = 64 - bitDiff(aHash, curTarget.bHash);

						/*
						string tilePath = brightnesses[curBright][k];
						CImg<unsigned char> curTile(tilePath.c_str());
						*/

						if (thisMatch > bestMatch) {
							bestMatch = thisMatch;
							bestTile = brightnesses[curBright][k];
							//cout << "New match of " << bestMatch << ": " << curTarget.bHash << endl;
							vector<int> toErase;

							for (int m = 0; m < brightList.size(); m++)
							{
								if ( !(max(bright, brightList[m]) > bestMatch) )
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

		for (int i = 0; i < mosaicTiles.size(); i++) {
			tile curTile = mosaicTiles[i];
			outfile << hex << setw(2) << setfill('0') << uppercase << curTile.patternID << " ";
			if (i % 32 == 31) outfile << endl;
		}

		outfile << endl;

		for (int i = 0; i < mosaicTiles.size(); i++) {
			tile curTile = mosaicTiles[i];
			outfile << hex << setw(2) << setfill('0') << uppercase << curTile.tileID << " ";
			if (i % 32 == 31) outfile << endl;
		}

		outfile << endl << endl;

		/*

		for (int i = 0; i < mosaicTiles.size(); i++)
		{
			tile curTile = mosaicTiles[i];
			tile lastTile = lastMosaic[i];

			if (curTile.tileID != lastTile.tileID)
			{
				int ppuAddr = 0x2020 + i;
				int low = ppuAddr % 0x100;
				int high = ppuAddr / 0x100;
				outfile << "A9 " << hex << setw(2) << setfill('0') << uppercase << high;
				outfile << " 8D 06 20 A9 " << hex << setw(2) << setfill('0') << uppercase << low;
				outfile << " 8D 06 20 ";
				outfile << "A9 " << hex << setw(2) << setfill('0') << uppercase << curTile.tileID;
				outfile << " 8D 07 20 ";
			}
			if (curTile.patternID != lastTile.patternID)
			{
				int ramAddr = 0x5C20 + i;
				int low = ramAddr % 0x100;
				int high = ramAddr / 0x100;
				outfile << "A9 " << hex << setw(2) << setfill('0') << uppercase << curTile.patternID;
				outfile << " 8D " << hex << setw(2) << setfill('0') << uppercase << low;
				outfile << " " << hex << setw(2) << setfill('0') << uppercase << high << " ";
			}

		}

	    outfile << "2C 02 20 A9 00 8D 05 20 8D 05 20 ";

		outfile << "20 A3 FC ";

		*/


	} while (FindNextFile(hFind, &FindData) > 0);

	



	system("pause");
}
