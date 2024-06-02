#include <string>
#include <memory>
#include <map>
#include "spdlog/spdlog.h"
#include "gtest/gtest.h"

#include "../dictionary/dictionary.h"

class DictionaryTest : public testing::Test
{

protected:

    void SetUp() override
    {
        dict = std::unique_ptr<dictionary>(new dictionary(std::shared_ptr<saver>(new saver())));
        dict->process_command("PUT test1key test1value");
    }

    std::unique_ptr<dictionary> dict;
};

TEST_F(DictionaryTest, PutCommand)
{
    std::string ordinary_i = "PUT test2s testfirstsval";
    std::string rewrite_i = "PUT test1key rewrite";
    std::string noval_i = "PUT test4";
    std::string fence_i = "PuT test4 test4val";
    std::string lowercase_i = "put test5 test5val";
    std::string wrongkey_i = "PUT ??? value";
    std::string wrongvalue_i = "PUT key !!!!";

    std::string ordinary_r = dict->process_command(ordinary_i);
    std::string rewrite_r = dict->process_command(rewrite_i);
    std::string noval_r = dict->process_command(noval_i);
    std::string fence_r = dict->process_command(fence_i);
    std::string lowercase_r = dict->process_command(lowercase_i);
    std::string wrongkey_r = dict->process_command(wrongkey_i);
    std::string wrongvalue_r = dict->process_command(wrongvalue_i);

    EXPECT_EQ("OK ", ordinary_r);
    EXPECT_EQ("OK test1value", rewrite_r);
    EXPECT_EQ("Wrong command", noval_r);
    EXPECT_EQ("Wrong command", fence_r);
    EXPECT_EQ("Wrong command", lowercase_r);
    EXPECT_EQ("Wrong command", wrongkey_r);
    EXPECT_EQ("Wrong command", wrongvalue_r);
}

TEST_F(DictionaryTest, GetCommand)
{
    std::string ordinary_i = "GET test1key";
    std::string nonexist_i = "GET test2key";
    std::string nokey_i = "GET";
    std::string fence_i = "gET test1key";
    std::string wrongkey_i = "GET )))";

    std::string ordinary_r = dict->process_command(ordinary_i);
    std::string nonexist_r = dict->process_command(nonexist_i);
    std::string nokey_r = dict->process_command(nokey_i);
    std::string fence_r = dict->process_command(fence_i);
    std::string wrongkey_r = dict->process_command(wrongkey_i);

    EXPECT_EQ("OK test1value", ordinary_r);
    EXPECT_EQ("NE", nonexist_r);
    EXPECT_EQ("Wrong command", nokey_r);
    EXPECT_EQ("Wrong command", fence_r);
    EXPECT_EQ("Wrong command", wrongkey_r);
}

TEST_F(DictionaryTest, CountCommand)
{
    std::string ok_count_i = "COUNT";
    std::string odd_val_i = "COUNT 1";
    std::string fence_i = "COuNt";
    std::string lowercase_i = "count";

    std::string ok_count_r = dict->process_command(ok_count_i);
    std::string odd_val_r = dict->process_command(odd_val_i);
    std::string fence_r = dict->process_command(fence_i);
    std::string lowercase_r = dict->process_command(lowercase_i);

    EXPECT_EQ("OK 1", ok_count_r);
    EXPECT_EQ("Wrong command", odd_val_r);
    EXPECT_EQ("Wrong command", fence_r);
    EXPECT_EQ("Wrong command", lowercase_r);
}

TEST_F(DictionaryTest, DelCommand)
{
    std::string ok_del_i = "DEL test1key";
    std::string nonexist_i = "DEL doesnt";
    std::string nokey_i = "DEL";
    std::string fence_i = "DeL test1key";
    std::string wrongkey_i = "DEL )))";

    std::string ok_del_r = dict->process_command(ok_del_i);
    std::string nonexist_r = dict->process_command(nonexist_i);
    std::string nokey_r = dict->process_command(nokey_i);
    std::string fence_r = dict->process_command(fence_i);
    std::string wrongkey_r = dict->process_command(wrongkey_i);

    EXPECT_EQ("OK test1value", ok_del_r);
    EXPECT_EQ("NE", nonexist_r);
    EXPECT_EQ("Wrong command", nokey_r);
    EXPECT_EQ("Wrong command", fence_r);
    EXPECT_EQ("Wrong command", wrongkey_r);
}

TEST_F(DictionaryTest, DictionaryGet)
{
    std::map<std::string, std::string> expected = {{"test1key", "test1value"}, {"key4", "val4"}};
    std::string put1 = "PUT key3 val3";
    std::string put2 = "PUT key4 val4";
    std::string del = "DEL key3";

    dict->process_command(put1);
    dict->process_command(put2);
    dict->process_command(del);
    std::map<std::string, std::string> returned = dict->get_dictionary();

    EXPECT_EQ(expected, returned);
}

TEST_F(DictionaryTest, DictionarySet)
{

    std::map<std::string, std::string> to_set = {{"test1key", "test1value"}, {"test2key", "test2value"}, {"test7key", "7value"}};

    dict->set_dictionary(to_set);
    std::map<std::string, std::string> returned = dict->get_dictionary();

    EXPECT_EQ(to_set, returned);
}
