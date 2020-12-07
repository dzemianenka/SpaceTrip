#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

struct SHIP {
    double jump;
    double fuel;
};

const double DEFAULT_MIN_DISTANCE = 99999.99;
const int DEFAULT_INDEX_OF_MIN_DISTANCE = 999;
const int START_INDEX = 0;

double** initializeMap(int stars);
void createStarMap(double *const *starMap, int stars);
void calculateFuelMap(double *const *starMap, double *const *fuelMap, const SHIP &ship, int stars);
template <typename T> void doInsertDefault(T *array, int stars, T defaultValue);
void printMap(double *const *map, int stars, ofstream &file, const char *text);
void findTheBestWay(double *const *map, const SHIP &ship, int stars, ofstream &file);
void getMinDistanceToEachStar(double *const *map, double *minDistance, int stars);
void printOptimalWay(double *const *map, double const *minDistance, const SHIP &ship, int stars, ofstream &file);
void checkIfItPossibleToPassOptimalWay(double *const *map, const SHIP &ship, const int *way, int k, ofstream &file);

int main() {
    int stars;
    printf("Insert the number of stars(int): ");
    scanf("%d", &stars);

    SHIP ship{};
    printf("Insert the ship jump(double): ");
    scanf("%lf", &ship.jump);
    printf("Insert the ship fuel(double): ");
    scanf("%lf", &ship.fuel);

    char timeStr[34];
    time_t rawTime;
    struct tm * timeInfo;
    time(&rawTime);
    timeInfo = localtime (&rawTime);
    strftime(timeStr, 34, "%d.%m.%Y_%H:%M:%S_SpaceTrip.txt\n", timeInfo);

    ofstream file(timeStr, ios_base::app);
    if (!file.is_open()) {
        printf("Error to open file '%s'\n", timeStr);
        return 1;
    }

    auto **starMap = initializeMap(stars);
    createStarMap(starMap, stars);
    printMap(starMap, stars, file, "Star Map");
    findTheBestWay(starMap, ship, stars, file);

    auto **fuelMap = initializeMap(stars);
    calculateFuelMap(starMap, fuelMap, ship, stars);
    printMap(fuelMap, stars, file, "Fuel Map");
    findTheBestWay(fuelMap, ship, stars, file);

    file.close();
    return 0;
}

double** initializeMap(int stars) {
    auto **map = new double*[stars];
    for(int i=0; i < stars; i++) {
        map[i] = new double[stars];
    }
    return map;
}

void createStarMap(double *const *starMap, int stars) {
    printf("Insert the distance (star - star)\n");
    for (int i = 0; i < stars; i++) {
        starMap[i][i] = 0;
        double distance;
        for (int j = i + 1; j < stars; j++) {
            printf("%d - %d: ", i + 1, j + 1);
            scanf("%lf", &distance);
            starMap[i][j] = distance;
            starMap[j][i] = distance;
        }
    }
}

void calculateFuelMap(double *const *starMap, double *const *fuelMap, const SHIP &ship, int stars) {
    for (int i = 0; i < stars; i++) {
        for (int j = 0; j < stars; j++) {
            fuelMap[i][j] = ship.fuel * pow((1 - (ship.jump - starMap[i][j]) / ship.jump), 2);
        }
    }
}

template <typename T> void doInsertDefault(T *array, int stars, T defaultValue) {
    for (int i = 0; i < stars; i++) {
        array[i] = defaultValue;
    }
}

void printMap(double *const *map, int stars, ofstream &file, const char *text) {
    printf("%s\n", text);
    file << text << "\n";
    for (int i = 0; i < stars; i++) {
        for (int j = 0; j < stars; j++) {
            printf("%.2lf ", map[i][j]);
            file << fixed << setprecision(2) << map[i][j] << " ";
        }
        printf("\n");
        file << "\n";
    }
}

void findTheBestWay(double *const *map, const SHIP &ship, int stars, ofstream &file) {
    auto *minDistance = new double[stars];
    doInsertDefault(minDistance, stars, DEFAULT_MIN_DISTANCE);

    getMinDistanceToEachStar(map, minDistance, stars);

    printf("Min distance to star: ");
    file << "Min distance to star: ";
    for (int i = 0; i < stars; i++) {
        printf("%.2lf ", minDistance[i]);
        file << fixed << setprecision(2) << minDistance[i] << " ";
    }

    printOptimalWay(map, minDistance, ship, stars, file);
}

void getMinDistanceToEachStar(double *const *map, double *minDistance, int stars) {
    auto *visited = new bool[stars];
    doInsertDefault(visited, stars, false);

    minDistance[START_INDEX] = 0;
    int indexOfMinDistance;
    double temp, valueOfMinDistance;
    do {
        indexOfMinDistance = DEFAULT_INDEX_OF_MIN_DISTANCE;
        valueOfMinDistance = DEFAULT_MIN_DISTANCE;
        for (int i = 0; i < stars; i++) {
            if (!visited[i] && (minDistance[i] < valueOfMinDistance)) {
                valueOfMinDistance = minDistance[i];
                indexOfMinDistance = i;
            }
        }

        if (indexOfMinDistance != DEFAULT_INDEX_OF_MIN_DISTANCE) {
            for (int i = 0; i < stars; i++) {
                if (map[indexOfMinDistance][i] > 0) {
                    temp = valueOfMinDistance + map[indexOfMinDistance][i];
                    if (temp < minDistance[i]) {
                        minDistance[i] = temp;
                    }
                }
            }
            visited[indexOfMinDistance] = true;
        }
    } while (indexOfMinDistance < DEFAULT_INDEX_OF_MIN_DISTANCE);
}

void printOptimalWay(double *const *map, double const *minDistance, const SHIP &ship, int stars, ofstream &file) {
    int way[stars];
    int end = stars - 1;
    way[0] = stars;
    int k = 1;
    double reverseTraversal = minDistance[end];
    while (end != START_INDEX) {
        for (int i = 0; i < stars; i++) {
            if (map[i][end] != 0) {
                double temp = reverseTraversal - map[i][end];
                if (temp == minDistance[i]) {
                    reverseTraversal = temp;
                    end = i;
                    way[k] = i + 1;
                    k++;
                }
            }
        }
    }

    checkIfItPossibleToPassOptimalWay(map, ship, way, k, file);

    printf("\nOptimal way: ");
    file << "\nOptimal way: ";
    for (int i = k - 1; i >= 0; i--) {
        printf("%d ", way[i]);
        file << way[i] << " ";
    }
    printf("\n\n");
    file << "\n\n";
}

void checkIfItPossibleToPassOptimalWay(double *const *map, const SHIP &ship, const int *way, int k, ofstream &file) {
    for (int i = k - 1; i > 0; i--) {
        if (map[way[i]-1][way[i-1]-1] > ship.jump) {
            printf("\nError: jump of your ship less than optimal way");
            file << "\nError: jump of your ship less than optimal way";
        }
    }
}
