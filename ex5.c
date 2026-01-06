/***********
 ID: mironca1
 NAME: Carmel Miron
***********/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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

int getInt(char *s);
int isValidLength(char *s);
TVShow** getShowByIndex(int i);

void shrinkDB();
void expandDB();

void freeEpisode(Episode *e);
void freeSeason(Season *s);
void freeShow(TVShow *show);
void freeAll();

Season **findSeasonRecursive(Season **season, char *name);
Episode **findEpisodeRecursive(Episode **episode, char *name);

TVShow** findShow(char *name);
Season** findSeason(TVShow *show, char *name);
Episode** findEpisode(Season *season, char *name);

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
    int choice = 0;
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
        default: exit(EXIT_FAILURE);
    }
}

void deleteMenu() {
    int choice = 0;
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
        default: exit(EXIT_FAILURE);
    }
}

void printMenuSub() {
    int choice = 0;
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
        default: exit(EXIT_FAILURE);
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
            default: exit(EXIT_FAILURE);
        }
    } while (choice != 4);

    return 0;
}




TVShow** getShowByIndex(int i)
{
    assert(i >= 0);
    assert(i < dbSize*dbSize);

    int row = i / dbSize;
    int col = i % dbSize;
    return &database[row][col];
}



void shrinkDB()
{
    // if we need to delete the database enitirely
    if (dbSize == 1)
    {
        free(*database);
        free(database);
        database = NULL;
        dbSize = 0;
        return;
    }

    database = (TVShow***) realloc(database, (--dbSize)*sizeof(TVShow**));
    *database = (TVShow**) realloc(*database, (dbSize*dbSize)*sizeof(TVShow*));


    for (int i = 0; i < dbSize; i++)
        database[i] = *database + i*dbSize;       
}
void expandDB()
{
    database = (TVShow***) realloc(database, (++dbSize)*sizeof(TVShow**));

    if (dbSize == 1) // we expanded from an empty database
        *database = NULL; // must clear *database since the next line reallocs it
    *database = (TVShow**) realloc(*database, (dbSize*dbSize)*sizeof(TVShow*));


    // When we expand we must clear the newly allocated part.
    int newAllocBeginning = (dbSize-1)*(dbSize-1);
    for (int i = newAllocBeginning; i < dbSize*dbSize; i++)
        (*database)[i] = NULL;

    // move rows
    for (int i = 0; i < dbSize; i++)
        database[i] = *database + i*dbSize;

}


void addShow()
{
    printf("Enter the name of the show:\n");

    TVShow* newShow = (TVShow*) malloc(sizeof(TVShow)); 
    newShow->name = getString();
    newShow->seasons = NULL;


    if (findShow(newShow->name)) // show already exists
    {
        freeShow(newShow);
        printf("Show already exists.\n");
        return;
    }

    if (dbSize == 0 || database[dbSize-1][dbSize-1]) // database is full
        expandDB();

    int isShifting = 0;
    TVShow* previousShow;
    for (int i = 0; i < dbSize*dbSize; i++)
    {
        TVShow **currentShow = getShowByIndex(i);
        if (!isShifting)
        {
            if (!*currentShow) // We reached the empty spots
            {
                *currentShow = newShow;
                return; // We wont need to shift beyond this point since everything will be NULL.
            }
            
            int cmp = strcmp((*currentShow)->name, newShow->name);
            if (cmp > 0) // The new show's name appears before the ith show
            {
                isShifting = 1; // From now on we need to shift.
                previousShow = *currentShow;
                *currentShow = newShow;
            }
        }
        else // isShifting == true
        {
            // Swap shows forward until we reach a NULL one 

            if (previousShow == NULL)
                return;

            TVShow* temp = *currentShow;
            *currentShow = previousShow;
            previousShow = temp;
        }
    }
}

