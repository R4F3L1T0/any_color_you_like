#include <vector>
#include <algorithm>
#include <limits>
#include <iostream>
#include <fstream>
#include <ctime>
#include <string>


using namespace std;

int rnd(int a) {
	return rand()%a;
}


clock_t start = clock(); 

#define TURNTIME float(clock()-start)/CLOCKS_PER_SEC
#define INF numeric_limits<float>::infinity()
#define N_bins 320											// Number of bins of the polymer		
#define N_LABELS 7											// Number of different labels

class Solution;
class LeaderSearch;


////////////////
/// FUNCTIONS //
////////////////

// To read the polymer from a file
vector< vector<int> > read_poly(string file_name)
{
    ifstream file(file_name);
    vector< vector<int> > pol;

    int col;
    for (int i = 0; i < N_bins; ++i)
    {
    	vector<int> tmp;
    	for (int k = 0; k < N_LABELS; ++k)
    	{
    		file>>col;
    		tmp.push_back(col);
    	}
    	pol.push_back(tmp);
    }

    file.close();

    return pol;
}

// To save the polymer into a file
void save_poly(string file_name,vector< vector<int> >& pol)
{
	ofstream file(file_name);

	for (int i = 0; i <pol.size(); ++i)
	{
		for (int j = 0; j < pol[i].size(); ++j) file<<pol[i][j]<<" ";
		file<<endl;
	}

	file.close();
}

// To save the score (value of the overlap) into a file
void save_score(string file_name,int score)
{
	ofstream file(file_name);
	file<<score<<endl;
	file.close();
}
////////////////

///////////////
/// Classes ///
///////////////

/// Our Solution is a vector of ints that contains the coupling of labels
/// The index represents the reference set while the values their corresponding values 
/// Es. if solution[1]=3 it means that the label 1 in the reference correspond to the the label 3 in the other polymer
class Solution {
public:
	vector<int> sol;
	float score;

	Solution(){}
	Solution(int n)
	{
		sol.clear();
		for (int i = 0; i < N_LABELS; ++i)
			sol.push_back(i);

		random_shuffle(sol.begin(),sol.end());
		score = -INF;
	}

	// To chage the solution
	void mutate()
	{
		int r = rnd(100);
		if (r < 50)
		{
			int a = rnd(N_LABELS);
			int b = rnd(N_LABELS);

			while (a == b)
				b = rnd(N_LABELS);
			
			int tmp = sol[a];
			sol[a] = sol[b];
			sol[b] = tmp;
		}
		else if (r < 75)
		{
			int a = rnd(N_LABELS);
			int b = rnd(N_LABELS);
			int c = rnd(N_LABELS);

			while (a == b)
				b = rnd(N_LABELS);
			while (c == b || c == a)
				c = rnd(N_LABELS);

			int tmp = sol[a];
			sol[a] = sol[b];
			sol[b] = sol[c];
			sol[c] = tmp;			
		}

		score = -INF;
	}
	
};
/// Basic properties of the calss Search
class Search {
public:

	Solution best;
	vector< vector<int> > ref;
	vector< vector<int> > target;

	float score(Solution* rules)
	{
		float score = 0.;	
		for (int i = 0; i < N_bins; ++i)
			for (int k = 0; k < N_LABELS; ++k)	
				score+=min(ref[i][k],target[i][rules->sol[k]]);
			
		return score;
	}
};

/// To compare and order two solutions 
bool sol_comp(Solution s1, Solution s2)
{
	return s1.score > s2.score;
}

/// Completion of the class search, it uses Genetic Search with emplying the method mutate to change the solution
/// It's called LeaderSearch because when generating a new population it replicate more frequently the best solution
class LeaderSearch: public Search {
public:
	vector<Solution> population;
	vector<float> weights;
	int n_sol;
	
