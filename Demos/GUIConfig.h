WINDOW_TITLE("Recursion to the Rescue!")

RUN_TESTS_MENU_OPTION()
MENU_ORDER("ShiftSchedulingGUI.cpp",
           "DisasterGUI.cpp")
           
TEST_ORDER("ShiftScheduling.cpp",
           "WinSumLoseSum.cpp",
           "DisasterPlanning.cpp")

TEST_BARRIER("ShiftSchedulingGUI.cpp", "ShiftScheduling.cpp")
TEST_BARRIER("DisasterGUI.cpp",        "DisasterPlanning.cpp")
