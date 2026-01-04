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
        database[i] = *database + i*dbSize;       
}
void expandDB()
{
    database = (TVShow***) realloc(database, (++dbSize)*sizeof(TVShow**));
    *database = (TVShow**) realloc(*database, (dbSize*dbSize)*sizeof(TVShow*));
    
    for (int i = 0; i < dbSize; i++)
        database[i] = *database + i*dbSize;

    // When we expand we must clear the newly allocated part.
    int newAllocBeginning = (dbSize-1)*(dbSize-1);
    for (int i = newAllocBeginning; i < dbSize*dbSize; i++)
        (*database)[i] = NULL;
}



void addShow()
{
    printf("Enter the name of the show:\n");

    TVShow* newShow = (TVShow*) malloc(sizeof(*newShow)); 
    newShow->name = getString();
    newShow->seasons = NULL;


    if (findShow(newShow->name)) // show already exists
    {
        free(newShow);
        printf("Show already exists.\n");
        return;
    }
    if (dbSize == 0 || database[dbSize-1][dbSize-1]) // database is full
        expandDB();

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
        if (!(*database)[i])
            return NULL;
        if (strcmp((*database)[i]->name, name) == 0)
            return &(*database)[i];
    }
    return NULL;
}





void freeShow(TVShow *show)
{
    free(show->name);

    Season *season = show->seasons;
    while (season)
    {
        Season *temp = season->next;
        freeSeason(show->seasons);
        season = temp;
    }

    free(show);
}

void deleteShow()
{
    printf("Enter the name of the show:\n");
    char *name = getString();

    TVShow** match = NULL;
    int i;
    for (int i = 0; i < dbSize*dbSize; i++)
        if (strcmp((*database)[i]->name, name) == 0)
            match = &(*database)[i];
    free(name);

    if (!match)
    {
        printf("Show not found\n");
        return;
    }
    freeShow(*match);
    *match = NULL;

    // Shift remaining shows
    while (++i < dbSize*dbSize && (*database)[i]) // while we have more spots to shift
        (*database)[i-1] = (*database)[i];

    // Shrink
    if (!(*database)[(dbSize-1)*(dbSize-1)+1]) // if we can shrink
        shrinkDB();

}

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
    TVShow** show = findShow(showName);
    free(showName);

    if (!show)
    {
        printf("Show not found.\n");
        return;
    }

    printf("Enter the name of the season:\n");

    char *seasonName = getString();
    if (findSeason(*show, seasonName))
    {
        printf("Season already exists.\n");
        free(seasonName);
        return;
    }

    printf("Enter the position:\n");
    char *posStr = getString();
    int pos = atoi(posStr); // We can assume non negative integer
    free(posStr);

    Season *newSeason = (Season*)malloc(sizeof(*newSeason));

    newSeason->episodes = NULL;
    newSeason->name = seasonName;
    newSeason->next = NULL;


    if (!(*show)->seasons)
    {
        (*show)->seasons = newSeason;
        return;
    }

    addSeasonRecursive(&(*show)->seasons, newSeason, pos);
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
    TVShow *show = *findShow(showName);
    free(showName);

    if (!show)
    {
        printf("Show not found.\n");
        return;
    }

    printf("Enter the name of the season:\n");
    char *seasonName = getString();
    Season **season= findSeason(show, seasonName);
    free(seasonName);

    if (!season)
    {
        printf("Season not found.\n");
        return;
    }

    while (*season)
    {
        *season = (*season)->next;
        season = &(*season)->next;
    }

    free(*season);
}

void freeSeason(Season *s)
{
   free(s->name);

   Episode *episode = s->episodes;
   while (episode)
   {
        Episode *temp = episode->next;
        free(episode);
        episode = temp;
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
        printf("Show not found\n");
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
            printf("\t\tEpisode %d: %s (%s)", episodeIndex, episode->name, episode->length);
            episode = episode->next;
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
            if (!database[i][j])
            {
                putchar('\n');
                return;
            }
            printf("[%s]", database[i][j]->name); 
        }
        putchar('\n');
    }
}


int validLength(char *s)
{
    int hours = 0;
    int minutes = 0;
    int seconds = 0;
    int matches = sscanf(s, "%d:%d:%d", &hours, &minutes, &seconds);

    return (matches == 3 || 
        hours >= 0 || hours <= 99 ||
        minutes >= 0 || minutes <= 59 ||
        seconds >= 0 || seconds <= 59);
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
        if (!validLength(length))
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


    if (!season->episodes) // We have no episodes
    {
        newEpisode->next = NULL;
        season->episodes = newEpisode;
        return;
    }
    
    Episode **posForEpisode = &season->episodes;
    for (int i = 0; i < pos; i++)
    {
        if (!(*posForEpisode)->next)
        {
            pos = i;
            break;
        }
        posForEpisode = &(*posForEpisode)->next;
    }

    Episode *next = *posForEpisode;
    *posForEpisode = newEpisode;
    newEpisode->next = next;
}

void deleteEpisode()
{
}

void printEpisode()
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
}




void freeAll()
{
}




// TODO: one getString is leaked. prob episode.
//       make sure this thing is readable and not a mess
//       finish deleteEpisode and freeAll (ooh maybe its this thats leaking)
