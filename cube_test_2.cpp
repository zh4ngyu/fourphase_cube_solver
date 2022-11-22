//#include <stdio.h>
//#include <math.h>
#include <string>
#include <iostream>
#include <time.h>









void operate_edge(int* cube, int* operation, int operation_leng);
void operate_corner_position(int* cube, int* operation, int operation_leng);
void operate_corner_orientation(int* cube, int* operation, int operation_leng);
void operate(int* cube, int* operation, int operation_leng);
void operate_phase_check(int* cube, int* operation, int operation_leng, int phase_ind);

void show_cube(int* cube);
int check_cube(int* cube, int phase_ind);

int* translate(char* mixUp, int mix_leng);

int* solve_phase(int* cube, int* init_case, int* crit_ind, int phase_ind, int max_moves);
int** solve_cube(int* mix_sequence, int mix_leng, int phase_max_moves[4]);
void print_solution(int** solution, int phase_max_moves[4], int op_transform[19]);


int* new_cube(){
    int* cube = new int[28]{0,1,2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15, 16,17,18,19, 0,0,0,0, 0,0,0,0};
    return cube;
}
void copy_cube(int* cube, int* copy_){
    for (int i = 0; i < 28; i++){
        copy_[i] = cube[i];
    }
}

void moves_order(int orientation_ind){
    static char op[3][20] = {"Yellow-white", "Orange-red", "Blue-green"};
    static int order[6][3] = {
        {0, 1, 2},
        {0, 2, 1},
        {2, 1, 0},
        {2, 0, 1},
        {1, 0, 2},
        {1, 2, 0},
    };
    printf("\n%s > %s > %s", op[order[orientation_ind][0]], op[order[orientation_ind][1]], op[order[orientation_ind][2]]);
}


// 5 is the length limit of each operation name
char operation_name[19][5] = {
    " ","L","L'","L2","R","R'","R2","U","U'","U2","D","D'","D2","F","F'","F2","B","B'","B2"
};




//D2 R' B' R2 U2 F' D L2
//char mixup[] = { "R' D U2 B2 L2 B D2 B2 F L2 F' D2 L R2 F2 U' B D U2" };



int main(){
    // create a cube and show in console ====================================================
    int* cube = new_cube();
    show_cube(cube);


    // get scramble sequence and translate to operation index ===============================
    std::string scramble;
    char char_string[500];

    std::cout << "Enter scramble sequence: \n";
    std::getline(std::cin, scramble);
    strcpy_s(char_string, scramble.c_str());

    int length = 0;
    for (int i = 0; char_string[i] != '\0'; i++){
        length++;
    }
        
    char* mix = new char[length];
    for (int i = 0; i < length; i++){
        mix[i] = char_string[i];
    }
        
    int mix_leng = length;
    int* mix_sequence = translate(mix, mix_leng);


    // scramble the cube ====================================================================
    operate(cube, mix_sequence, mix_leng);
    show_cube(cube);




    // transform mix sequence to 6 orientaion and solve =====================================

    int op_transform[6][19] = {
        {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18},
        {0,16,17,18,13,14,15,7,8,9,10,11,12,1,2,3,4,5,6},
        {0,1,2,3,4,5,6,16,17,18,13,14,15,7,8,9,10,11,12},
        {0,16,17,18,13,14,15,4,5,6,1,2,3,7,8,9,10,11,12},
        {0,7,8,9,10,11,12,4,5,6,1,2,3,13,14,15,16,17,18},
        {0,7,8,9,10,11,12,13,14,15,16,17,18,1,2,3,4,5,6}
    };

    int total_orientation = 6;
    int phase_max_moves[4] = {6, 6, 7, 8};

    printf("\nplease wait...\n");

    for (int i = 0; i < total_orientation; i++) {

        // transform mix sequence ===========================================================
        int* mix_transform = new int[mix_leng];
        for (int j = 0; j < mix_leng; j++){
            mix_transform[j] = op_transform[i][mix_sequence[j]];
        }
            

        // solve cube and time ==============================================================
        clock_t start, end;
        start = clock();

        int** solution = solve_cube(mix_transform, mix_leng, phase_max_moves);

        end = clock();
        float time = end - start;
        printf("\n\n%f seconds elapsed", (time) / CLOCKS_PER_SEC);


        // print the solution ===============================================================
        moves_order(i);
        printf("\nsolution %d/6: \n", i + 1);
        print_solution(solution, phase_max_moves, op_transform[i]);
        printf("\n\n");
    }

    
    // clean RAM
    delete[] cube;

    
    printf("\n\n2022-11-06\n525080170@qq.com\n\npress ENTER to exit\n");
    getchar();
    
    return 0;
}






