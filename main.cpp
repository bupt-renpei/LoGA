#include <iostream>
//#include <cstdlib> // For Random()
#include <sys/time.h> // For time record
//#include <time.h>
//#include <ctime>

#define EVOLUTION_NUM 50 // The number of evolution // 100
#define SOLUTION_DIM 8 // The dimension of the solution (AlexNet) // 8
#define POP_NUM 100 // The number of population in the group //100 (Note: %2==0)
#define P_CROSS 0.8 // The possibility of the crossover process
#define P_MUTE 0.005 // The possibility of the mutate process

#define DEVICE_MEC_NUM 3 // The number of device in MEC scenario (T+E+C) // 3
#define DEVICE_D2D_NUM 4 // The number of device in D2D scenario (T+T+T+T) // 4

// TODO measure
// ping
#define NETWORK_BANDWIDTH_TE 85 // The bandwidth of the network (mbps)
#define NETWORK_BANDWIDTH_EC 1024
#define NETWORK_BANDWIDTH_DD 700
#define NETWORK_LATENCY_TE 5 // The network latency in 5G networks (ms), which includes the process time of data packet
#define NETWORK_LATENCY_EC 35
#define NETWORK_LATENCY_DD 3

#define EARLY_EXIT_THRESHOLD 200 // This can be set manually by third-party // This can accelerate the process
#define Lo_ALAPH 50 // Penalty factor

using namespace std;

/*
 * Tool Part, which includes:
 * 1. Random() // generate an random number [0,1)
 * 2. Round() // 4/5
 * 3. Max() // print the index and value of the max number in an (int)array
 */

double Random()
{
    return (double)rand()/RAND_MAX;
}

int Round(double x)
{
    return (int)(x+0.5);
}

// return the index of the max value
int Max(int array[], int len)
{
    int Max_index = 0;
    int Max_value = array[0];
    for(int i = 1; i < len; i++)
    {
        if(array[i] > Max_value)
        {
            Max_index = i;
            Max_value = array[i];
        }
    }
    return Max_index;
}

// return the index of the min value
int Min(double array[], int len)
{
    int Min_index = 0;
    double Min_value = array[0];
    for(int i = 1; i < len; i++)
    {
        if(array[i] < Min_value)
        {
            Min_index = i;
            Min_value = array[i];
        }
    }
    return Min_index;
}

/*
 * Init Part, which includes:
 * 1. print_init_Latency_Cost_Array()
 * 2. print_init_Data_Trans_size()
 */

void print_init_Latency_Cost_Array(int latency_cost_arr[][SOLUTION_DIM], int line_NUM)
{
    for(int i = 0; i < line_NUM; i++)
    {
        for(int j = 0; j < SOLUTION_DIM-1; j++)
        {
            cout << latency_cost_arr[i][j] << " ";
        }
        cout << latency_cost_arr[i][SOLUTION_DIM-1] << endl;
    }
}

void print_init_Data_Trans_size(int data_trans_arr[], int row_NUM)
{
    for(int i = 0; i < row_NUM-1; i++)
    {
        cout << data_trans_arr[i] << " ";
    }
    cout << data_trans_arr[row_NUM-1] << endl;
}

/*
 * GA Part, which includes:
 * 0. GA_initGroup()
 * 1. GA_Fitness() // the fitness func.
 * 2. GA_Select() // the select func.
 * 3. GA_Crossover() // the crossover func.
 * 4. GA_Mutate() // the mutate func.
 */

/*
 * init the original population group
 * - 0 : computing at T side (40%)
 * - 1 : computing at E side (35%)
 * - 2 : computing at C side (25%)
 */
void GA_initGroup(int group[][SOLUTION_DIM])
{
    struct timeval NOW;
    gettimeofday(&NOW, NULL);
    srand(NOW.tv_sec*1000000 + NOW.tv_usec);

    for(int i = 0; i < POP_NUM; i++)
    {
        for(int j = 0; j < SOLUTION_DIM; j++)
        {
            if(Random() < 0.4)
                group[i][j] = 0; // compute at T side
            else if(Random() < 0.75)
                group[i][j] = 1; // compute at E side
            else
                group[i][j] = 2; // compute at C side
        }
    }
}

void print_GA_Group(int group[][SOLUTION_DIM])
{
    for(int i = 0; i < POP_NUM; i++)
    {
        for(int j = 0; j < SOLUTION_DIM-1; j++)
        {
            cout << group[i][j] << " ";
        }
        cout << group[i][SOLUTION_DIM-1] << endl;
    }
}

