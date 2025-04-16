#include "DisasterPlanning.h"
using namespace std;
bool isCovered(const string& city,
               const Map<string, Set<string>>& roadNetwork,
               const Set<string>& supplyLocations);

/*
 * Recursive backtracking function to try placing supplies in a given number of cities.
 */
bool canBeMadeDisasterReady(const Map<string, Set<string>>& roadNetwork,
                            Vector<string>& cities,
                            int numCities,
                            int index,
                            Set<string>& supplyLocations) {
    if (supplyLocations.size() > numCities) {
        return false;
    }
    if (index == cities.size()) {
        // Check if all cities are covered
        for (string city : cities) {
            if (!isCovered(city, roadNetwork, supplyLocations)) {
                return false;
            }
        }
        return true;
    }

    // Choice 1: Don't put a supply in this city
    if (canBeMadeDisasterReady(roadNetwork, cities, numCities, index + 1, supplyLocations)) {
        return true;
    }

    // Choice 2: Put a supply in this city
    supplyLocations += cities[index];
    if (canBeMadeDisasterReady(roadNetwork, cities, numCities, index + 1, supplyLocations)) {
        return true;
    }
    supplyLocations -= cities[index];

    return false;
}

/*
 * Main function to call backtracking to find a valid supply placement.
 */
Optional<Set<string>> placeEmergencySupplies(const Map<string, Set<string>>& roadNetwork,
                                             int numCities) {
    Set<string> result;
    Vector<string> cities = roadNetwork.keys();

    if (canBeMadeDisasterReady(roadNetwork, cities, numCities, 0, result)) {
        return result;
    }

    return Nothing;
}

/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST ("Reports an error if numCities < 0") {
    EXPECT_ERROR(placeEmergencySupplies({ }, -137));
}

PROVIDED_TEST ("Works for map with no cities.") {
    /* Should be able to find the solution whether there's 0 allowed cities,
     * 137 allowed cities, etc.
     */
    EXPECT_EQUAL(placeEmergencySupplies({ }, 0), {});
    EXPECT_EQUAL(placeEmergencySupplies({ }, 137), {});
}

PROVIDED_TEST("Works for map with one city.") {
    Map<string, Set<string>> map = makeSymmetric({
        { "Solipsist", {} }
    });

    /* Shouldn't matter how many cities we use, as long as it isn't zero! */
    EXPECT_EQUAL    (placeEmergencySupplies(map, 0), Nothing);
    EXPECT_NOT_EQUAL(placeEmergencySupplies(map, 1), Nothing);
    EXPECT_NOT_EQUAL(placeEmergencySupplies(map, 2), Nothing);
}

PROVIDED_TEST("Works for map with one city, and produces output.") {
    Map<string, Set<string>> map = makeSymmetric({
        { "Solipsist", {} }
    });

    EXPECT_EQUAL(placeEmergencySupplies(map, 0), Nothing);
    EXPECT_EQUAL(placeEmergencySupplies(map, 1), {"Solipsist"});
    EXPECT_EQUAL(placeEmergencySupplies(map, 2), {"Solipsist"});
}

PROVIDED_TEST("Works for map with two linked cities.") {
    Map<string, Set<string>> map = makeSymmetric({
        { "A", { "B" } },
        { "B", {     } }
    });

    EXPECT_EQUAL    (placeEmergencySupplies(map, 0), Nothing);
    EXPECT_NOT_EQUAL(placeEmergencySupplies(map, 1), Nothing);
    EXPECT_NOT_EQUAL(placeEmergencySupplies(map, 2), Nothing);
}

PROVIDED_TEST("Works for map with two linked cities, and produces output.") {
    Map<string, Set<string>> map = makeSymmetric({
                                                  { "A", { "B" } },
                                                  });

    EXPECT_EQUAL(placeEmergencySupplies(map, 0), Nothing);

    Optional<Set<string>> locations1 = placeEmergencySupplies(map, 1);
    EXPECT_NOT_EQUAL(locations1, Nothing);
    EXPECT_EQUAL(locations1.value().size(), 1);
    EXPECT(locations1.value().isSubsetOf({ "A", "B" }));

    Optional<Set<string>> locations2 = placeEmergencySupplies(map, 2);
    EXPECT_NOT_EQUAL(locations2, Nothing);
    EXPECT(locations2.value().size() <= 2);
    EXPECT(locations2.value().isSubsetOf({ "A", "B" }));
}

