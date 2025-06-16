#include <Arduino.h>
#include <unity.h>
#include "TinyConfig.h"

TinyConfig tc;

void test_init() {
    TEST_ASSERT_TRUE(tc.StartTC());
    TEST_ASSERT_EQUAL(TinyConfigError::None, tc.getLastError());
}

void test_resetConfig() {
    TEST_ASSERT_TRUE(tc.set("test_key", 123));
    TEST_ASSERT_EQUAL(123, tc.getInt("test_key", 0));
    TEST_ASSERT_TRUE(tc.resetConfig());
    TEST_ASSERT_EQUAL(TinyConfigError::None, tc.getLastError());
    TEST_ASSERT_EQUAL(0, tc.getInt("test_key", 0));
}

void test_set_and_get() {
    tc.resetConfig();
    TEST_ASSERT_TRUE(tc.set("int_key", 42));
    TEST_ASSERT_TRUE(tc.set("float_key", 3.14f));
    TEST_ASSERT_TRUE(tc.set("str_key", String("hello")));

    TEST_ASSERT_EQUAL(42, tc.getInt("int_key", -1));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 3.14f, tc.getFloat("float_key", -1.0f));
    TEST_ASSERT_EQUAL_STRING("hello", tc.getString("str_key", "fail").c_str());
}

void test_getAll_functions() {
    tc.resetConfig();
    TEST_ASSERT_TRUE(tc.set("key1", 1));
    TEST_ASSERT_TRUE(tc.set("key2", 2.5f));
    TEST_ASSERT_TRUE(tc.set("key3", String("test")));

    String all = tc.getAll("{}");
    DynamicJsonDocument doc = tc.getAllJson();

    TEST_ASSERT_TRUE(doc.containsKey("key1"));
    TEST_ASSERT_TRUE(doc.containsKey("key2"));
    TEST_ASSERT_TRUE(doc.containsKey("key3"));

    TEST_ASSERT_EQUAL(1, doc["key1"].as<int>());
    TEST_ASSERT_FLOAT_WITHIN(0.01, 2.5f, doc["key2"].as<float>());
    TEST_ASSERT_EQUAL_STRING("test", doc["key3"].as<String>().c_str());

    TEST_ASSERT_TRUE(all.startsWith("{"));
    TEST_ASSERT_TRUE(all.endsWith("}"));
    TEST_ASSERT_TRUE(all.indexOf("\"key1\":1") != -1);
    TEST_ASSERT_TRUE(all.indexOf("\"key2\":2.5") != -1);
    TEST_ASSERT_TRUE(all.indexOf("\"key3\":\"test\"") != -1);
}

void test_fallback() {
    tc.resetConfig();
    TEST_ASSERT_EQUAL(123, tc.getInt("notfound", 123));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 1.23f, tc.getFloat("notfound", 1.23f));
    TEST_ASSERT_EQUAL_STRING("fallback", tc.getString("notfound", "fallback").c_str());
}

void test_max_file_size() {
    tc.resetConfig();
    tc.setMaxFileSize(20);
    String big = "";
    for (int i = 0; i < 100; ++i) big += 'A';
    TEST_ASSERT_FALSE(tc.set("big", big));
    TEST_ASSERT_EQUAL(TinyConfigError::FileSizeTooLarge, tc.getLastError());
}

void test_stop_and_error() {
    TEST_ASSERT_TRUE(tc.StopTC());
    TEST_ASSERT_FALSE(tc.set("after_stop", 1));
    TEST_ASSERT_EQUAL(TinyConfigError::NotInitialized, tc.getLastError());
    TEST_ASSERT_EQUAL(42, tc.getInt("after_stop", 42));
}

void test_deleteKey() {
    tc.resetConfig();
    TEST_ASSERT_TRUE(tc.set("delete_me", 99));
    TEST_ASSERT_EQUAL(99, tc.getInt("delete_me", 0));
    TEST_ASSERT_TRUE(tc.deleteKey("delete_me"));
    TEST_ASSERT_EQUAL(0, tc.getInt("delete_me", 0));
    TEST_ASSERT_FALSE(tc.deleteKey("non_existent"));
}

void setup() {
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_init);
    RUN_TEST(test_resetConfig);
    RUN_TEST(test_set_and_get);
    RUN_TEST(test_getAll_functions);
    RUN_TEST(test_fallback);
    RUN_TEST(test_deleteKey);
    RUN_TEST(test_max_file_size);
    RUN_TEST(test_stop_and_error);
    UNITY_END();
}

void loop() {}