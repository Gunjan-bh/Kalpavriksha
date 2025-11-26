#include<stdio.h>
#include<stdlib.h>
#include<ctype.h> 
#include "Players_data.h"

#define MAX_NAME_LENGTH 51
#define MAX_PLAYER_ID 1500
#define NUMBER_OF_TEAMS 10

typedef struct {
    int id;
    char* name;
    char* teamName;
    char* role;
    float pi; 
} TempPlayer;

typedef struct PlayerNode{
    int playerID;
    char* name;
    char* teamName;
    char* role;
    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;
    struct PlayerNode* next;
} PlayerNode;

typedef struct{
    int teamID;
    char* name;
    int totalPlayers;
    float averageBattingStrikeRate; 
    float avgStrikeRateBatterAllRounder; 
    int totalBatsmenAllRounders;
    PlayerNode *batsmen;
    PlayerNode *bowlers;
    PlayerNode *allRounders;
} TeamNode;

TeamNode teamList[NUMBER_OF_TEAMS];

int stringLength(const char* str){
    if (str == NULL) return 0;
    int length = 0;
    while(*str){
        length++;
        str++;
    }
    return length;
}

char* stringCopy(const char* source){
    if (source == NULL) return NULL;
    int len = stringLength(source);
    char* destination = (char*)malloc(len + 1);
    
    if (destination == NULL) {
        return NULL;
    }
    
    char* ptr = destination;
    while(*source){
        *ptr = *source;
        source++;
        ptr++;
    }
    *ptr = '\0';
    return destination;
}

int stringCompare(const char* str1, const char* str2){
    if (str1 == NULL || str2 == NULL) {
        if (str1 == str2) return 0;
        if (str1 == NULL) return -1;
        return 1;
    }
    
    while(*str1 && *str2){
        if(*str1 < *str2){
            return -1;
        }
        else if(*str1 > *str2){
            return 1;
        }
        str1++;
        str2++;
    }
    if(*str1) return 1;
    if(*str2) return -1;
    return 0;
}

void initializeTeams(){
    for(int i = 0; i < teamCount; i++){
        teamList[i].teamID = i + 1;
        teamList[i].name = stringCopy(teams[i]);
        teamList[i].totalPlayers = 0;
        teamList[i].totalBatsmenAllRounders = 0;
        teamList[i].averageBattingStrikeRate = 0;
        teamList[i].avgStrikeRateBatterAllRounder = 0;
        teamList[i].batsmen = NULL;
        teamList[i].bowlers = NULL;
        teamList[i].allRounders = NULL;
    }
}

void insertSorted(PlayerNode** head, PlayerNode* newPlayer){
    if(*head == NULL || newPlayer->performanceIndex > (*head)->performanceIndex){
        newPlayer->next = *head;
        *head = newPlayer;
        return;
    }
    PlayerNode* current = *head;
    while(current->next != NULL && current->next->performanceIndex > newPlayer->performanceIndex){
        current = current->next;
    }
    newPlayer->next = current->next;
    current->next = newPlayer;
}