/*
 * the fitness of each solution is the total latency
 * Total_latency = Computing_latency + Trans_latency
 */
void GA_Fitness(int population[POP_NUM][SOLUTION_DIM], double fitness_arr[POP_NUM], int latency_cost_arr[][SOLUTION_DIM], int data_trans_size_arr[SOLUTION_DIM+1])
{
    int computing_latency[POP_NUM];
    double data_trans_latency[POP_NUM];
    double T_part[POP_NUM];

    int com_latency_per_solution_sum;
    double data_latency_per_solution_sum;
    int T_part_count;

    for(int i = 0; i < POP_NUM; i++)
    {
        com_latency_per_solution_sum = 0;
        data_latency_per_solution_sum = 0;
        T_part_count = 0;

        for(int j = 0; j < SOLUTION_DIM; j++)
        {
            int current_result_position = population[i][j];

            // computing_latency Array
            com_latency_per_solution_sum += latency_cost_arr[current_result_position][j];

            // T_part
            if(current_result_position == 0)
                T_part_count++;
        }

        for(int j = 0; j < SOLUTION_DIM; j++)
        {
            int current_result_position = population[i][j];
            int pass_result_position;

            // data_trans_latency Array
            if(j == 0)
                pass_result_position = 0;
            else
                pass_result_position = population[i][j-1];

            /*
             * For MEC scenario, it includes 0-1-2
             * For D2D scenario, it includes 0-1, but the 1 can be regard as 1_1, 1_2, 1_3
             * Here we detail MEC scenario as an example
             */
            double send_latency, trans_latency;
            switch(current_result_position)
            {
                case 0:
                    switch(pass_result_position)
                    {
                        case 0:
                            // data_latency_per_solution_sum += 0;
                            break;
                        case 1:
                            send_latency = data_trans_size_arr[current_result_position]*1.0/NETWORK_BANDWIDTH_TE;
                            trans_latency = NETWORK_LATENCY_TE*1.0/2; // as NETWORK_LATENCY_TE is the end-to-end latency, so it should be /2
                            break;
                        case 2:
                            send_latency = data_trans_size_arr[current_result_position]*1.0/NETWORK_BANDWIDTH_TE + data_trans_size_arr[current_result_position]*1.0/NETWORK_BANDWIDTH_EC;
                            trans_latency = (NETWORK_LATENCY_TE + NETWORK_LATENCY_EC)*1.0/2;
                            break;
                    }
                case 1:
                    switch(pass_result_position)
                    {
                        case 0:
                            send_latency = data_trans_size_arr[current_result_position]*1.0/NETWORK_BANDWIDTH_TE;
                            trans_latency = NETWORK_LATENCY_TE*1.0/2; // as NETWORK_LATENCY_TE is the end-to-end latency, so it should be /2
                            break;
                        case 1:
                            // data_latency_per_solution_sum += 0;
                            break;
                        case 2:
                            send_latency = data_trans_size_arr[current_result_position]*1.0/NETWORK_LATENCY_EC;
                            trans_latency = NETWORK_LATENCY_EC*1.0/2;
                            break;
                    }
                case 2:
                    switch(pass_result_position)
                    {
                        case 0:
                            send_latency = data_trans_size_arr[current_result_position]*1.0/NETWORK_BANDWIDTH_TE + data_trans_size_arr[current_result_position]*1.0/NETWORK_BANDWIDTH_EC;
                            trans_latency = (NETWORK_LATENCY_TE + NETWORK_LATENCY_EC)*1.0/2;
                            break;
                        case 1:
                            send_latency = data_trans_size_arr[current_result_position]*1.0/NETWORK_LATENCY_EC;
                            trans_latency = NETWORK_LATENCY_EC*1.0/2;
                            break;
                        case 2:
                            break;
                    }
            }
            data_latency_per_solution_sum += (send_latency + trans_latency);
        }
        computing_latency[i] = com_latency_per_solution_sum;

        // add the result trans latency to data_trans_latency[POP_NUM]
        data_trans_latency[i] = data_latency_per_solution_sum;
        switch(population[i][SOLUTION_DIM-1])
        {
            case 0:
                break;
            case 1:
                data_trans_latency[i] += (data_trans_size_arr[SOLUTION_DIM]*1.0/NETWORK_BANDWIDTH_TE + NETWORK_LATENCY_TE*1.0/2);
                break;
            case 2:
                data_trans_latency[i] += ((data_trans_size_arr[SOLUTION_DIM]*1.0/NETWORK_BANDWIDTH_TE + data_trans_size_arr[SOLUTION_DIM]*1.0/NETWORK_BANDWIDTH_EC) + ((NETWORK_LATENCY_TE + NETWORK_LATENCY_EC)*1.0/2));
                break;
        }

        // T_part
        T_part[i] = T_part_count*1.0/SOLUTION_DIM;
    }

    for(int i = 0; i < POP_NUM; i++)
    {
        // consider the Intent-oriented aspect (LoGA) at the same time
        fitness_arr[i] = computing_latency[i] + data_trans_latency[i] - T_part[i]*Lo_ALAPH;
    }
    // TODO Round(double -> int) to accelerate the computing
}

