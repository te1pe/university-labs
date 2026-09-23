#include <gtest/gtest.h>
#include "lifem.h"

static int aliveCount(const Field& field) {
    int count = 0;
    for (int row = 0; row < field.getSize(); ++row) {
        for (int column = 0; column < field.getSize(); ++column) {
            if (field.getState(row, column)) {
                ++count;
            }
        }
    }
    return count;
}

TEST(FieldTest, EmptyAfterConstruction) {
    Field field;

    EXPECT_EQ(field.getSize(), FIELD_SIDE);
    EXPECT_EQ(aliveCount(field), 0);
}

TEST(FieldTest, SetAndGetState) {
    Field field;

    field.setState(3, 7, true);
    EXPECT_TRUE(field.getState(3, 7));
    EXPECT_FALSE(field.getState(7, 3));

    field.setState(3, 7, false);
    EXPECT_FALSE(field.getState(3, 7));
}

TEST(FieldTest, NeighbourBitsStayUntouched) {
    Field field;

    field.setState(0, 8, true);
    EXPECT_FALSE(field.getState(0, 7));
    EXPECT_FALSE(field.getState(0, 9));
    EXPECT_FALSE(field.getState(1, 8));
}

TEST(FieldTest, FieldIsTorus) {
    Field field;

    field.setState(0, 0, true);
    EXPECT_TRUE(field.getState(FIELD_SIDE, FIELD_SIDE));
    EXPECT_TRUE(field.getState(-FIELD_SIDE, -FIELD_SIDE));

    field.setState(-1, -1, true);
    EXPECT_TRUE(field.getState(FIELD_SIDE - 1, FIELD_SIDE - 1));
}

TEST(FieldTest, Clear) {
    Field field;
    field.setState(1, 1, true);
    field.setState(9, 9, true);

    field.clear();
    EXPECT_EQ(aliveCount(field), 0);
}

TEST(RuleTest, DigitsBecomeMask) {
    std::vector<bool> mask = ruleToMask("23");

    EXPECT_EQ(mask.size(), static_cast<size_t>(RULE_SIZE));
    EXPECT_TRUE(mask[2]);
    EXPECT_TRUE(mask[3]);
    EXPECT_FALSE(mask[0]);
    EXPECT_FALSE(mask[8]);
}

TEST(RuleTest, EmptyRuleForbidsEverything) {
    std::vector<bool> mask = ruleToMask("");

    for (int i = 0; i < RULE_SIZE; ++i) {
        EXPECT_FALSE(mask[i]);
    }
}

TEST(LifeModelTest, DefaultModelHasNameAndRule) {
    LifeModel model;

    EXPECT_EQ(model.getName(), DEFAULT_NAME);
    EXPECT_EQ(model.getRule(), "B3/S23");
    EXPECT_EQ(model.getIteration(), 0);
}

TEST(LifeModelTest, IterationCounterGrows) {
    LifeModel model("counter", "3", "23", {{0, 0}, {1, 1}});

    model.computeIteration();
    EXPECT_EQ(model.getIteration(), 1);

    model.computeIteration();
    EXPECT_EQ(model.getIteration(), 2);
}

TEST(LifeModelTest, BlockIsStillLife) {
    LifeModel model("block", "3", "23", {{5, 5}, {5, 6}, {6, 5}, {6, 6}});

    model.computeIteration();

    const Field& field = model.getField();
    EXPECT_EQ(aliveCount(field), 4);
    EXPECT_TRUE(field.getState(5, 5));
    EXPECT_TRUE(field.getState(6, 6));
}

TEST(LifeModelTest, BlinkerOscillatesWithPeriodTwo) {
    LifeModel model("blinker", "3", "23", {{9, 10}, {10, 10}, {11, 10}});

    model.computeIteration();
    EXPECT_TRUE(model.getField().getState(9, 10));
    EXPECT_TRUE(model.getField().getState(10, 10));
    EXPECT_TRUE(model.getField().getState(11, 10));
    EXPECT_EQ(aliveCount(model.getField()), 3);

    model.computeIteration();
    EXPECT_TRUE(model.getField().getState(10, 9));
    EXPECT_TRUE(model.getField().getState(10, 10));
    EXPECT_TRUE(model.getField().getState(10, 11));
}

TEST(LifeModelTest, LonelyCellDies) {
    LifeModel model("lonely", "3", "23", {{4, 4}});

    model.computeIteration();

    EXPECT_EQ(aliveCount(model.getField()), 0);
}

TEST(LifeModelTest, ThreeNeighboursGiveBirth) {
    LifeModel model("birth", "3", "23", {{1, 1}, {1, 2}, {2, 1}});

    model.computeIteration();

    EXPECT_TRUE(model.getField().getState(2, 2));
    EXPECT_EQ(aliveCount(model.getField()), 4);
}

TEST(LifeModelTest, CustomRuleKeepsCellsAlive) {
    LifeModel model("immortal", "3", "01234567", {{4, 4}});

    model.computeIteration();
    model.computeIteration();

    EXPECT_TRUE(model.getField().getState(4, 4));
}

TEST(LifeModelTest, GliderMovesDiagonally) {
    LifeModel model("glider", "3", "23", {{1, 0}, {2, 1}, {0, 2}, {1, 2}, {2, 2}});

    for (int i = 0; i < 4; ++i) {
        model.computeIteration();
    }

    const Field& field = model.getField();
    EXPECT_EQ(aliveCount(field), 5);
    EXPECT_TRUE(field.getState(1, 2));
    EXPECT_TRUE(field.getState(2, 3));
    EXPECT_TRUE(field.getState(3, 1));
    EXPECT_TRUE(field.getState(3, 2));
    EXPECT_TRUE(field.getState(3, 3));
}

TEST(LifeModelTest, AliveCellsRoundTrip) {
    std::vector<Point> cells = {{2, 3}, {7, 1}, {19, 19}};
    LifeModel model("dump", "3", "23", cells);

    std::vector<Point> alive = model.getAliveCells();

    EXPECT_EQ(alive.size(), cells.size());
    for (const Point& cell : cells) {
        EXPECT_TRUE(model.getField().getState(cell.y, cell.x));
    }
}

TEST(LifeModelTest, TorusWrapsNeighbours) {
    LifeModel model("edge", "3", "23", {{0, 0}, {19, 0}, {0, 19}});

    model.computeIteration();

    EXPECT_TRUE(model.getField().getState(19, 19));
}
