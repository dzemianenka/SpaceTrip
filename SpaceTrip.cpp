#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

struct SHIP {
    double jump;
    double fuel;
};

const char* NAME_OF_FILE = "SpaceTrip.txt";
const double DEFAULT_MIN_DISTANCE = 99999.99;
const int DEFAULT_INDEX_OF_MIN_DISTANCE = 999;
const int START_INDEX = 0;

double** initializeMap(int stars);
void createStarMap(double *const *starMap, int stars);
void calculateFuelMap(double *const *starMap, double *const *fuelMap, const SHIP &ship, int stars);
template <typename T> T* doInsertDefault(int stars, T defaultValue);
void printMap(double *const *map, int stars, ofstream &file, const char *text);
void findTheBestWay(double *const *map, const SHIP &ship, int stars, ofstream &file);
void getMinDistanceToEachStar(double *const *map, double *minDistance, int stars);
void printOptimalPath(double *const *map, double const *minDistance, const SHIP &ship, int stars, ofstream &file);
void checkIfItPossibleToPassOptimalPath(double *const *map, const SHIP &ship, const int *path, int k, ofstream &file);
void printDescription(ofstream &file, const char *text);
void printFooter(ofstream &file);

int main() {
    int stars;
    printf("Insert the number of stars(int): ");
    scanf("%d", &stars);

    SHIP ship{};
    printf("Insert the ship jump(double): ");
    scanf("%lf", &ship.jump);
    printf("Insert the ship fuel(double): ");
    scanf("%lf", &ship.fuel);

    ofstream file(NAME_OF_FILE, ios_base::app);
    if (!file.is_open()) {
        printf("Error to open file '%s'\n", NAME_OF_FILE);
        return 1;
    }

    auto **starMap = initializeMap(stars);
    createStarMap(starMap, stars);
    printMap(starMap, stars, file, "Star Map");
    findTheBestWay(starMap, ship, stars, file);
    printDescription(file, "faster");

    auto **fuelMap = initializeMap(stars);
    calculateFuelMap(starMap, fuelMap, ship, stars);
    printMap(fuelMap, stars, file, "Fuel Map");
    findTheBestWay(fuelMap, ship, stars, file);
    printDescription(file, "more economical");

    printFooter(file);
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

template <typename T> T* doInsertDefault(int stars, T defaultValue) {
    auto *array = new T[stars];
    for (int i = 0; i < stars; i++) {
        array[i] = defaultValue;
    }
    return array;
}

void printMap(double *const *map, int stars, ofstream &file, const char *text) {
    printf("\n%s\n", text);
    file << "\n" << text << "\n";
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
    auto *minDistance = doInsertDefault(stars, DEFAULT_MIN_DISTANCE);

    getMinDistanceToEachStar(map, minDistance, stars);

    printf("Min distance to another stars: ");
    file << "Min distance to another stars: ";
    for (int i = 0; i < stars; i++) {
        printf("%.2lf ", minDistance[i]);
        file << fixed << setprecision(2) << minDistance[i] << " ";
    }

    printOptimalPath(map, minDistance, ship, stars, file);
}

void getMinDistanceToEachStar(double *const *map, double *minDistance, int stars) {
    auto *visited = doInsertDefault(stars, false);

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

void printOptimalPath(double *const *map, double const *minDistance, const SHIP &ship, int stars, ofstream &file) {
    int path[stars];
    int end = stars - 1;
    path[0] = stars;
    int k = 1;
    double reverseTraversal = minDistance[end];
    while (end != START_INDEX) {
        for (int i = 0; i < stars; i++) {
            if (map[i][end] != 0) {
                double temp = reverseTraversal - map[i][end];
                if (temp == minDistance[i]) {
                    reverseTraversal = temp;
                    end = i;
                    path[k] = i + 1;
                    k++;
                }
            }
        }
    }

    checkIfItPossibleToPassOptimalPath(map, ship, path, k, file);

    printf("\nOptimal path: ");
    file << "\nOptimal path: ";
    for (int i = k - 1; i >= 0; i--) {
        printf("%d ", path[i]);
        file << path[i] << " ";
    }
    printf("\n");
    file << "\n";
}

void checkIfItPossibleToPassOptimalPath(double *const *map, const SHIP &ship, const int *path, int k, ofstream &file) {
    for (int i = k - 1; i > 0; i--) {
        if (map[path[i] - 1][path[i - 1] - 1] > ship.jump) {
            printf("\nError: jump of your ship less than optimal path");
            file << "\nError: jump of your ship less than optimal path";
        }
    }
}

void printDescription(ofstream &file, const char *text) {
    printf("This is a %s way\n", text);
    file << "This is a " << text << " way\n";
}

void printFooter(ofstream &file) {
    printf("\nYou can chose between fastest and more economical ways\n====================\n");
    file << "\nYou can chose between fastest and more economical ways\n====================\n";
}
