#include <gtest/gtest.h>

#include <Inventor/SbBox.h>

#include <limits>

TEST(SbBox3f, Constructors) {

    {
        SbBox3f b;
        EXPECT_TRUE(b.isEmpty());
    }

    {
        SbVec3f pmin(0, 1, 2);
        SbVec3f pmax(3, 4, 5);
        SbBox3f b(0, 1, 2, 3, 4, 5);
        EXPECT_TRUE(b.getMin() == pmin);
        EXPECT_TRUE(b.getMax() == pmax);
    }

    {
        SbVec3f pmin(0, 1, 2);
        SbVec3f pmax(3, 4, 5);
        SbBox3f b(pmin, pmax);
        EXPECT_TRUE(b.getMin() == pmin);
        EXPECT_TRUE(b.getMax() == pmax);
    }
}

TEST(SbBox3f, MakeEmpty) {
    SbVec3f pmin(0, 1, 2);
    SbVec3f pmax(3, 4, 5);
    SbBox3f b(pmin, pmax);

    EXPECT_FALSE(b.isEmpty());
    EXPECT_TRUE(b.hasVolume());

    b.makeEmpty();

    EXPECT_TRUE(b.isEmpty());
    EXPECT_FALSE(b.hasVolume());
}

TEST(SbBox3f, ExtendBy) {
    SbVec3f pmin(0, 1, 2);
    SbVec3f pmax(3, 4, 5);
    SbBox3f b;

    EXPECT_TRUE(b.isEmpty());

    b.extendBy(pmin);
    EXPECT_TRUE(b.getMax() == pmin);
    EXPECT_FALSE(b.isEmpty());

    b.extendBy(pmax);
    EXPECT_TRUE(b.getMin() == pmin);
    EXPECT_TRUE(b.getMax() == pmax);
}

TEST(SbBox3f, Size) {
    SbVec3f pmin(0, 1, 2);
    SbVec3f pmax(3, 4, 5);
    SbBox3f b(pmin, pmax);

    EXPECT_TRUE(b.getSize() == SbVec3f(3, 3, 3));
}

float
nextRand(float rangeMin, float rangeMax) {
    float f = rand() / (float)RAND_MAX;

    return rangeMin * (1 - f) + rangeMax * f;
}

TEST(SbBox3f, BoxPlaneIntersection) {
    srand(time(NULL));

    SbBox3f box(SbVec3f(-1.0f, -1.0, -1.0f), SbVec3f(1.0f, 1.0, 1.0f));

    // Never intersect
    float mindist = (box.getMin() - box.getCenter()).length();
    for (int i = 0; i < 1000; i++) {
        SbVec3f outside;
        do {
            outside = SbVec3f(nextRand(-10.0f, 10.0f), nextRand(-10.0f, 10.0f),
                              nextRand(-10.0f, 10.0f));
        } while ((outside - box.getCenter()).length() <= mindist);

        SbPlane plane(box.getCenter() - outside,
                      (box.getCenter() - outside).length());

        EXPECT_FALSE(box.intersect(plane))
            << outside[0] << " " << outside[1] << " " << outside[2];
    }

    // Always intersect
    for (int i = 0; i < 1000; i++) {
        SbVec3f inside, normal;
        do {
            inside = SbVec3f(nextRand(-10.0f, 10.0f), nextRand(-10.0f, 10.0f),
                             nextRand(-10.0f, 10.0f));
        } while (!box.intersect(inside));

        do {
            normal = SbVec3f(nextRand(-1.0f, 1.0f), nextRand(-1.0f, 1.0f),
                             nextRand(-1.0f, 1.0f));
        } while (normal.length() < 0.1f);

        normal.normalize();

        SbPlane plane(normal, normal.dot(inside));

        EXPECT_TRUE(box.intersect(plane));
    }
}