void print_solution(int** solution, int phase_max_moves[4], int op_transform[19]){

    if (solution[0][0] == 404){
        printf("\nfound no solution under the moves limit");
        return;
    }

    int solution_leng[4]{0,0,0,0};
    int total_leng = 0;

    for (int j = 0; j < 4; j++) {
        for (int m = 0; m < phase_max_moves[j]; m++){
            if (!solution[j][m]){
                continue;
            }
            solution_leng[j]++;
        }
        total_leng = total_leng + solution_leng[j];
    }

    for (int j = 0; j < 4; j++) {
        printf("\n");
        for (int m = 0; m < solution_leng[j]; m++){

            // to avoid R' or R2 after R, etc. ==============================================
            if(j<3 && m == solution_leng[j]-1 && solution[j+1][0]){
                if((solution[j][m]-1) / 3 == (solution[j+1][0]-1) / 3){

                    total_leng--;

                    int trans[3] = {1, 3, 2};
                    int trans_back[4] = {0, 1, 3, 2};
                    int op = (trans[(solution[j][m]-1) % 3] + trans[(solution[j+1][0]-1) % 3]) % 4;
                    //printf("change to %d\n", trans_back[op] + ((solution[j+1][0]-1) / 3) * 3);

                    solution[j][m] = trans_back[op] + ((solution[j+1][0]-1) / 3) * 3;
                    solution[j+1][0] = 0;
                }
            }

            // print solution move by move ==================================================
            for (int k = 1; k <= 18; k++) {
                if (op_transform[k] != solution[j][m]) {
                    continue;
                }
                printf(operation_name[k]);
                printf(" ");
            }
        }
    }
    printf("\n\ntotal moves: %d", total_leng);
}


void show_cube(int cube[28]){
    // 1: L, 2: R, 3: U, 4: D, 5: F, 6: B
    int corner2color[8][3][2] = {
        {{3,3}, {6,1}, {2,3}},
        {{3,9}, {2,1}, {5,3}},
        {{3,7}, {5,1}, {1,3}},
        {{3,1}, {1,1}, {6,3}},
        {{4,7}, {6,9}, {1,7}},
        {{4,9}, {2,9}, {6,7}},
        {{4,3}, {5,9}, {2,7}},
        {{4,1}, {1,9}, {5,7}},
    };
    int edge2color[12][2][2]={
        {{3,6}, {2,2}},
        {{3,8}, {5,2}},
        {{3,4}, {1,2}},
        {{3,2}, {6,2}},
        {{6,4}, {2,6}},
        {{5,6}, {2,4}},
        {{5,4}, {1,6}},
        {{6,6}, {1,4}},
        {{4,6}, {2,8}},
        {{4,2}, {5,8}},
        {{4,4}, {1,8}},
        {{4,8}, {6,8}},
    };

    //actually should be [6][10], but get used to start form 1 so...
    char color[7][10] = {
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',},
        {' ', ' ', ' ', ' ', ' ', 'o', ' ', ' ', ' ', ' ',},
        {' ', ' ', ' ', ' ', ' ', 'r', ' ', ' ', ' ', ' ',},
        {' ', ' ', ' ', ' ', ' ', 'y', ' ', ' ', ' ', ' ',},
        {' ', ' ', ' ', ' ', ' ', 'w', ' ', ' ', ' ', ' ',},
        {' ', ' ', ' ', ' ', ' ', 'b', ' ', ' ', ' ', ' ',},
        {' ', ' ', ' ', ' ', ' ', 'g', ' ', ' ', ' ', ' ',},
    };

    char corner_color[8][3] = {
        {'y', 'g', 'r'},
        {'y', 'r', 'b'},
        {'y', 'b', 'o'},
        {'y', 'o', 'g'},
        {'w', 'g', 'o'},
        {'w', 'r', 'g'},
        {'w', 'b', 'r'},
        {'w', 'o', 'b'},
    };

    char edge_color[12][2] = {
        {'y', 'r'},
        {'y', 'b'},
        {'y', 'o'},
        {'y', 'g'},
        {'g', 'r'},
        {'b', 'r'},
        {'b', 'o'},
        {'g', 'o'},
        {'w', 'r'},
        {'w', 'b'},
        {'w', 'o'},
        {'w', 'g'},
    };

    for(int i=0; i<8; i++){
        for(int j=0; j<3; j++){
            color[corner2color[i][j][0]][corner2color[i][j][1]] = 
            corner_color[cube[i]][(j+3-cube[i+20])%3];
        }
    }

    for(int i=0; i<12; i++){
        for(int j=0; j<2; j++){
            //printf("\nedge(%d %d): %d %d", i, j, cube[i+8]-8, (j + (cube[i+8]<0 ? 1 : 0) )%2);
            color[edge2color[i][j][0]][edge2color[i][j][1]] = 
            edge_color[abs(cube[i+8])-8][(j + (cube[i+8]<0 ? 1 : 0) )%2];
        }
    }


    // print in console

    printf("\n       %c %c %c\n", color[3][1], color[3][2], color[3][3]);
    printf("       %c %c %c\n", color[3][4], color[3][5], color[3][6]);
    printf("       %c %c %c\n\n", color[3][7], color[3][8], color[3][9]);

    printf("%c %c %c  %c %c %c  %c %c %c  %c %c %c\n",
        color[1][1], color[1][2], color[1][3], color[5][1], color[5][2], color[5][3],
        color[2][1], color[2][2], color[2][3], color[6][1], color[6][2], color[6][3]);
    printf("%c %c %c  %c %c %c  %c %c %c  %c %c %c\n",
        color[1][4], color[1][5], color[1][6], color[5][4], color[5][5], color[5][6],
        color[2][4], color[2][5], color[2][6], color[6][4], color[6][5], color[6][6]);
    printf("%c %c %c  %c %c %c  %c %c %c  %c %c %c\n\n",
        color[1][7], color[1][8], color[1][9], color[5][7], color[5][8], color[5][9],
        color[2][7], color[2][8], color[2][9], color[6][7], color[6][8], color[6][9]);

    printf("       %c %c %c\n", color[4][1], color[4][2], color[4][3]);
    printf("       %c %c %c\n", color[4][4], color[4][5], color[4][6]);
    printf("       %c %c %c\n\n", color[4][7], color[4][8], color[4][9]);

}



