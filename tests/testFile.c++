#include <gtest/gtest.h>

#include <Inventor/SbFile.h>

class SbFileTest : public ::testing::Test {
  protected:
    virtual void SetUp() {}
    virtual void TearDown() {}

    // Helper to create an unicode file on disk.
    SbString createUnicodeFilename() {
        SbString chineeseFilename = SbString::fromUtf8(
            "\345\215\216\346\226\207\344\273\277\345\256\213.ttf");
        if (!SbFile::exists(chineeseFilename)) {
            SbFile file;
            if (file.open(chineeseFilename, "wb")) {
                file.close();
            } else {
                printf("ERROR: Cannot create unicode file for testing.\n");
            }
        }

        return chineeseFilename;
    }
};

TEST_F(SbFileTest, BaseName) {

    SbString filename1("archive.tar.gz");
    SbString filename2("/tmp/archive.tar.gz");
    SbString filename3("C:/tmp\\archive.tar.gz");

    EXPECT_TRUE(SbFile::baseName(filename1) == "archive.tar.gz");
    EXPECT_TRUE(SbFile::baseName(filename2) == "archive.tar.gz");
    EXPECT_TRUE(SbFile::baseName(filename3) == "archive.tar.gz");

    SbString chineeseFont = SbString::fromUtf8(
        "/Library/Fonts/\345\215\216\346\226\207\344\273\277\345\256\213.ttf");
    EXPECT_TRUE(SbFile::baseName(chineeseFont) ==
                SbString::fromUtf8(
                    "\345\215\216\346\226\207\344\273\277\345\256\213.ttf"));
}

TEST_F(SbFileTest, Extension) {

    SbString filename("/tmp/../archive.tar.gz");

    EXPECT_TRUE(SbFile::extension(filename) == "tar.gz");

    SbString chineeseFont = SbString::fromUtf8(
        "/Library/Fonts/\345\215\216\346\226\207\344\273\277\345\256\213.ttf");
    EXPECT_TRUE(SbFile::extension(chineeseFont) == "ttf");
}

TEST_F(SbFileTest, Dirname) {
    SbString filename1("/tmp/archive.tar.gz");
    SbString filename2("C:/tmp\\archive.tar.gz");

    EXPECT_TRUE(SbFile::dirName(filename1) == "/tmp/");
    EXPECT_TRUE(SbFile::dirName(filename2) == "C:/tmp\\");
}

TEST_F(SbFileTest, Exist) {
    SbString filename = createUnicodeFilename();

    EXPECT_TRUE(SbFile::exists(filename));
    EXPECT_FALSE(SbFile::exists("dummyfile"));
    EXPECT_FALSE(SbFile::exists(""));
}

TEST_F(SbFileTest, Open) {
    SbString filename = createUnicodeFilename();

    SbFile file;
    ASSERT_TRUE(file.open(filename, "rb"));
    EXPECT_TRUE(file.isOpen());
    file.close();
    EXPECT_FALSE(file.isOpen());
}
