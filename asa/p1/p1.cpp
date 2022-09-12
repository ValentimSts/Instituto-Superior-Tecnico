/**
 * @author
 *   Grupo: al012
 *     - Valentim Santos ist199343
 *     - Tiago Santos ist199333
 */

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>


void computeInput();
void solveProblem1(std::vector<int> &values);
void LIS(std::vector<int> values);
void solveProblem2(std::vector<std::vector<int>> &values);
void LCIS(std::vector<int> &values1, std::vector<int> &values2, int i, int j);


int main()
{
    computeInput();
    return 0;
}


/**
 * @brief 
 *   reads the input given and executes the program
 *   accordingly. If the first int read is a 1 we want
 *   to solve problem1, if its a 2 we want to solve
 *   problem2.
 */
void computeInput()
{

    int problem, val;

    std::cin >> problem;

    if(!std::cin) {
        std::cerr << "input error" << std::endl;
        exit(1);
    }
    
    if(problem == 1) {

        std::vector<int> values1;

        while(std::cin >> val) {
            values1.push_back(val);
        }

        solveProblem1(values1);
    }
    else if(problem == 2) {

        bool first_vector = true;

        std::unordered_map<int, int> dupe_values;
        std::vector<std::vector<int>> values2;
        std::string line;

        while(getline(std::cin, line)) {
            
            if(!line.empty()) {

                std::stringstream stream(line);
                std::vector<int> aux;

                // only includes in the second vector the values common
                // to the first one, since these are skipped either way
                while(stream >> val) {
                    if(first_vector) {
                        dupe_values[val]++;
                        aux.push_back(val);
                    }
                    else 
                        if(dupe_values[val] > 0) 
                            aux.push_back(val);
                }

                first_vector = false;
                values2.push_back(aux);
            }
        }

        solveProblem2(values2);
    }
    else {
        std::cerr << "invalid input...ending" << std::endl;
    }
}


/* Problem 1 */
void solveProblem1(std::vector<int> &values)
{
    LIS(values);
}


/**
 * @brief
 *   finds the length of the longest increasing subsequence of a sequence, aswell as the number
 *   of subsequences of that same length.
 * 
 * std::vector<int> lengths:
 *   - vector containing the length of the longest increasing subsequence ending at
 *     each index. (lengths[2] stores the length of the LCS ending on index 2)
 * 
 * std::vector<int> counts:
 *   - vector containing the number of subsequences of max length ending at each
 *     index. (if there are two LCSs ending on index 3, counts[3] = 2)
 * 
 * @param values 
 */
void LIS(std::vector<int> values)
{
    int size = values.size();
    int max_len = 1;

    if (size <= 1) {
        std::cout << size << " " << max_len << std::endl;
        return;
    }

    // stores the number of subsequences of max length.
    int LIS_counter = 0;
    
    // initializes both the lengths and counts arrays with 1, as the maximum length of 
    // any subsequence is atleast one, and there also exists atleast one subsequence of
    // such length.
    std::vector<int> lengths(size, 1);
    std::vector<int> counts(size, 1);
    
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < i; j++) {
            
            // if the current value is greater than any of the previous ones, it means 
            // that this value is apart of a new subsequence.
            if(values[i] > values[j]) {
                
                // since values[i] is now the greatest element of the LIS ending on index
                // i, if lengths[i] is less than or equal to the lengths[j], then lengths[i] 
                // should be 1 bigger than lengths[j] since values[i] is greater than values[j]
                // and values[i] is the newest element to the LIS.
                if(lengths[i] <= lengths[j]) {
                    lengths[i] = lengths[j] + 1;

                    // updates the current max lis length, if necessary.
                    if(lengths[i] > max_len) {
                        max_len = lengths[i];
                        LIS_counter = 0;
                    }

                    counts[i] = counts[j];
                }

                // if the length of the LIS ending on index i (lengths[i]) - 1 is equal to lengths[j]
                // (length of an LIS without values[i]) it means that including values[i] in the
                // LIS ending on index j wouldn't increase its size but rather result in a different
                // LIS of that same size, and so we increment counts[i] accordingly.
                else if(lengths[i] - 1 == lengths[j])
                    // the number of LISs is continuous, and so, the new value of counts
                    // will be calculated using the previous ones.
                    counts[i] += counts[j];
            }
        }

        if(lengths[i] == max_len)
            LIS_counter += counts[i];
    }

    std::cout << max_len << " " << LIS_counter << std::endl;
}


/* Problem 2 */
void solveProblem2(std::vector<std::vector<int>> &values)
{
    int i = values[0].size();
    int j = values[1].size();

    LCIS(values[0], values[1], i, j);  
}


/**
 * @brief 
 *   finds the length of the longest common subsequence between two
 *   integer sequences given.
 * 
 * std::vector<int> lcis:
 *   - vector containing the length of the longest common increasing subsequence
 *     ending at each index. (if the length of the LCIS ending on i is 3, then lcis[i] = 3)
 * 
 * @param values1
 *   the first sequence.
 * @param values2
 *   the second sequence.
 * @param len1
 *   sequence one's length.
 * @param len2
 *   sequence two's length.
 */
void LCIS(std::vector<int> &values1, std::vector<int> &values2, int len1, int len2)
{
    int size = 0;

    // finds the longest vector to know how big the lcis vector should be.
    if(len1 > len2)
        size = len1;
    else
        size = len2;

    // we initialize the lcis vector as 0 since at the start there
    // are no known common increasing subsequences.
    std::vector<int> lcis(size, 0);

    // stores the length of the longest common increasing subsequence.
    int max_len = 0;

    for(int i = 0; i < len1; i++) {
        
        // the initial length of our current lcis will always be 0.
        int curr_lcis_len = 0;

        for(int j = 0; j < len2; j++) {
            
            // if both the values of the vectors are the same, then, values2[j] is an
            // element of an LCIS ending on index j.
            if(values1[i] == values2[j]) {
                // if our current lcis length is greater than or equal to the lcis length
                // ending on index j, since values[j] is a new element to the lcis, the length
                // of the lcis ending on j will be incremented.
                if(curr_lcis_len >= lcis[j]) {
                    lcis[j] = curr_lcis_len + 1;

                    // updates the current max lcis length, if necessary.
                    if(curr_lcis_len + 1 > max_len)
                        max_len = curr_lcis_len + 1;
                }
            }

            // if values1[i] is greater than values2[j] then values2[j] is not an element
            // of our current LCIS.
            else if(values1[i] > values2[j]) {
                // and so, if the length of the LCIS ending on j is greater than our current one,
                // we simply update curr_lcis_len.
                if(lcis[j] > curr_lcis_len)
                    curr_lcis_len = lcis[j];
            }
        }
    }

    std::cout << max_len << std::endl;
}