	LeaderSearch(int n_sol,vector< vector<int> > &ref, vector< vector<int> > &target)
	{
		this->ref = ref;
		this->target = target;
		this->n_sol = n_sol;

		for (int i = 0; i < n_sol; ++i)	population.push_back(Solution(N_LABELS));

		weights = vector<float>(N_LABELS,0);
		score_order_pop();
		best = population[0];
		reweight_rnd();
	}

/// To order the population of solutions 
	void score_order_pop()
	{
		for (int i = 0; i < n_sol; ++i)	population[i].score = score(&population[i]);
		sort(population.begin(), population.end(), sol_comp);
	}
	
/// To improve convergence labels are not extracted with equal probability: the labels that overlaps better are extracted less frequently 
	void reweight_rnd()
	{
		vector<float> scores;
		for (int z = 1; z < N_LABELS; ++z)
		{
			float s = 0.;
			int l = best.sol[z];
			int a=0,b=0;
			for (int i = 0; i < N_bins; ++i)
			{
				a+=min(ref[i][z],target[i][l]);
				b+=max(ref[i][z],target[i][l]);
			}
			s=a/b;
			scores.push_back(1-s);
		}

		float sum = 0.;
		for (int i = 0; i < N_LABELS; ++i)	sum += scores[i];
		for (int i = 0; i < N_LABELS; ++i)	scores[i] /= sum;
		
		sum = 0.;
		for (int i = 0; i < N_LABELS; ++i)
		{
			sum += scores[i];
			scores[i] = sum;
		}

		weights = vector<float>(scores);	
	}

	/// Calls the other methods to search the solution
	void search(int n_steps, int counter)
	{
		int k=0;
		for (int i = 0; i < n_steps; ++i)
		{

			gen_new_pop();
			score_order_pop();

			if (population[0].score > best.score)
			{
				best = population[0];
				k=0;
			}
			else
			{
				k++;
				if (k == counter) break;
			}
			reweight_rnd();
		}
	}

	/// Extract random number according to the lables overlap 
	int w_rnd()
	{
		float r = rnd(1000)/1000.;

		int i=0;
		for (i = 0; i < N_LABELS; ++i)
			if (weights[i] > r)
				break;
		return i;

	}

	/// To generate a new population
	void gen_new_pop()
	{
		for (int i = n_sol*0.75; i < n_sol; ++i) population[i] = best;

		for (int i = n_sol*0.5; i < n_sol*0.75; ++i) population[i] = Solution(N_LABELS);
	
		for( int i = 0; i < n_sol; ++i) population[i].mutate();
	}
	
};

int main()
{
	srand((unsigned)time(0));
	
	// Setting parameters
	int solution_population_size = 50;
	int max_number_of_iterations = 100000;
	int stopping_counter = 1000;			// max number of iteration without changing the best solution allowed. When reached the process is completed. 

	// Paths
	string path_ref="polymers/reference_polymer.txt";
	string path_tar="polymers/example.txt";
	string path_output="results/example_reordered.txt";
	
	// Reading the polymers 
	vector< vector<int> > ref = read_poly(path_ref);
	vector< vector<int> > tar = read_poly(path_tar);

	// Genetic Search in action
	LeaderSearch* solver = new LeaderSearch(solution_population_size, ref, tar);
	solver->search(max_number_of_iterations, stopping_counter);

	// Output 
	cout << "Best score: " << solver->best.score << endl;
	cout << "The best solution is: ";
	for (int i = 0; i < solver->best.sol.size(); ++i)	cout << solver->best.sol[i] << " ";
	cout << endl;	
	cout << "Time: " << TURNTIME << " s" << endl;
	cout<<"The process ended correctly"<<endl;

	// Saving the reordered polymer into a file
	vector< vector<int> > tmp = read_poly(path_ref);
	for (int i = 0; i < N_bins; ++i)
		for (int k = 0; k < N_LABELS; ++k) 
			tmp[i][k] = tar[i][solver->best.sol[k]];

	save_poly(path_output,tmp);
	
	return 0;
}