#define STRING_CHUNK_LENGTH 20
char* getString()
{
    char c;
    int i = 0;
    char *str = NULL;
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






TVShow** findShow(char *name)
{
    for (int i = 0; i < dbSize*dbSize; i++)
    {
        if (!*getShowByIndex(i))
            return NULL;
        if (strcmp((*getShowByIndex(i))->name, name) == 0)
            return getShowByIndex(i);
    }

    // No match found
    return NULL;
}





void freeShow(TVShow *show)
{
    free(show->name);

    Season *iterator = show->seasons;
    while (iterator)
    {
        Season *temp = iterator->next;
        freeSeason(iterator);
        iterator = temp;
    }

    free(show);
}

void deleteShow()
{
    printf("Enter the name of the show:\n");
    char *name = getString();
    TVShow** match = findShow(name);
    free(name);

    if (!match)
    {
        printf("Show not found\n");
        return;
    }


    freeShow(*match);


    // Shift remaining shows
    
    // Since we allocated database[][] as a 1d array, the index of any show in it will be its distance from the begining of the database.
    int i = match - *database;
    while (i < dbSize*dbSize - 1 && getShowByIndex(i)) // while we have more spots to shift
    {
        *getShowByIndex(i) = *getShowByIndex(i+1); // memcpy?
        i++;
    }

    *getShowByIndex(i) = NULL;


    // Shrink if needed
    if (dbSize == 1 || !*getShowByIndex((dbSize-1)*(dbSize-1)))
        shrinkDB();

}

void addSeason()
{
    printf("Enter the name of the show:\n");
    char *showName = getString();
    printf("Enter the name of the season:\n");
    char *seasonName = getString();
    printf("Enter the position:\n");
    char *posStr = getString();



    TVShow** show = findShow(showName);
    free(showName);

    if (!show)
    {
        printf("Show not found.\n");
        free(seasonName);
        free(posStr);
        return;
    }

    if (findSeason(*show, seasonName))
    {
        printf("Season already exists.\n");
        free(seasonName);
        free(posStr);
        return;
    }

    int pos = atoi(posStr); // We can assume non negative integer
    free(posStr);

    Season *newSeason = (Season*)malloc(sizeof(*newSeason));

    newSeason->episodes = NULL;
    newSeason->name = seasonName;

    Season **iterator = &(*show)->seasons;

    int i = 0;
    while (*iterator && i < pos)
    {
        i++;
        iterator = &(*iterator)->next;
    }

    newSeason->next = *iterator;
    *iterator = newSeason;
}

Season **findSeasonRecursive(Season **season, char *name)
{
    if (!*season)
        return NULL;
    if (strcmp((*season)->name, name) == 0)
        return season;

    return findSeasonRecursive(&(*season)->next, name);
}

Season **findSeason(TVShow *show, char *name)
{
    return findSeasonRecursive(&show->seasons, name);
}


void deleteSeason()
{
    printf("Enter the name of the show:\n");
    char *showName = getString();
    printf("Enter the name of the season:\n");
    char *seasonName = getString();


    TVShow **show = findShow(showName);
    free(showName);

    if (!show)
    {
        free(seasonName);
        printf("Show not found.\n");
        return;
    }

    Season **season= findSeason(*show, seasonName);
    free(seasonName);

    if (!season)
    {
        printf("Season not found.\n");
        return;
    }

    Season *toFree = *season;


    *season = toFree->next;

    freeSeason(toFree);
}

void freeSeason(Season *s)
{
   free(s->name);

   Episode *iterator = s->episodes;
   while (iterator)
   {
        Episode *temp = iterator->next;
        freeEpisode(iterator);
        iterator = temp;
   }

   free(s);
}









void printShow()
{
    printf("Enter the name of the show:\n");
    char *name = getString();
    TVShow** match = findShow(name);
    free(name);

    if (!match)
    {
        printf("Show not found.\n");
        return;
    }

    TVShow *show = *match;

    printf("Name: %s\n", show->name);

    printf("Seasons:\n");

    Season *season = show->seasons;
    int showIndex = 0;
    while (season)
    {
        printf("\tSeason %d: %s\n", showIndex++, season->name); 

        //Episodes
        Episode *episode = season->episodes;
        int episodeIndex = 0;
        while (episode)
        {
            printf("\t\tEpisode %d: %s (%s)\n", episodeIndex, episode->name, episode->length);
            episode = episode->next;
            episodeIndex++;
        }



        season = season->next;
    }
}




void printArray()
{
    for (int i = 0; i < dbSize; i++)
    {
        for (int j = 0; j < dbSize; j++)
        {
            char *toPrint = database[i][j] ? database[i][j]->name : "NULL";
            printf("[%s]%s", toPrint, (j < dbSize-1) ? " " : ""); 
        }
        putchar('\n');
    }
}


#define RETURN_NAN -1
int getInt(char *s)
{
    if (s[0] < '0' || s[0] > '9' || s[1] < '0' || s[1] > '9')
        return RETURN_NAN;

    // the tens place will be the distance of the first digit from 0 multiplied by 10.
    return 10 * (s[0] - '0') + (s[1] - '0');
}


#define LENGTH_COLON_1 2
#define LENGTH_COLON_2 5
#define LENGTH_NUM_1 0
#define LENGTH_NUM_2 3
#define LENGTH_NUM_3 6

#define MAX_HOURS 99
#define MAX_MINUTES 59
#define MAX_SECONDS 59

int isValidLength(char *s)
{
    if (strlen(s) != 8)
        return 0;

    if (s[LENGTH_COLON_1] != ':' || s[LENGTH_COLON_2] != ':')
        return 0;


    int hours = getInt(s+LENGTH_NUM_1);
    int minutes = getInt(s+LENGTH_NUM_2);
    int seconds = getInt(s+LENGTH_NUM_3);

    if (hours == RETURN_NAN || minutes == RETURN_NAN || seconds == RETURN_NAN)
        return 0;

    return 
        hours   >= 0 && hours   <= MAX_HOURS &&
        minutes >= 0 && minutes <= MAX_MINUTES &&
        seconds >= 0 && seconds <= MAX_SECONDS;
}

Episode **findEpisodeRecursive(Episode **episode, char *name)
{
    if (!*episode)
        return NULL;
    if (strcmp((*episode)->name, name) == 0)
        return episode;

    return findEpisodeRecursive(&(*episode)->next, name);
}

Episode **findEpisode(Season *season, char *name)
{
    return findEpisodeRecursive(&season->episodes, name);
}



void addEpisode()
{
    printf("Enter the name of the show:\n");
    char *showName = getString();
    TVShow **matchShow = findShow(showName);
    free(showName);

    if (!matchShow)
    {
        printf("Show not found.\n");
        return;
    }

    printf("Enter the name of the season:\n");
    char *seasonName = getString();
    Season **matchSeason = findSeason(*matchShow, seasonName);
    free(seasonName);

    if (!matchSeason)
    {
        printf("Season not found.\n");
        return;
    }

    Season *season = *matchSeason;


    printf("Enter the name of the episode:\n");
    char *episodeName = getString();
    printf("Enter the length (xx:xx:xx):\n");
    char *length;

    while (1)
    {
        length = getString();
        if (!isValidLength(length))
        {
            printf("Invalid length, enter again:\n");
            free(length);
        }
        else
            break;
    }

    printf("Enter the position:\n");
    char *posStr = getString();
    int pos = atoi(posStr); // We can assume non negative integer
    free(posStr);


    Episode **match = findEpisode(season, episodeName);

    if (match)
    {
        printf("Episode already exists.\n");
        free(length);
        free(episodeName);
        return;
    }


    Episode *newEpisode = (Episode*) malloc(sizeof(Episode));
    newEpisode->length = length;
    newEpisode->name = episodeName;

    Episode **iterator = &season->episodes;
    int i = 0;
    while (*iterator && i < pos)
    {
        i++;
        iterator = &(*iterator)->next;
    }

    newEpisode->next = *iterator;
    *iterator = newEpisode;
}

void freeEpisode(Episode *e)
{
    free(e->length);
    free(e->name);

    free(e);
}
void deleteEpisode()
{
    printf("Enter the name of the show:\n");
    char *showName = getString();
    TVShow **matchShow = findShow(showName);
    free(showName);

    if (!matchShow)
    {
        printf("Show not found.\n");
        return;
    }

    printf("Enter the name of the season:\n");
    char *seasonName = getString();
    Season **matchSeason = findSeason(*matchShow, seasonName);
    free(seasonName);

    if (!matchSeason)
    {
        printf("Season not found.\n");
        return;
    }

    printf("Enter the name of the episode:\n");
    char *episodeName = getString();
    Episode **episode = findEpisode(*matchSeason, episodeName);
    free(episodeName);

    if (!episode)
    {
        printf("Episode not found.\n");
        return;
    }


    Episode *toFree = *episode;

    *episode = toFree->next;

    freeEpisode(toFree);

}

void printEpisode()
{
    printf("Enter the name of the show:\n");
    char *showName = getString();
    printf("Enter the name of the season:\n");
    char *seasonName = getString();
    printf("Enter the name of the episode:\n");
    char *episodeName = getString();




    TVShow **matchShow = findShow(showName);
    free(showName);

    if (!matchShow)
    {
        free(seasonName);
        free(episodeName);
        printf("Show not found.\n");
        return;
    }

    Season **matchSeason = findSeason(*matchShow, seasonName);
    free(seasonName);

    if (!matchSeason)
    {
        free(episodeName);
        printf("Season not found.\n");
        return;
    }

    Episode **matchEpisode = findEpisode(*matchSeason, episodeName);
    free(episodeName);

    if (!matchEpisode)
    {
        printf("Episode not found.\n");
        return;
    }

    Episode *episode = *matchEpisode;
    printf("Name: %s\nLength: %s\n", episode->name, episode->length);
}




void freeAll()
{
    for (int i = 0; i < dbSize; i++)
        for (int j = 0; j < dbSize; j++)
            if (database[i][j])
                freeShow(database[i][j]);

    if (database)
    {
        free(*database);
        free(database);
    }
}