int* translate(char* mixUp, int mixUpLength) {
    // translate from characters to numbers

    int* operation = new int[mixUpLength];
    for (int i = 0; i < mixUpLength; i++){
        operation[i] = 0;
    }
        

    char* pickChar;
    char basicOperation[] = { "LRUDFB" };
    for (int i = 0; i < 6; i++) {
        pickChar = strchr(mixUp, basicOperation[i]);
        while (pickChar != NULL) {
            operation[pickChar - mixUp] = i * 3 + 1;
            pickChar = strchr(pickChar + 1, basicOperation[i]);
        }
    }

    pickChar = strchr(mixUp, '\'');
    while (pickChar != NULL) {
        operation[pickChar - mixUp - 1]++;
        pickChar = strchr(pickChar + 1, '\'');
    }
    pickChar = strchr(mixUp, '2');
    while (pickChar != NULL) {
        operation[pickChar - mixUp - 1]++;
        operation[pickChar - mixUp - 1]++;
        pickChar = strchr(pickChar + 2, '2');
    }

    return operation;
}





int** solve_cube(int* mix_sequence, int mix_leng, int phase_max_moves[4]){

    


    int* cube = new_cube();
    operate(cube, mix_sequence, mix_leng);
    int* cube_temp = new_cube();
    copy_cube(cube, cube_temp);


    int init_case[4] = {0,0,0,0};
    int crit_ind[4] = {0,0,0,0};
    int** solution = new int*[4]();

    

    for (int i = 0; i < 4; i++){
        delete[] solution[i];
        //printf("\nphase %d initial %d", i, init_case[i]);
        solution[i] = solve_phase(cube_temp, &init_case[i], &crit_ind[i], i, phase_max_moves[i]);

        // go to last phase if no solution found
        if (solution[i][0] == 404) {
            //printf("\nphase back");
            init_case[i] = 0;
            crit_ind[i] = 0;
            
            copy_cube(cube, cube_temp);
            for (int j = 0; j < i-1; j++){
                operate(cube_temp, solution[j], phase_max_moves[j]);
            }
            // each loop ends with i++, i-2 means i-1
            i = i - 2;
            
            if(i < -1){
                break;
            }
            
        }else{
            operate(cube_temp, solution[i], phase_max_moves[i]);
        }
    }
/*
    printf("\n\nscramble sequence:\n");
    mix_cube = operate(cube, mix_sequence, mix_leng);
    for (int i = 0; i < mix_leng; i++){
        printf(operation_name[mix_sequence[i]]);
        //printf(" ");
    }
    show_cube(mix_cube);

    if (solution[0][0] != 404){
        
        for(int j = 0; j<4; j++){
            printf("\nphase %d solution:\n", j);
            for (int i=0; i<phase_max_moves[j]; i++){
            
                        printf(operation_name[solution[j][i]]);
                        printf(" ");
            }
            mix_cube = operate(mix_cube, solution[j], phase_max_moves[j]);
            show_cube(mix_cube);
        }
    }
*/   
    delete[] cube, cube_temp;
    return solution;
}