PROVIDED_TEST("Works for four disconnected cities.") {
    Map<string, Set<string>> map = makeSymmetric({
        { "A", { } },
        { "B", { } },
        { "C", { } },
        { "D", { } }
    });

    EXPECT_EQUAL(placeEmergencySupplies(map, 0), Nothing);
    EXPECT_EQUAL(placeEmergencySupplies(map, 1), Nothing);
    EXPECT_EQUAL(placeEmergencySupplies(map, 2), Nothing);
    EXPECT_EQUAL(placeEmergencySupplies(map, 3), Nothing);
    EXPECT_NOT_EQUAL(placeEmergencySupplies(map, 4), Nothing);
}

PROVIDED_TEST("Works for four disconnected cities, and produces output.") {
    Map<string, Set<string>> map = makeSymmetric({
        { "A", { } },
        { "B", { } },
        { "C", { } },
        { "D", { } }
    });

    EXPECT_EQUAL(placeEmergencySupplies(map, 0), Nothing);
    EXPECT_EQUAL(placeEmergencySupplies(map, 1), Nothing);
    EXPECT_EQUAL(placeEmergencySupplies(map, 2), Nothing);
    EXPECT_EQUAL(placeEmergencySupplies(map, 3), Nothing);
    EXPECT_EQUAL(placeEmergencySupplies(map, 4), {"A", "B", "C", "D"});
}

PROVIDED_TEST("Can solve ethene example, regardless of ordering.") {
    /* Because Map and Set internally store items in sorted order, the order
     * in which you iterate over the cities when making decisions is sensitive
     * to the order of those cities' names. This test looks at a map like
     * this one, trying out all possible orderings of the city names:
     *
     *             *
     *             |
     *        * -- * -- * -- *
     *                  |
     *                  *
     *
     * There are 6! = 720 possible permutations of the ordering of these six
     * cities. If your code is able to solve the problem correctly for all of
     * those orderings, there's a good chance that you're correctly
     * covering and uncovering cities at each step. On the other hand, if
     * your code runs into issues here, it may indicate that the way in which
     * you're covering and uncovering cities accidentally uncovers a city that
     * you have previously covered.
     */
    Vector<string> cities = { "A", "B", "C", "D", "E", "F" };
    do {
        /* Ethene arrangement:
         *
         *        0
         *       1234
         *         5
         */
        Map<string, Set<string>> map = makeSymmetric({
            { cities[2], { cities[0], cities[1], cities[3] } },
            { cities[3], { cities[4], cities[5] } }
        });

        /* We should be able to cover everything with two cities:
         * city 2 and city 3.
         */
        EXPECT_NOT_EQUAL(placeEmergencySupplies(map, 2), Nothing);

        /* We should not be able to cover everything with one city. */
        EXPECT_EQUAL(placeEmergencySupplies(map, 1), Nothing);
    } while (next_permutation(cities.begin(), cities.end()));
}

PROVIDED_TEST("Can solve ethene example, regardless of ordering, and produces output.") {
    /* Because Map and Set internally store items in sorted order, the order
     * in which you iterate over the cities when making decisions is sensitive
     * to the order of those cities' names. This test looks at a map like
     * this one, trying out all possible orderings of the city names:
     *
     *             *
     *             |
     *        * -- * -- * -- *
     *                  |
     *                  *
     *
     * There are 6! = 720 possible permutations of the ordering of these six
     * cities. If your code is able to solve the problem correctly for all of
     * those orderings, there's a good chance that you're correctly
     * covering and uncovering cities at each step. On the other hand, if
     * your code runs into issues here, it may indicate that the way in which
     * you're covering and uncovering cities accidentally uncovers a city that
     * you have previously covered.
     */
    Vector<string> cities = { "A", "B", "C", "D", "E", "F" };
    do {
        /* Ethene arrangement:
         *
         *        0
         *       1234
         *         5
         */
        Map<string, Set<string>> map = makeSymmetric({
            { cities[2], { cities[0], cities[1], cities[3] } },
            { cities[3], { cities[4], cities[5] } }
        });

        /* We should be able to cover everything with two cities:
         * city 2 and city 3.
         */
        EXPECT_EQUAL(placeEmergencySupplies(map, 2), { cities[2], cities[3] });

        /* We should not be able to cover everything with one city. */
        EXPECT_EQUAL(placeEmergencySupplies(map, 1), Nothing);
    } while (next_permutation(cities.begin(), cities.end()));
}