int binarySearchTeam(const char* teamName){
    int left = 0, right = teamCount - 1;
    while(left <= right){
        int mid = left + (right - left) / 2;
        int comparision = stringCompare(teamList[mid].name, teamName);
        if(comparision == 0) return mid;
        else if(comparision < 0) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}

float calculatePerformanceIndex(PlayerNode* newPlayer){
    float safeAvg = (newPlayer->battingAverage > 0) ? newPlayer->battingAverage : 0.0f;
    float safeSR = (newPlayer->strikeRate > 0) ? newPlayer->strikeRate : 0.0f;
    int safeWkts = (newPlayer->wickets > 0) ? newPlayer->wickets : 0;
    float safeER = (newPlayer->economyRate > 0) ? newPlayer->economyRate : 0.0f;

    float performanceIndex;
    if(stringCompare(newPlayer->role, "Batsman") == 0){
        performanceIndex = (safeAvg * safeSR) / 100.0f;
    }
    else if(stringCompare(newPlayer->role, "Bowler") == 0){
        performanceIndex = (safeWkts * 2.0f) + (100.0f - safeER);
    }
    else{
        performanceIndex = ((safeAvg * safeSR) / 100.0f) + (safeWkts * 2.0f);
    }
    return performanceIndex;
}

void updateTeam(PlayerNode* newPlayer){
    int teamIndex = binarySearchTeam(newPlayer->teamName);
    if(teamIndex == -1){
        printf("No team found.\n");
        return;
    }
    
    if(stringCompare(newPlayer->role, "Batsman") == 0){
        insertSorted(&teamList[teamIndex].batsmen, newPlayer);
        teamList[teamIndex].avgStrikeRateBatterAllRounder = ((teamList[teamIndex].avgStrikeRateBatterAllRounder * teamList[teamIndex].totalBatsmenAllRounders) + newPlayer->strikeRate) / (teamList[teamIndex].totalBatsmenAllRounders + 1);
        teamList[teamIndex].totalBatsmenAllRounders++;
    }
    else if(stringCompare(newPlayer->role, "All-rounder") == 0){
        insertSorted(&teamList[teamIndex].allRounders, newPlayer);
        teamList[teamIndex].avgStrikeRateBatterAllRounder = ((teamList[teamIndex].avgStrikeRateBatterAllRounder * teamList[teamIndex].totalBatsmenAllRounders) + newPlayer->strikeRate) / (teamList[teamIndex].totalBatsmenAllRounders + 1);
        teamList[teamIndex].totalBatsmenAllRounders++;
    }
    else{ 
        insertSorted(&teamList[teamIndex].bowlers, newPlayer);
    }
    
    teamList[teamIndex].averageBattingStrikeRate = ((teamList[teamIndex].averageBattingStrikeRate * teamList[teamIndex].totalPlayers) + newPlayer->strikeRate) / (teamList[teamIndex].totalPlayers + 1);
    teamList[teamIndex].totalPlayers++;
}

void initializePlayers(){
    for(int i = 0; i < playerCount; i++){
        PlayerNode *newPlayer = (PlayerNode*)malloc(sizeof(PlayerNode));
        if (!newPlayer) {
            fprintf(stderr, "Memory allocation failed.\n");
            return;
        }
        newPlayer->playerID = players[i].id;
        newPlayer->name = stringCopy(players[i].name);
        newPlayer->teamName = stringCopy(players[i].team);
        newPlayer->role = stringCopy(players[i].role);
        newPlayer->totalRuns = players[i].totalRuns;
        newPlayer->battingAverage = players[i].battingAverage;
        newPlayer->strikeRate = players[i].strikeRate;
        newPlayer->wickets = players[i].wickets;
        newPlayer->economyRate = players[i].economyRate;
        newPlayer->performanceIndex = calculatePerformanceIndex(newPlayer);
        newPlayer->next = NULL;
        updateTeam(newPlayer);
    }
}

void freeMemory(){
    for(int i = 0; i < teamCount; i++){
        PlayerNode* lists[3] = {teamList[i].batsmen, teamList[i].bowlers, teamList[i].allRounders};
        for(int role = 0; role < 3; role++){
            PlayerNode* current = lists[role];
            while(current != NULL){
                PlayerNode* temp = current;
                current = current->next;
                if (temp->name) free(temp->name);
                if (temp->teamName) free(temp->teamName);
                if (temp->role) free(temp->role);
                free(temp);
            }
        }
        if (teamList[i].name) free(teamList[i].name);
    }
}

int containsSpecialCharacters(const char* str){
    if (str == NULL) return 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isalnum((unsigned char)str[i]) && str[i] != ' ') {
            return 1; 
        }
    }
    return 0; 
}

int getValidRole(){
    int role;
    while(1){
        if(scanf("%d", &role) != 1){
            printf("Invalid Role. Enter again: ");
            while(getchar() != '\n'); 
        }
        else{
            if(getchar() != '\n' || role < 1 || role > 3){
                printf("Invalid Role. Enter again: ");
                while(getchar() != '\n'); 
            }
            else{
                return role;
            }
        }
    }
}

int getValidInteger(){
    int number;
    while(1){
        if(scanf("%d", &number) != 1){
            printf("Invalid input. Enter again: ");
            while(getchar() != '\n'); 
        }
        else{
            if(getchar() != '\n'){
                printf("Invalid input. Enter again: ");
                while(getchar() != '\n'); 
            }
            else{
                if (number < 0) {
                    printf("Input cannot be negative. Enter again: ");
                    continue; 
                }
                return number;
            }
        }
    }
}