int* solve_phase(int* cube, int* init_case, int* crit_ind, int phase_ind, int max_moves){

    int* solution = new int[max_moves]();

    if (!(*init_case) && check_cube(cube, phase_ind)){
        //printf("\nphase %d solution:\nskip", phase_ind);
        //show_cube(cube);
        //int temp = *init_case;
        //*init_case = temp + 1;
        return solution;
    }




    static const int total_phases = 4;

    // sort the move by group 
    static int operation_sort[18] = { 3,6,9,12,15,18,7,8,10,11,1,2,4,5,13,14,16,17 };
    static int phase_operations[total_phases] = { 18,14,10,6 };
    // last move to enter next group
    static int critical_operation[4][6] = { {13,14,16,17},{1,4},{7,10},{3,6,9,12,15,18} };
    static int critical_operation_amount[4] = { 4,2,2,6 };


    int* cube_temp = new_cube();
    int* cube_temp_2 = new_cube();


    for(int moves = 1; moves <= max_moves; moves++){
        // initialize operation sequence
        int* sequence = new int[moves]();

        // total case number
        long long int case_total = (long long int) pow(phase_operations[phase_ind], moves-1);
        if(case_total <= *init_case){
            continue;
        }


        // loop all cases
        for(int case_ind = *init_case; case_ind < case_total; case_ind++){



            int overlap = 0;
            for (int i = 0; i < moves - 1; i++) {
                int op_ind = case_ind % (long long int) pow(phase_operations[phase_ind], i+1)
                    / (long long int) pow(phase_operations[phase_ind], i);
                sequence[moves - i - 2] = operation_sort[op_ind];

                if (i < moves - 2 && (sequence[moves - i - 2] - 1) / 3 == (sequence[moves - i - 3] - 1) / 3) {
                    // to avoid R' or R2 after R, etc.
                    overlap = 1;
                    break;
                }
            }
            if (overlap == 1){
                continue;
            }

            copy_cube(cube, cube_temp);
            operate_phase_check(cube_temp, sequence, moves, phase_ind);

            // operate and check
            for (int i = *crit_ind; i < critical_operation_amount[phase_ind]; i++){
                int critical_op = critical_operation[phase_ind][i];

                copy_cube(cube_temp, cube_temp_2);
                operate_phase_check(cube_temp_2, &critical_op, 1, phase_ind);
                
                int result = check_cube(cube_temp_2, phase_ind);
                

                if (result){
                    sequence[moves-1] = critical_op;
                    
                    //printf("\nphase %d solution:\n", phase_ind);
                    for (int j=0; j<moves; j++){
                        solution[j] = sequence[j];
                        //printf(operation_name[solution[j]]);
                        //printf(" ");
                    }

                    if(i < critical_operation_amount[phase_ind] - 1){
                        *crit_ind = i + 1;
                        *init_case = case_ind;
                    }else{
                        *crit_ind = 0;
                        *init_case = case_ind + 1;
                    }
                    
                    
                    delete[] sequence, cube_temp, cube_temp_2;
                    return solution;
                };
            } 
        }
        delete[] sequence;
    }
    delete[] cube_temp, cube_temp_2;
    //printf("\nfound no solution for phase %d under moves limit %d", phase_ind, max_moves);
    solution[0] = 404;
    return solution;
}







