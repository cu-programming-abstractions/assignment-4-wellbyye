#include "ShiftScheduling.h"
using namespace std;

/* Helper to check for overlap between a shift and a set of shifts */
bool isNonOverlapping(const Shift& shift, const Set<Shift>& selected) {
    for (const Shift& s : selected) {
        if (overlapsWith(shift, s)) return false;
    }
    return true;
}

int numSchedulesForHelper(const Set<Shift>& shifts, int maxHours, Set<Shift>& chosen) {
    if (maxHours < 0) return 0;

    int count = 1; // base case: current combination is valid

    for (const Shift& shift : shifts) {
        if (isNonOverlapping(shift, chosen)) {
            Set<Shift> remaining = shifts;
            remaining.remove(shift);
            chosen.add(shift);

            count += numSchedulesForHelper(remaining, maxHours - lengthOf(shift), chosen);
            chosen.remove(shift); // backtrack
        }
    }

    return count;
}

int numSchedulesFor(const Set<Shift>& shifts, int maxHours) {
    Set<Shift> chosen;
    return numSchedulesForHelper(shifts, maxHours, chosen) - 1; // subtract base empty case
}

Set<Shift> bestScheduleForHelper(const Set<Shift>& shifts, int maxHours,
                                 Set<Shift>& chosen, Set<Shift>& bestSoFar) {
    int currentHours = 0;
    for (const Shift& s : chosen) currentHours += lengthOf(s);

    int bestHours = 0;
    for (const Shift& s : bestSoFar) bestHours += lengthOf(s);

    if (currentHours > bestHours) {
        bestSoFar = chosen;
    }

    for (const Shift& shift : shifts) {
        if (isNonOverlapping(shift, chosen) && maxHours >= lengthOf(shift)) {
            Set<Shift> remaining = shifts;
            remaining.remove(shift);
            chosen.add(shift);

            bestScheduleForHelper(remaining, maxHours - lengthOf(shift), chosen, bestSoFar);
            chosen.remove(shift); // backtrack
        }
    }

    return bestSoFar;
}

Set<Shift> maxProfitSchedule(const Set<Shift>& shifts, int maxHours) {
    Set<Shift> chosen, best;
    return bestScheduleForHelper(shifts, maxHours, chosen, best);
}


/* * * * * * Test cases from the starter files below this point. * * * * * */
#include "vector.h"
#include "GUI/SimpleTest.h"

PROVIDED_TEST("numSchedulesFor reports errors with illegal numbers of hours.") {
    EXPECT_ERROR(numSchedulesFor({}, -137));
    EXPECT_ERROR(numSchedulesFor({}, -1));
    EXPECT_NO_ERROR(numSchedulesFor({}, 0));
}

PROVIDED_TEST("numSchedulesFor works with just one shift.") {
    Set<Shift> shifts = {
        { Day::MONDAY, 9, 17 },  // Monday, 9AM - 5PM
    };

    EXPECT_EQUAL(numSchedulesFor(shifts, 24), 2);
    EXPECT_EQUAL(numSchedulesFor(shifts, 8), 2);
    EXPECT_EQUAL(numSchedulesFor(shifts, 7), 1);
}

PROVIDED_TEST("numSchedulesFor does not pick overlapping shifts.") {
    Set<Shift> shifts = {
        { Day::MONDAY,  9, 17 },  // Monday,  9AM - 5PM
        { Day::MONDAY, 10, 18 },  // Monday, 10AM - 6PM
    };

    EXPECT_EQUAL(numSchedulesFor(shifts, 100), 3);
    EXPECT_EQUAL(numSchedulesFor(shifts, 8), 3);
    EXPECT_EQUAL(numSchedulesFor(shifts, 7), 1);
}

PROVIDED_TEST("numSchedulesFor does not pick shifts exceeding time limit.") {
    Set<Shift> shifts = {
        { Day::MONDAY,    10, 20 }, // 10-hour shift
        { Day::TUESDAY,   10, 15 }, //  5-hour shift
        { Day::WEDNESDAY, 10, 16 }, //  6-hour shift
    };

    EXPECT_EQUAL(numSchedulesFor(shifts, 137), 8);
    EXPECT_EQUAL(numSchedulesFor(shifts, 11), 5);
    EXPECT_EQUAL(numSchedulesFor(shifts, 10), 4);
    EXPECT_EQUAL(numSchedulesFor(shifts, 6), 3);
}