void print_GA_Fitness(double fitness_arr[POP_NUM])
{
    for(int i = 0; i < POP_NUM-1; i++)
    {
        cout << fitness_arr[i] << " ";
    }
    cout << fitness_arr[POP_NUM-1] << endl;
}
/*
 * The lower value (latency) have more opportunity to be selected
 */
void GA_Select(int Old_Population[POP_NUM][SOLUTION_DIM], int latency_cost_arr[][SOLUTION_DIM], int data_trans_size_arr[SOLUTION_DIM+1])
{
    int New_Population[POP_NUM][SOLUTION_DIM];
    double pr[POP_NUM], cum_pr[POP_NUM];
    GA_Fitness(Old_Population, pr, latency_cost_arr, data_trans_size_arr);

    double pr_sum = 0;
    for(int i = 0; i < POP_NUM; i++)
        pr_sum += pr[i];

    for(int i = 0; i < POP_NUM; i++)
        pr[i] = 1 - pr[i]/pr_sum;

    double re_pr_sum = 0;
    for(int i = 0; i < POP_NUM; i++)
        re_pr_sum += pr[i];

    for(int i = 0; i < POP_NUM; i++)
        pr[i] = pr[i]/re_pr_sum;

    cum_pr[0] = pr[0];
    for(int i = 1; i < POP_NUM; i++)
        cum_pr[i] = cum_pr[i-1] + pr[i];

    // Roulette Wheel Selection (RWS)
    struct timeval NOW;
    gettimeofday(&NOW, NULL);
    srand(NOW.tv_sec*1000000 + NOW.tv_usec);

    for(int i = 0; i < POP_NUM; i++)
    {
        double random = Random();
        for(int j = 0; j < POP_NUM-1; j++)
        {
            if(random < cum_pr[0])
            {
                for(int k = 0; k < SOLUTION_DIM; k++)
                    New_Population[i][k] = Old_Population[0][k];
                break;
            } else if(random >= cum_pr[j] && random <= cum_pr[j+1])
            {
                for(int k = 0; k < SOLUTION_DIM; k++)
                    New_Population[i][k] = Old_Population[j+1][k];
                break;
            }
        }
    }

    for(int i = 0; i < POP_NUM; i++)
        for(int j = 0; j < SOLUTION_DIM; j++)
            Old_Population[i][j] = New_Population[i][j];
}

void GA_Crossover(int New_Population[POP_NUM][SOLUTION_DIM])
{
    /*
     * Choose the chromosome that need cross
     * record it's index in the array
     */
    int need_cr[POP_NUM] = {0}; // cross pool

    struct timeval NOW;
    gettimeofday(&NOW, NULL);
    srand(NOW.tv_sec*1000000 + NOW.tv_usec);

    int count = 0;
    for(int i = 0; i < POP_NUM; i++)
    {
        if(Random() < P_CROSS)
        {
            need_cr[count++] = i;
        }
    }
    for(int i = 0; i< count-1; i++) // Test
    {
        cout << need_cr[i] << "-";
    }
    cout << need_cr[count-1] << endl;

    if(count % 2 != 0)
        count++;

    cout << "The value of the count is: " << count << endl;

    /*
     * Choose the cross point
    */
    int cr_point = 0;
    while(cr_point == 0)
        cr_point = rand()%SOLUTION_DIM;
    // cout << cr_point << endl; // Test

    /*
     * Cross
     */
    int cr_temp;
    for(int i = 0; i < count; i+=2)
    {
        for(int j = cr_point; j < SOLUTION_DIM; j++)
        {
            cr_temp = New_Population[need_cr[i]][j];
            New_Population[need_cr[i]][j] = New_Population[need_cr[i+1]][j];
            New_Population[need_cr[i+1]][j] = cr_temp;
        }
    }
}