void operate_edge(int* cube, int* operation, int operationLength){
for (int i = 0; i < operationLength; i++) {

        int temp;
        int oper = operation[i];


        switch (oper) {

        case 0: 
            break;


            //L
        case 1:
            temp = cube[10];
            cube[10] = cube[15];   
            cube[15] = cube[18];   
            cube[18] = cube[14];   
            cube[14] = temp;

            break;
            

            //L'
        case 2:
            temp = cube[10];
            cube[10] = cube[14];   
            cube[14] = cube[18];   
            cube[18] = cube[15];   
            cube[15] = temp;

            break;


            //L2
        case 3:
            temp = cube[10];
            cube[10] = cube[18]; 
            cube[18] = temp;
            temp = cube[14];
            cube[14] = cube[15];
            cube[15] = temp;

            break;


            //R
        case 4:
            temp = cube[8];
            cube[8] = cube[13];   
            cube[13] = cube[16];   
            cube[16] = cube[12];   
            cube[12] = temp;

            break;
            

            //R'
        case 5:
            temp = cube[8];
            cube[8] = cube[12];   
            cube[12] = cube[16];   
            cube[16] = cube[13];   
            cube[13] = temp;

            break;


            //R2
        case 6:
            temp = cube[8];
            cube[8] = cube[16]; 
            cube[16] = temp;
            temp = cube[12];
            cube[12] = cube[13];
            cube[13] = temp;

            break;

        // U
        case 7:
            temp = cube[8];
            cube[8] = cube[11];   
            cube[11] = cube[10];   
            cube[10] = cube[9];   
            cube[9] = temp;


            break;
        // U'
        case 8:
            temp = cube[8];
            cube[8] = cube[9];   
            cube[9] = cube[10];   
            cube[10] = cube[11];   
            cube[11] = temp;

            break;

            //U2
        case 9:
            temp = cube[8];
            cube[8] = cube[10]; 
            cube[10] = temp;
            temp = cube[9];
            cube[9] = cube[11];
            cube[11] = temp;

            break;

        // D
        case 10:
            temp = cube[16];
            cube[16] = cube[17];   
            cube[17] = cube[18];   
            cube[18] = cube[19];   
            cube[19] = temp;

            break;
        // D'
        case 11:
            temp = cube[16];
            cube[16] = cube[19];   
            cube[19] = cube[18];   
            cube[18] = cube[17];   
            cube[17] = temp;

            break;

            //D2
        case 12:
            temp = cube[18];
            cube[18] = cube[16]; 
            cube[16] = temp;
            temp = cube[19];
            cube[19] = cube[17];
            cube[17] = temp;

            break;

        //F
        case 13:
            temp = cube[9];
            cube[9] = -cube[14];
            cube[14] = -cube[17];
            cube[17] = -cube[13];   
            cube[13] = -temp;  

            break;

        //F'
        case 14:
            temp = cube[9];
            cube[9] = -cube[13];
            cube[13] = -cube[17];
            cube[17] = -cube[14];   
            cube[14] = -temp;   

            break;

        //F2
        case 15:
            temp = cube[13];
            cube[13] = cube[14]; 
            cube[14] = temp;
            temp = cube[9];
            cube[9] = cube[17];
            cube[17] = temp;

            break;


//B
        case 16:
            temp = cube[11];
            cube[11] = -cube[12];
            cube[12] = -cube[19];
            cube[19] = -cube[15];   
            cube[15] = -temp;    

            break;


        //B'
        case 17:
            temp = cube[11];
            cube[11] = -cube[15];
            cube[15] = -cube[19];
            cube[19] = -cube[12];   
            cube[12] = -temp;    

            break;
        //B2
        case 18:
            temp = cube[11];
            cube[11] = cube[19]; 
            cube[19] = temp;
            temp = cube[12];
            cube[12] = cube[15];
            cube[15] = temp;

            break;




        default:
            printf("\ninvalid operation: %d!", oper);
            break;
        }
    }

}