PROVIDED_TEST("numSchedulesFor handles one pair of overlapping shifts.") {
    Set<Shift> shifts = {
        { Day::MONDAY,    10, 14 }, //  4-hour shift
        { Day::MONDAY,    12, 16 }, //  4-hour shift
        { Day::WEDNESDAY, 10, 14 }, //  4-hour shift
    };

    EXPECT_EQUAL(numSchedulesFor(shifts, 137), 6);
    EXPECT_EQUAL(numSchedulesFor(shifts, 8), 6);
}


PROVIDED_TEST("numSchedulesFor handles two pairs of overlapping shifts.") {
    Set<Shift> shifts = {
        { Day::MONDAY,    10, 14 }, //  4-hour shift
        { Day::MONDAY,    12, 16 }, //  4-hour shift
        { Day::WEDNESDAY, 10, 14 }, //  4-hour shift
        { Day::WEDNESDAY, 12, 16 }, //  4-hour shift
    };

    EXPECT_EQUAL(numSchedulesFor(shifts, 137), 9);
    EXPECT_EQUAL(numSchedulesFor(shifts, 8), 9);
}


PROVIDED_TEST("numSchedulesFor handles two pairs of overlapping shifts with time limits.") {
    Set<Shift> shifts = {
        { Day::MONDAY,    10, 14 }, //  4-hour shift
        { Day::MONDAY,    12, 18 }, //  6-hour shift
        { Day::WEDNESDAY, 10, 14 }, //  4-hour shift
        { Day::WEDNESDAY, 12, 18 }, //  6-hour shift
    };

    EXPECT_EQUAL(numSchedulesFor(shifts, 137), 9);
    EXPECT_EQUAL(numSchedulesFor(shifts, 8), 6);
}

PROVIDED_TEST("numSchedulesFor passes the example from the assignment description.") {
    Set<Shift> shifts = {
        { Day::MONDAY,     8, 12 },  // Mon  8AM - 12PM
        { Day::MONDAY,    12, 16 },  // Mon 12PM -  4PM
        { Day::MONDAY,    16, 20 },  // Mon  4PM -  8PM
        { Day::MONDAY,     8, 14 },  // Mon  8AM -  2PM
        { Day::MONDAY,    14, 20 },  // Mon  2PM -  8PM
        { Day::TUESDAY,    8, 12 },  // Tue  8AM - 12PM
        { Day::TUESDAY,   12, 16 },  // Tue 12PM -  4PM
        { Day::TUESDAY,   16, 20 },  // Tue  4PM -  8PM
        { Day::TUESDAY,    8, 14 },  // Tue  8AM -  2PM
        { Day::TUESDAY,   14, 20 },  // Tue  2PM -  8PM
        { Day::WEDNESDAY,  8, 12 },  // Wed  8AM - 12PM
        { Day::WEDNESDAY, 12, 16 },  // Wed 12PM -  4PM
        { Day::WEDNESDAY, 16, 20 },  // Wed  4PM -  8PM
        { Day::WEDNESDAY,  8, 14 },  // Wed  8AM -  2PM
        { Day::WEDNESDAY, 14, 20 },  // Wed  2PM -  8PM
    };

    /* There should be 1,044 options with 20 hours available. */
    EXPECT_EQUAL(numSchedulesFor(shifts, 20), 1044);
}


PROVIDED_TEST("numSchedulesFor stress test: Don't generate combinations with overlapping shifts.") {
    /* All of these shifts overlap one another. If you try producing all combinations
     * of these shifts and only check at the end whether they're valid, you'll be
     * checking 2^100 ~= 10^30 combinations of shifts, which will take so long the
     * sun will have burnt out before you're finished.
     *
     * Instead, as you're going through your decision tree and building up your shifts,
     * make sure not to include any shifts that clearly conflict with something you
     * picked earlier.
     */
    Set<Shift> trickySet;
    for (int i = 0; i < 100; i++) {
        trickySet += Shift{ Day::MONDAY, i, i + 200 };
    }
    EXPECT_EQUAL(trickySet.size(), 100);

    /* You can pick at most one of the shifts. There are 100 shifts, plus
     * the one remaining option of picking nothing at all.
     */
    EXPECT_EQUAL(numSchedulesFor(trickySet, 300), 101);
}