PROVIDED_TEST("Works for six cities in a line, regardless of order.") {
    /* Because Map and Set internally store items in sorted order, the order
     * in which you iterate over the cities when making decisions is sensitive
     * to the order of those cities' names. This test looks at a map like
     * this one, trying out all possible orderings of the city names:
     *
     *        * -- * -- * -- * -- * -- *
     *
     * There are 6! = 720 possible permutations of the ordering of these six
     * cities in a line. If your code is able to solve the problem correctly
     * for all of those orderings, there's a good chance that you're correctly
     * covering and uncovering cities at each step. On the other hand, if
     * your code runs into issues here, it may indicate that the way in which
     * you're covering and uncovering cities accidentally uncovers a city that
     * you have previously covered.
     */
    Vector<string> cities = { "A", "B", "C", "D", "E", "F" };
    do {
        /* Linear arrangement. */
        Map<string, Set<string>> map;
        for (int i = 1; i + 1 < cities.size(); i++) {
            map[cities[i]] = { cities[i - 1], cities[i + 1] };
        }

        map = makeSymmetric(map);

        /* We should be able to cover everything with two cities, specifically,
         * the cities one spot in from the two sides.
         */
        EXPECT_NOT_EQUAL(placeEmergencySupplies(map, 2), Nothing);

        /* We should not be able to cover everything with one city. */
        EXPECT_EQUAL(placeEmergencySupplies(map, 1), Nothing);
    } while (next_permutation(cities.begin(), cities.end()));
}

PROVIDED_TEST("Works for six cities in a line, regardless of order, and produces output.") {
    /* Because Map and Set internally store items in sorted order, the order
     * in which you iterate over the cities when making decisions is sensitive
     * to the order of those cities' names. This test looks at a map like
     * this one, trying out all possible orderings of the city names:
     *
     *        * -- * -- * -- * -- * -- *
     *
     * There are 6! = 720 possible permutations of the ordering of these six
     * cities in a line. If your code is able to solve the problem correctly
     * for all of those orderings, there's a good chance that you're correctly
     * covering and uncovering cities at each step. On the other hand, if
     * your code runs into issues here, it may indicate that the way in which
     * you're covering and uncovering cities accidentally uncovers a city that
     * you have previously covered.
     */
    Vector<string> cities = { "A", "B", "C", "D", "E", "F" };
    do {
        /* Linear arrangement. */
        Map<string, Set<string>> map;
        for (int i = 1; i + 1 < cities.size(); i++) {
            map[cities[i]] = { cities[i - 1], cities[i + 1] };
        }

        map = makeSymmetric(map);

        /* We should be able to cover everything with two cities, specifically,
         * the cities one spot in from the two sides.
         */
        Optional<Set<string>> chosen = placeEmergencySupplies(map, 2);
        EXPECT_NOT_EQUAL(chosen, Nothing);

        EXPECT_EQUAL(chosen.value().size(), 2);
        EXPECT(chosen.value().contains(cities[1]));
        EXPECT(chosen.value().contains(cities[4]));

        /* We should not be able to cover everything with one city. */
        EXPECT_EQUAL(placeEmergencySupplies(map, 1), Nothing);
    } while (next_permutation(cities.begin(), cities.end()));
}

/* The "Don't Be Greedy" sample world. */
const Map<string, Set<string>> kDontBeGreedy = makeSymmetric({
                                                              { "A", { "B" } },
                                                              { "B", { "C", "D" } },
                                                              { "C", { "D" } },
                                                              { "D", { "F", "G" } },
                                                              { "E", { "F" } },
                                                              { "F", { "G" } },
                                                              });