void GA_Mutate(int New_Population[POP_NUM][SOLUTION_DIM], int scenario)
{
    struct timeval NOW;
    gettimeofday(&NOW, NULL);
    srand(NOW.tv_sec*1000000 + NOW.tv_usec);

    for(int i = 0; i < POP_NUM; i++)
    {
        for(int j = 0; j < SOLUTION_DIM; j++)
        {
            if(Random() < P_MUTE)
            {
                New_Population[i][j] = rand()%scenario;
            }
        }
    }
}

/*
 * Main Part.
 */

int main() {
    struct timeval START;
    struct timeval END;
    gettimeofday(&START, NULL);

    cout << "Hello, World!" << endl;

    // init latency cost array, this should be set manually in advance
    int latency_cost_array_mec_scenario[DEVICE_MEC_NUM][SOLUTION_DIM] = {{80, 79, 80, 68, 67, 46, 57, 36},{12, 23, 14, 16, 21, 17, 19, 13},{12, 16, 18, 19, 21, 13, 15, 17}}; // e.g., AlexNet: [3: TEC][8]
    print_init_Latency_Cost_Array(latency_cost_array_mec_scenario, DEVICE_MEC_NUM);
//    int latency_cost_array_d2d_scenario[DEVICE_D2D_NUM][SOLUTION_DIM] = {0}; // = {{},{},{},{}};
//    print_init_Latency_Cost_Array(latency_cost_array_d2d_scenario, DEVICE_D2D_NUM);

    // init data transmission size array
    /*
     * use AlexNet for an example
     * input --> Conv1 --> Conv2 --> Conv3 --> Conv4 --> Conv5 --> FC1 --> FC2 --> FC3 --> Result
     * [0, row_NUM-2]: the input size for per layer
     * [row_NUM-1]: the size of the result
     *
     * Note: the value should be set manually
     */
    int data_trans_size_array[SOLUTION_DIM+1] = {30, 40, 50, 60, 55, 43, 78, 34, 2}; // the input size of per layer + the size of output
    print_init_Data_Trans_size(data_trans_size_array, SOLUTION_DIM+1);

    /*
     * GA Part:
     * 1. GA_initGroup()
     * 2. loop
     *      GA_Fitness(); // Fitness
     *      GA_Select(); // Select
     *      GA_Crossover(); // Crossover
     *      GA_Mutate(); // Mutate
     */

    int Population[POP_NUM][SOLUTION_DIM];
    GA_initGroup(Population); // init the population
    cout << "init Group" << endl;
    print_GA_Group(Population); // print the population

    double fitness_value[POP_NUM];
    GA_Fitness(Population, fitness_value, latency_cost_array_mec_scenario, data_trans_size_array); //  get the fitness value of the current population
    print_GA_Fitness(fitness_value);

    for(int i = 0; i < EVOLUTION_NUM; i++)
    {
        cout << "The " << i << "th generation." << endl;

        cout << "-----Select-----" << endl;
        GA_Select(Population, latency_cost_array_mec_scenario, data_trans_size_array); // Select
        print_GA_Group(Population); // Test

        cout << "-----Crossover-----" << endl;
        GA_Crossover(Population); // Crossover
        print_GA_Group(Population); // Test

        cout << "-----Mutate-----" << endl;
        GA_Mutate(Population, DEVICE_MEC_NUM); // Mutate
        print_GA_Group(Population); // Test

        GA_Fitness(Population, fitness_value, latency_cost_array_mec_scenario, data_trans_size_array); //  get the fitness value of the current population
        print_GA_Fitness(fitness_value);

        double current_best_fitness = fitness_value[Min(fitness_value, POP_NUM)];
        if(current_best_fitness < EARLY_EXIT_THRESHOLD)
        {
            cout << "The best fitness is " << current_best_fitness << endl;
            break;
        }
        cout << "The best fitness is " << current_best_fitness << endl;
    }

    gettimeofday(&END, NULL);
    unsigned long Time_Cost = (END.tv_sec - START.tv_sec) * 1000000 + (END.tv_usec - START.tv_usec);
    cout << "Time cost: " << Time_Cost << " us." << endl;
    return 0;
}
// TODO to record the min value of per generation
// Note: the parameters should be considered carefully, as the result seems not optimized