PROVIDED_TEST("numSchedulesFor stress test: Don't generate options exceeding time limit.") {
    /* Here's a collection of one shift per hour of the week. Your worker has exactly
     * one hour free. If you try all possible combinations of these shifts, ignoring time
     * constraints, you will have to check over 2^100 = 10^30 combinations, which will
     * take longer than the length of the known universe to process.
     *
     * Instead, as you're exploring the decision tree to generate shift combinations,
     * make sure not to add shifts that would exceed the time limit.
     */
    Set<Shift> trickySet;
    for (Day day: { Day::SUNDAY,
                    Day::MONDAY,
                    Day::TUESDAY,
                    Day::WEDNESDAY,
                    Day::THURSDAY,
                    Day::FRIDAY,
                    Day::SATURDAY}) {
        for (int start = 0; start < 24; start++) {
            trickySet += Shift{ day, start, start + 1, 10 };
        }
    }
    EXPECT_EQUAL(trickySet.size(), 7 * 24);

    /* There are 7 * 24 different shifts you can pick, and you can only pick one
     * of them. Add in the option of picking nothing and you have all the options.
     */
    EXPECT_EQUAL(numSchedulesFor(trickySet, 1), 1 + 7 * 24);
}


PROVIDED_TEST("numSchedulesFor stress test: Handles realistic set of shifts") {
    Set<Shift> shifts = {
                         { Day::SUNDAY,  8, 14 },
                         { Day::SUNDAY, 12, 18 },

                         { Day::MONDAY,  8, 12 },
                         { Day::MONDAY, 12, 16 },
                         { Day::MONDAY, 16, 20 },
                         { Day::MONDAY,  8, 16 },
                         { Day::MONDAY, 12, 20 },

                         { Day::TUESDAY,  8, 12 },
                         { Day::TUESDAY, 12, 16 },
                         { Day::TUESDAY, 16, 20 },
                         { Day::TUESDAY,  8, 16 },
                         { Day::TUESDAY, 12, 20 },

                         { Day::WEDNESDAY,  8, 12 },
                         { Day::WEDNESDAY, 12, 16 },
                         { Day::WEDNESDAY, 16, 20 },
                         { Day::WEDNESDAY,  8, 16 },
                         { Day::WEDNESDAY, 12, 20 },

                         { Day::THURSDAY,  8, 12 },
                         { Day::THURSDAY, 12, 16 },
                         { Day::THURSDAY, 16, 20 },
                         { Day::THURSDAY,  8, 16 },
                         { Day::THURSDAY, 12, 20 },

                         { Day::FRIDAY,  8, 12 },
                         { Day::FRIDAY, 12, 16 },
                         { Day::FRIDAY, 16, 20 },
                         { Day::FRIDAY,  8, 16 },
                         { Day::FRIDAY, 12, 20 },

                         { Day::SATURDAY,  8, 14 },
                         { Day::SATURDAY, 12, 18 },
                         };

    /* There are 38,107 possible schedules you can form from these shifts. */
    EXPECT_COMPLETES_IN(3.0, {
        EXPECT_EQUAL(numSchedulesFor(shifts, 25), 38107);
    });
}



/* This nice utility function lets you call highestValueScheduleFor, passing in
 * a Vector of shifts rather than a Set. This makes it a bit easier to test things.
 * You shouldn't need this function outside of these test cases.
 */
Set<Shift> asSet(const Vector<Shift>& shifts) {
    Set<Shift> result;
    for (Shift s: shifts) {
        result += s;
    }
    return result;
}

PROVIDED_TEST("maxProfitSchedule works on one shift.") {
    Set<Shift> shifts = {
        { Day::MONDAY, 9, 17, 1000 },  // Monday, 9AM - 5PM, value is 1000
    };

    /* With unbounded time, you should pick the one shift. */
    EXPECT_EQUAL(maxProfitSchedule(shifts, 24), shifts);

    /* With exactly the right amount of time, you should pick the one shift. */
    EXPECT_EQUAL(maxProfitSchedule(shifts, 24), shifts);

    /* With too little time, you cannot pick the shift. */
    EXPECT_EQUAL(maxProfitSchedule(shifts, 1), {});
}