PROVIDED_TEST("Solves \"Don't be Greedy\" from the handout."); {
    EXPECT_EQUAL(placeEmergencySupplies(kDontBeGreedy, 0), Nothing);
    EXPECT_EQUAL(placeEmergencySupplies(kDontBeGreedy, 1), Nothing);
    EXPECT_NOT_EQUAL(placeEmergencySupplies(kDontBeGreedy, 2), Nothing);
}

PROVIDED_TEST("Solves \"Don't be Greedy\" from the handout, and produces output."); {
    EXPECT_EQUAL(placeEmergencySupplies(kDontBeGreedy, 0), Nothing);
    EXPECT_EQUAL(placeEmergencySupplies(kDontBeGreedy, 1), Nothing);
    EXPECT_EQUAL(placeEmergencySupplies(kDontBeGreedy, 2), {"B", "F"});
}

PROVIDED_TEST("Solves \"Don't be Greedy,\" regardless of ordering, and produces output."); {
    /* Because Map and Set internally store items in sorted order, the order
     * in which you iterate over the cities when making decisions is sensitive
     * to the order of those cities' names. This test looks at a map like
     * this one, trying out all possible orderings of the city names:
     *
     *     0       4
     *     |       |
     *     1 - 2 - 3
     *      \ / \ /
     *       5   6
     *
     * There are 7! = 5,040 possible permutations of the ordering of these seven
     * cities. If your code is able to solve the problem correctly for all of
     * those orderings, there's a good chance that you're correctly
     * covering and uncovering cities at each step. On the other hand, if
     * your code runs into issues here, it may indicate that the way in which
     * you're covering and uncovering cities accidentally uncovers a city that
     * you have previously covered.
     */
    Vector<string> cities = { "A", "B", "C", "D", "E", "F", "G" };
    do {
        Map<string, Set<string>> map = makeSymmetric({
                                                      { cities[1], { cities[0], cities[2], cities[5] } },
                                                      { cities[2], { cities[3], cities[5], cities[6] } },
                                                      { cities[3], { cities[4], cities[6] } },
                                                      });

        /* We should be able to cover everything with two cities. */
        EXPECT_EQUAL(placeEmergencySupplies(map, 2), { cities[1], cities[3] });
        EXPECT_EQUAL(placeEmergencySupplies(map, 1), Nothing);
    } while (next_permutation(cities.begin(), cities.end()));
}

PROVIDED_TEST("Stress test: 6 x 6 grid."); {
    Map<string, Set<string>> grid;

    /* Build the grid. */
    char maxRow = 'F';
    int  maxCol = 6;
    for (char row = 'A'; row <= maxRow; row++) {
        for (int col = 1; col <= maxCol; col++) {
            if (row != maxRow) {
                grid[row + to_string(col)] += (char(row + 1) + to_string(col));
            }
            if (col != maxCol) {
                grid[row + to_string(col)] += (char(row) + to_string(col + 1));
            }
        }
    }
    grid = makeSymmetric(grid);


    /* 10x factor of safety relative to my middle-of-the-line computer. */
    EXPECT_COMPLETES_IN(20.0,
                        EXPECT_NOT_EQUAL(placeEmergencySupplies(grid, 10), Nothing);
                        );
}

PROVIDED_TEST("Stress test: 6 x 6 grid, with output."); {
    Optional<Set<string>> locations;
    char maxRow = 'F';
    int  maxCol = 6;

    Map<string, Set<string>> grid;

    /* Build the grid. */
    for (char row = 'A'; row <= maxRow; row++) {
        for (int col = 1; col <= maxCol; col++) {
            if (row != maxRow) {
                grid[row + to_string(col)] += (char(row + 1) + to_string(col));
            }
            if (col != maxCol) {
                grid[row + to_string(col)] += (char(row) + to_string(col + 1));
            }
        }
    }
    grid = makeSymmetric(grid);

    /* 10x factor of safety relative to my middle-of-the-line computer. */
    EXPECT_COMPLETES_IN(20.0,
                        locations = placeEmergencySupplies(grid, 10);
                        );
    EXPECT_NOT_EQUAL(locations, Nothing);
    EXPECT_LESS_THAN_OR_EQUAL_TO(locations.value().size(), 10);

    for (char row = 'A'; row <= maxRow; row++) {
        for (int col = 1; col <= maxCol; col++) {
            EXPECT(isCovered(row + to_string(col), grid, locations.value()));
        }
    }
}