void operate_corner_orientation(int* cube, int* operation, int operationLength){

    for (int i = 0; i < operationLength; i++) {

        int temp;
        int oper = operation[i];


        switch (oper) {

        case 0: 
            break;


    //L
        case 1:
            
            temp = cube[22];
            cube[22] = (cube[23] + 1) % 3;
            cube[23] = (cube[24] + 2) % 3;
            cube[24] = (cube[27] + 1) % 3;
            cube[27] = (temp + 2) % 3;
            break;
            

            //L'
        case 2:
            temp = cube[22];
            cube[22] = (cube[27] + 1) % 3;
            cube[27] = (cube[24] + 2) % 3;
            cube[24] = (cube[23] + 1) % 3;
            cube[23] = (temp + 2) % 3;
            break;


            //L2
        case 3:
            temp = cube[22];
            cube[22] = cube[24];
            cube[24] = temp;
            temp = cube[23];
            cube[23] = cube[27];
            cube[27] = temp;
            break;


            //R
        case 4:
            temp = cube[20];
            cube[20] = (cube[21] + 1) % 3;
            cube[21] = (cube[26] + 2) % 3;
            cube[26] = (cube[25] + 1) % 3;
            cube[25] = (temp + 2) % 3;
            break;
            

            //R'
        case 5:            
            temp = cube[20];
            cube[20] = (cube[25] + 1) % 3;
            cube[25] = (cube[26] + 2) % 3;
            cube[26] = (cube[21] + 1) % 3;
            cube[21] = (temp + 2) % 3;
            break;


            //R2
        case 6:
            temp = cube[20];
            cube[20] = cube[26];
            cube[26] = temp;
            temp = cube[21];
            cube[21] = cube[25];
            cube[25] = temp;
            break;

        // U
        case 7:
            temp = cube[20];
            cube[20] = cube[23];
            cube[23] = cube[22];
            cube[22] = cube[21];
            cube[21] = temp;
            break;
        // U'
        case 8:
            temp = cube[20];
            cube[20] = cube[21];
            cube[21] = cube[22];
            cube[22] = cube[23];
            cube[23] = temp;
            break;

            //U2
        case 9:
            temp = cube[20];
            cube[20] = cube[22];
            cube[22] = temp;
            temp = cube[21];
            cube[21] = cube[23];
            cube[23] = temp;
            break;

        // D
        case 10:
            temp = cube[25];
            cube[25] = cube[26];
            cube[26] = cube[27];
            cube[27] = cube[24];
            cube[24] = temp;
            break;
        // D'
        case 11:
            temp = cube[25];
            cube[25] = cube[24];
            cube[24] = cube[27];
            cube[27] = cube[26];
            cube[26] = temp;
            break;

            //D2
        case 12:
            temp = cube[25];
            cube[25] = cube[27];
            cube[27] = temp;
            temp = cube[24];
            cube[24] = cube[26];
            cube[26] = temp;
            break;

        //F
        case 13:
            temp = cube[21];
            cube[21] = (cube[22] + 1) % 3;
            cube[22] = (cube[27] + 2) % 3;
            cube[27] = (cube[26] + 1) % 3;
            cube[26] = (temp + 2) % 3;
            break;

        //F'
        case 14:
            temp = cube[21];
            cube[21] = (cube[26] + 1) % 3;
            cube[26] = (cube[27] + 2) % 3;
            cube[27] = (cube[22] + 1) % 3;
            cube[22] = (temp + 2) % 3;
            break;
        //F2
        case 15:
            temp = cube[21];
            cube[21] = cube[27];
            cube[27] = temp;
            temp = cube[22];
            cube[22] = cube[26];
            cube[26] = temp;
            break;


//B
        case 16:
            temp = cube[20];
            cube[20] = (cube[25] + 2) % 3;
            cube[25] = (cube[24] + 1) % 3;
            cube[24] = (cube[23] + 2) % 3;
            cube[23] = (temp + 1) % 3;
            break;


        //B'
        case 17:
            temp = cube[20];
            cube[20] = (cube[23] + 2) % 3;
            cube[23] = (cube[24] + 1) % 3;
            cube[24] = (cube[25] + 2) % 3;
            cube[25] = (temp + 1) % 3;
            break;
        //B2
        case 18:
            temp = cube[20];
            cube[20] = cube[24];
            cube[24] = temp;
            temp = cube[23];
            cube[23] = cube[25];
            cube[25] = temp;
            break;




        default:
            printf("\ninvalid operation: %d!", oper);
            break;
        }
    }
}