PROVIDED_TEST("maxProfitSchedule works on a pair of overlapping shifts.") {
    Vector<Shift> shifts = {
        { Day::MONDAY,  9, 17, 1000 },  // Monday,  9AM - 5PM, value is 1000
        { Day::MONDAY, 10, 18, 2000 },  // Monday, 10AM - 6PM, value is 2000
    };

    /* Pick the second one; it's better. */
    EXPECT_EQUAL(maxProfitSchedule(asSet(shifts), 100), { shifts[1] });
}


PROVIDED_TEST("maxProfitSchedule doesn't always use highest-value shift.") {
    Vector<Shift> shifts = {
        { Day::MONDAY,    10, 20, 1000 }, // 10-hour shift, value is 1000
        { Day::TUESDAY,   10, 15,  500 }, //  5-hour shift, value is 500
        { Day::WEDNESDAY, 10, 16,  501 }, //  6-hour shift, value is 501
    };

    /* The correct strategy is to forgo the highest-value shift in favor of the two
     * shorter shifts.
     */
    auto schedule = maxProfitSchedule(asSet(shifts), 11);
    EXPECT_EQUAL(schedule, { shifts[1], shifts[2] });
}

PROVIDED_TEST("maxProfitSchedule doesn't always use shift with highest value per time.") {
    Vector<Shift> shifts = {
        { Day::MONDAY,    10, 17, 21 }, //  7-hour shift, value is 21 ($3 / hour)
        { Day::TUESDAY,   10, 16, 12 }, //  6-hour shift, value is 12 ($2 / hour)
        { Day::WEDNESDAY, 10, 16, 12 }, //  6-hour shift, value is 12 ($2 / hour)
    };

    /* If you have 12 hours, the correct strategy is to pick the two six-hour shifts
     * for a total of $24 value. Picking the shift with the highest value per unit
     * time (the seven-hour shift) produces only $21 value.
     */
    auto schedule = maxProfitSchedule(asSet(shifts), 12);
    EXPECT_EQUAL(schedule, { shifts[1], shifts[2] });
}


PROVIDED_TEST("maxProfitSchedule works on the example from the assignment description.") {
    Vector<Shift> shifts = {
        { Day::MONDAY,     8, 12, 27 },  // Mon  8AM - 12PM, value 27 *
        { Day::MONDAY,    12, 16, 28 },  // Mon 12PM -  4PM, value 28 *
        { Day::MONDAY,    16, 20, 25 },  // Mon  4PM -  8PM, value 25 *
        { Day::MONDAY,     8, 14, 39 },  // Mon  8AM -  2PM, value 39
        { Day::MONDAY,    14, 20, 31 },  // Mon  2PM -  8PM, value 31
        { Day::TUESDAY,    8, 12,  7 },  // Tue  8AM - 12PM, value  7
        { Day::TUESDAY,   12, 16,  7 },  // Tue 12PM -  4PM, value  7
        { Day::TUESDAY,   16, 20, 11 },  // Tue  4PM -  8PM, value 11
        { Day::TUESDAY,    8, 14, 10 },  // Tue  8AM -  2PM, value 10
        { Day::TUESDAY,   14, 20,  8 },  // Tue  2PM -  8PM, value  8
        { Day::WEDNESDAY,  8, 12, 10 },  // Wed  8AM - 12PM, value 10
        { Day::WEDNESDAY, 12, 16, 11 },  // Wed 12PM -  4PM, value 11
        { Day::WEDNESDAY, 16, 20, 13 },  // Wed  4PM -  8PM, value 13
        { Day::WEDNESDAY,  8, 14, 19 },  // Wed  8AM -  2PM, value 19
        { Day::WEDNESDAY, 14, 20, 25 },  // Wed  2PM -  8PM, value 25 *
    };

    /* Get back the solution. */
    Set<Shift> computedSolution = maxProfitSchedule(asSet(shifts), 20);

    /* Form the correct answer. It's the starred entries. */
    Set<Shift> actualSolution = {
        shifts[0], shifts[1], shifts[2], shifts[14]
    };

    EXPECT_EQUAL(computedSolution, actualSolution);
}

