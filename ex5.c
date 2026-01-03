/***********
 ID:
 NAME:
***********/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Episode {
    char *name;
    char *length;
    struct Episode *next;
} Episode;

typedef struct Season {
    char *name;
    Episode *episodes;
    struct Season *next;
} Season;

typedef struct TVShow {
    char *name;
    Season *seasons;
} TVShow;

TVShow ***database = NULL;
int dbSize = 0;

char *getString();
int getInt();

int validLength(char *s);
int countShows();

void shrinkDB();
void expandDB();

void freeEpisode(Episode *e);
void freeSeason(Season *s);
void freeShow(TVShow *show);
void freeAll();

Season *findSeason(TVShow *show, char *name);
Episode *findEpisode(Season *season, char *name);

void addShow();
void addSeason();
void addEpisode();

void deleteShow();
void deleteSeason();
void deleteEpisode();

void printEpisode();
void printShow();
void printArray();

void addMenu() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Add a TV show\n");
    printf("2. Add a season\n");
    printf("3. Add an episode\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: addShow(); break;
        case 2: addSeason(); break;
        case 3: addEpisode(); break;
    }
}

void deleteMenu() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Delete a TV show\n");
    printf("2. Delete a season\n");
    printf("3. Delete an episode\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: deleteShow(); break;
        case 2: deleteSeason(); break;
        case 3: deleteEpisode(); break;
    }
}

void printMenuSub() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Print a TV show\n");
    printf("2. Print an episode\n");
    printf("3. Print the array\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: printShow(); break;
        case 2: printEpisode(); break;
        case 3: printArray(); break;
    }
}

void mainMenu() {
    printf("Choose an option:\n");
    printf("1. Add\n");
    printf("2. Delete\n");
    printf("3. Print\n");
    printf("4. Exit\n");
}

int main() {
    int choice;
    do {
        mainMenu();
        scanf("%d", &choice);
        getchar();
        switch (choice) {
            case 1: addMenu(); break;
            case 2: deleteMenu(); break;
            case 3: printMenuSub(); break;
            case 4: freeAll(); break;
        }
    } while (choice != 4);
    return 0;
}


void shrinkDB()
{

    database = (TVShow***) realloc(database, (--dbSize)*sizeof(TVShow**));
    *database = (TVShow**) realloc(*database, (dbSize*dbSize)*sizeof(TVShow*));

    for (int i = 0; i < dbSize; i++)
        database[i] = *database + i*dbSize*sizeof(TVShow*);       
}
void expandDB()
{
    database = (TVShow***) realloc(database, (++dbSize)*sizeof(TVShow**));
    *database = (TVShow**) realloc(*database, (dbSize*dbSize)*sizeof(TVShow*));
    
    for (int i = 0; i < dbSize; i++)
        database[i] = *database + i*dbSize*sizeof(TVShow*);

    // When we expand we must clear the newly allocated part.
    int newAllocBeginning = (dbSize-1)*(dbSize-1);
    for (int i = newAllocBeginning; i < dbSize*dbSize; i++)
        database[i] = NULL;
}



void addShow()
{
    printf("Enter the name of the show:\n");

    //TODO check if already present
    
    TVShow* newShow = (TVShow*) malloc(sizeof(*newShow)); 
    newShow->name = getString();
    newShow->seasons = NULL;

    int isShifting = 0;
    TVShow* previousShow;
    for (int i = 0; i < dbSize*dbSize; i++)
    {
        if (!isShifting)
        {
            if ((*database)[i] == NULL) // We reached the empty spots
            {
                (*database)[i] = newShow;
                return; // We wont need to shift beyond this point since everything will be NULL.
            }
            
            int cmp = strcmp((*database)[i]->name, newShow->name);
            if (cmp == 0) // The new show already appears in the database.
            {
                printf("Show already exists.\n");
                freeShow(newShow);
                addShow(); // Prompt the user to add a show again.
            }
            if (cmp > 0) // The new show's name appears before the ith show
            {
                isShifting = 1; // From now on we need to shift.
                previousShow = (*database)[i];
                (*database)[i] = newShow;
            }
        }
        else // isShifting == true
        {
            // Swap shows forward until we reach a NULL one 

            if (previousShow == NULL)
                return;

            TVShow* temp = (*database)[i];
            (*database)[i] = previousShow;
            previousShow = temp;

        }
    }
}

#define STRING_CHUNK_LENGTH 20
char* getString()
{
    char c;
    int i;
    char *str;
    while ((c = getchar()) != '\n')
    {
        if (i % STRING_CHUNK_LENGTH == 0)
            str = (char*) realloc(str, (i+STRING_CHUNK_LENGTH));

        str[i++] = c;
    }

    if (i % STRING_CHUNK_LENGTH == 0)
        str = (char*) realloc(str, (i+1));
    str[i] = '\0';

    return str;
}






TVShow *findShow(char *name)
{
    for (int i = 0; i < dbSize*dbSize; i++)
        if (strcmp((*database)[i]->name, name) == 0)
            return (*database)[i];

    return NULL;
}





void freeShow(TVShow *show)
{
    free(show->name);
    freeSeason(show->seasons);

    free(show);
}

/*
void deleteShow()
{
    printf("Enter the name of the show:\n");
    char *name = getString();

    for (int i = 0; i < dbSize*dbSize; i++)
    {
        if (strcmp((*database)[i]->name, name) == 0) // Found a match
        {
            freeShow((*database)[i]);

            // Shift next shows backwards.
            TVShow* previousShow = //TODO TODO;
        }
    }
}*/

void addSeasonRecursive(Season **nodePtr, Season *toAdd, int pos)
{
    Season *node = *nodePtr;
    if (node->next == NULL)
        node->next = toAdd;
    else if (pos > 0)
        addSeasonRecursive(&node->next, toAdd, pos-1);
    else // We need to insert inside the linked list
    {
        toAdd->next = node;
        *nodePtr = toAdd;
    }
}

void addSeason()
{
    printf("Enter the name of the show:\n");

    char *showName = getString();
    TVShow* show = findShow(showName);
    free(showName);

    if (!show)
    {
        printf("Show not found.\n");
        addSeason();
    }

    printf("Enter the name of the season:\n");

    char *seasonName = getString();
    if (findSeason(show, seasonName))
    {
        printf("Season already exists.\n");
        free(seasonName);
        addSeason();
    }

    printf("Enter the position:\n");
    char *posStr = getString();
    int pos = atoi(posStr);
    free(posStr);

    if (pos < 0) // <0 because we only accept positive integers
    {
        printf("Position must be a non-negative integer\n");
        addSeason();
    }

    Season *newSeason = (Season*)malloc(sizeof(*newSeason));

    newSeason->episodes = NULL;
    newSeason->name = seasonName;
    newSeason->next = NULL;

    addSeasonRecursive(&show->seasons, newSeason, pos);
}

Season *findSeasonRecursive(Season *season, char *name)
{
    if (!season)
        return NULL;
    if (strcmp(season->name, name) == 0)
        return season;

    return findSeasonRecursive(season->next, name);
}

Season *findSeason(TVShow *show, char *name)
{
    return findSeasonRecursive(show->seasons, name);
}