void operate_corner_position(int* cube, int* operation, int operationLength){

    for (int i = 0; i < operationLength; i++) {

        int temp;
        int oper = operation[i];


        switch (oper) {

        case 0: 
            break;


            //L
        case 1:
            
            temp = cube[2];
            cube[2] = cube[3];
            cube[3] = cube[4];
            cube[4] = cube[7];
            cube[7] = temp;
            break;
            

            //L'
        case 2:
            temp = cube[2];
            cube[2] = cube[7];
            cube[7] = cube[4];
            cube[4] = cube[3];
            cube[3] = temp;
            break;


            //L2
        case 3:
            temp = cube[2];
            cube[2] = cube[4];
            cube[4] = temp;
            temp = cube[3];
            cube[3] = cube[7];
            cube[7] = temp;
            break;


            //R
        case 4:
            temp = cube[0];
            cube[0] = cube[1];
            cube[1] = cube[6];
            cube[6] = cube[5];
            cube[5] = temp;
            break;
            

            //R'
        case 5:            
            temp = cube[0];
            cube[0] = cube[5];
            cube[5] = cube[6];
            cube[6] = cube[1];
            cube[1] = temp;
            break;


            //R2
        case 6:
            temp = cube[0];
            cube[0] = cube[6];
            cube[6] = temp;
            temp = cube[1];
            cube[1] = cube[5];
            cube[5] = temp;
            break;

        // U
        case 7:
            temp = cube[0];
            cube[0] = cube[3];
            cube[3] = cube[2];
            cube[2] = cube[1];
            cube[1] = temp;
            break;
        // U'
        case 8:
            temp = cube[0];
            cube[0] = cube[1];
            cube[1] = cube[2];
            cube[2] = cube[3];
            cube[3] = temp;
            break;

            //U2
        case 9:
            temp = cube[0];
            cube[0] = cube[2];
            cube[2] = temp;
            temp = cube[1];
            cube[1] = cube[3];
            cube[3] = temp;
            break;

        // D
        case 10:
            temp = cube[5];
            cube[5] = cube[6];
            cube[6] = cube[7];
            cube[7] = cube[4];
            cube[4] = temp;
            break;
        // D'
        case 11:
            temp = cube[5];
            cube[5] = cube[4];
            cube[4] = cube[7];
            cube[7] = cube[6];
            cube[6] = temp;
            break;

            //D2
        case 12:
            temp = cube[5];
            cube[5] = cube[7];
            cube[7] = temp;
            temp = cube[4];
            cube[4] = cube[6];
            cube[6] = temp;
            break;

        //F
        case 13:
            temp = cube[1];
            cube[1] = cube[2];
            cube[2] = cube[7];
            cube[7] = cube[6];
            cube[6] = temp;
            break;

        //F'
        case 14:
            temp = cube[1];
            cube[1] = cube[6];
            cube[6] = cube[7];
            cube[7] = cube[2];
            cube[2] = temp;
            break;
        //F2
        case 15:
            temp = cube[1];
            cube[1] = cube[7];
            cube[7] = temp;
            temp = cube[2];
            cube[2] = cube[6];
            cube[6] = temp;
            break;


//B
        case 16:
            temp = cube[0];
            cube[0] = cube[5];
            cube[5] = cube[4];
            cube[4] = cube[3];
            cube[3] = temp;
            break;


        //B'
        case 17:
            temp = cube[0];
            cube[0] = cube[3];
            cube[3] = cube[4];
            cube[4] = cube[5];
            cube[5] = temp;
            break;
        //B2
        case 18:
            temp = cube[0];
            cube[0] = cube[4];
            cube[4] = temp;
            temp = cube[3];
            cube[3] = cube[5];
            cube[5] = temp;
            break;




        default:
            printf("\ninvalid operation: %d!", oper);
            break;
        }
    }

}


void operate(int* cube, int* operation, int operation_leng){
    operate_edge(cube, operation, operation_leng);
    operate_corner_orientation(cube, operation, operation_leng);
    operate_corner_position(cube, operation, operation_leng);
}
void operate_phase_check(int* cube, int* operation, int operation_leng, int phase_ind){

    switch(phase_ind){
        case 0: 
            operate_edge(cube, operation, operation_leng); 
            break;
        case 1: 
            operate_edge(cube, operation, operation_leng);
            operate_corner_orientation(cube, operation, operation_leng);
            break;
        case 2:
            operate_edge(cube, operation, operation_leng);
            operate_corner_position(cube, operation, operation_leng);
            break;
        case 3:
            operate_edge(cube, operation, operation_leng);
            operate_corner_position(cube, operation, operation_leng);
            break;
        default:
            break;
    }

}





int check_phase_0(int* cube){
    int result = 0;
    if(
        cube[8]>0 &&
        cube[9]>0 &&
        cube[10]>0 &&
        cube[11]>0 &&
        cube[12]>0 &&
        cube[13]>0 &&
        cube[14]>0 &&
        cube[15]>0 &&
        cube[16]>0 &&
        cube[17]>0 &&
        cube[18]>0
            ){
                result = 1;
            }
    return result;
}