float getValidFloat(){
    float number;
    while(1){
        if(scanf("%f", &number) != 1){
            printf("Invalid input. Enter again: ");
            while(getchar() != '\n'); 
        }
        else{
            if(getchar() != '\n'){
                printf("Invalid input. Enter again: ");
                while(getchar() != '\n'); 
            }
            else{
                if (number < 0.0f) {
                    printf("Input cannot be negative. Enter again: ");
                    continue; 
                }
                return number;
            }
        }
    }
}

int getValidTeamId(){
    while(1){
        int id = getValidInteger();
        if(id < 1 || id > teamCount){
            printf("Enter ID in range 1-10: ");
        }
        else{
            return id;
        }
    }
}

int playerIdExists(int id){
    for(int i = 0; i < teamCount; i++){
        PlayerNode* temp = teamList[i].batsmen;
        while(temp != NULL){
            if(temp->playerID == id) return 1;
            temp = temp->next;
        }
        temp = teamList[i].allRounders;
        while(temp != NULL){
            if(temp->playerID == id) return 1;
            temp = temp->next;
        }
        temp = teamList[i].bowlers;
        while(temp != NULL){
            if(temp->playerID == id) return 1;
            temp = temp->next;
        }
    }
    return 0;
}

int playerNameExists(const char* name){
    for(int i = 0; i < teamCount; i++){
        PlayerNode* temp = teamList[i].batsmen;
        while(temp != NULL){
            if(stringCompare(temp->name, name) == 0) return 1;
            temp = temp->next;
        }
        temp = teamList[i].allRounders;
        while(temp != NULL){
            if(stringCompare(temp->name, name) == 0) return 1;
            temp = temp->next;
        }
        temp = teamList[i].bowlers;
        while(temp != NULL){
            if(stringCompare(temp->name, name) == 0) return 1;
            temp = temp->next;
        }
    }
    return 0;
}

int getValidPlayerId(){
    while(1){
        int id = getValidInteger();
        if(id < 1 || id > MAX_PLAYER_ID){
            printf("Enter ID in range 1-%d: ", MAX_PLAYER_ID); 
        }
        else if(playerIdExists(id)){
            printf("ID already exists. Enter again: ");
        }
        else{
            return id;
        }
    }
}