PROVIDED_TEST("maxProfitSchedule reports an error with negative hours.") {
    /* Available shifts. */
    Vector<Shift> shifts = {
        { Day::MONDAY,     8, 12, 27 },  // Mon  8AM - 12PM, value 27
        { Day::MONDAY,    12, 16, 28 },  // Mon 12PM -  4PM, value 28
        { Day::MONDAY,    16, 20, 25 },  // Mon  4PM -  8PM, value 25
        { Day::MONDAY,     8, 14, 39 },  // Mon  8AM -  2PM, value 39
        { Day::MONDAY,    14, 20, 31 },  // Mon  2PM -  8PM, value 31
        { Day::TUESDAY,    8, 12,  7 },  // Tue  8AM - 12PM, value  7
        { Day::TUESDAY,   12, 16,  7 },  // Tue 12PM -  4PM, value  7
        { Day::TUESDAY,   16, 20, 11 },  // Tue  4PM -  8PM, value 11
        { Day::TUESDAY,    8, 14, 10 },  // Tue  8AM -  2PM, value 10
        { Day::TUESDAY,   14, 20,  8 },  // Tue  2PM -  8PM, value  8
        { Day::WEDNESDAY,  8, 12, 10 },  // Wed  8AM - 12PM, value 10
        { Day::WEDNESDAY, 12, 16, 11 },  // Wed 12PM -  4PM, value 11
        { Day::WEDNESDAY, 16, 20, 13 },  // Wed  4PM -  8PM, value 13
        { Day::WEDNESDAY,  8, 14, 19 },  // Wed  8AM -  2PM, value 19
        { Day::WEDNESDAY, 14, 20, 25 },  // Wed  2PM -  8PM, value 25
    };

    /* Should be an error. */
    EXPECT_ERROR(maxProfitSchedule(asSet(shifts), -1));

    /* Still an error even if there are no shifts. */
    EXPECT_ERROR(maxProfitSchedule({}, -1));
}


PROVIDED_TEST("maxProfitSchedule handles zero free hours.") {
    /* Available shifts. */
    Vector<Shift> shifts = {
        { Day::MONDAY,     8, 12, 27 },  // Mon  8AM - 12PM, value 27
        { Day::MONDAY,    12, 16, 28 },  // Mon 12PM -  4PM, value 28
        { Day::MONDAY,    16, 20, 25 },  // Mon  4PM -  8PM, value 25
        { Day::MONDAY,     8, 14, 39 },  // Mon  8AM -  2PM, value 39
        { Day::MONDAY,    14, 20, 31 },  // Mon  2PM -  8PM, value 31
        { Day::TUESDAY,    8, 12,  7 },  // Tue  8AM - 12PM, value  7
        { Day::TUESDAY,   12, 16,  7 },  // Tue 12PM -  4PM, value  7
        { Day::TUESDAY,   16, 20, 11 },  // Tue  4PM -  8PM, value 11
        { Day::TUESDAY,    8, 14, 10 },  // Tue  8AM -  2PM, value 10
        { Day::TUESDAY,   14, 20,  8 },  // Tue  2PM -  8PM, value  8
        { Day::WEDNESDAY,  8, 12, 10 },  // Wed  8AM - 12PM, value 10
        { Day::WEDNESDAY, 12, 16, 11 },  // Wed 12PM -  4PM, value 11
        { Day::WEDNESDAY, 16, 20, 13 },  // Wed  4PM -  8PM, value 13
        { Day::WEDNESDAY,  8, 14, 19 },  // Wed  8AM -  2PM, value 19
        { Day::WEDNESDAY, 14, 20, 25 },  // Wed  2PM -  8PM, value 25
    };

    /* Convert vector to set */
    Set<Shift> shiftSet = asSet(shifts);

    /* Shouldn't be an error if time is zero - that means we just don't pick anything. */
    EXPECT_EQUAL(maxProfitSchedule(shiftSet, 0).size(), 0);
}

