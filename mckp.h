#include <algorithm>
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

class VectorUtils
{
public:
	static vector<vector<int>> IntVector(int size1, int size2)
	{
		vector<vector<int>> newVector(size1);
		for (int vector1 = 0; vector1 < size1; vector1++)
		{
			newVector[vector1] = vector<int>(size2);
		}

		return newVector;
	}

	static vector<vector<bool>> BoolVector(int size1, int size2)
	{
		vector<vector<bool>> newVector(size1);
		for (int vector1 = 0; vector1 < size1; vector1++)
		{
			newVector[vector1] = vector<bool>(size2);
		}

		return newVector;
	}
};

class ComputeSolution {
private:
	int knapsackCapacity = 0;
	vector<int> profit;
	vector<int> weight;
	vector<int> classes;
public:
	ComputeSolution(int newCapacity, vector<int> newProfit, vector<int> newWeight, vector<int> newClasses);
	int getKnapsackCapacity();
	vector<int> getItemProfit();
	vector<int> getItemWeight();
	vector<int> getItemClass();
	bool checkClass(int const items, int const last, vector<int>& classes);
	bool compute(int const items, int const object, int const q, vector<int>& classes, vector<vector<int>>& matrix);
	bool getSolution(int const profitLast, int const profitCurrent);
	int getProfit(int const items, int const itemClass, vector<int>& knapsackCapacity, vector<int>& classes);
};

ComputeSolution::ComputeSolution(int newCapacity, vector<int> newProfit, vector<int> newWeight, vector<int> newClasses)
{
	this->knapsackCapacity = newCapacity;
	this->profit = newProfit;
	this->weight = newWeight;
	this->classes = newClasses;
}

int ComputeSolution::getKnapsackCapacity()
{
	return knapsackCapacity;
}

vector<int> ComputeSolution::getItemProfit()
{
	return profit;
}

vector<int> ComputeSolution::getItemWeight()
{
	return weight;
}

vector<int> ComputeSolution::getItemClass()
{
	return classes;
}

bool ComputeSolution::checkClass(int const items, int const last, vector<int>& classes)
{
	return classes[items] == last;
}

bool ComputeSolution::compute(int const items, int const object, int const q, vector<int>& classes, vector<vector<int>>& matrix)
{
	int maxProfitForEachItem = 0;
	for (int k = 1; k <= items; k++)
	{
		if (classes[k] == classes[object] && matrix[q][k] > maxProfitForEachItem)
		{
			maxProfitForEachItem = matrix[q][k];
		}
	}
	return matrix[q][object] == maxProfitForEachItem;
}

bool ComputeSolution::getSolution(int const profitLast, int const profitCurrent)
{
	return profitCurrent > profitLast;
}

int ComputeSolution::getProfit(int const items, int const itemClass, vector<int>& knapsackCapacity, vector<int>& classes)
{
	int maxProfit = 0;
	for (int k = 1; k < items; k++)
	{
		if (classes[k] == itemClass && knapsackCapacity[k] > maxProfit)
		{
			maxProfit = knapsackCapacity[k];
		}
	}
	return maxProfit;
}

class MultipleChoiceKnapsackProblem {
private:
	ComputeSolution* computeSolution;
	int maxProfit = 0;
public:
	MultipleChoiceKnapsackProblem(ComputeSolution* const computeSolution);
	vector<int> solveProblem();
private:
	void updateMaxProfit(int const newProfit);
	int computeMaxProfit(int const profitLast, int const profitCurrent);
	vector<int> isInSolution(int const item, int const knapsackCapacity, const vector<vector<bool>>& solution,
		vector<int>& classes, vector<vector<int>>& matrix, const vector<int>& weight);
};

MultipleChoiceKnapsackProblem::MultipleChoiceKnapsackProblem(ComputeSolution* const computeSolution)
{
	this->computeSolution = computeSolution;
}

vector<int> MultipleChoiceKnapsackProblem::solveProblem()
{
	int knapsackCapacity = computeSolution->getKnapsackCapacity();
	vector<int> profit = computeSolution->getItemProfit();
	vector<int> weight = computeSolution->getItemWeight();
	vector<int> classes = computeSolution->getItemClass();

	int items = profit.size() - 1;
	vector<vector<int>> matrix = VectorUtils::IntVector(knapsackCapacity + 1, items + 1);
	vector<vector<bool>> solution = VectorUtils::BoolVector(knapsackCapacity + 1, items + 1);

	// Define q number in table
	for (int q = 0; q <= knapsackCapacity; q++)
	{
		matrix[q][0] = q;
	}

	// compute max profit for each object
	for (int q = 1; q <= knapsackCapacity; q++)
	{
		for (int k = 1; k <= items; k++)
		{
			int profitLast = computeSolution->getProfit(k, classes[k] - 1, matrix[q], classes);
			int profitCurrent = 0;
			if (weight[k] <= q)
			{
				profitCurrent = profit[k] + computeSolution->getProfit(k, classes[k] - 1, matrix[q - weight[k]], classes);
			}
			matrix[q][k] = computeMaxProfit(profitLast, profitCurrent);
			updateMaxProfit(matrix[q][k]);
			solution[q][k] = computeSolution->getSolution(profitLast, profitCurrent);
		}
	}

	return isInSolution(items, knapsackCapacity, solution, classes, matrix, weight);
}

void MultipleChoiceKnapsackProblem::updateMaxProfit(int const newProfit)
{
	if (newProfit > maxProfit)
	{
		maxProfit = newProfit;
	}
}

int MultipleChoiceKnapsackProblem::computeMaxProfit(int const profitLast, int const profitCurrent)
{
	return max(profitLast, profitCurrent);
}

vector<int> MultipleChoiceKnapsackProblem::isInSolution(int const item, int const knapsackCapacity, const vector<vector<bool>>& solution,
	vector<int>& classes, vector<vector<int>>& matrix, const vector<int>& weight)
{
	int lastSolution = 0;
	vector<int> vectorOfSolution(item + 1);
	for (int k = item, q = knapsackCapacity; k > 0; k--)
	{
		if (solution[q][k] && computeSolution->compute(item, k, q, classes, matrix))
		{
			if (computeSolution->checkClass(k, lastSolution, classes))
			{
				continue;
			}
			q = q - weight[k];
			lastSolution = classes[k];
			vectorOfSolution[k] = 1;
		}
		else
		{
			vectorOfSolution[k] = 0;
		}
	}
	return vectorOfSolution;
}

/*
int main()
{
	int capacity = 50;
	vector<int> profit = { 0, 15, 11, 5, 8, 12, 18, 20, 14 , 5, 30, 5 };
	vector<int> weight = { 0, 8, 4, 4, 3, 5, 14, 11, 5, 3, 20, 10 };
	vector<int> classes = { 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4 };

	ComputeSolution* c = new ComputeSolution(capacity, profit, weight, classes);
	MultipleChoiceKnapsackProblem* k = new MultipleChoiceKnapsackProblem(c);
	vector<int> solution = k->solveProblem();
	int sum = 0;
	for (int i = 0; i < solution.size(); i++)
	{
		if (solution[i]) {
			sum += weight[i];
			cout << profit[i] << " ";
		}
	}
	cout << endl;
	cout << "Total weight: " << sum;

	return 0;
}
*/