void addNewPlayer(){
    PlayerNode* newPlayer = (PlayerNode*)malloc(sizeof(PlayerNode));
    if(!newPlayer){
        printf("Failed to allocate memory!\n");
        return;
    }

    printf("Enter Team ID to add player: ");
    int teamID = getValidTeamId();
    newPlayer->teamName = stringCopy(teamList[teamID - 1].name);
    printf("Enter Player Details: \n");

    printf("Player ID: ");
    newPlayer->playerID = getValidPlayerId();

    printf("Name: ");
    while(getchar() != '\n'); 
    
    char* name_buffer = (char*)malloc(MAX_NAME_LENGTH);
    if (!name_buffer) { free(newPlayer->teamName); free(newPlayer); return; }
    
    while(1) { 
        if (fgets(name_buffer, MAX_NAME_LENGTH, stdin) == NULL) {
            printf("Error reading name. Enter again: ");
            continue;
        }

        int len = stringLength(name_buffer);
        if (len > 0 && name_buffer[len - 1] == '\n') {
            name_buffer[len - 1] = '\0';
            len--;
        }
        
        int is_empty = 1;
        for (int i = 0; i < len; i++) {
            if (name_buffer[i] != ' ' && name_buffer[i] != '\t' && name_buffer[i] != '\0') {
                is_empty = 0;
                break;
            }
        }
        if (is_empty) {
            printf("Name cannot be blank. Enter again: ");
            continue;
        }

        if (containsSpecialCharacters(name_buffer)) {
            printf("Name contains special characters. Only letters, numbers, and spaces are allowed. Enter again: ");
            continue;
        }

        if (playerNameExists(name_buffer)) {
            printf("Player name already exists. Enter a unique name: ");
        } else {
            break; 
        }
    }
    
    newPlayer->name = stringCopy(name_buffer);
    free(name_buffer);
    
    if (!newPlayer->name) { free(newPlayer->teamName); free(newPlayer); return; }

    printf("Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    int role = getValidRole();
    if(role == 1) newPlayer->role = stringCopy("Batsman");
    else if(role == 2) newPlayer->role = stringCopy("Bowler");
    else newPlayer->role = stringCopy("All-rounder");

    printf("Total Runs (>= 0): ");
    newPlayer->totalRuns = getValidInteger();

    printf("Batting Average (>= 0): ");
    newPlayer->battingAverage = getValidFloat();

    printf("Strike Rate (>= 0): ");
    newPlayer->strikeRate = getValidFloat();

    printf("Wickets (>= 0): ");
    newPlayer->wickets = getValidInteger();

    printf("Economy Rate (>= 0): ");
    newPlayer->economyRate = getValidFloat();

    newPlayer->performanceIndex = calculatePerformanceIndex(newPlayer);
    newPlayer->next = NULL;
    updateTeam(newPlayer);

    printf("Player added successfully to Team %s!\n", newPlayer->teamName);
}

void playerDetails(PlayerNode* player){
    printf("%-5d %-20s %-15s %-8d %-8.2f %-8.2f %-8d %-8.2f %-12.2f\n", 
        player->playerID, player->name, player->role, player->totalRuns, 
        player->battingAverage, player->strikeRate, player->wickets, 
        player->economyRate, player->performanceIndex);
}

void displayPlayersOfTeam(){
    printf("Enter Team ID: ");
    int teamID = getValidTeamId();
    TeamNode team = teamList[teamID - 1];
    
    printf("Players of Team %s:\n", team.name);
    printf("\n====================================================================================================\n");
    printf("%-5s %-20s %-15s %-8s %-8s %-8s %-8s %-8s %-12s\n", "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf. Index");
    printf("====================================================================================================\n");
    
    PlayerNode* temp = team.batsmen;
    while(temp != NULL){
        playerDetails(temp);
        temp = temp->next;
    }
    temp = team.allRounders;
    while(temp != NULL){
        playerDetails(temp);
        temp = temp->next;
    }
    temp = team.bowlers;
    while(temp != NULL){
        playerDetails(temp);
        temp = temp->next;
    }
    printf("====================================================================================================\n");
    printf("\nTotal Players: %d\n", team.totalPlayers);
    printf("Average Batting Strike Rate: %.2f\n",team.averageBattingStrikeRate);
}

void displayTeamsBySR(){
    int teamIndices[teamCount];
    for(int i = 0; i < teamCount; i++){
        teamIndices[i] = i;
    }
    
    for(int i = 0; i < teamCount; i++){
        for(int j = 0; j < teamCount - i - 1; j++){
            if(teamList[teamIndices[j]].avgStrikeRateBatterAllRounder < teamList[teamIndices[j + 1]].avgStrikeRateBatterAllRounder){
                int temp = teamIndices[j];
                teamIndices[j] = teamIndices[j + 1];
                teamIndices[j + 1] = temp;
            }
        }
    }
    
    printf("Teams Sorted by Average Batting Strike Rate\n");
    printf("=========================================================\n");
    printf("%-3s %-15s %-10s %-14s\n", "ID", "Team Name", "Avg Bat SR", "Total Players");
    printf("=========================================================\n");
    for(int i = 0; i < teamCount; i++){
        TeamNode team = teamList[teamIndices[i]];
        printf("%-3d %-15s %-10.2f %-2d\n", team.teamID, team.name, team.avgStrikeRateBatterAllRounder, team.totalPlayers);
    }
    printf("=========================================================\n");
}

int gatherPlayers(int role, TempPlayer** sortPlayers){
    int count = 0;
    *sortPlayers = (TempPlayer*)malloc(playerCount * sizeof(TempPlayer)); 
    if (!*sortPlayers) {
        fprintf(stderr, "Memory error in sorting.\n");
        return 0;
    }

    for(int i = 0; i < teamCount; i++){
        PlayerNode* current = NULL;
        if(role == 1) current = teamList[i].batsmen;
        else if(role == 2) current = teamList[i].bowlers;
        else current = teamList[i].allRounders;

        while(current != NULL){
            (*sortPlayers)[count].id = current->playerID;
            (*sortPlayers)[count].name = current->name;
            (*sortPlayers)[count].teamName = current->teamName;
            (*sortPlayers)[count].role = current->role;
            (*sortPlayers)[count].pi = current->performanceIndex;
            count++;
            current = current->next;
        }
    }
    return count;
}

void sortPlayersByPI(TempPlayer arr[], int n) {
    int i, j, max_idx;
    TempPlayer temp;
    for (i = 0; i < n - 1; i++) {
        max_idx = i;
        for (j = i + 1; j < n; j++) {
            if (arr[j].pi > arr[max_idx].pi) {
                max_idx = j;
            }
        }
        temp = arr[max_idx];
        arr[max_idx] = arr[i];
        arr[i] = temp;
    }
}

void printGlobalPlayerRow(TempPlayer* p) {
    printf("%-5d %-20s %-15s %-15s %-8.2f\n", 
           p->id, p->name, p->teamName, p->role, p->pi);
}


void displayTopKPlayers(){
    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    int role = getValidRole();
    printf("Enter number of players (K): ");
    int k = getValidInteger();

    TempPlayer* global_players = NULL;
    int total_count = gatherPlayers(role, &global_players);

    if (total_count == 0) {
        printf("No players found for this role.\n");
        if (global_players) free(global_players);
        return;
    }
    
    sortPlayersByPI(global_players, total_count); 
    
    printf("\nTop %d %s across all teams:\n", k, (role == 1 ? "Batsmen" : (role == 2 ? "Bowlers" : "All-rounders")));
    printf("===========================================================================\n");
    printf("%-5s %-20s %-15s %-15s %-8s\n", "ID", "Name", "Team", "Role", "Perf. Index");
    printf("===========================================================================\n");

    int limit = (k < total_count) ? k : total_count;
    for(int i = 0; i < limit; i++){ 
        printGlobalPlayerRow(&global_players[i]);
    }
    printf("===========================================================================\n");

    if (global_players) free(global_players);
}

void displayAllPlayersByRole(){
    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    int role = getValidRole();

    TempPlayer* global_players = NULL;
    int total_count = gatherPlayers(role, &global_players);
    
    if (total_count == 0) {
        printf("No players found for this role.\n");
        if (global_players) free(global_players);
        return;
    }
    
    sortPlayersByPI(global_players, total_count); 
    
    printf("\nAll %s across all teams, sorted by performance index:\n", (role == 1 ? "Batsmen" : (role == 2 ? "Bowlers" : "All-rounders")));
    printf("===========================================================================\n");
    printf("%-5s %-20s %-15s %-15s %-8s\n", "ID", "Name", "Team", "Role", "Perf. Index");
    printf("===========================================================================\n");

    for(int i = 0; i < total_count; i++){ 
        printGlobalPlayerRow(&global_players[i]);
    }
    printf("===========================================================================\n");

    if (global_players) free(global_players);
}

void displayMenu(){
    printf("\n==============================================================================\n");
    printf("ICC ODI Player Performance Analyzer");
    printf("\n==============================================================================\n");
    printf("1. Add Player to Team\n");
    printf("2. Display Players of a Specific Team\n");
    printf("3. Display Teams by Average Batting Strike Rate\n");
    printf("4. Display Top K Players Globally by Role (Performance Index)\n");
    printf("5. Display all Players of specific role Across All Teams by performance index\n");
    printf("6. Exit\n");
    printf("==============================================================================\n");
    printf("Enter your choice: ");
}

void initiateAnalyzer(){
    int choice = 0;
    while(choice != 6){
        displayMenu();
        choice = getValidInteger();
        switch(choice){
            case 1:
                addNewPlayer();
                break;
            case 2:
                displayPlayersOfTeam();
                break;
            case 3:
                displayTeamsBySR();
                break;
            case 4:
                displayTopKPlayers();
                break;
            case 5:
                displayAllPlayersByRole();
                break;
            case 6:
                freeMemory();
                return;
            default:
                printf("Invalid choice.\n");
        }
    }
}

int main(){
    initializeTeams();
    initializePlayers();
    initiateAnalyzer();
}