int check_phase_1(int* cube){
    int result = 0;
    if(
        cube[12] > 11 &&
        cube[12] < 16 &&
        cube[13] > 11 &&
        cube[13] < 16 &&
        cube[14] > 11 &&
        cube[14] < 16 &&
        cube[15] > 11 &&
        cube[15] < 16 &&
        !cube[20] &&
        !cube[21] &&
        !cube[22] &&
        !cube[23] &&
        !cube[24] &&
        !cube[25] &&
        !cube[26] 
            ){
                result = 1;
            }
    return result;
}
int check_phase_2(int* cube){
    int result = 0;
    if(
        !(cube[0] % 2) &&
        !(cube[2] % 2) &&
        !(cube[4] % 2) &&
        !(cube[6] % 2) &&
        !(cube[8] % 2) &&
        !(cube[10] % 2) &&
        !(cube[16] % 2) &&
        !(cube[18] % 2) 
            ){
                // to check if F U L face has even numbers of yellow(1) and orange(2) corners
                int face_U_1 = 0, face_U_2 = 0, face_L_1 = 0, face_L_2 = 0, face_F_1 = 0, face_F_2 = 0;

                switch (cube[3])
                {
                case 3:
                    face_U_1++;
                    face_L_1++;
                    face_U_2++;
                    face_L_2++;
                    break;
                case 1:
                    face_U_1++;
                    face_L_1++;
                    break;
                case 7:
                    face_U_2++;
                    face_L_2++;
                    break;                
                default:
                    break;
                }

                switch (cube[0])
                {
                case 0:
                    face_U_1++;
                    break;
                case 2:
                    face_U_1++;
                    face_U_2++;
                    break;
                case 4:
                    face_U_2++;
                    break;                
                default:
                    break;
                }


                switch (cube[1])
                {
                case 3:
                    face_U_1++;
                    face_U_2++;
                    face_F_1++;
                    face_F_2++;
                    break;
                case 1:
                    face_U_1++;
                    face_F_1++;
                    break;
                case 7:
                    face_U_2++;
                    face_F_2++;
                    break;                
                default:
                    break;
                }

                switch (cube[2])
                {
                case 0:
                    face_U_1++;
                    face_L_1++;
                    face_F_1++;
                    break;
                case 2:
                    face_U_1++;
                    face_U_2++;
                    face_L_1++;
                    face_L_2++;
                    face_F_1++;
                    face_F_2++;
                    break;
                case 4:
                    face_U_2++;
                    face_L_2++;
                    face_F_2++;
                    break;                
                default:
                    break;
                }
                
                switch (cube[7])
                {
                case 3:
                    face_L_1++;
                    face_L_2++;
                    face_F_1++;
                    face_F_2++;
                    break;
                case 1:
                    face_L_1++;
                    face_F_1++;
                    break;
                case 7:
                    face_L_2++;
                    face_F_2++;
                    break;                
                default:
                    break;
                }


                switch (cube[4])
                {
                case 0:
                    face_L_1++;
                    break;
                case 2:
                    face_L_1++;
                    face_L_2++;
                    break;
                case 4:
                    face_L_2++;
                    break;                
                default:
                    break;
                }

                switch (cube[6])
                {
                case 0:
                    face_F_1++;
                    break;
                case 2:
                    face_F_1++;
                    face_F_2++;
                    break;
                case 4:
                    face_F_2++;
                    break;                
                default:
                    break;
                }                

                if (
                    !(face_F_1 % 2) &&
                    !(face_F_2 % 2) &&
                    !(face_U_1 % 2) &&
                    !(face_U_2 % 2) &&
                    !(face_L_1 % 2) &&
                    !(face_L_2 % 2) 
                ){
                    result = 1;
                }
    }
    return result;
}
int check_phase_3(int* cube){
    int result = 0;
    if (
                cube[0] == 0 &&
                cube[1] == 1 &&
                cube[2] == 2 &&
                cube[3] == 3 &&
                cube[4] == 4 &&
                cube[8] == 8 &&
                cube[9] == 9 &&
                cube[10] == 10 &&
                cube[11] == 11 &&
                cube[12] == 12 &&
                cube[13] == 13 &&
                cube[14] == 14 &&
                cube[16] == 16 

            ){
                result = 1;
            }
    return result;
}
int check_cube(int* cube, int phase_ind){
    int result = 0;

    switch(phase_ind){
        case 0:
            result = check_phase_0(cube);
            break;

        case 1:
            result = check_phase_1(cube);
            break;

        case 2:
            result = check_phase_2(cube);
            break;

        case 3:
            result = check_phase_3(cube);
            break;

        default:
            break;
    }
    
    return result;
}

