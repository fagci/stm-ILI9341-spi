#include "ILI9341_lib/graph.h"
#include "test_suite/primitives.h"

int main(void) {
    LCD_init();
    LCD_setOrientation(ORIENTATION_LANDSCAPE_MIRROR);

    TEST_fillPrimitives(20);

    while (1);
}