PROVIDED_TEST("maxProfitSchedule stress test: Don't generate combinations with overlaps.") {
    /* All of these shifts overlap one another. If you try producing all combinations
     * of these shifts and only check at the end whether they're valid, you'll be
     * checking 2^100 ~= 10^30 combinations of shifts, which will take so long the
     * sun will have burnt out before you're finished.
     *
     * Instead, as you're going through your decision tree and building up your shifts,
     * make sure not to include any shifts that clearly conflict with something you
     * picked earlier.
     */
    Set<Shift> trickySet;
    for (int i = 0; i < 100; i++) {
        // Assuming Shift has a constructor like Shift(Day, startHour, endHour)
        trickySet.insert(Shift{ Day::MONDAY, 1, 2 });  // Only start and end hours
    }
    EXPECT_EQUAL(trickySet.size(), 100);

    // Assuming maxProfitSchedule only picks non-overlapping shifts
    auto result = maxProfitSchedule(trickySet, 1);  // Max 1 hour total, should pick only one shift
    EXPECT_EQUAL(result.size(), 1);  // Only one shift should be chosen
}


PROVIDED_TEST("maxProfitSchedule stress test: Don't generate combinations exceeding time limit.") {
    /* Here's a collection of one shift per hour of the week. Your worker has exactly
     * one hour free. If you try all possible combinations of these shifts, ignoring time
     * constraints, you will have to check over 2^100 = 10^30 combinations, which will
     * take longer than the length of the known universe to process.
     *
     * Instead, as you're exploring the decision tree to generate shift combinations,
     * make sure not to add shifts that would exceed the time limit.
     */
    Set<Shift> trickySet;
    for (Day day : { Day::SUNDAY, Day::MONDAY, Day::TUESDAY, Day::WEDNESDAY, Day::THURSDAY, Day::FRIDAY, Day::SATURDAY }) {
        for (int start = 0; start < 24; start++) {
            trickySet.insert(Shift{ day, start, start + 1 });  // Each shift lasts for 1 hour
        }
    }
    EXPECT_EQUAL(trickySet.size(), 7 * 24);  // 7 days * 24 hours = 168 shifts

    auto result = maxProfitSchedule(trickySet, 1);  // The max time limit is 1 hour
    EXPECT_EQUAL(result.size(), 1);  // Only one shift should be selected
}


PROVIDED_TEST("maxProfitSchedule stress test: Handles realistic example.") {
    /* Available shifts. */
    Vector<Shift> shifts = {
                            { Day::SUNDAY,  8, 14, 12 },
                            { Day::SUNDAY, 12, 18, 36 },

                            { Day::MONDAY,  8, 12, 44 },
                            { Day::MONDAY, 12, 16, 32 },
                            { Day::MONDAY, 16, 20,  0 },
                            { Day::MONDAY,  8, 16, 16 },
                            { Day::MONDAY, 12, 20, 22 },

                            { Day::TUESDAY,  8, 12, 48 },
                            { Day::TUESDAY, 12, 16, 20 },
                            { Day::TUESDAY, 16, 20, 24 },
                            { Day::TUESDAY,  8, 16, 24 },
                            { Day::TUESDAY, 12, 20, 80 },

                            { Day::WEDNESDAY,  8, 12, 20 },
                            { Day::WEDNESDAY, 12, 16,  8 },
                            { Day::WEDNESDAY, 16, 20,  8 },
                            { Day::WEDNESDAY,  8, 16, 40 },
                            { Day::WEDNESDAY, 12, 20, 16 },

                            { Day::THURSDAY,  8, 12, 40 },
                            { Day::THURSDAY, 12, 16,  0 },
                            { Day::THURSDAY, 16, 20, 24 },
                            { Day::THURSDAY,  8, 16, 56 },
                            { Day::THURSDAY, 12, 20, 32 },

                            { Day::FRIDAY,  8, 12,  4 },
                            { Day::FRIDAY, 12, 16,  8 },
                            { Day::FRIDAY, 16, 20, 40 },
                            { Day::FRIDAY,  8, 16, 72 },
                            { Day::FRIDAY, 12, 20, 40 },

                            { Day::SATURDAY,  8, 14, 18 },
                            { Day::SATURDAY, 12, 18, 66 },
                            };

    /* Convert vector to set */
    Set<Shift> shiftSet = asSet(shifts);

    /* This code should finish in at most four seconds. */
    EXPECT_COMPLETES_IN(4.0, {
        auto answer = maxProfitSchedule(shiftSet, 25);  // Max 25 hours allowed
        EXPECT_EQUAL(answer, { shifts[2], shifts[7], shifts[11], shifts[17], shifts[24] });
    });
}

